#include "EnemyAISystem.hpp"
#include <algorithm>
EnemyIntent EnemyAISystem::chooseIntent(const Enemy& enemy, int turnIndex) const
{
	if (enemy.movePattern.empty())
	{
		return { EnemyIntentType::Attack, 0 };
	}
	int i = turnIndex % static_cast<int>(enemy.movePattern.size());
	return enemy.movePattern[i];
}
void EnemyAISystem::applyIntent(Enemy& enemy, Player& player) const
{
	switch (enemy.intent.type)
	{
	    case EnemyIntentType::Attack:
		{
			int damage = enemy.intent.value + enemy.strength;
			if (enemy.weak > 0)
			{
				damage = static_cast<int>(damage * 0.75f);
			}//??????????????
			if (player.vulnerable > 0)
			{
				damage = static_cast<int>(damage * 1.5f);
			}
			int blocked = std::min(player.block, damage);
			player.block -= blocked;
			damage -= blocked;
			player.hp -= damage;
			break;
		}
	    case EnemyIntentType::Block:
			enemy.block += enemy.intent.value;
		break;
	    case EnemyIntentType::Buff:
			enemy.strength += enemy.intent.value;
		break;
		default:
			break;
	}
}