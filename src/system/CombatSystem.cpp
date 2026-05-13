
#include "CombatSystem.hpp"


bool CombatSystem::hasCommittedResult() const {
    return false;
}

ErrorCode CombatSystem::startCombat(
    RunState& runState,
    const EncounterDef& encounterDef,
    const EnemyDatabase& enemyDatabase,
    CardDatabase& cardDatabase
) {
    return ErrorCode::OK;

}

ErrorCode CombatSystem::playCard(int handIndex, TargetId targetId) {
    return ErrorCode();
}

ErrorCode CombatSystem::endPlayerTurn() {
    return ErrorCode();
}

BattleResult CombatSystem::getBattleResult() const {
    return BattleResult::Ongoing;
}

ErrorCode CombatSystem::commitResultToRunState(RunState& runState) {
    return ErrorCode();
}

const Player& CombatSystem::getPlayer() const {
    static Player p;
    return p;
}

const std::vector<Enemy>& CombatSystem::getEnemies() const {
    static std::vector<Enemy> e;
    return e;
}

const CombatDeck& CombatSystem::getDeck() const {
    static CombatDeck d;
    return d;
}

int CombatSystem::getEnergy() const {
    return 0;
}

int CombatSystem::getTurnIndex() const {
    return 0;
}

CombatPhase CombatSystem::getPhase() const {
    return CombatPhase::Finished;
}

Enemy* CombatSystem::getAliveEnemyByTargetId(TargetId targetId) {
    return nullptr;
}

const Enemy* CombatSystem::getAliveEnemyByTargetId(TargetId targetId) const {
    return nullptr;
}

bool CombatSystem::areAllEnemiesDefeated() const {
    return false;
}

void CombatSystem::startPlayerTurn() {
}

void CombatSystem::startEnemyTurn() {
}

ErrorCode CombatSystem::canPlayCard(int handIndex, TargetId targetId) const {
    return ErrorCode();
}

const CardDef& CombatSystem::getCardDefFromHand(int handIndex) const {
    static CardDef c;
    return c;
}

void CombatSystem::payCardCost(const CardDef& card) {
}

void CombatSystem::applyCardEffects(const CardDef& card, TargetId targetId) {
}

void CombatSystem::applySingleEffect(const CardEffect& effect, TargetId targetId) {
}

void CombatSystem::movePlayedCardAfterResolve(int handIndex, const CardDef& card) {
}

void CombatSystem::dealDamageToEnemy(TargetId targetId, int amount) {
}

void CombatSystem::gainBlock(int amount) {
}

void CombatSystem::drawCards(int count) {
}

void CombatSystem::gainEnergy(int amount) {
}

void CombatSystem::applyVulnerableToEnemy(int amount) {
}

void CombatSystem::applyWeakToEnemy(int amount) {
}

void CombatSystem::gainStrength(int amount) {
}

void CombatSystem::generateEnemyIntent() {
}

void CombatSystem::executeEnemyIntent() {
}

void CombatSystem::checkBattleResult() {
}