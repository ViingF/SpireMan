
#include "IntentView.hpp"
#include "TextUtils.hpp"
#include <string>
IntentView::IntentView() {
    position = {1410,500};
}

void IntentView::draw(sf::RenderWindow &window,sf::Texture &texture) {
    sf::RectangleShape intent({64,64});
    intent.setTexture(&texture);
    intent.setPosition(position);

    window.draw(intent);
}

void IntentView::draw(sf::RenderWindow &window,sf::Texture &texture,sf::Font &font,int value) {
    if (value < 0) {
        return;
    }

    sf::RectangleShape intent({64,64});
    intent.setTexture(&texture);
    intent.setPosition(position);

    sf::Text text = TextUtils::createWhiteText(
        font,
        std::to_string(value),
        25,
        {position.x-10, position.y+55}
    );

    window.draw(intent);
    window.draw(text);
}