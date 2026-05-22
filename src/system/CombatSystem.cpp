#include "CombatSystem.hpp"
#include <random>

#include "config/Constants.hpp"
#include "core/Logger.hpp"

namespace {
    constexpr int kVictoryGoldMin = 10;
    constexpr int kVictoryGoldMax = 20;

    EnemyIntent adjustEnemyIntent(EnemyIntent intent)
    {
        if (intent.type == EnemyIntentType::Attack) {
            intent.value = adjustEnemyValue(
                intent.value,
                ENEMY_ATTACK_PERCENT,
                ENEMY_ATTACK_FLAT_BONUS,
                0
            );
        }

        return intent;
    }
}


ErrorCode CombatSystem::startCombat(RunState& runState, const EncounterDef& encounterDef,
    const EnemyDatabase& enemyDatabase, CardDatabase& cardDatabase) {

    LOG_INFO(
    "Combat started: encounterId=" << encounterDef.id
    << ", isBoss=" << encounterDef.isBoss
);

    // 重置状态（同前）
    phase_ = CombatPhase::Finished;
    player_ = Player{};
    enemies_.clear();
    deck_ = CombatDeck{};
    energy_ = 0;
    turnIndex_ = 0;
    result_ = BattleResult::Ongoing;
    resultCommitted_ = false;
    isBossEncounter_ = encounterDef.isBoss;


    cardDatabase_ = &cardDatabase;
    rng_ = &runState.rng;

    // 玩家状态复制
    player_.maxHp = runState.player.maxHp;
    player_.hp = runState.player.hp;
    // ... 其他属性

    // 根据 EncounterDef 创建多个敌人
    int enemyCount = 0;

    for (const auto& slot : encounterDef.enemies)
    {
        // 最多只允许 3 个敌人
        if (enemyCount >= 3)
        {
            break;
        }

        if (!enemyDatabase.exists(slot.enemyId))
        {
            LOG_WARN("Enemy id not found in encounter: enemyId=" << slot.enemyId);
            continue;
        }


        const EnemyDef& def = enemyDatabase.get(slot.enemyId);

        Enemy enemy;
        enemy.id = def.id;
        enemy.name = def.name;
        enemy.maxHp = adjustEnemyValue(
    def.maxHp,
    ENEMY_HP_PERCENT,
    ENEMY_HP_FLAT_BONUS,
    1
);

        enemy.hp = enemy.maxHp;
        enemy.block = 0;
        enemy.strength = 0;
        enemy.vulnerable = 0;
        enemy.weak = 0;

        enemy.movePattern.clear();
        enemy.movePattern.reserve(def.movePattern.size());

        for (const EnemyIntent& intent : def.movePattern) {
            enemy.movePattern.push_back(adjustEnemyIntent(intent));
        }


        enemies_.push_back(enemy);

        enemyCount++;
    }

    if (enemies_.empty()) {
        LOG_ERROR("Combat start failed: no valid enemies");
        return ErrorCode::INVALID_SCENE_STATE;
    }


    cardSystem_.buildCombatDeck(deck_, runState.masterDeck);
    cardSystem_.shuffleDrawPile(deck_, *rng_);
    startPlayerTurn();  // 内部会抽牌、生成敌人意图等
    return ErrorCode::OK;
}
bool CombatSystem::hasCommittedResult() const
{
    return resultCommitted_;
}
const Player& CombatSystem::getPlayer() const
{
    return player_;
}
const std::vector<Enemy>& CombatSystem::getEnemies() const
{
    return enemies_;
}
const CombatDeck& CombatSystem::getDeck() const
{
    return deck_;
}
int CombatSystem::getEnergy() const
{
    return energy_;
}
int CombatSystem::getTurnIndex() const
{
    return turnIndex_;
}
CombatPhase CombatSystem::getPhase() const
{
    return phase_;
}
BattleResult CombatSystem::getBattleResult() const
{
    return result_;
}
ErrorCode CombatSystem::playCard(int handIndex, TargetId targetId)
{
    if (phase_ != CombatPhase::PlayerTurn)
    {
        LOG_WARN("Play card failed: not player turn");
        return ErrorCode::INVALID_SCENE_STATE;
    }
    if (result_ != BattleResult::Ongoing)
    {
        return ErrorCode::INVALID_SCENE_STATE;
    }
    if (handIndex < 0 || handIndex >= static_cast<int>(deck_.hand.size()))
    {
        LOG_WARN("Play card failed: invalid handIndex=" << handIndex);
        return ErrorCode::CARD_NOT_IN_HAND;
    }
    const CardInstance& inst = deck_.hand[handIndex];
    if (!cardDatabase_->exists(inst.cardId))
    {
        return ErrorCode::INVALID_TARGET;
    }
    const CardDef& card = cardDatabase_->get(inst.cardId);
    ErrorCode targetCheck = canPlayCard(handIndex, targetId);
    if (targetCheck != ErrorCode::OK) {
        return targetCheck;
    }

    // 扣费
    if (energy_ < card.cost) {
        return ErrorCode::NOT_ENOUGH_ENERGY;
    }
    energy_ -= card.cost;


    applyCardEffects(card, targetId);

    // 移除打出的牌
    movePlayedCardAfterResolve(inst, card);

    // 检查敌人是否死亡
    checkBattleResult();

    LOG_INFO(
    "Card played: cardId=" << card.id
    << ", name=" << card.name
    << ", targetId=" << targetId
    << ", energyLeft=" << energy_
);


    return ErrorCode::OK;
}
ErrorCode CombatSystem::canPlayCard(int handIndex, TargetId targetId) const
{
    const CardInstance& inst = deck_.hand[handIndex];
    const CardDef& card = cardDatabase_->get(inst.cardId);

    if (card.cost < 0) {
        return ErrorCode::CARD_UNPLAYABLE;
    }

    switch (card.target)
    {
    case TargetType::Enemy:
        if (targetId == NoTarget) return ErrorCode::INVALID_TARGET;
        if (getAliveEnemyByTargetId(targetId) == nullptr) return ErrorCode::INVALID_TARGET;
        break;
        case TargetType::AllEnemies:
    if (targetId != NoTarget) return ErrorCode::INVALID_TARGET;
    if (areAllEnemiesDefeated()) return ErrorCode::INVALID_TARGET;
    break;

    case TargetType::Self:
        if (targetId != NoTarget) return ErrorCode::INVALID_TARGET;
        break;
    case TargetType::None:
        if (targetId != NoTarget) return ErrorCode::INVALID_TARGET;
        break;
    default:
        return ErrorCode::INVALID_TARGET;
    }
    return ErrorCode::OK;
}
ErrorCode CombatSystem::endPlayerTurn()
{
    if (phase_ != CombatPhase::PlayerTurn)
    {
        return ErrorCode::INVALID_SCENE_STATE;
    }
    if (result_ != BattleResult::Ongoing)
    {
        return ErrorCode::INVALID_SCENE_STATE;
    }

    // 先结算本回合已有的玩家易伤、虚弱衰减
    tickPlayerEndTurnStatuses();

    // 再触发“回合结束时，如果此牌在手牌中”的诅咒效果
    triggerEndTurnHandCardEffects();

    checkBattleResult();
    if (result_ != BattleResult::Ongoing)
    {
        return ErrorCode::OK;
    }

    // 最后弃牌
    cardSystem_.discardAllHand(deck_);

    phase_ = CombatPhase::EnemyTurn;

    for (auto& enemy : enemies_)
    {
        if (enemy.hp > 0)
        {
            enemy.block = 0;
        }
    }

    executeEnemyIntents();

    tickEnemiesEndTurnStatuses();

    checkBattleResult();

    if (result_ == BattleResult::Ongoing) {
        startPlayerTurn();
    }
    else
    {
        phase_ = CombatPhase::Finished;
    }
    LOG_INFO("Player turn ended: turnIndex=" << turnIndex_);

    return ErrorCode::OK;
}

