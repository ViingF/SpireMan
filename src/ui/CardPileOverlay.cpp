#include "CardPileOverlay.hpp"

#include "database/CardDatabase.hpp"
#include "model/CardDef.hpp"

#include <algorithm>
#include <sstream>
#include <vector>

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

std::vector<std::string> wrapText(
    const std::string& text,
    std::size_t maxCharsPerLine
)
{
    std::vector<std::string> lines;
    std::istringstream input(text);

    std::string word;
    std::string line;

    while (input >> word) {
        if (
            line.size() + word.size() + 1 >
            maxCharsPerLine
        ) {
            if (!line.empty()) {
                lines.push_back(line);
                line.clear();
            }
        }

        if (!line.empty()) {
            line += " ";
        }

        line += word;
    }

    if (!line.empty()) {
        lines.push_back(line);
    }

    return lines;
}

void drawWrappedText(
    sf::RenderWindow& window,
    const sf::Font& font,
    const std::string& text,
    unsigned int size,
    sf::Vector2f position,
    float lineSpacing,
    sf::Color color,
    std::size_t maxCharsPerLine
)
{
    const std::vector<std::string> lines =
        wrapText(text, maxCharsPerLine);

    for (std::size_t i = 0; i < lines.size(); ++i) {
        sf::Text lineText =
            makeText(font, lines[i], size);

        lineText.setFillColor(color);
        lineText.setPosition(sf::Vector2f(
            position.x,
            position.y +
                static_cast<float>(i) * lineSpacing
        ));

        window.draw(lineText);
    }
}

}

void CardPileOverlay::open(
    const std::string& title,
    const std::vector<CardInstance>* cards
)
{
    title_ = title;
    cards_ = cards;
    page_ = 0;
    clampPage();
}

void CardPileOverlay::close()
{
    cards_ = nullptr;
    title_.clear();
    page_ = 0;
}

bool CardPileOverlay::isOpen() const
{
    return cards_ != nullptr;
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
        // 弹窗打开时，非点击事件也不需要让底层场景处理
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
    const sf::Font& font,
    const CardDatabase& cardDatabase
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
        cards_ == nullptr
            ? 0
            : static_cast<int>(cards_->size());

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

    const sf::FloatRect closeRect = getCloseRect(window);

    sf::RectangleShape closeButton(closeRect.size);
    closeButton.setPosition(closeRect.position);
    closeButton.setFillColor(sf::Color(120, 65, 65));
    closeButton.setOutlineThickness(2.0f);
    closeButton.setOutlineColor(sf::Color(240, 210, 180));
    window.draw(closeButton);

    sf::Text closeText = makeText(font, "Close", 24);
    closeText.setFillColor(sf::Color::White);
    closeText.setPosition(sf::Vector2f(
        closeRect.position.x + 35.0f,
        closeRect.position.y + 16.0f
    ));
    window.draw(closeText);

    if (cardCount <= 0) {
        sf::Text emptyText =
            makeText(font, "No cards.", 32);

        emptyText.setFillColor(sf::Color(230, 230, 230));
        emptyText.setPosition(sf::Vector2f(
            panelRect.position.x + 40.0f,
            panelRect.position.y + 120.0f
        ));
        window.draw(emptyText);
    }

    const int cardsPerPage = getCardsPerPage();
    const int startIndex = page_ * cardsPerPage;

    for (int i = 0; i < cardsPerPage; ++i) {
        const int cardIndex = startIndex + i;

        if (cardIndex >= cardCount) {
            break;
        }

        const CardInstance& instance = (*cards_)[cardIndex];

        std::string cardName = "Unknown Card";
        std::string cardDescription;
        int cost = 0;

        if (cardDatabase.exists(instance.cardId)) {
            const CardDef& def =
                cardDatabase.get(instance.cardId);

            cardName = def.name;
            cardDescription = def.description;
            cost = def.cost;
        }

        const sf::FloatRect rect =
            getCardRect(window, i);

        sf::RectangleShape cardShape(rect.size);
        cardShape.setPosition(rect.position);
        cardShape.setFillColor(sf::Color(70, 70, 95));
        cardShape.setOutlineThickness(3.0f);
        cardShape.setOutlineColor(sf::Color(220, 210, 170));
        window.draw(cardShape);

        sf::Text nameText = makeText(font, cardName, 25);
        nameText.setFillColor(sf::Color::White);
        nameText.setPosition(sf::Vector2f(
            rect.position.x + 18.0f,
            rect.position.y + 14.0f
        ));
        window.draw(nameText);

        std::ostringstream meta;
        meta << "Cost: " << cost
             << "    ID: " << instance.instanceId;

        sf::Text metaText = makeText(font, meta.str(), 18);
        metaText.setFillColor(sf::Color(210, 210, 210));
        metaText.setPosition(sf::Vector2f(
            rect.position.x + 18.0f,
            rect.position.y + 48.0f
        ));
        window.draw(metaText);

        drawWrappedText(
            window,
            font,
            cardDescription,
            18,
            sf::Vector2f(
                rect.position.x + 18.0f,
                rect.position.y + 80.0f
            ),
            24.0f,
            sf::Color(225, 225, 225),
            24
        );
    }

    const sf::FloatRect prevRect = getPrevRect(window);
    const sf::FloatRect nextRect = getNextRect(window);

    sf::RectangleShape prevButton(prevRect.size);
    prevButton.setPosition(prevRect.position);
    prevButton.setFillColor(sf::Color(80, 80, 120));
    window.draw(prevButton);

    sf::Text prevText = makeText(font, "Previous", 24);
    prevText.setFillColor(sf::Color::White);
    prevText.setPosition(sf::Vector2f(
        prevRect.position.x + 36.0f,
        prevRect.position.y + 18.0f
    ));
    window.draw(prevText);

    sf::RectangleShape nextButton(nextRect.size);
    nextButton.setPosition(nextRect.position);
    nextButton.setFillColor(sf::Color(80, 80, 120));
    window.draw(nextButton);

    sf::Text nextText = makeText(font, "Next", 24);
    nextText.setFillColor(sf::Color::White);
    nextText.setPosition(sf::Vector2f(
        nextRect.position.x + 62.0f,
        nextRect.position.y + 18.0f
    ));
    window.draw(nextText);

    std::ostringstream pageStream;
    pageStream << "Page "
               << page_ + 1
               << " / "
               << getPageCount();

    sf::Text pageText = makeText(font, pageStream.str(), 24);
    pageText.setFillColor(sf::Color(230, 230, 230));
    pageText.setPosition(sf::Vector2f(
        panelRect.position.x + panelRect.size.x * 0.5f - 60.0f,
        panelRect.position.y + panelRect.size.y - 72.0f
    ));
    window.draw(pageText);
}

