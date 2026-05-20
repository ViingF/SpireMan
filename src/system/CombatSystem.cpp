#include "CombatSystem.hpp"
ErrorCode CombatSystem::startCombat(RunState& runState, const EncounterDef& encounterDef,
    const EnemyDatabase& enemyDatabase, CardDatabase& cardDatabase) {
    // 重置状态（同前）
    phase_ = CombatPhase::Finished;
    player_ = Player{};
    enemies_.clear();
    deck_ = CombatDeck{};
    energy_ = 0;
    turnIndex_ = 0;
    result_ = BattleResult::Ongoing;
    resultCommitted_ = false;

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
            continue;
        }

        const EnemyDef& def = enemyDatabase.get(slot.enemyId);

        Enemy enemy;
        enemy.id = def.id;
        enemy.name = def.name;
        enemy.maxHp = def.maxHp;
        enemy.hp = def.maxHp;
        enemy.block = 0;
        enemy.strength = 0;
        enemy.vulnerable = 0;
        enemy.weak = 0;
        enemy.movePattern = def.movePattern;

        enemies_.push_back(enemy);

        enemyCount++;
    }

    if (enemies_.empty()) return ErrorCode::INVALID_SCENE_STATE; // 没有敌人，报错

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
        return ErrorCode::INVALID_SCENE_STATE;
    }
    if (result_ != BattleResult::Ongoing)
    {
        return ErrorCode::INVALID_SCENE_STATE;
    }
    if (handIndex < 0 || handIndex >= static_cast<int>(deck_.hand.size()))
    {
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

    return ErrorCode::OK;
}
ErrorCode CombatSystem::canPlayCard(int handIndex, TargetId targetId) const
{
    const CardInstance& inst = deck_.hand[handIndex];
    const CardDef& card = cardDatabase_->get(inst.cardId);

    switch (card.target)
    {
    case TargetType::Enemy:
        if (targetId == NoTarget) return ErrorCode::INVALID_TARGET;
        if (getAliveEnemyByTargetId(targetId) == nullptr) return ErrorCode::INVALID_TARGET;
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
    //依旧检查一遍
    if (phase_ != CombatPhase::PlayerTurn)
    {
        return ErrorCode::INVALID_SCENE_STATE;
    }
    if (result_ != BattleResult::Ongoing)
    {
        return ErrorCode::INVALID_SCENE_STATE;
    }
    //弃牌阶段
    cardSystem_.discardAllHand(deck_);
    phase_ = CombatPhase::EnemyTurn;
    for (auto& enemy : enemies_)
    {
        if (enemy.hp > 0)
        {
            enemy.block = 0;
        }
    }//设计文档里说明只有一个敌人，因此直接清空格挡即可
    executeEnemyIntents();

    // 检查战斗结果
    checkBattleResult();
    // 如果战斗未结束，开始下一玩家回合
    if (result_ == BattleResult::Ongoing) {
        startPlayerTurn();
    }
    else
    {
        phase_ = CombatPhase::Finished;
    }
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
        runState.player.hp = player_.hp;//战士被动回六血
        runState.player.maxHp = player_.maxHp;
    }
    if (result_ == BattleResult::Defeat)
    {
        runState.player.hp = 0;
    }
    resultCommitted_ = true;
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
    for (const auto& effect : card.effects) {
        switch (effect.type) {
        case EffectType::DealDamage:
            dealDamageToEnemy(targetId, effect.value);
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
            applyVulnerableToEnemy(targetId, effect.value);
            break;
        case EffectType::ApplyWeak:
            applyWeakToEnemy(targetId, effect.value);
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
        result_ = BattleResult::Defeat;
        phase_ = CombatPhase::Finished;
    }
    else if (areAllEnemiesDefeated()) {
        result_ = BattleResult::Victory;
        phase_ = CombatPhase::Finished;
    }
}