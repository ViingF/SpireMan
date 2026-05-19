
#include "EventScene.hpp"

#include <SFML/Config.hpp>

#include <sstream>
#include <string>
#include <vector>

#include "ui/TextUtils.hpp"

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

void drawTextureCover(
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

    const float targetWidth = 1080.0f;
    const float targetHeight = 1080.0f;

    const float scaleX =
        targetWidth / static_cast<float>(textureSize.x);

    const float scaleY =
        targetHeight / static_cast<float>(textureSize.y);

    sprite.setScale(sf::Vector2f(scaleX, scaleY));

    sprite.setPosition(viewTopLeft);

    window.draw(sprite);
}

void drawDarkOverlay(sf::RenderWindow& window)
{
    sf::RectangleShape overlay(getViewSize(window));
    overlay.setPosition(getViewTopLeft(window));
    overlay.setFillColor(sf::Color(0, 0, 0, 120));
    window.draw(overlay);
}

void drawFallbackBackground(sf::RenderWindow& window)
{
    sf::RectangleShape background(getViewSize(window));
    background.setPosition(getViewTopLeft(window));
    background.setFillColor(sf::Color(28, 25, 35));
    window.draw(background);
}

void drawEventBackground(
    sf::RenderWindow& window,
    ResourceManager& resources,
    const EventDef& eventDef
)
{
    if (
        !eventDef.backgroundTextureId.empty() &&
        resources.hasTexture(eventDef.backgroundTextureId)
    ) {
        drawTextureCover(
            window,
            resources.getTexture(eventDef.backgroundTextureId)
        );

        drawDarkOverlay(window);
        return;
    }

    drawFallbackBackground(window);
}

}

EventScene::EventScene(GameContext& context, EventId eventId)
    : Scene(context)
    , eventId_(eventId)
{
    createChoiceButtons();
    syncChoiceButtons();
}


void EventScene::handleEvent(
    const sf::Event& event,
    const sf::RenderWindow& window
)
{
    if (transition_.target != SceneType::None || resolved_) {
        return;
    }

    const EventDef& eventDef = context.events.get(eventId_);

    syncChoiceButtons();

    for (Button& button : choiceButtons_) {
        button.handleEvent(event, window);
    }

    for (int i = 0; i < static_cast<int>(choiceButtons_.size()); ++i) {
        if (choiceButtons_[i].wasClicked()) {
            choiceButtons_[i].reset();
            choose(i);
            return;
        }
    }

    sf::Vector2i pixelPosition;
    if (!readLeftClickPosition(event, pixelPosition)) {
        return;
    }

    const sf::Vector2f mousePos =
        window.mapPixelToCoords(pixelPosition);

    for (int i = 0; i < static_cast<int>(eventDef.choices.size()); ++i) {
        const EventChoiceDef& choice = eventDef.choices[i];

        if (!canChoose(choice) && getChoiceRect(i).contains(mousePos)) {
            resultMessage_ = "Not enough gold.";
            return;
        }
    }
}


void EventScene::update(float)
{
}

