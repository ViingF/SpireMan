#include "ShopScene.hpp"

#include "config/Constants.hpp"

#include <algorithm>
#include <iostream>
#include <sstream>
#include <string>

#include "ui/TextUtils.hpp"
#include "ui/TopInfoBar.hpp"

namespace {

bool readLeftClickPosition(
    const sf::Event& event,
    sf::Vector2i& pixelPosition
)
{
    if (const auto* mouse =
            event.getIf<sf::Event::MouseButtonPressed>()) {
        if (mouse->button == sf::Mouse::Button::Left) {
            pixelPosition = mouse->position;
            return true;
        }
    }

    return false;
}

    bool isShopBannedCard(CardId cardId, const CardDef& card)
{
    return cardId == 1 ||
           cardId == 2 ||
           card.name == "Strike" ||
           card.name == "Defend" ||
           card.name == "打击" ||
           card.name == "防御" ||
           card.textureId == "Strike" ||
           card.textureId == "Defend";
}


}

ShopScene::ShopScene(GameContext& context)
    : Scene(context),
      removeCardButton_(
          sf::Vector2f(1180.0f, 710.0f),
          sf::Vector2f(420.0f, 80.0f),
          context.resources.getFont("zh-R"),
          "丢弃卡牌"
      ),
      leaveButton_(
          sf::Vector2f(1180.0f, 820.0f),
          sf::Vector2f(420.0f, 90.0f),
          context.resources.getFont("zh-R"),
          "离开"
      ),
      cancelRemoveButton_(
          sf::Vector2f(1540.0f, 125.0f),
          sf::Vector2f(250.0f, 70.0f),
          context.resources.getFont("zh-R"),
          "取消"
      ),
      removePrevPageButton_(
          sf::Vector2f(620.0f, 930.0f),
          sf::Vector2f(210.0f, 70.0f),
          context.resources.getFont("zh-R"),
          "上一页"
      ),
      removeNextPageButton_(
          sf::Vector2f(1090.0f, 930.0f),
          sf::Vector2f(210.0f, 70.0f),
          context.resources.getFont("zh-R"),
          "下一页"
          ),
    mapIconButton_(
        sf::Vector2f(0.0f, 0.0f),
        sf::Vector2f(64.0f, 64.0f),
        context.resources.getFont("zh-R"),
        ""
    )

{
    std::vector<CardId> candidates;

    for (CardId cardId : context.cards.getRewardCardIds()) {
        if (!context.cards.exists(cardId)) {
            continue;
        }

        const CardDef& card = context.cards.get(cardId);

        if (isShopBannedCard(cardId, card)) {
            continue;
        }

        candidates.push_back(cardId);
    }

    std::shuffle(
        candidates.begin(),
        candidates.end(),
        context.runState.rng
    );

    const int offerCount = std::min(
        SHOP_CARD_COUNT,
        static_cast<int>(candidates.size())
    );

    for (int i = 0; i < offerCount; ++i) {
        cardOffers_.push_back(candidates[i]);
        sold_.push_back(false);
    }

    mapIconButton_.setTexture(
    context.resources.getTexture("map")
);

}


void ShopScene::handleEvent(
    const sf::Event& event,
    const sf::RenderWindow& window
)
{
    shopView_.layoutButtons(
        mapIconButton_,
        removeCardButton_,
        leaveButton_,
        cancelRemoveButton_,
        removePrevPageButton_,
        removeNextPageButton_,
        window
    );

    if (transition_.target != SceneType::None) {
        return;
    }

    if (removingCard_) {
        if (removeCardScene_) {
            removeCardScene_->handleEvent(event, window);

            if (removeCardScene_->isFinished()) {
                finishRemoveCardFromScene();
            }
        }

        return;
    }


    mapIconButton_.handleEvent(event, window);

    if (mapIconButton_.wasClicked()) {
        
        transition_.openMapPreview = true;
        transition_.target = SceneType::Map;
        mapIconButton_.reset();
        return;
    }

    if (transition_.target != SceneType::None) {
        return;
    }

    removeCardButton_.handleEvent(event, window);
    leaveButton_.handleEvent(event, window);

    if (removeCardButton_.wasClicked()) {
        removeCardButton_.reset();
        startRemoveCard();
        return;
    }

    if (leaveButton_.wasClicked()) {
        leaveButton_.reset();
        leaveShop();
        return;
    }

    sf::Vector2i pixelPosition;

    if (!readLeftClickPosition(event, pixelPosition)) {
        return;
    }

    const sf::Vector2f mousePos =
        window.mapPixelToCoords(pixelPosition);

    int clickedIndex = -1;

    if (shopView_.handleCardClick(
        mousePos,
        static_cast<int>(cardOffers_.size()),
        clickedIndex
    )) {
        context.audio.playSound("Click");
        buyCard(clickedIndex);
    }

}





void ShopScene::update(float dt)
{
    if (removeCardScene_) {
        removeCardScene_->update(dt);

        if (removeCardScene_->isFinished()) {
            finishRemoveCardFromScene();
        }
    }
}


