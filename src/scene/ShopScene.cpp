#include "ShopScene.hpp"

#include "config/Constants.hpp"

#include <algorithm>
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
    : Scene(context)
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

    if (getLeaveRect().contains(mousePos)) {
        leaveShop();
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
        messageText.setPosition(sf::Vector2f(panelX + 40.0f, 720.0f));
        window.draw(messageText);
    }

    const sf::FloatRect leaveRect = getLeaveRect();

    sf::RectangleShape leaveButton(leaveRect.size);
    leaveButton.setPosition(leaveRect.position);
    leaveButton.setFillColor(sf::Color(110, 70, 45, 230));
    leaveButton.setOutlineThickness(3.0f);
    leaveButton.setOutlineColor(sf::Color(240, 210, 160));
    window.draw(leaveButton);

    sf::Text leaveText = makeText(font, "Leave", 34);
    leaveText.setFillColor(sf::Color::White);
    leaveText.setPosition(sf::Vector2f(
        leaveRect.position.x + 160.0f,
        leaveRect.position.y + 25.0f
    ));
    window.draw(leaveText);
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
