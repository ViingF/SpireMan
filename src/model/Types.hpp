
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
using RelicId = int;
using TargetId = int;
using EnemyId = int;
using EventId = int;
using CardInstanceId = int;
constexpr TargetId NoTarget = -1;

enum class BattleResult {
    Ongoing,
    Victory,
    Defeat
};

enum class SceneType {
    None,
    Menu,
    CharacterSelect,
    Map,
    Combat,
    Reward,
    Event,
    Campfire,
    Shop,
    CardRemove,
    End
};


enum class CardType {
    Attack,
    Skill,
    Curse
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
    RitualDagger,
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

struct CardEffect {
    EffectType type;
    int value;
};

struct RunPlayerState {
    int maxHp;
    int hp;
};

enum class EventEffectType {
    None,
    GainGold,
    LoseGold,
    LoseHp,
    HealHp,
    GainMaxHp,
    RemoveCard,
    AddCard,
    Curse,
    StartCombat
};

enum class MapNodeType {
    Combat,
    Campfire,
    Event,
    Shop
};

enum class MapNodeState {
    Locked,
    Available,
    Completed
};

#endif //SPIRELIKE_TYPES_HPP