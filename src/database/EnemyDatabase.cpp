#include "EnemyDatabase.hpp"

#include "database/DataLoader.hpp"

#include <algorithm>
#include <random>
#include <stdexcept>
#include <string>
#include <vector>

#include "core/Logger.hpp"
#include "model/Enemy.hpp"

static std::string trimLocal(const std::string& text)
{
    std::size_t begin = 0;

    while (
        begin < text.size() &&
        std::isspace(static_cast<unsigned char>(text[begin]))
    ) {
        ++begin;
    }

    std::size_t end = text.size();

    while (
        end > begin &&
        std::isspace(static_cast<unsigned char>(text[end - 1]))
    ) {
        --end;
    }

    return text.substr(begin, end - begin);
}

static bool startsWith(
    const std::string& text,
    const std::string& prefix
)
{
    return text.rfind(prefix, 0) == 0;
}

static bool isAllDigits(const std::string& text)
{
    if (text.empty()) {
        return false;
    }

    for (char c : text) {
        if (!std::isdigit(static_cast<unsigned char>(c))) {
            return false;
        }
    }

    return true;
}

static int toIntOrDefault(
    const std::string& text,
    int defaultValue = 0
)
{
    if (!isAllDigits(text)) {
        return defaultValue;
    }

    try {
        return std::stoi(text);
    }
    catch (...) {
        return defaultValue;
    }
}

static int firstNumberOrDefault(
    const std::string& text,
    int defaultValue = 0
)
{
    std::size_t begin = std::string::npos;

    for (std::size_t i = 0; i < text.size(); ++i) {
        if (std::isdigit(static_cast<unsigned char>(text[i]))) {
            begin = i;
            break;
        }
    }

    if (begin == std::string::npos) {
        return defaultValue;
    }

    std::size_t end = begin;

    while (
        end < text.size() &&
        std::isdigit(static_cast<unsigned char>(text[end]))
    ) {
        ++end;
    }

    return toIntOrDefault(
        text.substr(begin, end - begin),
        defaultValue
    );
}

static std::vector<std::string> splitLocal(
    const std::string& text,
    char delimiter
)
{
    std::vector<std::string> result;

    std::size_t start = 0;

    while (start <= text.size()) {
        const std::size_t end = text.find(delimiter, start);

        if (end == std::string::npos) {
            result.push_back(trimLocal(text.substr(start)));
            break;
        }

        result.push_back(
            trimLocal(text.substr(start, end - start))
        );

        start = end + 1;
    }

    return result;
}

static EnemyIntent makeIntent(
    EnemyIntentType type,
    int value
)
{
    EnemyIntent intent;
    intent.type = type;
    intent.value = value;
    return intent;
}
static int parseAttackValue(const std::string& text)
{
    std::string rest;

    if (startsWith(text, "Attack")) {
        rest = text.substr(std::string("Attack").size());
    }
    else if (startsWith(text, "HeavyScalingAttack")) {
        rest = text.substr(std::string("HeavyScalingAttack").size());
    }
    else if (startsWith(text, "ScalingAttack")) {
        rest = text.substr(std::string("ScalingAttack").size());
    }
    else {
        return firstNumberOrDefault(text, 0);
    }

    if (rest.empty()) {
        return 0;
    }

    const std::size_t toPos = rest.find("To");

    if (toPos != std::string::npos) {
        int left = toIntOrDefault(rest.substr(0, toPos), 0);
        int right = toIntOrDefault(rest.substr(toPos + 2), left);

        return (left + right) / 2;
    }

    const std::size_t xPos = rest.find('x');

    if (xPos != std::string::npos) {
        int damage = toIntOrDefault(rest.substr(0, xPos), 0);
        int hits = toIntOrDefault(rest.substr(xPos + 1), 1);

        if (damage > 0) {
            return damage * hits;
        }

        return firstNumberOrDefault(rest.substr(xPos + 1), 0);
    }

    return toIntOrDefault(rest, firstNumberOrDefault(rest, 0));
}
static EnemyIntent parseEnemyActionToken(
    const std::string& token,
    const std::string& moveName
)
{
    const std::string text = trimLocal(token);
    const std::string name = trimLocal(moveName);

    if (text.empty()) {
        return makeIntent(EnemyIntentType::Buff, 0);
    }

    // 兼容 Attack:18 这种格式。
    // 拆分后 moveName 是 Attack，token 是 18。
    if (isAllDigits(text)) {
        if (name == "Attack") {
            return makeIntent(
                EnemyIntentType::Attack,
                toIntOrDefault(text, 0)
            );
        }

        return makeIntent(EnemyIntentType::Buff, 0);
    }

    if (
        startsWith(text, "Attack") ||
        startsWith(text, "ScalingAttack") ||
        startsWith(text, "HeavyScalingAttack")
    ) {
        return makeIntent(
            EnemyIntentType::Attack,
            parseAttackValue(text)
        );
    }

    if (startsWith(text, "Block")) {
        return makeIntent(
            EnemyIntentType::Block,
            firstNumberOrDefault(text, 0)
        );
    }

    if (
        startsWith(text, "AllyBlock") ||
        startsWith(text, "AlliesBlock") ||
        startsWith(text, "TeamBlock")
    ) {
        return makeIntent(
            EnemyIntentType::Block,
            firstNumberOrDefault(text, 0)
        );
    }

    if (
        startsWith(text, "GainStrength") ||
        startsWith(text, "AllyStrength") ||
        startsWith(text, "AlliesStrength") ||
        startsWith(text, "TeamStrength") ||
        startsWith(text, "GainRitual") ||
        startsWith(text, "GainMetallicize") ||
        startsWith(text, "GainThorns") ||
        startsWith(text, "GainSharpHide") ||
        startsWith(text, "Artifact")
    ) {
        return makeIntent(
            EnemyIntentType::Buff,
            firstNumberOrDefault(text, 0)
        );
    }

    // ApplyWeak、ApplyFrail、AddDazed、Summon、Split、Escape 等
    // 当前结构无法表达，统一作为 Buff(0) 占位。
    return makeIntent(EnemyIntentType::Buff, 0);
}
static EnemyIntent parseEnemyIntent(const std::string& text)
{
    const std::string item = trimLocal(text);
    const std::size_t pos = item.find(':');

    if (pos == std::string::npos) {
        return parseEnemyActionToken(item, item);
    }

    const std::string moveName = trimLocal(item.substr(0, pos));
    const std::string actionText = trimLocal(item.substr(pos + 1));

    EnemyIntent best = makeIntent(EnemyIntentType::Buff, 0);

    for (const std::string& part : splitLocal(actionText, '+')) {
        EnemyIntent current = parseEnemyActionToken(part, moveName);

        if (current.type == EnemyIntentType::Attack) {
            return current;
        }

        if (
            best.type == EnemyIntentType::Buff &&
            best.value == 0
        ) {
            best = current;
        }
    }

    return best;
}


static std::vector<EnemyIntent> parseMovePattern(
    const std::string& text
)
{
    std::vector<EnemyIntent> pattern;

    for (const std::string& item : splitLocal(text, ';')) {
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
    LOG_INFO("Loading Enemies csv: path=" << path);

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

    constexpr int kMaxAct = 4;

    for (std::size_t i = 1; i < rows.size(); ++i) {
        const auto& row = rows[i];

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

    LOG_INFO("Cards loaded: count=" << enemies.size());

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