void EventScene::draw(sf::RenderWindow& window)
{
    const sf::Font& font = context.resources.getFont("zh-R");
    const EventDef& eventDef = context.events.get(eventId_);

    drawEventBackground(
        window,
        context.resources,
        eventDef
    );

    const sf::View& view = window.getView();
    const sf::Vector2f viewCenter = view.getCenter();
    const sf::Vector2f viewSize = view.getSize();

    const sf::Vector2f viewTopLeft(
        viewCenter.x - viewSize.x * 0.5f,
        viewCenter.y - viewSize.y * 0.5f
    );

    sf::RectangleShape topBar(sf::Vector2f(viewSize.x, 86.0f));
    topBar.setPosition(viewTopLeft);
    topBar.setFillColor(sf::Color(0, 0, 0, 135));
    window.draw(topBar);

    std::ostringstream status;
    status << "Event"
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

    const float panelX = 1130.0f;
    const float panelY = 120.0f;
    const float panelWidth = 850.0f;
    const float panelHeight = 900.0f;

    sf::RectangleShape panel(sf::Vector2f(panelWidth, panelHeight));
    panel.setPosition(sf::Vector2f(panelX, panelY));
    panel.setFillColor(sf::Color(0, 0, 0, 125));
    window.draw(panel);

    sf::Text title = makeText(font, eventDef.title, 46);
    title.setFillColor(sf::Color::White);
    title.setPosition(sf::Vector2f(panelX + 40.0f, panelY + 35.0f));
    window.draw(title);

    drawWrappedText(
        window,
        font,
        eventDef.description,
        28,
        sf::Vector2f(panelX + 40.0f, panelY + 115.0f),
        38.0f,
        sf::Color(230, 230, 230),
        34
    );

    for (int i = 0; i < static_cast<int>(eventDef.choices.size()); ++i) {
        const sf::FloatRect rect = getChoiceRect(i);

        const EventChoiceDef& choice = eventDef.choices[i];
        const bool enabled = canChoose(choice);

        if (i < static_cast<int>(choiceButtons_.size())) {
            choiceButtons_[i].draw(window);
        }



        sf::Text choiceText = makeText(font, choice.text, 28);
        choiceText.setFillColor(sf::Color::White);
        choiceText.setPosition(sf::Vector2f(
            rect.position.x + 24.0f,
            rect.position.y + 14.0f
        ));
        window.draw(choiceText);

        sf::Text descText = makeText(font, choice.description, 21);
        descText.setFillColor(sf::Color(215, 215, 215));
        descText.setPosition(sf::Vector2f(
            rect.position.x + 24.0f,
            rect.position.y + 56.0f
        ));
        window.draw(descText);
    }

    if (!resultMessage_.empty()) {
        drawWrappedText(
            window,
            font,
            resultMessage_,
            24,
            sf::Vector2f(panelX + 40.0f, panelY + 780.0f),
            32.0f,
            sf::Color(240, 220, 160),
            36
        );
    }
}


SceneTransition EventScene::getTransition() const
{
    return transition_;
}

sf::FloatRect EventScene::getChoiceRect(int index) const
{
    const float x = 1180.0f;
    const float y = 610.0f + static_cast<float>(index) * 125.0f;
    const float width = 660.0f;
    const float height = 96.0f;

    return sf::FloatRect{
        sf::Vector2f(x, y),
        sf::Vector2f(width, height)
    };
}




void EventScene::choose(int choiceIndex)
{
    const EventDef& eventDef = context.events.get(eventId_);

    EventResolveResult result =
        eventSystem_.resolveChoice(
            context.runState,
            eventDef,
            choiceIndex,
            context.cards
        );

    if (result.error != ErrorCode::OK) {
        resultMessage_ = result.message;
        return;
    }

    resultMessage_ = result.message;
    resolved_ = true;

    if (result.playerDead) {
        transition_.target = SceneType::End;
        transition_.battleResult = BattleResult::Defeat;
        return;
    }

    if (result.requiresCardRemove) {
        transition_.target = SceneType::CardRemove;
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

    mapSystem_.completeSelectedNode(context.runState);

    if (mapSystem_.isRouteFinished(context.runState)) {
        if (mapSystem_.advanceToNextActIfPossible(
                context.runState,
                context.events,
                context.encounters
            )) {
            transition_.target = SceneType::Map;
            } else {
                transition_.target = SceneType::End;
                transition_.battleResult = BattleResult::Victory;
            }
    } else {
        transition_.target = SceneType::Map;
    }

}

bool EventScene::canChoose(const EventChoiceDef& choice) const
{
    int requiredGold = 0;
    bool removesCard = false;

    for (const EventEffect& effect : choice.effects) {
        if (effect.type == EventEffectType::LoseGold) {
            requiredGold += std::max(0, effect.value);
        }

        if (effect.type == EventEffectType::RemoveCard) {
            removesCard = true;
        }
    }

    if (removesCard && context.runState.gold < requiredGold) {
        return false;
    }

    return true;
}

void EventScene::createChoiceButtons()
{
    const EventDef& eventDef = context.events.get(eventId_);

    choiceButtons_.clear();
    choiceButtons_.reserve(eventDef.choices.size());

    for (int i = 0; i < static_cast<int>(eventDef.choices.size()); ++i) {
        const sf::FloatRect rect = getChoiceRect(i);

        choiceButtons_.emplace_back(
            rect.position,
            rect.size,
            context.resources.getFont("zh-R"),
            ""
        );
    }
}

void EventScene::syncChoiceButtons()
{
    const EventDef& eventDef = context.events.get(eventId_);

    const int count = std::min(
        static_cast<int>(choiceButtons_.size()),
        static_cast<int>(eventDef.choices.size())
    );

    for (int i = 0; i < count; ++i) {
        choiceButtons_[i].setEnabled(canChoose(eventDef.choices[i]));
    }
}
