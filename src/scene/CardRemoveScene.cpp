#include "CardRemoveScene.hpp"

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
    sf::Text text(font, content, size);
    return text;
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
        if (line.size() + word.size() + 1 > maxCharsPerLine) {
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
    std::vector<std::string> lines =
        wrapText(text, maxCharsPerLine);

    for (std::size_t i = 0; i < lines.size(); ++i) {
        sf::Text lineText = makeText(font, lines[i], size);
        lineText.setFillColor(color);
        lineText.setPosition(sf::Vector2f(
            position.x,
            position.y + static_cast<float>(i) * lineSpacing
        ));

        window.draw(lineText);
    }
}

}

CardRemoveScene::CardRemoveScene(GameContext& context)
    : Scene(context)
{
    if (context.runState.pendingRemoveCardCount <= 0) {
        continueAfterCardRemoval();
    }
}

void CardRemoveScene::handleEvent(
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

    if (getPrevPageRect().contains(mousePos)) {
        page_ -= 1;
        clampPage();
        return;
    }

    if (getNextPageRect().contains(mousePos)) {
        page_ += 1;
        clampPage();
        return;
    }

    const int cardsPerPage = getCardsPerPage();
    const int startIndex = page_ * cardsPerPage;
    const int deckSize =
        static_cast<int>(context.runState.masterDeck.size());

    for (int i = 0; i < cardsPerPage; ++i) {
        const int deckIndex = startIndex + i;

        if (deckIndex >= deckSize) {
            break;
        }

        if (getCardRect(i).contains(mousePos)) {
            chooseCardByDeckIndex(deckIndex);
            return;
        }
    }
}

void CardRemoveScene::update(float)
{
    clampPage();
}

void CardRemoveScene::draw(sf::RenderWindow& window)
{
    window.clear(sf::Color(32, 32, 45));

    const sf::Font& font = context.resources.getFont("zh-R");

    sf::RectangleShape topBar(sf::Vector2f(1920.0f, 90.0f));
    topBar.setPosition(sf::Vector2f(0.0f, 0.0f));
    topBar.setFillColor(sf::Color(0, 0, 0, 150));
    window.draw(topBar);

    std::ostringstream status;
    status << "Remove Card"
           << "    HP: " << context.runState.player.hp
           << " / " << context.runState.player.maxHp
           << "    Gold: " << context.runState.gold
           << "    Deck: " << context.runState.masterDeck.size();

    sf::Text statusText = makeText(font, status.str(), 28);
    statusText.setFillColor(sf::Color(245, 240, 220));
    statusText.setPosition(sf::Vector2f(60.0f, 28.0f));
    window.draw(statusText);

    std::ostringstream titleText;
    titleText << "Choose "
              << context.runState.pendingRemoveCardCount
              << " card";

    if (context.runState.pendingRemoveCardCount > 1) {
        titleText << "s";
    }

    titleText << " to remove";

    sf::Text title = makeText(font, titleText.str(), 44);
    title.setFillColor(sf::Color::White);
    title.setPosition(sf::Vector2f(100.0f, 125.0f));
    window.draw(title);

    if (!message_.empty()) {
        drawWrappedText(
            window,
            font,
            message_,
            24,
            sf::Vector2f(100.0f, 180.0f),
            32.0f,
            sf::Color(240, 220, 160),
            72
        );
    }

    const int cardsPerPage = getCardsPerPage();
    const int startIndex = page_ * cardsPerPage;
    const int deckSize =
        static_cast<int>(context.runState.masterDeck.size());

    for (int i = 0; i < cardsPerPage; ++i) {
        const int deckIndex = startIndex + i;

        if (deckIndex >= deckSize) {
            break;
        }

        const CardInstance& instance =
            context.runState.masterDeck[deckIndex];

        const bool cardExists =
            context.cards.exists(instance.cardId);

        std::string cardName = "Unknown Card";
        std::string cardDescription;
        int cardCost = 0;

        if (cardExists) {
            const CardDef& def = context.cards.get(instance.cardId);

            cardName = def.name;
            cardDescription = def.description;
            cardCost = def.cost;
        }

        const sf::FloatRect rect = getCardRect(i);

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

        drawWrappedText(
            window,
            font,
            cardDescription,
            18,
            sf::Vector2f(
                rect.position.x + 18.0f,
                rect.position.y + 88.0f
            ),
            25.0f,
            sf::Color(225, 225, 225),
            24
        );
    }

    const sf::FloatRect prevRect = getPrevPageRect();
    sf::RectangleShape prevButton(prevRect.size);
    prevButton.setPosition(prevRect.position);
    prevButton.setFillColor(sf::Color(80, 80, 120));
    window.draw(prevButton);

    sf::Text prevText = makeText(font, "Previous", 24);
    prevText.setFillColor(sf::Color::White);
    prevText.setPosition(sf::Vector2f(
        prevRect.position.x + 42.0f,
        prevRect.position.y + 20.0f
    ));
    window.draw(prevText);

    const sf::FloatRect nextRect = getNextPageRect();
    sf::RectangleShape nextButton(nextRect.size);
    nextButton.setPosition(nextRect.position);
    nextButton.setFillColor(sf::Color(80, 80, 120));
    window.draw(nextButton);

    sf::Text nextText = makeText(font, "Next", 24);
    nextText.setFillColor(sf::Color::White);
    nextText.setPosition(sf::Vector2f(
        nextRect.position.x + 72.0f,
        nextRect.position.y + 20.0f
    ));
    window.draw(nextText);

    std::ostringstream pageTextStream;
    pageTextStream << "Page "
                   << page_ + 1
                   << " / "
                   << getPageCount();

    sf::Text pageText = makeText(font, pageTextStream.str(), 24);
    pageText.setFillColor(sf::Color(230, 230, 230));
    pageText.setPosition(sf::Vector2f(880.0f, 956.0f));
    window.draw(pageText);
}

