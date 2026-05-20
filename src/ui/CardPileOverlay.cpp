#include "CardPileOverlay.hpp"

#include "CardView.hpp"
#include "model/CardDef.hpp"

#include <algorithm>
#include <sstream>

#include "TextUtils.hpp"

namespace {

    sf::Text makeText(
            const sf::Font& font,
            const std::string& content,
            unsigned int size
        ) {
        return sf::Text(font, TextUtils::fromUtf8(content), size);
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

sf::Vector2f getViewTopLeft(
    const sf::RenderWindow& window
)
{
    const sf::View& view = window.getView();
    const sf::Vector2f center = view.getCenter();
    const sf::Vector2f size = view.getSize();

    return sf::Vector2f(
        center.x - size.x * 0.5f,
        center.y - size.y * 0.5f
    );
}

sf::Vector2f getViewSize(
    const sf::RenderWindow& window
)
{
    return window.getView().getSize();
}

void drawButton(
    sf::RenderWindow& window,
    const sf::Font& font,
    const sf::FloatRect& rect,
    const std::string& text,
    sf::Color fillColor
)
{
    sf::RectangleShape button(rect.size);
    button.setPosition(rect.position);
    button.setFillColor(fillColor);
    button.setOutlineThickness(2.0f);
    button.setOutlineColor(sf::Color(240, 210, 180));
    window.draw(button);

    sf::Text buttonText = makeText(font, text, 24);
    buttonText.setFillColor(sf::Color::White);

    const sf::FloatRect bounds =
        buttonText.getLocalBounds();

    buttonText.setPosition(sf::Vector2f(
        rect.position.x +
            rect.size.x * 0.5f -
            bounds.size.x * 0.5f,
        rect.position.y +
            rect.size.y * 0.5f -
            bounds.size.y * 0.5f -
            6.0f
    ));

    window.draw(buttonText);
}

}

void CardPileOverlay::open(
    const std::string& title,
    const std::vector<PileCardViewData>& cards
)
{
    title_ = title;
    cards_ = cards;
    page_ = 0;
    clampPage();
}


void CardPileOverlay::close()
{
    cards_.clear();
    title_.clear();
    page_ = 0;
}

bool CardPileOverlay::isOpen() const
{
    return !title_.empty();
}

bool CardPileOverlay::handleEvent(
    const sf::Event& event,
    const sf::RenderWindow& window
)
{
    if (!isOpen()) {
        return false;
    }

    sf::Vector2i pixelPosition;

    if (!readLeftClickPosition(event, pixelPosition)) {
        return true;
    }

    const sf::Vector2f mousePos =
        window.mapPixelToCoords(pixelPosition);

    if (getCloseRect(window).contains(mousePos)) {
        close();
        return true;
    }

    if (getPrevRect(window).contains(mousePos)) {
        page_ -= 1;
        clampPage();
        return true;
    }

    if (getNextRect(window).contains(mousePos)) {
        page_ += 1;
        clampPage();
        return true;
    }

    return true;
}

void CardPileOverlay::draw(
    sf::RenderWindow& window,
    const sf::Font& font
) const
{
    if (!isOpen()) {
        return;
    }

    const sf::Vector2f viewTopLeft = getViewTopLeft(window);
    const sf::Vector2f viewSize = getViewSize(window);

    sf::RectangleShape dim(viewSize);
    dim.setPosition(viewTopLeft);
    dim.setFillColor(sf::Color(0, 0, 0, 170));
    window.draw(dim);

    const sf::FloatRect panelRect = getPanelRect(window);

    sf::RectangleShape panel(panelRect.size);
    panel.setPosition(panelRect.position);
    panel.setFillColor(sf::Color(35, 35, 50, 245));
    panel.setOutlineThickness(3.0f);
    panel.setOutlineColor(sf::Color(230, 220, 180));
    window.draw(panel);

    const int cardCount =
        static_cast<int>(cards_.size());

    std::ostringstream titleStream;
    titleStream << title_ << "    Cards: " << cardCount;

    sf::Text titleText =
        makeText(font, titleStream.str(), 42);

    titleText.setFillColor(sf::Color::White);
    titleText.setPosition(sf::Vector2f(
        panelRect.position.x + 40.0f,
        panelRect.position.y + 30.0f
    ));
    window.draw(titleText);

    drawButton(
        window,
        font,
        getCloseRect(window),
        "Close",
        sf::Color(120, 65, 65)
    );

    if (cardCount <= 0) {
        sf::Text emptyText =
            makeText(font, "No cards.", 32);

        emptyText.setFillColor(sf::Color(230, 230, 230));
        emptyText.setPosition(sf::Vector2f(
            panelRect.position.x + 40.0f,
            panelRect.position.y + 130.0f
        ));
        window.draw(emptyText);
    }

    const int cardsPerPage = getCardsPerPage();
    const int startIndex = page_ * cardsPerPage;
    const int endIndex = std::min(
        startIndex + cardsPerPage,
        static_cast<int>(cards_.size())
    );

    for (int cardIndex = startIndex;
         cardIndex < endIndex;
         ++cardIndex) {
        const int visibleIndex =
            cardIndex - startIndex;

        const PileCardViewData& data =
            cards_[cardIndex];

        if (data.cardDef == nullptr) {
            continue;
        }

        const sf::FloatRect rect =
            getCardRect(window, visibleIndex);

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
         }


    const bool hasPrev =
        page_ > 0;

    const bool hasNext =
        page_ + 1 < getPageCount();

    drawButton(
        window,
        font,
        getPrevRect(window),
        "Previous",
        hasPrev
            ? sf::Color(80, 80, 120)
            : sf::Color(55, 55, 70)
    );

    drawButton(
        window,
        font,
        getNextRect(window),
        "Next",
        hasNext
            ? sf::Color(80, 80, 120)
            : sf::Color(55, 55, 70)
    );

    std::ostringstream pageStream;
    pageStream << "Page "
               << page_ + 1
               << " / "
               << getPageCount();

    sf::Text pageText =
        makeText(font, pageStream.str(), 24);

    pageText.setFillColor(sf::Color(230, 230, 230));

    const sf::FloatRect pageBounds =
        pageText.getLocalBounds();

    pageText.setPosition(sf::Vector2f(
        panelRect.position.x +
            panelRect.size.x * 0.5f -
            pageBounds.size.x * 0.5f,
        panelRect.position.y +
            panelRect.size.y -
            72.0f
    ));

    window.draw(pageText);
}

int CardPileOverlay::getCardsPerPage() const
{
    return 5;
}

int CardPileOverlay::getPageCount() const
{
    if (cards_.empty()) {
        return 1;
    }

    const int cardCount =
        static_cast<int>(cards_.size());

    const int cardsPerPage =
        getCardsPerPage();

    return (cardCount + cardsPerPage - 1) /
           cardsPerPage;
}

void CardPileOverlay::clampPage()
{
    const int pageCount =
        getPageCount();

    if (page_ < 0) {
        page_ = 0;
    }

    if (page_ >= pageCount) {
        page_ = pageCount - 1;
    }
}

sf::FloatRect CardPileOverlay::getPanelRect(
    const sf::RenderWindow& window
) const
{
    const sf::Vector2f viewTopLeft =
        getViewTopLeft(window);

    const sf::Vector2f viewSize =
        getViewSize(window);

    return sf::FloatRect{
        sf::Vector2f(
            viewTopLeft.x + 90.0f,
            viewTopLeft.y + 80.0f
        ),
        sf::Vector2f(
            viewSize.x - 180.0f,
            viewSize.y - 150.0f
        )
    };
}

sf::FloatRect CardPileOverlay::getCloseRect(
    const sf::RenderWindow& window
) const
{
    const sf::FloatRect panel =
        getPanelRect(window);

    return sf::FloatRect{
        sf::Vector2f(
            panel.position.x +
                panel.size.x -
                170.0f,
            panel.position.y + 28.0f
        ),
        sf::Vector2f(130.0f, 58.0f)
    };
}

sf::FloatRect CardPileOverlay::getPrevRect(
    const sf::RenderWindow& window
) const
{
    const sf::FloatRect panel =
        getPanelRect(window);

    return sf::FloatRect{
        sf::Vector2f(
            panel.position.x +
                panel.size.x * 0.5f -
                250.0f,
            panel.position.y +
                panel.size.y -
                90.0f
        ),
        sf::Vector2f(180.0f, 62.0f)
    };
}

sf::FloatRect CardPileOverlay::getNextRect(
    const sf::RenderWindow& window
) const
{
    const sf::FloatRect panel =
        getPanelRect(window);

    return sf::FloatRect{
        sf::Vector2f(
            panel.position.x +
                panel.size.x * 0.5f +
                70.0f,
            panel.position.y +
                panel.size.y -
                90.0f
        ),
        sf::Vector2f(180.0f, 62.0f)
    };
}

sf::FloatRect CardPileOverlay::getCardRect(
    const sf::RenderWindow& window,
    int visibleIndex
) const
{
    const sf::FloatRect panel =
        getPanelRect(window);

    constexpr int columns = 5;

    const int column =
        visibleIndex % columns;

    const float cardWidth = 294.0f;
    const float cardHeight = 414.0f;
    const float gapX = 28.0f;

    const float totalWidth =
        static_cast<float>(columns) * cardWidth +
        static_cast<float>(columns - 1) * gapX;

    const float startX =
        panel.position.x +
        panel.size.x * 0.5f -
        totalWidth * 0.5f;

    const float startY =
        panel.position.y + 145.0f;

    return sf::FloatRect{
        sf::Vector2f(
            startX +
                static_cast<float>(column) *
                    (cardWidth + gapX),
            startY
        ),
        sf::Vector2f(
            cardWidth,
            cardHeight
        )
    };
}
