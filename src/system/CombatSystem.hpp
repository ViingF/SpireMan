#pragma once
#include "CardSystem.hpp"
#include "EnemyAISystem.hpp"
#include <algorithm>
#include <vector>
#include <random>

#include "database/CardDatabase.hpp"
#include "database/EnemyDatabase.hpp"
#include "model/CardDef.hpp"
#include "model/CombatDeck.hpp"
#include "model/EncounterDef.hpp"
#include "model/RunState.hpp"

class CombatSystem {
public:
    bool hasCommittedResult() const;

    ErrorCode startCombat(RunState& runState, const EncounterDef& encounterDef,
        const EnemyDatabase& enemyDatabase, CardDatabase& cardDatabase);

    ErrorCode playCard(int handIndex, TargetId targetId);
    ErrorCode endPlayerTurn();

    BattleResult getBattleResult() const;
    ErrorCode commitResultToRunState(RunState& runState);

    // 只读 getter
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

    ErrorCode canPlayCard(int handIndex, TargetId targetId) const;
    const CardDef& getCardDefFromHand(int handIndex) const;
    void payCardCost(const CardDef& card);
    void applyCardEffects(const CardDef& card, TargetId targetId);
    void movePlayedCardAfterResolve(const CardInstance& playedCard, const CardDef& card);
    void generateEnemyIntents();
    void executeEnemyIntents();
    void checkBattleResult();

    // 单次效果应用（供 applyCardEffects 调用）
    void dealDamageToEnemy(TargetId targetId, int amount);
    void gainBlock(int amount);
    void drawCards(int count);
    void gainEnergy(int amount);
    void applyVulnerableToEnemy(TargetId targetId, int amount);
    void applyWeakToEnemy(TargetId targetId, int amount);
    void gainStrength(int amount);

    static void decreaseTimedStatus(int& value);
    void tickPlayerEndTurnStatuses();
    void tickEnemiesEndTurnStatuses();


    // 成员变量
    CombatPhase phase_ = CombatPhase::Finished;
    Player player_;
    std::vector<Enemy> enemies_;
    CombatDeck deck_;
    int energy_ = 0;
    int turnIndex_ = 0;
    BattleResult result_ = BattleResult::Ongoing;
    bool resultCommitted_ = false;

    CardSystem cardSystem_;
    EnemyAISystem enemyAI_;
    CardDatabase* cardDatabase_ = nullptr;
    std::mt19937* rng_ = nullptr;
};
