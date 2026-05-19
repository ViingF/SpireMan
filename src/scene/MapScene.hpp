#ifndef SPIRELIKE_MAPSCENE_HPP
#define SPIRELIKE_MAPSCENE_HPP

#include "core/Scene.hpp"
#include "core/SceneTransition.hpp"
#include "system/MapSystem.hpp"

#include <SFML/Graphics.hpp>

#include "ui/Button.hpp"
#include "ui/CardPileOverlay.hpp"

class MapScene : public Scene {
public:
    explicit MapScene(GameContext& context);

    void handleEvent(
        const sf::Event& event,
        const sf::RenderWindow& window
    ) override;

    void update(float dt) override;

    void draw(sf::RenderWindow& window) override;

    SceneTransition getTransition() const override;

private:
    sf::Vector2f getNodePosition(int index, int total) const;
    float getNodeDisplaySize(const MapNode& node,int total) const;

    int getNodeIndexAtPosition(sf::Vector2f mousePos) const;

    sf::Sprite makeNodeSprite(
        const MapNode& node,
        int total
    ) const;

    void enterNode(int nodeIndex);
    bool isBossNode(const MapNode& node, int total) const;

    std::string getNodeTextureId(
        const MapNode& node,
        int total
    ) const;
    int getLayerNodeCount(int layer) const;
    int getMaxLayer() const;
    std::vector<PileCardViewData> buildPileViewData(
        const std::vector<CardInstance>& cards
    );
    const sf::Texture& getCardTemplateTexture(CardType type);
    const sf::Texture* getCardArtTexture(const CardDef& cardDef);
    CardRenderTextures getCardRenderTextures(const CardDef& cardDef);


private:
    MapSystem mapSystem_;
    SceneTransition transition_;
    Button deckButton_;
    Button saveQuitButton_;
    CardPileOverlay deckOverlay_;


};

#endif // SPIRELIKE_MAPSCENE_HPP