void ShopScene::draw(sf::RenderWindow& window)
{
    const sf::Font& font = context.resources.getFont("zh-R");

    shopView_.layoutButtons(
        mapIconButton_,
        removeCardButton_,
        leaveButton_,
        cancelRemoveButton_,
        removePrevPageButton_,
        removeNextPageButton_,
        window
    );

    const std::vector<ShopCardViewData> shopCards =
        buildShopCardViewData();

    shopView_.drawMain(
        window,
        context,
        font,
        shopCards,
        message_,
        getRemoveCost(),
        mapIconButton_,
        removeCardButton_,
        leaveButton_
    );

    if (removingCard_ && removeCardScene_) {
        removeCardScene_->draw(window);
    }
}


SceneTransition ShopScene::getTransition() const
{
    return transition_;
}

void ShopScene::buyCard(int index)
{
    if (index < 0 ||
        index >= static_cast<int>(cardOffers_.size())) {
        return;
    }

    if (sold_[index]) {
        context.failureToast.show("这张卡已经被卖出了");
        return;
    }

    if (context.runState.gold < SHOP_CARD_COST) {
        context.failureToast.show("金币不足");
        return;
    }


    context.runState.gold -= SHOP_CARD_COST;

    const CardId cardId = cardOffers_[index];

    CardInstance temp;
    temp.instanceId = context.runState.nextCardInstanceId++;
    temp.cardId = cardId;

    context.runState.masterDeck.push_back(temp);


    sold_[index] = true;
    message_ = "卡牌已购买";
}

void ShopScene::leaveShop()
{
    mapSystem_.completeSelectedNode(context.runState);

    if (mapSystem_.isRouteFinished(context.runState)) {
        transition_.target = SceneType::End;
        transition_.battleResult = BattleResult::Victory;
    } else {
        transition_.target = SceneType::Map;
    }
}

int ShopScene::getRemoveCost() const
{
    return SHOP_REMOVE_BASE_COST * (shopRemoveCount_ + 1);
}

void ShopScene::startRemoveCard()
{

    if (context.runState.masterDeck.empty()) {
        context.failureToast.show("没有可丢弃的卡牌");
        return;
    }


    const int cost = getRemoveCost();

    if (context.runState.gold < cost) {
        std::ostringstream text;
        text << "金币不足，需要 "
             << cost
             << " 金币";

        context.failureToast.show(text.str());
        return;
    }

    context.runState.gold -= cost;
    context.runState.pendingRemoveCardCount = 1;
    context.runState.pendingEventEffects.clear();

    pendingShopRemoveCost_ = cost;

    removeCardScene_ = std::make_unique<CardRemoveScene>(
        context,
        CardRemoveSceneMode::Embedded
    );

    removingCard_ = true;
    removePage_ = 0;
    message_.clear();


}

void ShopScene::resetTransition()
{
    transition_ = SceneTransition{};
}

const sf::Texture& ShopScene::getCardTemplateTexture(CardType type) const
{
    switch (type) {
        case CardType::Attack:
            return context.resources.getTexture("Attack");

        case CardType::Skill:
            return context.resources.getTexture("Skill");

        case CardType::Curse:
            return context.resources.getTexture("Curse");
    }

    return context.resources.getTexture("Attack");
}

const sf::Texture* ShopScene::getCardArtTexture(
    const CardDef& cardDef
) const
{
    if (
        cardDef.textureId.empty() ||
        !context.resources.hasTexture(cardDef.textureId)
    ) {
        return nullptr;
    }

    return &context.resources.getTexture(cardDef.textureId);
}

CardRenderTextures ShopScene::getCardRenderTextures(
    const CardDef& cardDef
) const
{
    CardRenderTextures textures;

    textures.templateTexture =
        &getCardTemplateTexture(cardDef.type);

    textures.artTexture =
        getCardArtTexture(cardDef);

    return textures;
}

std::vector<ShopCardViewData> ShopScene::buildShopCardViewData() const
{
    std::vector<ShopCardViewData> result;
    result.reserve(cardOffers_.size());

    for (int i = 0; i < static_cast<int>(cardOffers_.size()); ++i) {
        const CardId cardId = cardOffers_[i];

        if (!context.cards.exists(cardId)) {
            continue;
        }

        const CardDef& def = context.cards.get(cardId);

        ShopCardViewData data;
        data.cardId = cardId;
        data.cardDef = &def;
        data.textures = getCardRenderTextures(def);
        data.price = SHOP_CARD_COST;
        data.sold = sold_[i];


        result.push_back(data);
    }

    return result;
}

void ShopScene::finishRemoveCardFromScene()
{
    const bool removed = context.runState.pendingRemoveCardCount <= 0;

    removeCardScene_.reset();
    removingCard_ = false;
    context.runState.pendingEventEffects.clear();

    if (removed) {
        shopRemoveCount_ += 1;

        std::ostringstream text;
        text << "丢弃卡牌并花费 "
             << pendingShopRemoveCost_
             << " 金币.";

        if (!context.runState.masterDeck.empty()) {
            text << " 下一次丢弃花费 "
                 << getRemoveCost()
                 << " 金币.";
        }

        message_ = text.str();
    } else {
        context.runState.gold += pendingShopRemoveCost_;
        context.runState.pendingRemoveCardCount = 0;
        message_ = "取消删除卡牌";
    }

    pendingShopRemoveCost_ = 0;
}
