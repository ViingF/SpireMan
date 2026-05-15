#include "ShopScene.hpp"

#include "config/Constants.hpp"

#include <algorithm>
#include <iostream>
#include <sstream>
#include <string>

namespace {

sf::Text makeText(
    const sf::Font& font,
    const std::string& content,
    unsigned int size
)
{
    return sf::Text(font, content, size);
}

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

sf::Vector2f getViewTopLeft(const sf::RenderWindow& window)
{
    const sf::View& view = window.getView();
    const sf::Vector2f center = view.getCenter();
    const sf::Vector2f size = view.getSize();

    return sf::Vector2f(
        center.x - size.x * 0.5f,
        center.y - size.y * 0.5f
    );
}

sf::Vector2f getViewSize(const sf::RenderWindow& window)
{
    return window.getView().getSize();
}

void drawTexture1920x1080(
    sf::RenderWindow& window,
    sf::Texture& texture
)
{
    sf::Sprite sprite(texture);

    const sf::Vector2f viewTopLeft = getViewTopLeft(window);
    const sf::Vector2u textureSize = texture.getSize();

    if (textureSize.x == 0 || textureSize.y == 0) {
        return;
    }

    const float scaleX =
        1920.0f / static_cast<float>(textureSize.x);

    const float scaleY =
        1080.0f / static_cast<float>(textureSize.y);

    sprite.setScale(sf::Vector2f(scaleX, scaleY));
    sprite.setPosition(viewTopLeft);

    window.draw(sprite);
}

void drawFallbackBackground(sf::RenderWindow& window)
{
    sf::RectangleShape background(getViewSize(window));
    background.setPosition(getViewTopLeft(window));
    background.setFillColor(sf::Color(24, 22, 30));
    window.draw(background);
}

void drawShopBackground(
    sf::RenderWindow& window,
    ResourceManager& resources
)
{
    if (resources.hasTexture("shopScene")) {
        drawTexture1920x1080(
            window,
            resources.getTexture("shopScene")
        );
        return;
    }

    drawFallbackBackground(window);
}

}

ShopScene::ShopScene(GameContext& context)
    : Scene(context),
      removeCardButton_(
          sf::Vector2f(1180.0f, 710.0f),
          sf::Vector2f(420.0f, 80.0f),
          context.resources.getFont("zh-R"),
          "Remove Card"
      ),
      leaveButton_(
          sf::Vector2f(1180.0f, 820.0f),
          sf::Vector2f(420.0f, 90.0f),
          context.resources.getFont("zh-R"),
          "Leave"
      ),
      cancelRemoveButton_(
          sf::Vector2f(1540.0f, 125.0f),
          sf::Vector2f(250.0f, 70.0f),
          context.resources.getFont("zh-R"),
          "Cancel"
      ),
      removePrevPageButton_(
          sf::Vector2f(620.0f, 930.0f),
          sf::Vector2f(210.0f, 70.0f),
          context.resources.getFont("zh-R"),
          "Previous"
      ),
      removeNextPageButton_(
          sf::Vector2f(1090.0f, 930.0f),
          sf::Vector2f(210.0f, 70.0f),
          context.resources.getFont("zh-R"),
          "Next"
      )
{
    for (int i = 0; i < SHOP_CARD_COUNT; ++i) {
        CardId cardId =
            context.cards.chooseRandomRewardCardId(context.runState.rng);

        if (cardId != 0) {
            cardOffers_.push_back(cardId);
            sold_.push_back(false);
        }
    }
}


