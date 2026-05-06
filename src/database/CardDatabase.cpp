#include "CardDatabase.hpp"
#include "database/DataLoader.hpp"

#include <stdexcept>

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

            if (row[2] == "Attack")
            {
                card.type = CardType::Attack;
            }
            else if (row[2] == "Skill")
            {
                card.type = CardType::Skill;
            }
            else
            {
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

            CardEffect effect1;

            if (row[5] == "DealDamage")
            {
                effect1.type = EffectType::DealDamage;
            }
            else if (row[5] == "GainBlock")
            {
                effect1.type = EffectType::GainBlock;
            }
            else if (row[5] == "DrawCards")
            {
                effect1.type = EffectType::DrawCards;
            }
            else if (row[5] == "GainEnergy")
            {
                effect1.type = EffectType::GainEnergy;
            }
            else if (row[5] == "ApplyVulnerable")
            {
                effect1.type = EffectType::ApplyVulnerable;
            }
            else if (row[5] == "ApplyWeak")
            {
                effect1.type = EffectType::ApplyWeak;
            }
            else if (row[5] == "GainStrength")
            {
                effect1.type = EffectType::GainStrength;
            }
            else
            {
                return ErrorCode::INVALID_ENUM_VALUE;
            }

            effect1.value = std::stoi(row[6]);

            card.effects.push_back(effect1);


            if (!row[7].empty())
            {
                CardEffect effect2;

                if (row[7] == "DealDamage")
                {
                    effect2.type = EffectType::DealDamage;
                }
                else if (row[7] == "GainBlock")
                {
                    effect2.type = EffectType::GainBlock;
                }
                else if (row[7] == "DrawCards")
                {
                    effect2.type = EffectType::DrawCards;
                }
                else if (row[7] == "GainEnergy")
                {
                    effect2.type = EffectType::GainEnergy;
                }
                else if (row[7] == "ApplyVulnerable")
                {
                    effect2.type = EffectType::ApplyVulnerable;
                }
                else if (row[7] == "ApplyWeak")
                {
                    effect2.type = EffectType::ApplyWeak;
                }
                else if (row[7] == "GainStrength")
                {
                    effect2.type = EffectType::GainStrength;
                }
                else
                {
                    return ErrorCode::INVALID_ENUM_VALUE;
                }

                effect2.value = std::stoi(row[8]);

                card.effects.push_back(effect2);
            }


            card.description = row[9];

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
