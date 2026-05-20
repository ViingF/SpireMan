#include "scene/RewardScene.hpp"

#include "config/Constants.hpp"
#include "system/MapSystem.hpp"
#include "ui/TopInfoBar.hpp"

RewardScene::RewardScene(
    GameContext& context
)
: Scene(context),mapIconButton_(
sf::Vector2f(0.0f, 0.0f),
sf::Vector2f(64.0f, 64.0f),
context.resources.getFont("zh-R"),
""
)
{
    mapIconButton_.setTexture(
    context.resources.getTexture("map")
);


    for (int i = 0; i < 3; ++i) {
        CardId id = context.cards.chooseRandomRewardCardId(
            context.runState.rng
        );

        rewardCardIds_.push_back(id);

        rewardCardViews_.emplace_back(
            id,
            sf::Vector2f(
                300.f + static_cast<float>(i) * 510.f,
                400.f
            )
        );
    }

}

void RewardScene::handleEvent(
    const sf::Event& event,
    const sf::RenderWindow& window
)
{

    TopInfoBar::layoutMapButton(mapIconButton_, window);
    mapIconButton_.handleEvent(event, window);

    if (mapIconButton_.wasClicked()) {
        context.audio.playSound("Click");

        transition.openMapPreview = true;
        transition.target = SceneType::Map;

        mapIconButton_.reset();
        return;
    }


    if (const auto* mousePressed =
        event.getIf<sf::Event::MouseButtonPressed>()) {
        if (mousePressed->button != sf::Mouse::Button::Left) {
            return;
        }

        sf::Vector2f mousePos =
            window.mapPixelToCoords(mousePressed->position);

        for (std::size_t i = 0; i < rewardCardViews_.size(); ++i) {
            if (rewardCardViews_[i].contains(mousePos)) {
                chooseCard(rewardCardIds_[i]);
                return;
            }
        }
        }

}

void RewardScene::update(float dt)
{
}

void RewardScene::draw(sf::RenderWindow& window)
{
    window.clear(sf::Color(40, 40, 60));

    const sf::Font& font =
        context.resources.getFont("zh-R");

    for (std::size_t i = 0; i < rewardCardIds_.size(); ++i) {
        if (!context.cards.exists(rewardCardIds_[i])) {
            continue;
        }

        const CardDef& def =
            context.cards.get(rewardCardIds_[i]);

        const CardRenderTextures textures =
            getCardRenderTextures(def);

        rewardCardViews_[i].draw(
            window,
            def,
            textures,
            font
        );
    }
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

}



SceneTransition RewardScene::getTransition() const
{
    return transition;
}

void RewardScene::chooseCard(CardId cardId)
{
    if (!context.cards.exists(cardId)) {
        return;
    }

    CardInstance newCard;
    newCard.instanceId = context.runState.nextCardInstanceId;
    newCard.cardId = cardId;

    context.runState.nextCardInstanceId += 1;
    context.runState.masterDeck.push_back(newCard);

    MapSystem mapSystem;

    if (!context.runState.mapNodes.empty()) {
        mapSystem.completeSelectedNode(context.runState);

        if (mapSystem.isRouteFinished(context.runState)) {
            if (mapSystem.advanceToNextActIfPossible(
                    context.runState,
                    context.events,
                    context.encounters
                )) {
                transition.target = SceneType::Map;
                } else {
                    transition.target = SceneType::End;
                    transition.battleResult = BattleResult::Victory;
                }
        } else {
            transition.target = SceneType::Map;
        }


        return;
    }

    transition.target = SceneType::Map;
}

const sf::Texture& RewardScene::getCardTemplateTexture(
    CardType type
)
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

const sf::Texture* RewardScene::getCardArtTexture(
    const CardDef& cardDef
)
{
    if (
        cardDef.textureId.empty() ||
        !context.resources.hasTexture(cardDef.textureId)
    ) {
        return nullptr;
    }

    return &context.resources.getTexture(cardDef.textureId);
}

CardRenderTextures RewardScene::getCardRenderTextures(
    const CardDef& cardDef
)
{
    CardRenderTextures textures;

    textures.templateTexture =
        &getCardTemplateTexture(cardDef.type);

    textures.artTexture =
        getCardArtTexture(cardDef);

    return textures;
}

void RewardScene::resetTransition()
{
    transition = SceneTransition{};
}
