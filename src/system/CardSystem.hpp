#pragma once
#include<random>
#include<vector>
#include "model/CombatDeck.hpp"

class CardSystem
{
public:
	void buildCombatDeck(CombatDeck& deck, const std::vector<CardInstance>& masterDeck);//将牌组中的牌复制到抽牌堆
	void shuffleDrawPile(CombatDeck& deck, std::mt19937& rng);//打乱抽牌堆顺序
	void drawCards(CombatDeck& deck, int count, std::mt19937& rng);//抽牌
	ErrorCode moveHandToDiscard(CombatDeck& deck, int handIndex);//将指定下标的手牌移入弃牌堆
	ErrorCode moveHandToExhaust(CombatDeck& deck, int handIndex);//将指定下标的手牌移入消耗牌堆
	void discardAllHand(CombatDeck& deck);//弃所有手牌
	int getHandSize(const CombatDeck& deck) const;//这四个函数只读，做ui可能会用到（？）
	int getDrawPileSize(const CombatDeck& deck) const;
	int getDiscardPileSize(const CombatDeck& deck) const;
	int getExhaustPileSize(const CombatDeck& deck) const;
};
