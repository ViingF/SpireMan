#include "EnemyDatabase.hpp"

#include "database/DataLoader.hpp"

#include <stdexcept>
#include <random>
#include <vector>

#include "model/Enemy.hpp"

static EnemyIntentType parseEnemyIntentType(const std::string& text)
{
    if (text == "Attack")
        return EnemyIntentType::Attack;

    if (text == "Block")
        return EnemyIntentType::Block;

    if (text == "Buff")
        return EnemyIntentType::Buff;

    throw std::runtime_error("Invalid EnemyIntentType");
}

static EnemyIntent parseEnemyIntent(const std::string& text)
{
    size_t pos = text.find(':');

    if (pos == std::string::npos)
    {
        throw std::runtime_error("Invalid enemy move format");
    }

    std::string typeText = text.substr(0, pos);
    std::string valueText = text.substr(pos + 1);

    EnemyIntent intent;
    intent.type = parseEnemyIntentType(typeText);
    intent.value = std::stoi(valueText);

    return intent;
}

static std::vector<EnemyIntent> parseMovePattern(const std::string& text)
{
    std::vector<EnemyIntent> pattern;

    size_t start = 0;

    while (start < text.size())
    {
        size_t end = text.find(';', start);

        std::string item;

        if (end == std::string::npos)
        {
            item = text.substr(start);
            start = text.size();
        }
        else
        {
            item = text.substr(start, end - start);
            start = end + 1;
        }

        if (!item.empty())
        {
            pattern.push_back(parseEnemyIntent(item));
        }
    }

    return pattern;
}

ErrorCode EnemyDatabase::loadFromCsv(const std::string& path)
{
    enemies.clear();

    std::vector<std::vector<std::string>> rows;

    ErrorCode code = DataLoader::readCsv(path, rows);

    if (code != ErrorCode::OK)
    {
        return code;
    }

    if (rows.size() <= 1)
    {
        return ErrorCode::DATA_FORMAT_ERROR;
    }

    for (size_t i = 1; i < rows.size(); ++i)
    {
        const auto& row = rows[i];

        if (row.size() < 4)
        {
            return ErrorCode::DATA_FORMAT_ERROR;
        }

        EnemyDef enemy;

        try
        {
            enemy.id = std::stoi(row[0]);

            if (enemies.contains(enemy.id))
            {
                return ErrorCode::DUPLICATE_ID;
            }

            enemy.name = row[1];
            enemy.maxHp = std::stoi(row[2]);
            enemy.movePattern = parseMovePattern(row[3]);

            if (enemy.movePattern.empty())
            {
                return ErrorCode::DATA_FORMAT_ERROR;
            }

            enemies[enemy.id] = enemy;
        }
        catch (const std::invalid_argument&)
        {
            return ErrorCode::DATA_FORMAT_ERROR;
        }
        catch (const std::out_of_range&)
        {
            return ErrorCode::DATA_FORMAT_ERROR;
        }
        catch (...)
        {
            return ErrorCode::INVALID_ENUM_VALUE;
        }
    }

    return ErrorCode::OK;
}

const EnemyDef& EnemyDatabase::get(EnemyId id) const
{
    auto it = enemies.find(id);

    if (it == enemies.end())
    {
        throw std::runtime_error("Enemy not found");
    }

    return it->second;
}

bool EnemyDatabase::exists(EnemyId id) const
{
    return enemies.contains(id);
}

EnemyId EnemyDatabase::chooseEnemyIdByFloor(
    int floor,
    std::mt19937& rng
) const
{
    if (enemies.empty())
    {
        throw std::runtime_error("EnemyDatabase is empty");
    }

    std::vector<EnemyId> ids;

    for (const auto& pair : enemies)
    {
        ids.push_back(pair.first);
    }

    std::uniform_int_distribution<size_t> dist(
        0,
        ids.size() - 1
    );

    return ids[dist(rng)];
}