ErrorCode CombatSystem::commitResultToRunState(RunState& runState)
{
    if (result_ == BattleResult::Ongoing)
    {
        return ErrorCode::INVALID_SCENE_STATE;
    }
    if (resultCommitted_) 
    {
        return ErrorCode::OK;  // 已提交过，幂等返回
    }
    if (result_ == BattleResult::Victory)
    {
        runState.player.maxHp = player_.maxHp;

        if (isBossEncounter_) {
            // 击败 Boss 后回满血
            runState.player.hp = runState.player.maxHp;
        } else {
            // 普通战斗仍然回复 6 点血
            runState.player.hp = std::min(
                runState.player.maxHp,
                player_.hp + 6
            );
        }

        std::uniform_int_distribution<int> goldDist(
            kVictoryGoldMin,
            kVictoryGoldMax
        );

        const int goldReward = goldDist(runState.rng);
        runState.gold += goldReward;
    }


    if (result_ == BattleResult::Defeat)
    {
        runState.player.hp = 0;
    }
    resultCommitted_ = true;

    LOG_INFO(
    "Combat result committed: result=" << toString(result_)
    << ", hp=" << runState.player.hp
    << "/" << runState.player.maxHp
    << ", gold=" << runState.gold
);

    return ErrorCode::OK;
}
void CombatSystem::gainBlock(int amount)
{
    player_.block += amount;
}
void CombatSystem::drawCards(int count)
{
    cardSystem_.drawCards(deck_,count,*rng_);
}
void CombatSystem::gainEnergy(int amount)
{
    energy_ += amount;
}
Enemy* CombatSystem::getAliveEnemyByTargetId(TargetId targetId) {
    if (targetId < 0 || targetId >= static_cast<TargetId>(enemies_.size())) 
    {
        return nullptr;
    }
    if (enemies_[targetId].hp <= 0) 
    {
        return nullptr;
    }
    return &enemies_[targetId];
}
const Enemy* CombatSystem::getAliveEnemyByTargetId(TargetId targetId) const 
{
    if (targetId < 0 || targetId >= static_cast<TargetId>(enemies_.size())) 
    {
        return nullptr;
    }
    if (enemies_[targetId].hp <= 0) 
    {
        return nullptr;
    }
    return &enemies_[targetId];
}
void CombatSystem::dealDamageToEnemy(TargetId targetId, int amount)
{
    Enemy* enemy = getAliveEnemyByTargetId(targetId);
    if (!enemy) return;
    int damage = amount + player_.strength;
    if (player_.weak > 0)
    {
        damage = static_cast<int>(damage * 0.75f);
    }
    if (enemy->vulnerable > 0) 
    {
      damage = static_cast<int>(damage * 1.5f);
    }

    int blocked = std::min(enemy->block, damage);
    enemy->block -= blocked;
    damage -= blocked;

    enemy->hp -= damage;
    if (enemy->hp < 0) enemy->hp = 0;
}
bool CombatSystem::areAllEnemiesDefeated() const
{
    int check = 0;
    for (int i = 0; i < enemies_.size(); i++)
    {
        if (enemies_[i].hp > 0)
        {
            check++;
        }
    }
    if (check == 0)
    {
        return 1;
    }
    else
    {
        return 0;
    }
}
void CombatSystem::gainStrength(int amount)
{
    player_.strength += amount;
}
void CombatSystem::applyVulnerableToEnemy(TargetId targetId, int amount) 
{
    Enemy* enemy = getAliveEnemyByTargetId(targetId);
    if (enemy) {
        enemy->vulnerable += amount;
    }
}
void CombatSystem::applyWeakToEnemy(TargetId targetId, int amount) 
{
    Enemy* enemy = getAliveEnemyByTargetId(targetId);
    if (enemy) {
        enemy->weak += amount;
    }
}
const CardDef& CombatSystem::getCardDefFromHand(int handIndex) const
{
    const CardInstance& inst = deck_.hand[handIndex];
    return cardDatabase_->get(inst.cardId);
}
void CombatSystem::startPlayerTurn()
{
    turnIndex_++;                     
    player_.block = 0;                
    energy_ = 3;                     
    cardSystem_.drawCards(deck_, 5, *rng_);  
    generateEnemyIntents();           
    phase_ = CombatPhase::PlayerTurn;
}
void CombatSystem::startEnemyTurn()
{
    phase_ = CombatPhase::EnemyTurn;
}
void CombatSystem::payCardCost(const CardDef& card)
{
    energy_ -= card.cost;
}
void CombatSystem::applyCardEffects(const CardDef& card, TargetId targetId)
{
    const bool targetAllEnemies = (card.target == TargetType::AllEnemies);

    for (const auto& effect : card.effects) {
        switch (effect.type) {
            case EffectType::DealDamage:
                if (targetAllEnemies) {
                    for (TargetId i = 0; i < static_cast<TargetId>(enemies_.size()); ++i) {
                        dealDamageToEnemy(i, effect.value);
                    }
                }
                else if (card.target == TargetType::Self) {
                    // 用于“失去生命”的诅咒效果，不经过格挡
                    losePlayerHp(effect.value);
                }
                else {
                    dealDamageToEnemy(targetId, effect.value);
                }
                break;


            case EffectType::GainBlock:
                gainBlock(effect.value);
                break;

            case EffectType::DrawCards:
                drawCards(effect.value);
                break;

            case EffectType::GainEnergy:
                gainEnergy(effect.value);
                break;

            case EffectType::ApplyVulnerable:
                if (card.target == TargetType::Self) {
                    applyVulnerableToPlayer(effect.value);
                }
                else if (targetAllEnemies) {
                    for (TargetId i = 0; i < static_cast<TargetId>(enemies_.size()); ++i) {
                        applyVulnerableToEnemy(i, effect.value);
                    }
                }
                else {
                    applyVulnerableToEnemy(targetId, effect.value);
                }
                break;


            case EffectType::ApplyWeak:
                if (card.target == TargetType::Self) {
                    applyWeakToPlayer(effect.value);
                }
                else if (targetAllEnemies) {
                    for (TargetId i = 0; i < static_cast<TargetId>(enemies_.size()); ++i) {
                        applyWeakToEnemy(i, effect.value);
                    }
                }
                else {
                    applyWeakToEnemy(targetId, effect.value);
                }
                break;


            case EffectType::GainStrength:
                gainStrength(effect.value);
                break;

            default:
                break;
        }
    }
}