int CardPileOverlay::getCardsPerPage() const
{
    return 15;
}

int CardPileOverlay::getPageCount() const
{
    if (cards_ == nullptr || cards_->empty()) {
        return 1;
    }

    const int cardCount =
        static_cast<int>(cards_->size());

    const int cardsPerPage = getCardsPerPage();

    return (cardCount + cardsPerPage - 1) /
           cardsPerPage;
}

void CardPileOverlay::clampPage()
{
    const int pageCount = getPageCount();

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
    const sf::Vector2f viewTopLeft = getViewTopLeft(window);
    const sf::Vector2f viewSize = getViewSize(window);

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
    const sf::FloatRect panel = getPanelRect(window);

    return sf::FloatRect{
        sf::Vector2f(
            panel.position.x + panel.size.x - 170.0f,
            panel.position.y + 28.0f
        ),
        sf::Vector2f(130.0f, 58.0f)
    };
}

sf::FloatRect CardPileOverlay::getPrevRect(
    const sf::RenderWindow& window
) const
{
    const sf::FloatRect panel = getPanelRect(window);

    return sf::FloatRect{
        sf::Vector2f(
            panel.position.x + 520.0f,
            panel.position.y + panel.size.y - 90.0f
        ),
        sf::Vector2f(180.0f, 62.0f)
    };
}

sf::FloatRect CardPileOverlay::getNextRect(
    const sf::RenderWindow& window
) const
{
    const sf::FloatRect panel = getPanelRect(window);

    return sf::FloatRect{
        sf::Vector2f(
            panel.position.x + panel.size.x - 700.0f,
            panel.position.y + panel.size.y - 90.0f
        ),
        sf::Vector2f(180.0f, 62.0f)
    };
}

sf::FloatRect CardPileOverlay::getCardRect(
    const sf::RenderWindow& window,
    int visibleIndex
) const
{
    const sf::FloatRect panel = getPanelRect(window);

    constexpr int columns = 5;

    const int row = visibleIndex / columns;
    const int column = visibleIndex % columns;

    const float cardWidth = 300.0f;
    const float cardHeight = 180.0f;
    const float gapX = 36.0f;
    const float gapY = 30.0f;

    const float startX = panel.position.x + 45.0f;
    const float startY = panel.position.y + 125.0f;

    return sf::FloatRect{
        sf::Vector2f(
            startX +
                static_cast<float>(column) *
                    (cardWidth + gapX),
            startY +
                static_cast<float>(row) *
                    (cardHeight + gapY)
        ),
        sf::Vector2f(cardWidth, cardHeight)
    };
}
