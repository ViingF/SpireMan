
#ifndef SPIRELIKE_COMBATSYSTEM_HPP
#define SPIRELIKE_COMBATSYSTEM_HPP
#include "database/CardDatabase.hpp"
#include "model/CombatDeck.hpp"
#include "model/Enemy.hpp"
#include "model/Player.hpp"
#include "model/RunState.hpp"

class CombatSystem {
public:
    bool hasCommittedResult() const;

    void startCombat(
    RunState& runState,
    const EnemyDef& enemyDef,
    CardDatabase& cardDatabase
);


// 战斗中打出一张牌的唯一对外入口。
//
// targetId 的含义由 CardDef::target 决定：
// - TargetType::Enemy：targetId 必须是 CombatSystem::enemies_ 中合法且存活的运行时下标。
// - TargetType::Self：targetId 必须为 NoTarget。
// - TargetType::None：targetId 必须为 NoTarget。
//
// 如果 targetId 与 CardDef::target 不匹配，返回 INVALID_TARGET。
ErrorCode playCard(
    int handIndex,
    TargetId targetId
);


    // 结束玩家回合的唯一对外入口。
    // 内部负责弃牌、执行敌人意图、检查胜负，并在未结束时进入下一玩家回合。
    ErrorCode endPlayerTurn();

    BattleResult getBattleResult() const;

    // 将战斗结果提交回 RunState。
    // 只能在 result_ != BattleResult::Ongoing 时调用。
    ErrorCode commitResultToRunState(RunState& runState);

    const Player& getPlayer() const;
    const std::vector<Enemy>& getEnemies() const;
    const CombatDeck& getDeck() const;

    int getEnergy() const;
    int getTurnIndex() const;
    CombatPhase getPhase() const;

private:
Enemy* getAliveEnemyByTargetId(TargetId targetId);
const Enemy* getAliveEnemyByTargetId(TargetId targetId) const;

bool areAllEnemiesDefeated() const;

    void startPlayerTurn();
    void startEnemyTurn();

    ErrorCode canPlayCard(
        int handIndex,
        TargetId targetId
    ) const;

    const CardDef& getCardDefFromHand(
        int handIndex
    ) const;

    void payCardCost(
        const CardDef& card
    );

    void applyCardEffects(
        const CardDef& card,
        TargetId targetId
    );

    void applySingleEffect(
        const CardEffect& effect,
        TargetId targetId
    );

    void movePlayedCardAfterResolve(
        int handIndex,
        const CardDef& card
    );

    void dealDamageToEnemy(TargetId targetId, int amount);
    void gainBlock(int amount);
    void drawCards(int count);
    void gainEnergy(int amount);
    void applyVulnerableToEnemy(int amount);
    void applyWeakToEnemy(int amount);
    void gainStrength(int amount);

    void generateEnemyIntent();
    void executeEnemyIntent();

    void checkBattleResult();

private:
    CombatPhase phase_ = CombatPhase::Finished;

    Player player_;
    std::vector<Enemy> enemies_;
    CombatDeck deck_;

    int energy_ = 0;
    int turnIndex_ = 0;

    BattleResult result_ = BattleResult::Ongoing;
    bool resultCommitted_ = false;

    // CardSystem cardSystem_;
    // EnemyAISystem enemyAI_;

    CardDatabase* cardDatabase_ = nullptr;
    std::mt19937* rng_ = nullptr;

};

#endif //SPIRELIKE_COMBATSYSTEM_HPP