
#include "EventSystem.hpp"

#include <algorithm>
#include <sstream>

EventResolveResult EventSystem::resolveChoice(
    RunState& runState,
    const EventDef& eventDef,
    int choiceIndex
) const
{
    EventResolveResult result;

    if (choiceIndex < 0 ||
        choiceIndex >= static_cast<int>(eventDef.choices.size())) {
        result.error = ErrorCode::INVALID_SCENE_STATE;
        result.message = "Invalid event choice.";
        return result;
    }

    const EventChoiceDef& choice = eventDef.choices[choiceIndex];
    std::ostringstream message;

    for (const EventEffect& effect : choice.effects) {
        switch (effect.type) {
        case EventEffectType::None:
            message << "Nothing happened.\n";
            break;

        case EventEffectType::GainGold:
            runState.gold += effect.value;
            message << "Gained " << effect.value << " gold.\n";
            break;

        case EventEffectType::LoseHp: {
            const int before = runState.player.hp;
            runState.player.hp = std::max(0, runState.player.hp - effect.value);
            const int lost = before - runState.player.hp;
            message << "Lost " << lost << " HP.\n";
            break;
        }

        case EventEffectType::HealHp: {
            const int before = runState.player.hp;
            runState.player.hp = std::min(
                runState.player.maxHp,
                runState.player.hp + effect.value
            );
            const int healed = runState.player.hp - before;
            message << "Healed " << healed << " HP.\n";
            break;
        }

        case EventEffectType::GainMaxHp:
            runState.player.maxHp += effect.value;
            runState.player.hp += effect.value;
            message << "Gained " << effect.value << " Max HP.\n";
            break;

        case EventEffectType::StartCombat:
            result.startCombat = true;
            message << "A combat begins.\n";
            break;
        }
    }

    result.playerDead = runState.player.hp <= 0;
    result.message = message.str();

    return result;
}