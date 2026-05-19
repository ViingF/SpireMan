#include<iostream>
#include <algorithm> 
#include "CardSystem.hpp"
void  CardSystem::buildCombatDeck(CombatDeck& deck, const std::vector<CardInstance>& masterDeck)
{
	deck.drawPile = masterDeck;
}
void CardSystem::shuffleDrawPile(CombatDeck& deck, std::mt19937& rng)
{
	std::shuffle(deck.drawPile.begin(), deck.drawPile.end(), rng);
}
void CardSystem::drawCards(CombatDeck& deck, int count, std::mt19937& rng)
{
	for(int i=0;i<count;i++)
	{
		if (deck.drawPile.empty())
		{
			if (deck.discardPile.empty())
			{
				break;
			}
			deck.drawPile = std::move(deck.discardPile);
			deck.discardPile.clear();
			shuffleDrawPile(deck, rng);
		}
		deck.hand.push_back(deck.drawPile.back());
		deck.drawPile.pop_back();
	}
}
ErrorCode CardSystem::moveHandToDiscard(CombatDeck& deck, int handIndex) 
{
	if (handIndex < 0 || handIndex >= static_cast<int>(deck.hand.size())) 
	{
		return ErrorCode::CARD_NOT_IN_HAND;
	}
	deck.discardPile.push_back(deck.hand[handIndex]);
	deck.hand.erase(deck.hand.begin() + handIndex);
	return ErrorCode::OK;
}
ErrorCode CardSystem::moveHandToExhaust(CombatDeck& deck, int handIndex)
{
	if (handIndex < 0 || handIndex >= static_cast<int>(deck.hand.size()))
	{
		return ErrorCode::CARD_NOT_IN_HAND;
	}
	deck.exhaustPile.push_back(deck.hand[handIndex]);
	deck.hand.erase(deck.hand.begin() + handIndex);
	return ErrorCode::OK;
}
void CardSystem::discardAllHand(CombatDeck& deck)
{
	deck.discardPile.insert(deck.discardPile.end(), deck.hand.begin(), deck.hand.end());
	deck.hand.clear();
}
int CardSystem::getHandSize(const CombatDeck& deck) const 
{
	return static_cast<int>(deck.hand.size());
}
int CardSystem::getDrawPileSize(const CombatDeck& deck) const 
{
	return static_cast<int>(deck.drawPile.size());
}
int CardSystem::getDiscardPileSize(const CombatDeck& deck) const 
{
	return static_cast<int>(deck.discardPile.size());
}
int CardSystem::getExhaustPileSize(const CombatDeck& deck) const 
{
	return static_cast<int>(deck.exhaustPile.size());
}