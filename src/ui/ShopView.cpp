#include "../ui/ShopView.hpp"

#include "TopInfoBar.hpp"
#include "../ui/CardView.hpp"
#include "../ui/TextUtils.hpp"
#include "../model/CardDef.hpp"

sf::Text makeText(
        const sf::Font& font,
        const std::string& content,
        unsigned int size
    ) {
    return sf::Text(font, TextUtils::fromUtf8(content), size);
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
    if (resources.hasTexture("shopbackground")) {
        drawTexture1920x1080(
            window,
            resources.getTexture("shopbackground")
        );
        return;
    }

    drawFallbackBackground(window);
}

ShopView::ShopView() = default;

bool ShopView::handleCardClick(
    sf::Vector2f mousePos,
    int cardCount,
    int& clickedIndex
) const {
    for (int i = 0; i < cardCount; ++i) {
        if (getCardRect(i).contains(mousePos)) {
            clickedIndex = i;
            return true;
        }
    }

    return false;
}

sf::FloatRect ShopView::getCardRect(int index) const {
    const int row = index / cardsPerRow_;
    const int col = index % cardsPerRow_;

    const float x = startX_ + col * (cardWidth_ + spacing_);
    const float y = startY_ + row * (cardHeight_ + spacing_ + 60.f);

    return sf::FloatRect(
        sf::Vector2f(x, y),
        sf::Vector2f(cardWidth_, cardHeight_)
    );
}

void ShopView::drawMain(
    sf::RenderWindow& window,
    GameContext& context,
    const sf::Font& font,
    const std::vector<ShopCardViewData>& cards,
    const std::string& message,
    int removeCost,
    Button& mapIconButton,
    Button& removeCardButton,
    Button& leaveButton
) const
{
    drawShopBackground(window, context.resources);

    TopInfoBar::draw(
        window,
        context,
        font,
        std::nullopt,
        true,
        true,
        false
    );

    mapIconButton.draw(window);

    sf::Text title = makeText(font, "Merchant", 52);
    title.setFillColor(sf::Color::White);
    title.setPosition({panelX_ + 45.f, panelY_ + 40.f});
    window.draw(title);

    draw(window, cards, font);

    if (!message.empty()) {
        sf::Text messageText = makeText(font, message, 24);
        messageText.setFillColor(sf::Color(240, 220, 160));
        messageText.setPosition({panelX_ + 45.f, panelY_ + 420.f});
        window.draw(messageText);
    }

    removeCardButton.setText(
        "Remove Card: " + std::to_string(removeCost) + " Gold"
    );

    removeCardButton.setTexture(context.resources.getTexture("removecard"));
    removeCardButton.draw(window);
    leaveButton.setTexture(context.resources.getTexture("cancelButton"));
    leaveButton.draw(window);
}

void ShopView::draw(
    sf::RenderWindow& window,
    const std::vector<ShopCardViewData>& cards,
    const sf::Font& font
) const
{
    for (int i = 0; i < static_cast<int>(cards.size()); ++i) {
        const ShopCardViewData& data = cards[i];

        if (data.cardDef == nullptr) {
            continue;
        }

        const sf::FloatRect rect = getCardRect(i);

        CardView cardView(
            data.cardId,
            rect.position,
            rect.size
        );

        cardView.draw(
            window,
            *data.cardDef,
            data.textures,
            font
        );

        if (data.sold) {
            sf::RectangleShape overlay;
            overlay.setSize(rect.size);
            overlay.setPosition(rect.position);
            overlay.setFillColor(sf::Color(0, 0, 0, 150));
            window.draw(overlay);

            sf::Text soldText = TextUtils::createWhiteText(
                font,
                "Sold",
                28,
                {
                    rect.position.x + rect.size.x * 0.5f - 35.f,
                    rect.position.y + rect.size.y * 0.5f - 18.f
                }
            );

            window.draw(soldText);
        }
    }
}

void ShopView::layoutButtons(
    Button& mapIconButton,
    Button& removeCardButton,
    Button& leaveButton,
    Button& cancelRemoveButton,
    Button& removePrevPageButton,
    Button& removeNextPageButton,
    const sf::RenderWindow& window
) const
{
    TopInfoBar::layoutMapButton(mapIconButton, window);

    removeCardButton.setPosition({
        panelX_ + 190.0f,
        panelY_ + 400.0f
    });
    removeCardButton.setSize({
        panelWidth_ - 340.0f,
        400.0f
    });

    leaveButton.setPosition({
        10.0f,
        850.0f
    });
    leaveButton.setSize({
        panelWidth_ - 220.0f,
        90.0f
    });

    cancelRemoveButton.setPosition({
        1540.0f,
        125.0f
    });
    cancelRemoveButton.setSize({
        250.0f,
        70.0f
    });

    removePrevPageButton.setPosition({
        620.0f,
        930.0f
    });
    removePrevPageButton.setSize({
        210.0f,
        70.0f
    });

    removeNextPageButton.setPosition({
        1090.0f,
        930.0f
    });
    removeNextPageButton.setSize({
        210.0f,
        70.0f
    });
}

bool ShopView::handleRemoveCardClick(
    sf::Vector2f mousePos,
    int deckSize,
    int removePage,
    int& deckIndex
) const
{
    deckIndex = -1;

    const int cardsPerPage = getRemoveCardsPerPage();
    const int startIndex = removePage * cardsPerPage;

    for (int i = 0; i < cardsPerPage; ++i) {
        const int currentDeckIndex = startIndex + i;

        if (currentDeckIndex >= deckSize) {
            break;
        }

        if (getRemoveCardRect(i).contains(mousePos)) {
            deckIndex = currentDeckIndex;
            return true;
        }
    }

    return false;
}

int ShopView::getRemoveCardsPerPage() const
{
    return removeColumns_ * removeRows_;
}

sf::FloatRect ShopView::getRemoveCardRect(int visibleIndex) const
{
    const int row = visibleIndex / removeColumns_;
    const int col = visibleIndex % removeColumns_;

    const float x =
        removeCardStartX_
        + static_cast<float>(col)
        * (removeCardWidth_ + removeCardGapX_);

    const float y =
        removeCardStartY_
        + static_cast<float>(row)
        * (removeCardHeight_ + removeCardGapY_);

    return sf::FloatRect(
        sf::Vector2f(x, y),
        sf::Vector2f(removeCardWidth_, removeCardHeight_)
    );
}




