
#include "EventDatabase.hpp"

#include <algorithm>
#include <fstream>
#include <sstream>
#include <stdexcept>

bool EventDatabase::exists(EventId id) const
{
    return events_.find(id) != events_.end();
}

const EventDef& EventDatabase::get(EventId id) const
{
    auto it = events_.find(id);
    if (it == events_.end()) {
        throw std::runtime_error("Event id not found");
    }
    return it->second;
}

std::vector<EventId> EventDatabase::getAllEventIds() const
{
    std::vector<EventId> ids;
    ids.reserve(events_.size());

    for (const auto& pair : events_) {
        ids.push_back(pair.first);
    }

    std::sort(ids.begin(), ids.end());
    return ids;
}

EventId EventDatabase::chooseRandomEventId(std::mt19937& rng) const
{
    std::vector<EventId> ids = getAllEventIds();

    if (ids.empty()) {
        return 0;
    }

    std::uniform_int_distribution<int> dist(
        0,
        static_cast<int>(ids.size()) - 1
    );

    return ids[dist(rng)];
}
std::vector<std::string> parseCsvLine(const std::string& line)
{
    std::vector<std::string> result;
    std::string current;
    bool inQuotes = false;

    for (std::size_t i = 0; i < line.size(); ++i) {
        char c = line[i];

        if (c == '"') {
            if (
                inQuotes &&
                i + 1 < line.size() &&
                line[i + 1] == '"'
            ) {
                current += '"';
                ++i;
            } else {
                inQuotes = !inQuotes;
            }
        } else if (c == ',' && !inQuotes) {
            result.push_back(current);
            current.clear();
        } else {
            current += c;
        }
    }

    result.push_back(current);
    return result;
}
EventEffectType parseEventEffectType(const std::string& text)
{
    if (text == "None") {
        return EventEffectType::None;
    }

    if (text == "GainGold") {
        return EventEffectType::GainGold;
    }

    if (text == "LoseHp") {
        return EventEffectType::LoseHp;
    }

    if (text == "HealHp") {
        return EventEffectType::HealHp;
    }

    if (text == "GainMaxHp") {
        return EventEffectType::GainMaxHp;
    }

    if (text == "StartCombat") {
        return EventEffectType::StartCombat;
    }

    return EventEffectType::None;
}
std::vector<EventEffect> parseEffects(const std::string& text)
{
    std::vector<EventEffect> effects;

    if (text.empty()) {
        effects.push_back({EventEffectType::None, 0});
        return effects;
    }

    std::stringstream ss(text);
    std::string item;

    while (std::getline(ss, item, ';')) {
        std::size_t pos = item.find(':');

        if (pos == std::string::npos) {
            effects.push_back({
                parseEventEffectType(item),
                0
            });
            continue;
        }

        std::string typeText = item.substr(0, pos);
        std::string valueText = item.substr(pos + 1);

        int value = 0;

        try {
            value = std::stoi(valueText);
        } catch (...) {
            value = 0;
        }

        effects.push_back({
            parseEventEffectType(typeText),
            value
        });
    }

    if (effects.empty()) {
        effects.push_back({EventEffectType::None, 0});
    }

    return effects;
}

ErrorCode EventDatabase::loadFromCsv(const std::string& path)
{
    std::ifstream file(path);

    if (!file.is_open()) {
        return ErrorCode::FILE_NOT_FOUND;
    }

    events_.clear();

    std::string line;

    if (!std::getline(file, line)) {
        return ErrorCode::DATA_FORMAT_ERROR;
    }

    while (std::getline(file, line)) {
        if (line.empty()) {
            continue;
        }

        std::vector<std::string> columns = parseCsvLine(line);

        if (columns.size() < 8) {
            return ErrorCode::DATA_FORMAT_ERROR;
        }

        EventId eventId = 0;
        int choiceIndex = 0;

        try {
            eventId = std::stoi(columns[0]);
            choiceIndex = std::stoi(columns[3]);
        } catch (...) {
            return ErrorCode::DATA_FORMAT_ERROR;
        }

        const std::string& title = columns[1];
        const std::string& description = columns[2];
        const std::string& choiceText = columns[4];
        const std::string& choiceDescription = columns[5];
        const std::string& effectsText = columns[6];
        const std::string& backgroundTextureId = columns[7];

        if (eventId <= 0 || choiceIndex < 0) {
            return ErrorCode::DATA_FORMAT_ERROR;
        }

        EventDef& eventDef = events_[eventId];

        eventDef.id = eventId;
        eventDef.title = title;
        eventDef.description = description;

        if (eventDef.backgroundTextureId.empty()) {
            eventDef.backgroundTextureId = backgroundTextureId;
        } else if (eventDef.backgroundTextureId != backgroundTextureId) {
            return ErrorCode::DATA_FORMAT_ERROR;
        }
        if (
            static_cast<int>(eventDef.choices.size())
            <= choiceIndex
        ) {
            eventDef.choices.resize(choiceIndex + 1);
        }

        EventChoiceDef choice;
        choice.text = choiceText;
        choice.description = choiceDescription;
        choice.effects = parseEffects(effectsText);

        eventDef.choices[choiceIndex] = choice;
    }

    for (const auto& pair : events_) {
        const EventDef& eventDef = pair.second;

        if (
            eventDef.id <= 0 ||
            eventDef.title.empty() ||
            eventDef.description.empty() ||
            eventDef.backgroundTextureId.empty() ||
            eventDef.choices.empty()
        ) {
            return ErrorCode::DATA_FORMAT_ERROR;
        }

        for (const EventChoiceDef& choice : eventDef.choices) {
            if (choice.text.empty()) {
                return ErrorCode::DATA_FORMAT_ERROR;
            }
        }
    }

    return ErrorCode::OK;
}