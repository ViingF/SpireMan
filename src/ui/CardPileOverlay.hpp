#ifndef SPIRELIKE_CARDPILEOVERLAY_HPP
#define SPIRELIKE_CARDPILEOVERLAY_HPP

#include <SFML/Graphics.hpp>
#include <string>
#include <vector>

#include "Button.hpp"
#include "CardView.hpp"
#include "../model/Types.hpp"

struct CardDef;

struct PileCardViewData {
    CardId cardId = 0;
    CardInstanceId instanceId = 0;
    const CardDef* cardDef = nullptr;
    CardRenderTextures textures;
};


class CardPileOverlay {
public:
    explicit CardPileOverlay(sf::Font& font);

    void open(
        const std::string& title,
        const std::vector<PileCardViewData>& cards
    );

    void close();
    bool isOpen() const;

    bool handleEvent(
        const sf::Event& event,
        const sf::RenderWindow& window
    );

    void draw(
        sf::RenderWindow& window,
        const sf::Font& font
    );
    void setTextures(sf::Texture& texture);

private:
    std::vector<PileCardViewData> cards_;
    std::string title_;
    int page_ = 0;

    void layoutButtons(const sf::RenderWindow& window);

    int getCardsPerPage() const;
    int getPageCount() const;
    void clampPage();

    sf::FloatRect getPanelRect(const sf::RenderWindow& window) const;
    sf::FloatRect getCloseRect(const sf::RenderWindow& window) const;
    sf::FloatRect getPrevRect(const sf::RenderWindow& window) const;
    sf::FloatRect getNextRect(const sf::RenderWindow& window) const;
    sf::FloatRect getCardRect(
        const sf::RenderWindow& window,
        int visibleIndex
    ) const;

    Button closeButton_;
    Button prevButton_;
    Button nextButton_;
};


#endif