void CombatSystem::movePlayedCardAfterResolve(const CardInstance& playedCard, const CardDef& card)
{
    auto it = std::find_if(deck_.hand.begin(), deck_.hand.end(),[&playedCard](const CardInstance& inst) 
        {
            return inst.instanceId == playedCard.instanceId;
        });
    if (it == deck_.hand.end()) return;

    // 根据卡牌是否消耗，决定放入弃牌堆还是消耗堆
    if (card.exhaust) {
        deck_.exhaustPile.push_back(*it);
    }
    else {
        deck_.discardPile.push_back(*it);
    }
    deck_.hand.erase(it);
}
void CombatSystem::generateEnemyIntents()
{
    for (auto& enemy : enemies_) 
    {
        if (enemy.hp > 0) {
            enemy.intent = enemyAI_.chooseIntent(enemy, turnIndex_);
        }
    }
}
void CombatSystem::executeEnemyIntents()
{
    for (auto& enemy : enemies_) 
    {
        if (enemy.hp > 0) 
        {
            enemyAI_.applyIntent(enemy, player_);
        }
    }
}
void CombatSystem::checkBattleResult()
{
    if (player_.hp <= 0) {
        LOG_INFO("Battle result changed: " << toString(result_));
        result_ = BattleResult::Defeat;
        phase_ = CombatPhase::Finished;
    }
    else if (areAllEnemiesDefeated()) {
        LOG_INFO("Battle result changed: " << toString(result_));
        result_ = BattleResult::Victory;
        phase_ = CombatPhase::Finished;
    }
}

