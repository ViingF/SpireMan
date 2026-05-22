#include "scene/MenuScene.hpp"


#include "config/Constants.hpp"
#include "system/SaveSystem.hpp"
#include "ui/TextUtils.hpp"

namespace {

    bool startsWith(
        const std::string& text,
        const std::string& prefix
    )
    {
        return text.rfind(prefix, 0) == 0;
    }

    void appendUtf8(
        std::string& text,
        char32_t ch
    )
    {
        const std::uint32_t code =
            static_cast<std::uint32_t>(ch);

        if (code <= 0x7F) {
            text.push_back(static_cast<char>(code));
        } else if (code <= 0x7FF) {
            text.push_back(static_cast<char>(0xC0 | (code >> 6)));
            text.push_back(static_cast<char>(0x80 | (code & 0x3F)));
        } else if (code <= 0xFFFF) {
            text.push_back(static_cast<char>(0xE0 | (code >> 12)));
            text.push_back(static_cast<char>(0x80 | ((code >> 6) & 0x3F)));
            text.push_back(static_cast<char>(0x80 | (code & 0x3F)));
        } else if (code <= 0x10FFFF) {
            text.push_back(static_cast<char>(0xF0 | (code >> 18)));
            text.push_back(static_cast<char>(0x80 | ((code >> 12) & 0x3F)));
            text.push_back(static_cast<char>(0x80 | ((code >> 6) & 0x3F)));
            text.push_back(static_cast<char>(0x80 | (code & 0x3F)));
        }
    }

    void eraseLastUtf8Char(std::string& text)
    {
        if (text.empty()) {
            return;
        }

        std::size_t pos = text.size() - 1;

        while (
            pos > 0 &&
            (static_cast<unsigned char>(text[pos]) & 0xC0) == 0x80
        ) {
            --pos;
        }

        text.erase(pos);
    }

}


MenuScene::MenuScene(GameContext& context)
    :   Scene(context),

        nameButton(
          sf::Vector2f(20.f, 20.f),
          sf::Vector2f(120.f, 50.f),
          context.resources.getFont("zh-R"),
          "改名"
      ),

        startButton(
          sf::Vector2f(76.f, 820.f),
          sf::Vector2f(400.f, 100.f),
          context.resources.getFont("zh-R"),
          "开始"
      ),

        quitButton(
          sf::Vector2f(76.f, 960.f),
          sf::Vector2f(400.f, 100.f),
          context.resources.getFont("zh-R"),
          "退出游戏"
      ) ,
    loadButton(
          sf::Vector2f(76.f, 670.f),
          sf::Vector2f(400.f, 90.f),
          context.resources.getFont("zh-R"),
          "加载存档"
      ),
        background(context.resources.getTexture(
            "title"
        ))
{
    sf::Vector2u size = context.resources.getTexture(
            "title"
        ).getSize();
    float targetWidth = 1920;
    float targetHeight = 1080;
    float scaleX = targetWidth / size.x;
    float scaleY = targetHeight / size.y;

    background.setScale({scaleX, scaleY});
    loadButton.setEnabled(SaveSystem::hasSave());
    startButton.setTexture(context.resources.getTexture("blank"));
    quitButton.setTexture(context.resources.getTexture("blank"));
    loadButton.setTexture(context.resources.getTexture("blank"));
    nameButton.setTexture(context.resources.getTexture("blank"));
    if (context.runState.playerName.empty()) {
        context.runState.playerName = "Player";
    }

    nameInput = context.runState.playerName;

}

void MenuScene::handleEvent(
    const sf::Event& event,
    const sf::RenderWindow& window
)
{

    nameButton.handleEvent(event, window);

    if (nameButton.wasClicked())
    {
        

        editingName = true;
        nameInput = context.runState.playerName.empty()
            ? "Player"
            : context.runState.playerName;

        menuMessage = "输入名字,Enter确认，Esc取消";

        nameButton.reset();
        return;
    }

    if (editingName) {
        handleNameInputEvent(event);
        return;
    }

    startButton.handleEvent(event, window);
    quitButton.handleEvent(event, window);
    loadButton.handleEvent(event, window);

    if (startButton.wasClicked())
    {
        

        transition.target = SceneType::CharacterSelect;

        startButton.reset();
    }

    if (quitButton.wasClicked())
    {
        

        const_cast<sf::RenderWindow&>(window).close();

        quitButton.reset();
    }

    if (loadButton.wasClicked())
    {
        

        if (SaveSystem::hasSave()) {
            transition.target = SceneType::Map;
            transition.loadGame = true;
        }

        loadButton.reset();
    }
}



void MenuScene::update(float dt)
{

}

void MenuScene::draw(sf::RenderWindow& window)
{
    window.clear();

    window.draw(background);

    nameButton.draw(window);

    const std::string shownName =
        editingName
            ? nameInput + "_"
            : (
                context.runState.playerName.empty()
                    ? "Player"
                    : context.runState.playerName
              );

    sf::Text nameText = TextUtils::createWhiteText(
        context.resources.getFont("zh-R"),
        "名字：" + shownName,
        30,
        {160.f, 28.f}
    );

    window.draw(nameText);

    if (!menuMessage.empty()) {
        sf::Text messageText = TextUtils::createWhiteText(
            context.resources.getFont("zh-R"),
            menuMessage,
            24,
            {20.f, 85.f}
        );

        window.draw(messageText);
    }

    startButton.draw(window);
    quitButton.draw(window);
    loadButton.draw(window);
}


SceneTransition MenuScene::getTransition() const
{
    return transition;
}

void MenuScene::resetTransition()
{
    transition = SceneTransition{};
}

void MenuScene::handleNameInputEvent(const sf::Event& event)
{
    if (const auto* keyPressed =
        event.getIf<sf::Event::KeyPressed>())
    {
        if (keyPressed->code == sf::Keyboard::Key::Enter) {
            confirmNameInput();
            return;
        }

        if (keyPressed->code == sf::Keyboard::Key::Escape) {
            editingName = false;
            nameInput = context.runState.playerName;
            menuMessage.clear();
            return;
        }

        if (keyPressed->code == sf::Keyboard::Key::Backspace) {
            eraseLastUtf8Char(nameInput);
            return;
        }
    }

    if (const auto* textEntered =
        event.getIf<sf::Event::TextEntered>())
    {
        const char32_t code = textEntered->unicode;

        if (code >= 32 && code != 127) {
            appendUtf8(nameInput, code);
        }
    }
}

void MenuScene::confirmNameInput()
{
    if (startsWith(nameInput, "$/:")) {
        executeCommand(nameInput);
        return;
    }

    if (nameInput.empty()) {
        context.runState.playerName = "Player";
    } else {
        context.runState.playerName = nameInput;
    }

    editingName = false;
    menuMessage = "名字已设置为：" + context.runState.playerName;
}

void MenuScene::executeCommand(const std::string& input)
{
    const std::string command = input.substr(3);

    if (command == "win") {
        transition.target = SceneType::End;
        transition.battleResult = BattleResult::Victory;

        editingName = false;
        menuMessage = "已执行指令：胜利";
        return;
    }

    if (command == "author") {
        editingName = false;

        menuMessage = "作者： (我说空白也是名字),Nin.Hell,神人";
        return;
    }

    editingName = false;
    menuMessage = "未知指令：" + input;
}
