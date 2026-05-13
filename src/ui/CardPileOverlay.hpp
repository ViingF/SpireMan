#ifndef SPIRELIKE_CARDPILEOVERLAY_HPP
#define SPIRELIKE_CARDPILEOVERLAY_HPP

#include <SFML/Graphics.hpp>
#include <string>
#include <vector>

#include "model/CardInstance.hpp"

class CardDatabase;

class CardPileOverlay {
public:
    void open(
        const std::string& title,
        const std::vector<CardInstance>* cards
    );

    void close();
    bool isOpen() const;

    // 返回 true 表示事件已被弹窗消费，场景不应继续处理点击
    bool handleEvent(
        const sf::Event& event,
        const sf::RenderWindow& window
    );

    void draw(
        sf::RenderWindow& window,
        const sf::Font& font,
        const CardDatabase& cardDatabase
    ) const;

private:
    const std::vector<CardInstance>* cards_ = nullptr;
    std::string title_;
    int page_ = 0;

    int getCardsPerPage() const;
    int getPageCount() const;
    void clampPage();

    sf::FloatRect getPanelRect(
        const sf::RenderWindow& window
    ) const;

    sf::FloatRect getCloseRect(
        const sf::RenderWindow& window
    ) const;

    sf::FloatRect getPrevRect(
        const sf::RenderWindow& window
    ) const;

    sf::FloatRect getNextRect(
        const sf::RenderWindow& window
    ) const;

    sf::FloatRect getCardRect(
        const sf::RenderWindow& window,
        int visibleIndex
    ) const;
};

#endif
