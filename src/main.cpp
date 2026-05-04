#include <SFML/Graphics.hpp>
#include <iostream>
#define WINDOWRESOULUTION {1920,1080}
using namespace sf;
using namespace std;

void handle(const Event::KeyPressed& ek) {
    cout<<"Key pressed."<<endl;
}
void handle(const auto&) {}
int main() {
    RenderWindow window(VideoMode(WINDOWRESOULUTION),"SFML Window");
    window.setFramerateLimit(60);
    RectangleShape rec1,rec2,rec3;
    Texture tex1,tex2,tex3;
    tex1.loadFromFile("assets/images/title.png");
    rec1.setTexture(&tex1);
    rec1.setSize({1920,1080});
    while (window.isOpen()) {
        while (const optional<Event> event=window.pollEvent()) {
            if (event->is<Event::Closed>()) {
                window.close();
            }
            if (auto key=event->getIf<Event::KeyPressed>()) {
                if (key->code==sf::Keyboard::Key::A) {
                    event->visit([](auto& type){handle(type);});
                    cout<<"A pressed."<<endl;
                }
            }
        }
        window.clear();
        window.draw(rec1);
        window.display();
    }
    return 0;
}
