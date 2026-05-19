#ifndef SPIRELIKE_INTENTVIEW_HPP
#define SPIRELIKE_INTENTVIEW_HPP
#include <SFML/Graphics.hpp>

class IntentView {
public:
    IntentView();//这里不用传进来坐标原因是只有一个怪,目的的坐标固定
    void draw(sf::RenderWindow &window,sf::Texture &texture);
    void draw(sf::RenderWindow &window,sf::Texture &texture,sf::Font &font,int value);
    //考虑到三种状态有两种情况:一种是带有数值，另一种只有图标，所以对以上函数进行重载
private:
    sf::Vector2f position;
};


#endif //SPIRELIKE_INTENTVIEW_HPP