void CombatSystem::decreaseTimedStatus(int& value)
{
    if (value > 0) {
        --value;
    }
}

void CombatSystem::tickPlayerEndTurnStatuses()
{
    decreaseTimedStatus(player_.vulnerable);
    decreaseTimedStatus(player_.weak);
}

void CombatSystem::tickEnemiesEndTurnStatuses()
{
    for (auto& enemy : enemies_) {
        decreaseTimedStatus(enemy.vulnerable);
        decreaseTimedStatus(enemy.weak);
    }
}

void CombatSystem::triggerEndTurnHandCardEffects()
{
    std::vector<CardId> triggeredCardIds;
    triggeredCardIds.reserve(deck_.hand.size());

    for (const CardInstance& inst : deck_.hand) {
        if (!cardDatabase_->exists(inst.cardId)) {
            continue;
        }

        const CardDef& card = cardDatabase_->get(inst.cardId);

        // 当前规则：负费用诅咒牌不可打出，但回合结束时可触发自身效果
        if (card.type == CardType::Curse && card.cost < 0 && !card.effects.empty()) {
            triggeredCardIds.push_back(inst.cardId);
        }
    }

    for (CardId cardId : triggeredCardIds) {
        const CardDef& card = cardDatabase_->get(cardId);
        applyCardEffects(card, NoTarget);
    }
}

void CombatSystem::losePlayerHp(int amount)
{
    if (amount <= 0) {
        return;
    }

    player_.hp -= amount;

    if (player_.hp < 0) {
        player_.hp = 0;
    }
}

void CombatSystem::applyVulnerableToPlayer(int amount)
{
    if (amount <= 0) {
        return;
    }

    player_.vulnerable += amount;
}

void CombatSystem::applyWeakToPlayer(int amount)
{
    if (amount <= 0) {
        return;
    }

    player_.weak += amount;
}
