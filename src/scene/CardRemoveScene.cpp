#include "CardRemoveScene.hpp"

#include "database/CardDatabase.hpp"
#include "model/CardDef.hpp"

#include <algorithm>
#include <sstream>
#include <vector>

#include "ui/TextUtils.hpp"
#include "ui/TopInfoBar.hpp"

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

CardRemoveScene::CardRemoveScene(
    GameContext& context,
    CardRemoveSceneMode mode
)
: Scene(context),
  mapIconButton_(
      sf::Vector2f(0.0f, 0.0f),
      sf::Vector2f(64.0f, 64.0f),
      context.resources.getFont("zh-R"),
      ""
  ),
  mode_(mode)
{
    mapIconButton_.setTexture(
        context.resources.getTexture("map")
    );

    if (context.runState.pendingRemoveCardCount <= 0) {
        if (mode_ == CardRemoveSceneMode::Embedded) {
            finished_ = true;
        } else {
            continueAfterCardRemoval();
        }
    }
}


void CardRemoveScene::handleEvent(
    const sf::Event& event,
    const sf::RenderWindow& window
)
{
    TopInfoBar::layoutMapButton(mapIconButton_, window);
    mapIconButton_.handleEvent(event, window);

    if (mapIconButton_.wasClicked()) {

        if (mode_ == CardRemoveSceneMode::Embedded) {
            finished_ = true;
        } else {
            transition_.openMapPreview = true;
            transition_.target = SceneType::Map;
        }

        mapIconButton_.reset();
        return;
    }



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
        context.audio.playSound("Click");
        page_ -= 1;
        clampPage();
        return;
    }

    if (getNextPageRect().contains(mousePos)) {
        context.audio.playSound("Click");
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

        const CardInstance& instance =
    context.runState.masterDeck[deckIndex];

        if (!context.cards.exists(instance.cardId)) {
            continue;
        }

        const sf::FloatRect rect = getCardRect(i);

        CardView cardView(
            instance.cardId,
            rect.position,
            rect.size
        );

        if (cardView.contains(mousePos)) {
            context.audio.playSound("Click");
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

    TopInfoBar::draw(
    window,
    context,
    font,
    std::nullopt,
    true,
    true,
    false // map 图标由按钮绘制
);

    TopInfoBar::layoutMapButton(mapIconButton_, window);
    mapIconButton_.draw(window);



    std::ostringstream titleText;
    titleText << "选择 "
              << context.runState.pendingRemoveCardCount
              << " 卡牌";

    if (context.runState.pendingRemoveCardCount > 1) {
        titleText << "s";
    }

    titleText << "丢弃";

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

        if (!context.cards.exists(instance.cardId)) {
            continue;
        }

        const CardDef& def =
            context.cards.get(instance.cardId);

        const CardRenderTextures textures =
            getCardRenderTextures(def);

        const sf::FloatRect rect =
            getCardRect(i);

        CardView cardView(
            instance.cardId,
            rect.position,
            rect.size
        );

        cardView.draw(
            window,
            def,
            textures,
            font
        );

    }

    const sf::FloatRect prevRect = getPrevPageRect();
    sf::RectangleShape prevButton(prevRect.size);
    prevButton.setPosition(prevRect.position);
    prevButton.setTexture(&context.resources.getTexture("enabledButton"));
    window.draw(prevButton);

    sf::Text prevText = makeText(font, "上一页", 24);
    prevText.setFillColor(sf::Color::White);
    prevText.setPosition(sf::Vector2f(
        prevRect.position.x + 42.0f,
        prevRect.position.y + 20.0f
    ));
    window.draw(prevText);

    const sf::FloatRect nextRect = getNextPageRect();
    sf::RectangleShape nextButton(nextRect.size);
    nextButton.setPosition(nextRect.position);
    nextButton.setTexture(&context.resources.getTexture("enabledButton"));
    window.draw(nextButton);

    sf::Text nextText = makeText(font, "下一页", 24);
    nextText.setFillColor(sf::Color::White);
    nextText.setPosition(sf::Vector2f(
        nextRect.position.x + 72.0f,
        nextRect.position.y + 20.0f
    ));
    window.draw(nextText);

    std::ostringstream pageTextStream;
    pageTextStream << "页数 "
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
    constexpr int columns = 4;

    const int row = visibleIndex / columns;
    const int column = visibleIndex % columns;

    constexpr float cardWidth = 220.0f;
    constexpr float cardHeight = 310.0f;
    constexpr float gapX = 45.0f;
    constexpr float gapY = 40.0f;

    constexpr float startX = 450.0f;
    constexpr float startY = 245.0f;

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
    return 8;
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
        context.failureToast.show("删除失败");
        return;
    }


    message_ = "已经删除.\n";

    clampPage();

    if (context.runState.pendingRemoveCardCount <= 0) {
        if (mode_ == CardRemoveSceneMode::Embedded) {
            finished_ = true;
        } else {
            continueAfterCardRemoval();
        }
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
        page_ = 0;
        clampPage();
        return;
    }

    if (result.startCombat) {
        EncounterId encounterId =
            context.encounters.chooseEncounterIdByAct(
    context.runState.act,
    context.runState.rng
);


        context.runState.currentEncounterId = encounterId;

        transition_.target = SceneType::Combat;
        transition_.encounterId = encounterId;
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

void CardRemoveScene::resetTransition()
{
    transition_ = SceneTransition{};
}

bool CardRemoveScene::isFinished() const
{
    return finished_;
}

const sf::Texture& CardRemoveScene::getCardTemplateTexture(
    CardType type
) const
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

const sf::Texture* CardRemoveScene::getCardArtTexture(
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

CardRenderTextures CardRemoveScene::getCardRenderTextures(
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