void ShopScene::handleEvent(
    const sf::Event& event,
    const sf::RenderWindow& window
)
{
    if (transition_.target != SceneType::None) {
        return;
    }

    if (removingCard_) {
        cancelRemoveButton_.handleEvent(event, window);
        removePrevPageButton_.handleEvent(event, window);
        removeNextPageButton_.handleEvent(event, window);

        if (cancelRemoveButton_.wasClicked()) {
            cancelRemoveButton_.reset();
            removingCard_ = false;
            message_ = "Card removal cancelled.";
            return;
        }

        if (removePrevPageButton_.wasClicked()) {
            removePrevPageButton_.reset();
            removePage_ -= 1;
            clampRemovePage();
            return;
        }

        if (removeNextPageButton_.wasClicked()) {
            removeNextPageButton_.reset();
            removePage_ += 1;
            clampRemovePage();
            return;
        }

        sf::Vector2i pixelPosition;

        if (!readLeftClickPosition(event, pixelPosition)) {
            return;
        }

        const sf::Vector2f mousePos =
            window.mapPixelToCoords(pixelPosition);

        const int cardsPerPage = getRemoveCardsPerPage();
        const int startIndex = removePage_ * cardsPerPage;
        const int deckSize =
            static_cast<int>(context.runState.masterDeck.size());

        for (int i = 0; i < cardsPerPage; ++i) {
            const int deckIndex = startIndex + i;

            if (deckIndex >= deckSize) {
                break;
            }

            if (getDeckCardRect(i).contains(mousePos)) {
                removeCardByDeckIndex(deckIndex);
                return;
            }
        }

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

    for (int i = 0; i < static_cast<int>(cardOffers_.size()); ++i) {
        if (getCardRect(i).contains(mousePos)) {
            buyCard(i);
            return;
        }
    }
}




void ShopScene::update(float)
{
}

void ShopScene::draw(sf::RenderWindow& window)
{
    drawShopBackground(window, context.resources);

    const sf::Font& font = context.resources.getFont("zh-R");

    const sf::Vector2f viewTopLeft = getViewTopLeft(window);
    const sf::Vector2f viewSize = getViewSize(window);

    sf::RectangleShape topBar(sf::Vector2f(viewSize.x, 86.0f));
    topBar.setPosition(viewTopLeft);
    topBar.setFillColor(sf::Color(0, 0, 0, 140));
    window.draw(topBar);

    std::ostringstream status;
    status << "Shop"
           << "    HP: " << context.runState.player.hp
           << " / " << context.runState.player.maxHp
           << "    Gold: " << context.runState.gold
           << "    Floor: " << context.runState.floor;

    sf::Text statusText = makeText(font, status.str(), 28);
    statusText.setFillColor(sf::Color(245, 240, 220));
    statusText.setPosition(sf::Vector2f(
        viewTopLeft.x + 60.0f,
        viewTopLeft.y + 28.0f
    ));
    window.draw(statusText);

    const float panelX = 980.0f;
    const float panelY = 120.0f;
    const float panelWidth = 860.0f;
    const float panelHeight = 860.0f;

    sf::RectangleShape panel(sf::Vector2f(panelWidth, panelHeight));
    panel.setPosition(sf::Vector2f(panelX, panelY));
    panel.setFillColor(sf::Color(0, 0, 0, 135));
    panel.setOutlineThickness(2.0f);
    panel.setOutlineColor(sf::Color(230, 220, 180, 130));
    window.draw(panel);

    sf::Text title = makeText(font, "Merchant", 52);
    title.setFillColor(sf::Color::White);
    title.setPosition(sf::Vector2f(panelX + 40.0f, panelY + 35.0f));
    window.draw(title);

    sf::Text hint = makeText(
        font,
        "Buy cards to strengthen your deck.",
        28
    );
    hint.setFillColor(sf::Color(225, 225, 225));
    hint.setPosition(sf::Vector2f(panelX + 40.0f, panelY + 105.0f));
    window.draw(hint);

    for (int i = 0; i < static_cast<int>(cardOffers_.size()); ++i) {
        const sf::FloatRect rect = getCardRect(i);

        sf::RectangleShape cardBox(rect.size);
        cardBox.setPosition(rect.position);

        if (sold_[i]) {
            cardBox.setFillColor(sf::Color(60, 60, 60, 190));
        } else {
            cardBox.setFillColor(sf::Color(70, 70, 95, 225));
        }

        cardBox.setOutlineThickness(3.0f);
        cardBox.setOutlineColor(sf::Color(210, 210, 230));
        window.draw(cardBox);

        const CardId cardId = cardOffers_[i];
        const auto& card = context.cards.get(cardId);

        sf::Text cardName = makeText(font, card.name, 28);
        cardName.setFillColor(sf::Color::White);
        cardName.setPosition(sf::Vector2f(
            rect.position.x + 24.0f,
            rect.position.y + 16.0f
        ));
        window.draw(cardName);

        std::ostringstream priceText;

        if (sold_[i]) {
            priceText << "Sold";
        } else {
            priceText << SHOP_CARD_COST << " Gold";
        }

        sf::Text price = makeText(font, priceText.str(), 24);
        price.setFillColor(sf::Color(240, 220, 150));
        price.setPosition(sf::Vector2f(
            rect.position.x + rect.size.x - 160.0f,
            rect.position.y + 18.0f
        ));
        window.draw(price);

        sf::Text cardDesc = makeText(font, card.description, 21);
        cardDesc.setFillColor(sf::Color(215, 215, 215));
        cardDesc.setPosition(sf::Vector2f(
            rect.position.x + 24.0f,
            rect.position.y + 58.0f
        ));
        window.draw(cardDesc);
    }

    if (!message_.empty()) {
        sf::Text messageText = makeText(font, message_, 24);
        messageText.setFillColor(sf::Color(240, 220, 160));
        messageText.setPosition(sf::Vector2f(panelX + 40.0f, 640.0f));
        window.draw(messageText);
    }

    std::ostringstream removeTextStream;
    removeTextStream << "Remove Card: "
                     << getRemoveCost()
                     << " Gold";

    removeCardButton_.setText(removeTextStream.str());
    removeCardButton_.draw(window);
    leaveButton_.draw(window);
    if (removingCard_) {
        drawRemoveCardOverlay(window, font);
    }

}

SceneTransition ShopScene::getTransition() const
{
    return transition_;
}

sf::FloatRect ShopScene::getCardRect(int index) const
{
    const float x = 1040.0f;
    const float y = 280.0f + static_cast<float>(index) * 135.0f;
    const float width = 760.0f;
    const float height = 110.0f;

    return sf::FloatRect{
        sf::Vector2f(x, y),
        sf::Vector2f(width, height)
    };
}

sf::FloatRect ShopScene::getLeaveRect() const
{
    return sf::FloatRect{
        sf::Vector2f(1180.0f, 820.0f),
        sf::Vector2f(420.0f, 90.0f)
    };
}

void ShopScene::buyCard(int index)
{
    if (index < 0 ||
        index >= static_cast<int>(cardOffers_.size())) {
        return;
    }

    if (sold_[index]) {
        message_ = "This card has already been sold.";
        return;
    }

    if (context.runState.gold < SHOP_CARD_COST) {
        message_ = "Not enough gold.";
        return;
    }

    context.runState.gold -= SHOP_CARD_COST;

    const CardId cardId = cardOffers_[index];

    CardInstance temp;
    temp.instanceId = context.runState.nextCardInstanceId++;
    temp.cardId = cardId;

    context.runState.masterDeck.push_back(temp);


    sold_[index] = true;
    message_ = "Card purchased.";
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

sf::FloatRect ShopScene::getRemoveCardRect() const
{
    return sf::FloatRect{
        sf::Vector2f(1180.0f, 710.0f),
        sf::Vector2f(420.0f, 80.0f)
    };
}

int ShopScene::getRemoveCost() const
{
    return SHOP_REMOVE_BASE_COST * (shopRemoveCount_ + 1);
}

void ShopScene::startRemoveCard()
{

    if (context.runState.masterDeck.empty()) {
        message_ = "There are no cards to remove.";
        return;
    }

    const int cost = getRemoveCost();

    if (context.runState.gold < cost) {
        std::ostringstream text;
        text << "Not enough gold. Need "
             << cost
             << " gold.";

        message_ = text.str();
        return;
    }

    removingCard_ = true;
    removePage_ = 0;
    message_ = "Choose a card to remove.";
}

void ShopScene::removeCardByDeckIndex(int deckIndex)
{
    if (
        deckIndex < 0 ||
        deckIndex >= static_cast<int>(context.runState.masterDeck.size())
    ) {
        return;
    }

    const int cost = getRemoveCost();

    if (context.runState.gold < cost) {
        std::ostringstream text;
        text << "Not enough gold. Need "
             << cost
             << " gold.";

        message_ = text.str();
        removingCard_ = false;
        return;
    }

    std::string removedName = "a card";

    const CardId cardId =
        context.runState.masterDeck[deckIndex].cardId;

    if (context.cards.exists(cardId)) {
        removedName = context.cards.get(cardId).name;
    }

    context.runState.gold -= cost;

    context.runState.masterDeck.erase(
        context.runState.masterDeck.begin() + deckIndex
    );

    shopRemoveCount_ += 1;
    removingCard_ = false;

    clampRemovePage();

    std::ostringstream text;
    text << "Removed "
         << removedName
         << " for "
         << cost
         << " gold.";

    if (!context.runState.masterDeck.empty()) {
        text << " Next removal costs "
             << getRemoveCost()
             << " gold.";
    }

    message_ = text.str();
}

int ShopScene::getRemoveCardsPerPage() const
{
    return 15;
}

int ShopScene::getRemovePageCount() const
{
    const int deckSize =
        static_cast<int>(context.runState.masterDeck.size());

    if (deckSize <= 0) {
        return 1;
    }

    const int cardsPerPage = getRemoveCardsPerPage();

    return (deckSize + cardsPerPage - 1) / cardsPerPage;
}

void ShopScene::clampRemovePage()
{
    const int pageCount = getRemovePageCount();

    if (removePage_ < 0) {
        removePage_ = 0;
    }

    if (removePage_ >= pageCount) {
        removePage_ = pageCount - 1;
    }
}

sf::FloatRect ShopScene::getDeckCardRect(int visibleIndex) const
{
    constexpr int columns = 5;

    const int row = visibleIndex / columns;
    const int column = visibleIndex % columns;

    const float cardWidth = 300.0f;
    const float cardHeight = 190.0f;
    const float gapX = 40.0f;
    const float gapY = 34.0f;

    const float startX = 110.0f;
    const float startY = 245.0f;

    return sf::FloatRect{
        sf::Vector2f(
            startX + static_cast<float>(column) * (cardWidth + gapX),
            startY + static_cast<float>(row) * (cardHeight + gapY)
        ),
        sf::Vector2f(cardWidth, cardHeight)
    };
}

sf::FloatRect ShopScene::getRemovePrevPageRect() const
{
    return sf::FloatRect{
        sf::Vector2f(620.0f, 930.0f),
        sf::Vector2f(210.0f, 70.0f)
    };
}

sf::FloatRect ShopScene::getRemoveNextPageRect() const
{
    return sf::FloatRect{
        sf::Vector2f(1090.0f, 930.0f),
        sf::Vector2f(210.0f, 70.0f)
    };
}

sf::FloatRect ShopScene::getCancelRemoveRect() const
{
    return sf::FloatRect{
        sf::Vector2f(1540.0f, 125.0f),
        sf::Vector2f(250.0f, 70.0f)
    };
}

void ShopScene::drawRemoveCardOverlay(
    sf::RenderWindow& window,
    const sf::Font& font
)
{
    sf::RectangleShape dim(sf::Vector2f(1920.0f, 1080.0f));
    dim.setPosition(sf::Vector2f(0.0f, 0.0f));
    dim.setFillColor(sf::Color(0, 0, 0, 180));
    window.draw(dim);

    sf::RectangleShape panel(sf::Vector2f(1760.0f, 900.0f));
    panel.setPosition(sf::Vector2f(80.0f, 100.0f));
    panel.setFillColor(sf::Color(35, 35, 50, 245));
    panel.setOutlineThickness(3.0f);
    panel.setOutlineColor(sf::Color(230, 220, 180));
    window.draw(panel);

    std::ostringstream titleStream;
    titleStream << "Choose a card to remove"
                << "    Cost: "
                << getRemoveCost()
                << " Gold"
                << "    Deck: "
                << context.runState.masterDeck.size();

    sf::Text title = makeText(font, titleStream.str(), 38);
    title.setFillColor(sf::Color::White);
    title.setPosition(sf::Vector2f(120.0f, 130.0f));
    window.draw(title);

    cancelRemoveButton_.draw(window);

    const int cardsPerPage = getRemoveCardsPerPage();
    const int startIndex = removePage_ * cardsPerPage;
    const int deckSize =
        static_cast<int>(context.runState.masterDeck.size());

    for (int i = 0; i < cardsPerPage; ++i) {
        const int deckIndex = startIndex + i;

        if (deckIndex >= deckSize) {
            break;
        }

        const CardInstance& instance =
            context.runState.masterDeck[deckIndex];

        std::string cardName = "Unknown Card";
        std::string cardDescription;
        int cardCost = 0;

        if (context.cards.exists(instance.cardId)) {
            const CardDef& def = context.cards.get(instance.cardId);

            cardName = def.name;
            cardDescription = def.description;
            cardCost = def.cost;
        }

        const sf::FloatRect rect = getDeckCardRect(i);

        sf::RectangleShape cardShape(rect.size);
        cardShape.setPosition(rect.position);
        cardShape.setFillColor(sf::Color(70, 70, 95));
        cardShape.setOutlineThickness(3.0f);
        cardShape.setOutlineColor(sf::Color(220, 210, 170));
        window.draw(cardShape);

        sf::Text nameText = makeText(font, cardName, 26);
        nameText.setFillColor(sf::Color::White);
        nameText.setPosition(sf::Vector2f(
            rect.position.x + 18.0f,
            rect.position.y + 16.0f
        ));
        window.draw(nameText);

        std::ostringstream meta;
        meta << "Cost: " << cardCost
             << "    ID: " << instance.instanceId;

        sf::Text metaText = makeText(font, meta.str(), 19);
        metaText.setFillColor(sf::Color(210, 210, 210));
        metaText.setPosition(sf::Vector2f(
            rect.position.x + 18.0f,
            rect.position.y + 54.0f
        ));
        window.draw(metaText);

        sf::Text descText = makeText(font, cardDescription, 17);
        descText.setFillColor(sf::Color(225, 225, 225));
        descText.setPosition(sf::Vector2f(
            rect.position.x + 18.0f,
            rect.position.y + 90.0f
        ));
        window.draw(descText);
    }

    removePrevPageButton_.draw(window);
    removeNextPageButton_.draw(window);

    std::ostringstream pageTextStream;
    pageTextStream << "Page "
                   << removePage_ + 1
                   << " / "
                   << getRemovePageCount();

    sf::Text pageText = makeText(font, pageTextStream.str(), 24);
    pageText.setFillColor(sf::Color(230, 230, 230));
    pageText.setPosition(sf::Vector2f(880.0f, 956.0f));
    window.draw(pageText);
}

