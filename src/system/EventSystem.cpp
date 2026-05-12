#include "EventSystem.hpp"

#include "database/CardDatabase.hpp"

#include <algorithm>
#include <cstddef>
#include <sstream>

namespace {

constexpr CardId DEFAULT_CURSE_CARD_ID = 999;

int normalizePositiveValue(int value)
{
    return std::max(0, value);
}

int normalizeRemoveCardCount(int value)
{
    if (value <= 0) {
        return 1;
    }

    return value;
}

bool addCardToDeck(
    RunState& runState,
    CardId cardId,
    const CardDatabase& cardDatabase
)
{
    if (cardId <= 0 || !cardDatabase.exists(cardId)) {
        return false;
    }

    CardInstance card;
    card.instanceId = runState.nextCardInstanceId++;
    card.cardId = cardId;

    runState.masterDeck.push_back(card);


    return true;
}


}

EventResolveResult EventSystem::resolveChoice(
    RunState& runState,
    const EventDef& eventDef,
    int choiceIndex,
    const CardDatabase& cardDatabase
) const
{
    EventResolveResult result;

    runState.pendingRemoveCardCount = 0;
    runState.pendingEventEffects.clear();

    if (
        choiceIndex < 0 ||
        choiceIndex >= static_cast<int>(eventDef.choices.size())
    ) {
        result.error = ErrorCode::INVALID_SCENE_STATE;
        result.message = "Invalid event choice.";
        return result;
    }

    const EventChoiceDef& choice = eventDef.choices[choiceIndex];

    return resolveEffects(
        runState,
        choice.effects,
        cardDatabase
    );
}

EventResolveResult EventSystem::resolvePendingEventEffects(
    RunState& runState,
    const CardDatabase& cardDatabase
) const
{
    EventResolveResult result;

    if (runState.pendingRemoveCardCount > 0) {
        result.error = ErrorCode::INVALID_SCENE_STATE;
        result.message = "Card removal is still pending.";
        return result;
    }

    std::vector<EventEffect> effects = runState.pendingEventEffects;
    runState.pendingEventEffects.clear();

    return resolveEffects(
        runState,
        effects,
        cardDatabase
    );
}

ErrorCode EventSystem::removeCardByInstanceId(
    RunState& runState,
    CardInstanceId instanceId
) const
{
    if (runState.pendingRemoveCardCount <= 0) {
        return ErrorCode::INVALID_SCENE_STATE;
    }

    auto it = std::find_if(
        runState.masterDeck.begin(),
        runState.masterDeck.end(),
        [instanceId](const CardInstance& card) {
            return card.instanceId == instanceId;
        }
    );

    if (it == runState.masterDeck.end()) {
        return ErrorCode::INVALID_CARD_ID;
    }

    runState.masterDeck.erase(it);
    runState.pendingRemoveCardCount -= 1;

    if (runState.pendingRemoveCardCount < 0) {
        runState.pendingRemoveCardCount = 0;
    }

    return ErrorCode::OK;
}

EventResolveResult EventSystem::resolveEffects(
    RunState& runState,
    const std::vector<EventEffect>& effects,
    const CardDatabase& cardDatabase
) const
{
    EventResolveResult result;
    std::ostringstream message;

    for (std::size_t i = 0; i < effects.size(); ++i) {
        const EventEffect& effect = effects[i];

        switch (effect.type) {
        case EventEffectType::None:
            message << "Nothing happened.\n";
            break;

        case EventEffectType::GainGold: {
            const int value = normalizePositiveValue(effect.value);
            runState.gold += value;
            message << "Gained " << value << " gold.\n";
            break;
        }

        case EventEffectType::LoseGold: {
            const int value = normalizePositiveValue(effect.value);
            const int before = runState.gold;

            runState.gold = std::max(0, runState.gold - value);

            message << "Lost " << before - runState.gold << " gold.\n";
            break;
        }

        case EventEffectType::LoseHp: {
            const int value = normalizePositiveValue(effect.value);
            const int before = runState.player.hp;

            runState.player.hp = std::max(
                0,
                runState.player.hp - value
            );

            message << "Lost " << before - runState.player.hp << " HP.\n";
            break;
        }

        case EventEffectType::HealHp: {
            const int value = normalizePositiveValue(effect.value);
            const int before = runState.player.hp;

            runState.player.hp = std::min(
                runState.player.maxHp,
                runState.player.hp + value
            );

            message << "Healed " << runState.player.hp - before << " HP.\n";
            break;
        }

        case EventEffectType::GainMaxHp: {
            const int value = normalizePositiveValue(effect.value);

            runState.player.maxHp += value;
            runState.player.hp += value;

            message << "Gained " << value << " Max HP.\n";
            break;
        }

        case EventEffectType::RemoveCard: {
            result.playerDead = runState.player.hp <= 0;

            if (result.playerDead) {
                result.message = message.str();
                return result;
            }

            if (runState.masterDeck.empty()) {
                message << "There are no cards to remove.\n";
                break;
            }

            const int requestedCount =
                normalizeRemoveCardCount(effect.value);

            const int removableCount = std::min(
                requestedCount,
                static_cast<int>(runState.masterDeck.size())
            );

            runState.pendingRemoveCardCount = removableCount;

            runState.pendingEventEffects.assign(
                effects.begin() + static_cast<std::ptrdiff_t>(i + 1),
                effects.end()
            );

            result.requiresCardRemove = true;

            message << "Choose "
                    << removableCount
                    << " card";

            if (removableCount > 1) {
                message << "s";
            }

            message << " to remove.\n";

            result.message = message.str();
            return result;
        }

        case EventEffectType::AddCard: {
            CardId cardId = 0;

            if (effect.value > 0) {
                cardId = static_cast<CardId>(effect.value);
            } else {
                cardId = cardDatabase.chooseRandomRewardCardId(runState.rng);
            }

            if (!addCardToDeck(runState, cardId, cardDatabase)) {
                result.error = ErrorCode::INVALID_CARD_ID;
                result.message = "Invalid card id in AddCard effect.";
                return result;
            }

            message << "Added a card to your deck.\n";
            break;
        }

            case EventEffectType::Curse: {
            CardId curseCardId = DEFAULT_CURSE_CARD_ID;

            if (effect.value > 0) {
                curseCardId = static_cast<CardId>(effect.value);
            }

            if (!cardDatabase.exists(curseCardId)) {
                result.error = ErrorCode::INVALID_CARD_ID;
                result.message = "Invalid curse card id.";
                return result;
            }

            const CardDef& curseCard = cardDatabase.get(curseCardId);

            if (curseCard.type != CardType::Curse) {
                result.error = ErrorCode::INVALID_CARD_ID;
                result.message = "Curse effect must add a Curse type card.";
                return result;
            }

            CardInstance card;
            card.instanceId = runState.nextCardInstanceId++;
            card.cardId = curseCardId;

            runState.masterDeck.push_back(card);

            message << "Received a curse.\n";
            break;
            }


        case EventEffectType::StartCombat:
            result.startCombat = true;
            message << "A combat begins.\n";
            break;
        }

        if (runState.player.hp <= 0) {
            result.playerDead = true;
            result.message = message.str();
            return result;
        }
    }

    result.playerDead = runState.player.hp <= 0;
    result.message = message.str();

    return result;
}
