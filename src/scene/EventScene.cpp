
#include "EventScene.hpp"

#include <SFML/Config.hpp>

#include <sstream>
#include <string>
#include <vector>

namespace {

sf::Text makeText(
    const sf::Font& font,
    const std::string& content,
    unsigned int size
)
{
#if SFML_VERSION_MAJOR >= 3
    sf::Text text(font, content, size);
#else
    sf::Text text;
    text.setFont(font);
    text.setString(content);
    text.setCharacterSize(size);
#endif
    return text;
}

bool readLeftClickPosition(
    const sf::Event& event,
    sf::Vector2i& pixelPosition
)
{
#if SFML_VERSION_MAJOR >= 3
    if (const auto* mouse =
            event.getIf<sf::Event::MouseButtonPressed>()) {
        if (mouse->button == sf::Mouse::Button::Left) {
            pixelPosition = mouse->position;
            return true;
        }
    }
#else
    if (event.type == sf::Event::MouseButtonPressed &&
        event.mouseButton.button == sf::Mouse::Left) {
        pixelPosition = sf::Vector2i(
            event.mouseButton.x,
            event.mouseButton.y
        );
        return true;
    }
#endif

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
    sf::Color color
)
{
    std::vector<std::string> lines = wrapText(text, 70);

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

EventScene::EventScene(GameContext& context, EventId eventId)
    : Scene(context)
    , eventId_(eventId)
{
}

void EventScene::handleEvent(
    const sf::Event& event,
    const sf::RenderWindow& window
)
{
    if (transition_.target != SceneType::None || resolved_) {
        return;
    }

    sf::Vector2i pixelPosition;
    if (!readLeftClickPosition(event, pixelPosition)) {
        return;
    }

    const sf::Vector2f mousePos =
        window.mapPixelToCoords(pixelPosition);

    const EventDef& eventDef = context.events.get(eventId_);

    for (int i = 0; i < static_cast<int>(eventDef.choices.size()); ++i) {
        if (getChoiceRect(i).contains(mousePos)) {
            choose(i);
            return;
        }
    }
}

void EventScene::update(float)
{
}

void EventScene::draw(sf::RenderWindow& window)
{
    sf::RectangleShape background(sf::Vector2f(1920.0f, 1080.0f));
    background.setFillColor(sf::Color(28, 25, 35));
    window.draw(background);

    const sf::Font& font = context.resources.getFont("zh-R");
    const EventDef& eventDef = context.events.get(eventId_);

    sf::Text title = makeText(font, eventDef.title, 52);
    title.setFillColor(sf::Color::White);
    title.setPosition(sf::Vector2f(120.0f, 80.0f));
    window.draw(title);

    std::ostringstream stateText;
    stateText << "HP: " << context.runState.player.hp
              << " / " << context.runState.player.maxHp
              << "    Gold: " << context.runState.gold;

    sf::Text playerState = makeText(font, stateText.str(), 28);
    playerState.setFillColor(sf::Color(220, 220, 220));
    playerState.setPosition(sf::Vector2f(120.0f, 150.0f));
    window.draw(playerState);

    drawWrappedText(
        window,
        font,
        eventDef.description,
        32,
        sf::Vector2f(120.0f, 250.0f),
        42.0f,
        sf::Color(230, 230, 230)
    );

    for (int i = 0; i < static_cast<int>(eventDef.choices.size()); ++i) {
        const sf::FloatRect rect = getChoiceRect(i);

        sf::RectangleShape button(rect.size);
        button.setPosition(rect.position);
        button.setFillColor(sf::Color(70, 70, 95));
        button.setOutlineThickness(3.0f);
        button.setOutlineColor(sf::Color(210, 210, 230));
        window.draw(button);

        const EventChoiceDef& choice = eventDef.choices[i];

        sf::Text choiceText = makeText(font, choice.text, 30);
        choiceText.setFillColor(sf::Color::White);
        choiceText.setPosition(sf::Vector2f(
            rect.position.x + 24.0f,
            rect.position.y + 16.0f
        ));
        window.draw(choiceText);

        sf::Text descText = makeText(font, choice.description, 22);
        descText.setFillColor(sf::Color(215, 215, 215));
        descText.setPosition(sf::Vector2f(
            rect.position.x + 24.0f,
            rect.position.y + 58.0f
        ));
        window.draw(descText);
    }


    if (!resultMessage_.empty()) {
        drawWrappedText(
            window,
            font,
            resultMessage_,
            26,
            sf::Vector2f(120.0f, 900.0f),
            34.0f,
            sf::Color(240, 220, 160)
        );
    }
}

SceneTransition EventScene::getTransition() const
{
    return transition_;
}

sf::FloatRect EventScene::getChoiceRect(int index) const
{
    const float x = 120.0f;
    const float y = 470.0f + static_cast<float>(index) * 130.0f;
    const float width = 1180.0f;
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
            choiceIndex
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

    mapSystem_.completeSelectedNode(context.runState);

    if (mapSystem_.isRouteFinished(context.runState)) {
        transition_.target = SceneType::End;
        transition_.battleResult = BattleResult::Victory;
    } else {
        transition_.target = SceneType::Map;
    }
}