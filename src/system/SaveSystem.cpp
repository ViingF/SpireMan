#include "system/SaveSystem.hpp"

#include <cstdio>
#include <fstream>
#include <iomanip>

#include "core/Logger.hpp"

namespace {

    const char* SAVE_MAGIC_V1 = "SPIRELIKE_SAVE_V1";
    const char* SAVE_MAGIC_V2 = "SPIRELIKE_SAVE_V2";


void writeEventEffects(
    std::ofstream& out,
    const std::vector<EventEffect>& effects
)
{
    out << effects.size() << '\n';

    for (const EventEffect& effect : effects) {
        out << static_cast<int>(effect.type)
            << ' '
            << effect.value
            << '\n';
    }
}

bool readEventEffects(
    std::ifstream& in,
    std::vector<EventEffect>& effects
)
{
    std::size_t count = 0;

    if (!(in >> count)) {
        return false;
    }

    effects.clear();

    for (std::size_t i = 0; i < count; ++i) {
        int type = 0;
        int value = 0;

        if (!(in >> type >> value)) {
            return false;
        }

        EventEffect effect;
        effect.type = static_cast<EventEffectType>(type);
        effect.value = value;

        effects.push_back(effect);
    }

    return true;
}

}

bool SaveSystem::saveRun(
    const RunState& runState,
    const std::string& path
)
{
    LOG_INFO(
        "Saving run: path=" << path
        << ", player=" << runState.playerName
        << ", act=" << runState.act
        << ", floor=" << runState.floor
        << ", hp=" << runState.player.hp
        << "/" << runState.player.maxHp
        << ", gold=" << runState.gold
    );

    std::ofstream out(path);

    if (!out.is_open()) {
        LOG_ERROR("Save failed: cannot open file, path=" << path);
        return false;
    }

    if (!out.is_open()) {
        LOG_ERROR("Save failed: cannot open file, path=" << path);
        return false;
    }

    out << std::setprecision(9);

    out << SAVE_MAGIC_V2 << '\n';

    out << std::quoted(runState.playerName) << '\n';

    out << runState.player.maxHp << ' '
        << runState.player.hp << '\n';


    out << runState.act << ' '
        << runState.floor << ' '
        << runState.floorInAct << ' '
        << runState.gold << '\n';

    out << runState.currentEncounterId << ' '
        << runState.bossEncounterId << ' '
        << runState.nextCardInstanceId << ' '
        << runState.active << '\n';

    out << runState.currentMapNodeIndex << ' '
        << runState.selectedMapNodeIndex << ' '
        << runState.pendingRemoveCardCount << '\n';

    out << runState.rng << '\n';

    out << runState.masterDeck.size() << '\n';
    for (const CardInstance& card : runState.masterDeck) {
        out << card.instanceId << ' '
            << card.cardId << '\n';
    }

    out << runState.relics.size() << '\n';
    for (RelicId relicId : runState.relics) {
        out << relicId << '\n';
    }

    out << runState.mapNodes.size() << '\n';
    for (const MapNode& node : runState.mapNodes) {
        out << node.index << ' '
            << static_cast<int>(node.type) << ' '
            << static_cast<int>(node.state) << ' '
            << node.eventId << ' '
            << node.encounterId << ' '
            << node.layer << ' '
            << node.slot << ' '
            << node.orderInLayer << ' '
            << node.mapX << ' '
            << node.mapY << ' '
            << node.nextIndices.size();

        for (int nextIndex : node.nextIndices) {
            out << ' ' << nextIndex;
        }

        out << '\n';
    }

    writeEventEffects(
        out,
        runState.pendingEventEffects
    );

    out << runState.eventDrawCounts.size() << '\n';
    for (const auto& pair : runState.eventDrawCounts) {
        out << pair.first << ' '
            << pair.second << '\n';
    }

    LOG_INFO("Save success: path=" << path);

    return true;
}