SceneTransition CardRemoveScene::getTransition() const
{
    return transition_;
}

sf::FloatRect CardRemoveScene::getCardRect(int visibleIndex) const
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

sf::FloatRect CardRemoveScene::getPrevPageRect() const
{
    return sf::FloatRect{
        sf::Vector2f(620.0f, 930.0f),
        sf::Vector2f(210.0f, 70.0f)
    };
}

sf::FloatRect CardRemoveScene::getNextPageRect() const
{
    return sf::FloatRect{
        sf::Vector2f(1090.0f, 930.0f),
        sf::Vector2f(210.0f, 70.0f)
    };
}

int CardRemoveScene::getCardsPerPage() const
{
    return 15;
}

int CardRemoveScene::getPageCount() const
{
    const int deckSize =
        static_cast<int>(context.runState.masterDeck.size());

    const int cardsPerPage = getCardsPerPage();

    if (deckSize <= 0) {
        return 1;
    }

    return (deckSize + cardsPerPage - 1) / cardsPerPage;
}

void CardRemoveScene::clampPage()
{
    const int pageCount = getPageCount();

    if (page_ < 0) {
        page_ = 0;
    }

    if (page_ >= pageCount) {
        page_ = pageCount - 1;
    }
}

void CardRemoveScene::chooseCardByDeckIndex(int deckIndex)
{
    if (
        deckIndex < 0 ||
        deckIndex >= static_cast<int>(context.runState.masterDeck.size())
    ) {
        return;
    }

    const CardInstanceId instanceId =
        context.runState.masterDeck[deckIndex].instanceId;

    const ErrorCode code =
        eventSystem_.removeCardByInstanceId(
            context.runState,
            instanceId
        );

    if (code != ErrorCode::OK) {
        message_ = "Failed to remove card.";
        return;
    }

    message_ = "Removed a card.\n";

    clampPage();

    if (context.runState.pendingRemoveCardCount <= 0) {
        continueAfterCardRemoval();
    }
}

void CardRemoveScene::continueAfterCardRemoval()
{
    EventResolveResult result =
        eventSystem_.resolvePendingEventEffects(
            context.runState,
            context.cards
        );

    handleResolveResult(result);
}

void CardRemoveScene::handleResolveResult(
    const EventResolveResult& result
)
{
    if (!result.message.empty()) {
        message_ += result.message;
    }

    if (result.error != ErrorCode::OK) {
        return;
    }

    if (result.playerDead) {
        transition_.target = SceneType::End;
        transition_.battleResult = BattleResult::Defeat;
        return;
    }

    if (result.requiresCardRemove) {
        return;
    }

    if (result.startCombat) {
        EnemyId enemyId = context.enemies.chooseEnemyIdByFloor(
            context.runState.floor,
            context.runState.rng
        );

        context.runState.currentEnemyId = enemyId;

        transition_.target = SceneType::Combat;
        transition_.enemyId = enemyId;
        return;
    }

    finishNormalEventNode();
}

void CardRemoveScene::finishNormalEventNode()
{
    mapSystem_.completeSelectedNode(context.runState);

    if (mapSystem_.isRouteFinished(context.runState)) {
        transition_.target = SceneType::End;
        transition_.battleResult = BattleResult::Victory;
    } else {
        transition_.target = SceneType::Map;
    }
}
