#include "EnemyDatabase.hpp"

#include "database/DataLoader.hpp"

#include <algorithm>
#include <random>
#include <stdexcept>
#include <string>
#include <vector>

#include "model/Enemy.hpp"

static EnemyIntentType parseEnemyIntentType(const std::string& text)
{
    if (text == "Attack") {
        return EnemyIntentType::Attack;
    }

    if (text == "Block") {
        return EnemyIntentType::Block;
    }

    if (text == "Buff") {
        return EnemyIntentType::Buff;
    }

    throw std::runtime_error("Invalid EnemyIntentType");
}

static EnemyIntent parseEnemyIntent(const std::string& text)
{
    const std::size_t pos = text.find(':');

    if (pos == std::string::npos) {
        throw std::runtime_error("Invalid enemy move format");
    }

    const std::string typeText = text.substr(0, pos);
    const std::string valueText = text.substr(pos + 1);

    EnemyIntent intent;
    intent.type = parseEnemyIntentType(typeText);
    intent.value = std::stoi(valueText);

    return intent;
}

static std::vector<EnemyIntent> parseMovePattern(const std::string& text)
{
    std::vector<EnemyIntent> pattern;

    std::size_t start = 0;

    while (start < text.size()) {
        const std::size_t end = text.find(';', start);

        std::string item;

        if (end == std::string::npos) {
            item = text.substr(start);
            start = text.size();
        } else {
            item = text.substr(start, end - start);
            start = end + 1;
        }

        if (!item.empty()) {
            pattern.push_back(parseEnemyIntent(item));
        }
    }

    return pattern;
}

static bool parseBossFlag(const std::string& text)
{
    return text == "1" ||
           text == "true" ||
           text == "True" ||
           text == "TRUE" ||
           text == "Boss" ||
           text == "boss";
}

ErrorCode EnemyDatabase::loadFromCsv(const std::string& path)
{
    enemies.clear();

    std::vector<std::vector<std::string>> rows;

    ErrorCode code = DataLoader::readCsv(path, rows);

    if (code != ErrorCode::OK) {
        return code;
    }

    // 至少需要表头 + 一行数据
    if (rows.size() <= 1) {
        return ErrorCode::DATA_FORMAT_ERROR;
    }

    constexpr int kMaxAct = 3;

    for (std::size_t i = 1; i < rows.size(); ++i) {
        const auto& row = rows[i];

        // 新表结构要求 8 列：
        // id,name,maxHp,textureId,movePattern,isBoss,mapTextureId,act
        if (row.size() < 8) {
            return ErrorCode::DATA_FORMAT_ERROR;
        }

        EnemyDef enemy;

        try {
            enemy.id = std::stoi(row[0]);

            if (enemies.contains(enemy.id)) {
                return ErrorCode::DUPLICATE_ID;
            }

            enemy.name = row[1];
            enemy.maxHp = std::stoi(row[2]);
            enemy.textureId = row[3];
            enemy.movePattern = parseMovePattern(row[4]);
            enemy.isBoss = parseBossFlag(row[5]);
            enemy.mapTextureId = row[6];
            enemy.act = std::stoi(row[7]);

            // Boss 在地图上需要图标。
            // 如果 CSV 没填 mapTextureId，就默认使用战斗贴图 textureId。
            if (enemy.isBoss && enemy.mapTextureId.empty()) {
                enemy.mapTextureId = enemy.textureId;
            }

            if (
                enemy.id <= 0 ||
                enemy.name.empty() ||
                enemy.maxHp <= 0 ||
                enemy.textureId.empty() ||
                enemy.movePattern.empty() ||
                enemy.act <= 0 ||
                enemy.act > kMaxAct
            ) {
                return ErrorCode::DATA_FORMAT_ERROR;
            }

            enemies[enemy.id] = enemy;
        }
        catch (const std::invalid_argument&) {
            return ErrorCode::DATA_FORMAT_ERROR;
        }
        catch (const std::out_of_range&) {
            return ErrorCode::DATA_FORMAT_ERROR;
        }
        catch (...) {
            return ErrorCode::INVALID_ENUM_VALUE;
        }
    }

    return ErrorCode::OK;
}


const EnemyDef& EnemyDatabase::get(EnemyId id) const
{
    auto it = enemies.find(id);

    if (it == enemies.end()) {
        throw std::runtime_error("Enemy not found");
    }

    return it->second;
}

bool EnemyDatabase::exists(EnemyId id) const
{
    return enemies.contains(id);
}

EnemyId EnemyDatabase::chooseEnemyIdByFloor(
    int,
    std::mt19937& rng
) const
{
    if (enemies.empty()) {
        throw std::runtime_error("EnemyDatabase is empty");
    }

    std::vector<EnemyId> ids;

    for (const auto& pair : enemies) {
        const EnemyDef& enemy = pair.second;

        if (!enemy.isBoss) {
            ids.push_back(pair.first);
        }
    }

    if (ids.empty()) {
        throw std::runtime_error("No normal enemy found");
    }

    std::uniform_int_distribution<std::size_t> dist(
        0,
        ids.size() - 1
    );

    return ids[dist(rng)];
}

std::vector<EnemyId> EnemyDatabase::getAllBossIds() const
{
    std::vector<EnemyId> ids;

    for (const auto& pair : enemies) {
        const EnemyDef& enemy = pair.second;

        if (enemy.isBoss) {
            ids.push_back(pair.first);
        }
    }

    std::sort(ids.begin(), ids.end());

    return ids;
}

EnemyId EnemyDatabase::chooseRandomBossId(std::mt19937& rng) const
{
    std::vector<EnemyId> ids = getAllBossIds();

    if (ids.empty()) {
        return 0;
    }

    std::uniform_int_distribution<std::size_t> dist(
        0,
        ids.size() - 1
    );

    return ids[dist(rng)];
}

EnemyId EnemyDatabase::chooseEnemyIdByAct(
    int act,
    std::mt19937& rng
) const
{
    std::vector<EnemyId> ids;

    for (const auto& pair : enemies) {
        const EnemyDef& enemy = pair.second;

        if (!enemy.isBoss && enemy.act == act) {
            ids.push_back(pair.first);
        }
    }

    if (ids.empty()) {
        for (const auto& pair : enemies) {
            const EnemyDef& enemy = pair.second;

            if (!enemy.isBoss) {
                ids.push_back(pair.first);
            }
        }
    }

    if (ids.empty()) {
        throw std::runtime_error("No normal enemy found");
    }

    std::uniform_int_distribution<std::size_t> dist(
        0,
        ids.size() - 1
    );

    return ids[dist(rng)];
}

EnemyId EnemyDatabase::chooseRandomBossIdByAct(
    int act,
    std::mt19937& rng
) const
{
    std::vector<EnemyId> ids;

    for (const auto& pair : enemies) {
        const EnemyDef& enemy = pair.second;

        if (enemy.isBoss && enemy.act == act) {
            ids.push_back(pair.first);
        }
    }

    if (ids.empty()) {
        return chooseRandomBossId(rng);
    }

    std::uniform_int_distribution<std::size_t> dist(
        0,
        ids.size() - 1
    );

    return ids[dist(rng)];
}
