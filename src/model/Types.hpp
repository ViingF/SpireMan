
#ifndef SPIRELIKE_TYPES_HPP
#define SPIRELIKE_TYPES_HPP

#include <random>
#include <vector>

enum class ErrorCode {
    OK = 0,

    FILE_NOT_FOUND = 1001,
    RESOURCE_LOAD_FAILED = 1002,
    DATA_FORMAT_ERROR = 1003,
    INVALID_ENUM_VALUE = 1004,
    DUPLICATE_ID = 1005,

    INVALID_CARD_ID = 2001,
    CARD_NOT_IN_HAND = 2002,
    NOT_ENOUGH_ENERGY = 2003,
    INVALID_TARGET = 2004,

    INVALID_SCENE_STATE = 3001,
    NULL_POINTER = 3002,
    UNKNOWN_ERROR = 9999
};

using CardId = int;
using EnemyId = int;
using RelicId = int;
using TargetId = int;
constexpr TargetId NoTarget = -1;

enum class BattleResult {
    Ongoing,
    Victory,
    Defeat
};

enum class SceneType {
    None,
    Menu,
    Map,
    Combat,
    Reward,
    End
};


enum class CardType {
    Attack,
    Skill
};

enum class TargetType {
    Enemy,
    Self,
    None
};

enum class EffectType {
    DealDamage,
    GainBlock,
    DrawCards,
    GainEnergy,
    ApplyVulnerable,
    ApplyWeak,
    GainStrength
};

enum class EnemyIntentType {
    Attack,
    Block,
    Buff
};
enum class CombatPhase {
    PlayerTurn,
    EnemyTurn,
    Finished
};

using CardId = int;
using CardInstanceId = int;
struct CardInstance {
    CardInstanceId instanceId;
    CardId cardId;
};
struct CardEffect {
    EffectType type;
    int value;
};

struct CardDef {
    CardId id;
    std::string name;
    CardType type;
    int cost;
    TargetType target;
    std::vector<CardEffect> effects;
    std::string description;
};
struct Player {
    int maxHp = 70;
    int hp = 70;
    int block = 0;

    int strength = 0;
    int vulnerable = 0;
    int weak = 0;
};

struct EnemyIntent {
    EnemyIntentType type;
    int value;
};

struct Enemy {
    EnemyId id;
    std::string name;

    int maxHp;
    int hp;
    int block;

    int strength = 0;
    int vulnerable = 0;
    int weak = 0;

    EnemyIntent intent;
    std::vector<EnemyIntent> movePattern;
};

struct EnemyDef {
    EnemyId id;
    std::string name;
    int maxHp;
    std::vector<EnemyIntent> movePattern;
};

struct CombatDeck {
    std::vector<CardInstance> drawPile;
    std::vector<CardInstance> hand;
    std::vector<CardInstance> discardPile;
    std::vector<CardInstance> exhaustPile;
};
struct RunPlayerState {
    int maxHp;
    int hp;
};

struct RunState {
    RunPlayerState player {70, 70};
    std::vector<CardInstance> masterDeck;

    int floor = 0;
    int gold = 0;

    std::vector<RelicId> relics;

    std::mt19937 rng;

    EnemyId currentEnemyId = 0;
    CardInstanceId nextCardInstanceId = 1;

    bool active = false;
};

struct SceneTransition {
    SceneType target = SceneType::None;
    BattleResult battleResult = BattleResult::Ongoing;
    EnemyId enemyId = 0;
};

#endif //SPIRELIKE_TYPES_HPP