bool SaveSystem::loadRun(
    RunState& runState,
    const std::string& path
)
{
    LOG_INFO("Loading run: path=" << path);

    auto fail = [&](const std::string& reason) {
        LOG_ERROR("Load failed: path=" << path << ", reason=" << reason);
        return false;
    };

    std::ifstream in(path);

    if (!in.is_open()) {
        return fail("cannot open save file");
    }

    std::string magic;

    if (!(in >> magic)) {
        return fail("cannot read save magic");
    }

    if (
        magic != SAVE_MAGIC_V1 &&
        magic != SAVE_MAGIC_V2
    ) {
        return fail("invalid save magic: " + magic);
    }

    RunState loaded;

    if (magic == SAVE_MAGIC_V2) {
        if (!(in >> std::quoted(loaded.playerName))) {
            return false;
        }
    } else {
        loaded.playerName = "Player";
    }

    if (!(in >> loaded.player.maxHp >> loaded.player.hp)) {
        return false;
    }


    if (!(in >> loaded.act
             >> loaded.floor
             >> loaded.floorInAct
             >> loaded.gold)) {
        return false;
    }

    if (!(in >> loaded.currentEncounterId
             >> loaded.bossEncounterId
             >> loaded.nextCardInstanceId
             >> loaded.active)) {
        return false;
    }

    if (!(in >> loaded.currentMapNodeIndex
             >> loaded.selectedMapNodeIndex
             >> loaded.pendingRemoveCardCount)) {
        return false;
    }

    if (!(in >> loaded.rng)) {
        return false;
    }

    std::size_t deckSize = 0;

    if (!(in >> deckSize)) {
        return false;
    }

    loaded.masterDeck.clear();

    for (std::size_t i = 0; i < deckSize; ++i) {
        CardInstance card;

        if (!(in >> card.instanceId >> card.cardId)) {
            return false;
        }

        loaded.masterDeck.push_back(card);
    }

    std::size_t relicCount = 0;

    if (!(in >> relicCount)) {
        return false;
    }

    loaded.relics.clear();

    for (std::size_t i = 0; i < relicCount; ++i) {
        RelicId relicId = 0;

        if (!(in >> relicId)) {
            return false;
        }

        loaded.relics.push_back(relicId);
    }

    std::size_t nodeCount = 0;

    if (!(in >> nodeCount)) {
        return false;
    }

    loaded.mapNodes.clear();

    for (std::size_t i = 0; i < nodeCount; ++i) {
        MapNode node;
        int type = 0;
        int state = 0;
        std::size_t nextCount = 0;

        if (!(in >> node.index
                 >> type
                 >> state
                 >> node.eventId
                 >> node.encounterId
                 >> node.layer
                 >> node.slot
                 >> node.orderInLayer
                 >> node.mapX
                 >> node.mapY
                 >> nextCount)) {
            return false;
        }

        node.type = static_cast<MapNodeType>(type);
        node.state = static_cast<MapNodeState>(state);

        node.nextIndices.clear();

        for (std::size_t j = 0; j < nextCount; ++j) {
            int nextIndex = -1;

            if (!(in >> nextIndex)) {
                return false;
            }

            node.nextIndices.push_back(nextIndex);
        }

        loaded.mapNodes.push_back(node);
    }

    if (!readEventEffects(
            in,
            loaded.pendingEventEffects
        )) {
        return false;
    }

    std::size_t eventDrawCountSize = 0;

    if (!(in >> eventDrawCountSize)) {
        return false;
    }

    loaded.eventDrawCounts.clear();

    for (std::size_t i = 0; i < eventDrawCountSize; ++i) {
        EventId eventId = 0;
        int count = 0;

        if (!(in >> eventId >> count)) {
            return false;
        }

        loaded.eventDrawCounts[eventId] = count;
    }

    loaded.active = true;

    runState = loaded;

    LOG_INFO(
        "Load success: path=" << path
        << ", player=" << runState.playerName
        << ", act=" << runState.act
        << ", floor=" << runState.floor
        << ", hp=" << runState.player.hp
        << "/" << runState.player.maxHp
        << ", gold=" << runState.gold
    );

    return true;
}

bool SaveSystem::hasSave(
    const std::string& path
)
{
    std::ifstream in(path);
    return in.good();
}

bool SaveSystem::deleteSave(
    const std::string& path
)
{
    const bool ok = std::remove(path.c_str()) == 0;

    if (ok) {
        LOG_INFO("Save deleted: path=" << path);
    } else {
        LOG_WARN("Delete save failed or save not found: path=" << path);
    }

    return ok;
}

