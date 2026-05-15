#include "CardDatabase.hpp"
#include "database/DataLoader.hpp"

#include <stdexcept>

namespace {

    bool isBoolText(const std::string& text)
    {
        return text == "1" ||
               text == "0" ||
               text == "true" ||
               text == "false" ||
               text == "True" ||
               text == "False" ||
               text == "TRUE" ||
               text == "FALSE" ||
               text == "yes" ||
               text == "no" ||
               text == "Yes" ||
               text == "No";
    }

    bool parseBoolText(const std::string& text)
    {
        return text == "1" ||
               text == "true" ||
               text == "True" ||
               text == "TRUE" ||
               text == "yes" ||
               text == "Yes";
    }


    bool isNoEffect(const std::string& text)
    {
        return text.empty()
            || text == "None"
            || text == "none";
    }

    bool parseEffectType(
        const std::string& text,
        EffectType& type
    )
    {
        if (text == "DealDamage") {
            type = EffectType::DealDamage;
            return true;
        }

        if (text == "GainBlock") {
            type = EffectType::GainBlock;
            return true;
        }

        if (text == "DrawCards") {
            type = EffectType::DrawCards;
            return true;
        }

        if (text == "GainEnergy") {
            type = EffectType::GainEnergy;
            return true;
        }

        if (text == "ApplyVulnerable") {
            type = EffectType::ApplyVulnerable;
            return true;
        }

        if (text == "ApplyWeak") {
            type = EffectType::ApplyWeak;
            return true;
        }

        if (text == "GainStrength") {
            type = EffectType::GainStrength;
            return true;
        }

        if (text == "RitualDagger" || text == "ritualdagger") {
            type = EffectType::RitualDagger;
            return true;
        }

        return false;
    }

}


ErrorCode CardDatabase::loadFromCsv(
    const std::string& path
)
{
    cards.clear();

    std::vector<std::vector<std::string>> rows;

    ErrorCode code =
        DataLoader::readCsv(path, rows);

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

        if (row.size() < 10)
        {
            return ErrorCode::DATA_FORMAT_ERROR;
        }

        CardDef card;

        try
        {
            card.id = std::stoi(row[0]);

            if (cards.contains(card.id))
            {
                return ErrorCode::DUPLICATE_ID;
            }

            card.name = row[1];

            if (row[2] == "Attack") {
                card.type = CardType::Attack;
            }
            else if (row[2] == "Skill") {
                card.type = CardType::Skill;
            }
            else if (row[2] == "Curse") {
                card.type = CardType::Curse;
            }
            else {
                return ErrorCode::INVALID_ENUM_VALUE;
            }

            card.cost = std::stoi(row[3]);

            if (row[4] == "Enemy")
            {
                card.target = TargetType::Enemy;
            }
            else if (row[4] == "Self")
            {
                card.target = TargetType::Self;
            }
            else if (row[4] == "None")
            {
                card.target = TargetType::None;
            }
            else
            {
                return ErrorCode::INVALID_ENUM_VALUE;
            }

            if (!isNoEffect(row[5]))
            {
                CardEffect effect1;

                if (!parseEffectType(row[5], effect1.type))
                {
                    return ErrorCode::INVALID_ENUM_VALUE;
                }

                effect1.value = row[6].empty()
                    ? 0
                    : std::stoi(row[6]);

                card.effects.push_back(effect1);
            }

            if (!isNoEffect(row[7]))
            {
                CardEffect effect2;

                if (!parseEffectType(row[7], effect2.type))
                {
                    return ErrorCode::INVALID_ENUM_VALUE;
                }

                effect2.value = row[8].empty()
                    ? 0
                    : std::stoi(row[8]);

                card.effects.push_back(effect2);
            }



            card.description = row[9];

            card.exhaust = false;

            if (row.size() >= 12) {
                if (!isBoolText(row[11])) {
                    return ErrorCode::INVALID_ENUM_VALUE;
                }

                card.exhaust = parseBoolText(row[11]);
            }
            else if (row.size() >= 11 && isBoolText(row[10])) {
                 card.exhaust = parseBoolText(row[10]);
            }

            cards[card.id] = card;

        }
        catch (...)
        {
            return ErrorCode::DATA_FORMAT_ERROR;
        }
    }

    return ErrorCode::OK;
}

const CardDef& CardDatabase::get(
    CardId id
) const
{
    auto it = cards.find(id);

    if (it == cards.end())
    {
        throw std::runtime_error(
            "Card not found"
        );
    }

    return it->second;
}

bool CardDatabase::exists(
    CardId id
) const
{
    return cards.contains(id);
}

std::vector<CardId> CardDatabase::getAllCardIds() const
{
    std::vector<CardId> ids;

    for (const auto& pair : cards)
    {
        ids.push_back(pair.first);
    }

    return ids;
}

CardId CardDatabase::chooseRandomCardId(std::mt19937& rng) const
{
    std::vector<CardId> ids = getAllCardIds();

    if (ids.empty()) {
        return 0;
    }

    std::uniform_int_distribution<std::size_t> dist(
        0,
        ids.size() - 1
    );

    return ids[dist(rng)];
}

std::vector<CardId> CardDatabase::getCardIdsByType(CardType type) const
{
    std::vector<CardId> ids;

    for (const auto& pair : cards) {
        if (pair.second.type == type) {
            ids.push_back(pair.first);
        }
    }

    return ids;
}

std::vector<CardId> CardDatabase::getRewardCardIds() const
{
    std::vector<CardId> ids;

    for (const auto& pair : cards) {
        const CardDef& card = pair.second;

        if (card.type != CardType::Curse) {
            ids.push_back(pair.first);
        }
    }

    return ids;
}

CardId CardDatabase::chooseRandomRewardCardId(std::mt19937& rng) const
{
    std::vector<CardId> ids = getRewardCardIds();

    if (ids.empty()) {
        return 0;
    }

    std::uniform_int_distribution<std::size_t> dist(
        0,
        ids.size() - 1
    );

    return ids[dist(rng)];
}
