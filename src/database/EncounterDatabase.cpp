#include "EncounterDatabase.hpp"

#include "database/DataLoader.hpp"

#include <algorithm>
#include <cctype>
#include <random>
#include <stdexcept>
#include <string>
#include <vector>

#include "config/Constants.hpp"

namespace {

std::string trim(const std::string& text)
{
    std::size_t start = 0;

    while (
        start < text.size() &&
        std::isspace(static_cast<unsigned char>(text[start]))
    ) {
        ++start;
    }

    std::size_t end = text.size();

    while (
        end > start &&
        std::isspace(static_cast<unsigned char>(text[end - 1]))
    ) {
        --end;
    }

    return text.substr(start, end - start);
}

bool parseBossFlag(const std::string& text)
{
    return text == "1" ||
           text == "true" ||
           text == "True" ||
           text == "TRUE" ||
           text == "Boss" ||
           text == "boss";
}

std::vector<EncounterEnemySlot> parseEnemySlots(
    const std::string& text
)
{
    std::vector<EncounterEnemySlot> slots;

    std::size_t start = 0;

    while (start < text.size()) {
        const std::size_t end = text.find('|', start);

        std::string item;

        if (end == std::string::npos) {
            item = text.substr(start);
            start = text.size();
        }
        else {
            item = text.substr(start, end - start);
            start = end + 1;
        }

        item = trim(item);

        if (!item.empty()) {
            EncounterEnemySlot slot;
            slot.enemyId = std::stoi(item);

            if (slot.enemyId <= 0) {
                throw std::runtime_error("Invalid enemy id in encounter");
            }

            slots.push_back(slot);
        }
    }

    return slots;
}

}

ErrorCode EncounterDatabase::loadFromCsv(
    const std::string& path
)
{
    encounters_.clear();

    std::vector<std::vector<std::string>> rows;

    ErrorCode code = DataLoader::readCsv(path, rows);

    if (code != ErrorCode::OK) {
        return code;
    }

    if (rows.size() <= 1) {
        return ErrorCode::DATA_FORMAT_ERROR;
    }

    for (std::size_t i = 1; i < rows.size(); ++i) {
        const auto& row = rows[i];

        if (row.size() < 5) {
            return ErrorCode::DATA_FORMAT_ERROR;
        }

        EncounterDef encounter;

        try {
            encounter.id = std::stoi(row[0]);

            if (encounter.id <= 0) {
                return ErrorCode::DATA_FORMAT_ERROR;
            }

            if (encounters_.contains(encounter.id)) {
                return ErrorCode::DUPLICATE_ID;
            }

            encounter.name = row[1];
            encounter.enemies = parseEnemySlots(row[2]);
            encounter.isBoss = parseBossFlag(row[3]);
            encounter.act = std::stoi(row[4]);

            if (
                encounter.name.empty() ||
                encounter.enemies.empty() ||
                encounter.act <= 0 ||
                encounter.act > MAX_ACT
            ) {
                return ErrorCode::DATA_FORMAT_ERROR;
            }
            encounters_[encounter.id] = encounter;

        }
        catch (const std::invalid_argument&) {
            return ErrorCode::DATA_FORMAT_ERROR;
        }
        catch (const std::out_of_range&) {
            return ErrorCode::DATA_FORMAT_ERROR;
        }
        catch (...) {
            return ErrorCode::DATA_FORMAT_ERROR;
        }
    }


    return ErrorCode::OK;
}

bool EncounterDatabase::exists(
    EncounterId id
) const
{
    return encounters_.contains(id);
}

const EncounterDef& EncounterDatabase::get(
    EncounterId id
) const
{
    auto it = encounters_.find(id);

    if (it == encounters_.end()) {
        throw std::runtime_error("Encounter not found");
    }

    return it->second;
}

EncounterId EncounterDatabase::chooseEncounterIdByAct(
    int act,
    std::mt19937& rng
) const
{
    if (encounters_.empty()) {
        throw std::runtime_error("EncounterDatabase is empty");
    }

    const int safeAct = std::clamp(act, 1, MAX_ACT);

    std::vector<EncounterId> ids;

    for (const auto& pair : encounters_) {
        const EncounterDef& encounter = pair.second;

        if (
            !encounter.isBoss &&
            encounter.act == safeAct
        ) {
            ids.push_back(pair.first);
        }
    }

    if (ids.empty()) {
        for (const auto& pair : encounters_) {
            const EncounterDef& encounter = pair.second;

            if (!encounter.isBoss) {
                ids.push_back(pair.first);
            }
        }
    }

    if (ids.empty()) {
        throw std::runtime_error("No normal encounter found");
    }

    std::uniform_int_distribution<std::size_t> dist(
        0,
        ids.size() - 1
    );

    return ids[dist(rng)];
}


std::vector<EncounterId> EncounterDatabase::getAllBossEncounterIds() const
{
    std::vector<EncounterId> ids;

    for (const auto& pair : encounters_) {
        const EncounterDef& encounter = pair.second;

        if (encounter.isBoss) {
            ids.push_back(pair.first);
        }
    }

    std::sort(ids.begin(), ids.end());

    return ids;
}

EncounterId EncounterDatabase::chooseRandomBossEncounterId(
    int act,
    std::mt19937& rng
) const
{
    const int safeAct = std::clamp(act, 1, 3);

    std::vector<EncounterId> ids;

    for (const auto& pair : encounters_) {
        const EncounterDef& encounter = pair.second;

        if (
            encounter.isBoss &&
            encounter.act == safeAct
        ) {
            ids.push_back(pair.first);
        }
    }

    if (ids.empty()) {
        return 0;
    }

    std::sort(ids.begin(), ids.end());

    std::uniform_int_distribution<std::size_t> dist(
        0,
        ids.size() - 1
    );

    return ids[dist(rng)];
}

