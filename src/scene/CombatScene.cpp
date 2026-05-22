#include "CombatScene.hpp"

#include "core/ResourceManager.hpp"
#include "database/CardDatabase.hpp"
#include "ui/CardView.hpp"
#include "ui/EnergyView.hpp"
#include "ui/StateView.hpp"
#include "ui/TextUtils.hpp"

#include <algorithm>
#include <initializer_list>
#include <optional>
#include <string>
#include <vector>

#include "ui/HealthBar.hpp"
#include "ui/TopInfoBar.hpp"

namespace {

const float kEnemySpriteWidth = 1500.f;
const float kEnemySpriteHeight = 1000.f;
const float kHealthBarWidth = 240.f;
const float kHealthBarHeight = 30.f;
const float kStateGapBelowHealthBar = 19.f;

const float kPlayerDesignX = 113.f;
const float kPlayerHealthBarDesignY = 800.f;
const float kPlayerDisplayScale = 0.5f;
const float kPlayerDisplayWidth = 750.f;


struct CombatUnitStatus {
    int block = 0;
    int strength = 0;
    int vulnerable = 0;
    int weak = 0;
};


sf::Vector2f toScreenPosition(
    float designX,
    float designY,
    float scaleX,
    float scaleY
)
{
    return {designX * scaleX, designY * scaleY};
}

void drawStatusIcons(
    sf::RenderWindow& window,
    const sf::Font& font,
    ResourceManager& resources,
    float screenX,
    float screenY,
    const CombatUnitStatus& status
)
{
    std::vector<StateIconViewData> states;

    auto tryAdd = [&](int value, const char* textureId) {
        if (value <= 0 || !resources.hasTexture(textureId)) {
            return;
        }

        states.push_back({
            value,
            &resources.getTexture(textureId)
        });
    };

    tryAdd(status.block, "block");
    tryAdd(status.strength, "strength");
    tryAdd(status.vulnerable, "vulnerable");
    tryAdd(status.weak, "weak");

    StateView stateView({screenX, screenY});
    stateView.draw(window, font, states);
}

const float kDesignWidth = 1920.f;
const float kDesignHeight = 1080.f;
const float kPileButtonDesignSize = 128.f;

const float kHandCardWidth = 190.f;
const float kHandCardHeight = 268.f;
const float kHandCardSideExpand = 30.f;
const float CARD_START_X = 350.f;
const float CARD_START_Y = 650.f + 215.f - 20.f;
const float CARD_SPACING = 198.f;

    const sf::Texture* getIntentTexture(
        ResourceManager& resources,
        EnemyIntentType type
    )
{
    const char* textureId = nullptr;

    switch (type) {
        case EnemyIntentType::Attack:
            textureId = "attack";
            break;

        case EnemyIntentType::Block:
            textureId = "block";
            break;

        case EnemyIntentType::Buff:
            textureId = "buff";
            break;

        default:
            return nullptr;
    }

    if (!resources.hasTexture(textureId)) {
        return nullptr;
    }

    return &resources.getTexture(textureId);
}

    const char* combatFailureMessage(ErrorCode code)
{
    switch (code) {
        case ErrorCode::NOT_ENOUGH_ENERGY:
            return "能量不足";
        case ErrorCode::INVALID_TARGET:
            return "目标无效";
        case ErrorCode::CARD_NOT_IN_HAND:
            return "卡牌不在手牌中";
        case ErrorCode::INVALID_SCENE_STATE:
            return "当前状态无法操作";
        default:
            return "操作失败";
    }
}



} // namespace

CombatScene::CombatScene(
    GameContext& context,
    const EncounterDef& encounterDef
)
    : Scene(context),
      encounterDef_(encounterDef),
      context_(context),
      combatSystem_(),
      endTurnButton_(
          {1510.f, 893.f},
          {256.f, 106.f},
          context.resources.getFont("zh-R"),
          "结束回合"
      ),
      drawPileButton_(
          {10.f, 943.f},
          {128.f, 128.f},
          context.resources.getFont("zh-R"),
          "抽牌堆"
      ),
      discardPileButton_(
          {1780.f, 943.f},
          {128.f, 128.f},
          context.resources.getFont("zh-R"),
          "弃牌堆"
      ),
      exhaustPileButton_(
          {1780.f, 803.f},
          {128.f, 128.f},
          context.resources.getFont("zh-R"),
          "消耗堆"
          ),
    mapIconButton_(
        sf::Vector2f(0.0f, 0.0f),
        sf::Vector2f(64.0f, 64.0f),
        context.resources.getFont("zh-R"),
        ""
    ),pileOverlay_(context.resources.getFont("zh-R"))

{
    mapIconButton_.setTexture(
    context.resources.getTexture("map")
);


    enemyTextureIds_.clear();

    for (const EncounterEnemySlot& slot : encounterDef.enemies) {
        if (!context.enemies.exists(slot.enemyId)) {
            enemyTextureIds_.push_back("");
            continue;
        }

        const EnemyDef& enemyDef =
            context.enemies.get(slot.enemyId);

        enemyTextureIds_.push_back(enemyDef.textureId);
    }

    combatSystem_.startCombat(
        context.runState,
        encounterDef,
        context.enemies,
        context.cards
    );

    endTurnButton_.setPlaceholderStyle();
     if (context.resources.hasTexture("endTurnButton")) {
         endTurnButton_.setTexture(
             context.resources.getTexture("endTurnButton")
         );
     }

    drawPileButton_.setPlaceholderStyle();
    if (context.resources.hasTexture("drawPile")) {
        drawPileButton_.setTexture(
            context.resources.getTexture("drawPile")
        );
    }

    discardPileButton_.setPlaceholderStyle();
    if (context.resources.hasTexture("discardPile")) {
        discardPileButton_.setTexture(
            context.resources.getTexture("discardPile")
        );
    }

    exhaustPileButton_.setPlaceholderStyle();
    if (context.resources.hasTexture("exhaustPile")) {
        exhaustPileButton_.setTexture(
            context.resources.getTexture("exhaustPile")
        );
    }
    pileOverlay_.setTextures(context.resources.getTexture("enabledButton"));
}

void CombatScene::layoutPileButtons(const sf::RenderWindow& window)
{
    const float scaleX =
        static_cast<float>(window.getSize().x) / kDesignWidth;
    const float scaleY =
        static_cast<float>(window.getSize().y) / kDesignHeight;

    const sf::Vector2f buttonSize = {
        kPileButtonDesignSize * scaleX,
        kPileButtonDesignSize * scaleY
    };

    drawPileButton_.setSize(buttonSize);

    discardPileButton_.setSize(buttonSize);

    exhaustPileButton_.setSize(buttonSize);
}

void CombatScene::handleEvent(
    const sf::Event& event,
    const sf::RenderWindow& window
)
{
    TopInfoBar::layoutMapButton(mapIconButton_, window);
    mapIconButton_.handleEvent(event, window);

    if (mapIconButton_.wasClicked()) {

        transition_ = SceneTransition{};
        transition_.openMapPreview = true;

        mapIconButton_.reset();
        return;
    }


    layoutPileButtons(window);
    if (pileOverlay_.isOpen()) {
        pileOverlay_.handleEvent(event, window);
        return;
    }

    endTurnButton_.handleEvent(event, window);
    drawPileButton_.handleEvent(event, window);
    discardPileButton_.handleEvent(event, window);
    exhaustPileButton_.handleEvent(event, window);

    handlePileButtons(event, window);

    if (endTurnButton_.wasClicked()) {
        combatSystem_.endPlayerTurn();
        endTurnButton_.reset();
    }

    if (const auto mouseEvent = event.getIf<sf::Event::MouseButtonPressed>()) {
    if (mouseEvent->button != sf::Mouse::Button::Left) {
        return;
    }

    const sf::Vector2f mousePos =
        window.mapPixelToCoords(mouseEvent->position);

    const auto& deck = combatSystem_.getDeck();
    const auto& hand = deck.hand;

    if (
        selectedHandIndex_ >= 0 &&
        selectedHandIndex_ < static_cast<int>(hand.size())
    ) {
        const CardInstance& selectedInst = hand[selectedHandIndex_];

        if (context_.cards.exists(selectedInst.cardId)) {
            const CardDef& selectedCard =
                context_.cards.get(selectedInst.cardId);

            if (selectedCard.target == TargetType::Enemy) {
                const int enemyIndex = enemyGroupView_.getEnemyIndexAtPosition(
    mousePos,
    window,
    combatSystem_.getEnemies()
);



                if (enemyIndex >= 0) {
                    ErrorCode result =
                        combatSystem_.playCard(selectedHandIndex_, enemyIndex);

                    if (result == ErrorCode::OK) {
                        selectedHandIndex_ = -1;
                    } else {
                        context_.failureToast.show(combatFailureMessage(result));
                    }


                    updateBattleTransition();
                    return;
                }
            }
        }
    } else {
        selectedHandIndex_ = -1;
    }

    syncHandCardViews();

    // 2. 检测是否点击了手牌
    for (size_t i = 0; i < cardViews_.size(); ++i) {
        if (!cardViews_[i].contains(mousePos)) {
            continue;
        }
        context_.audio.playSound("Click");

        const CardInstance& inst = hand[i];

        if (!context_.cards.exists(inst.cardId)) {
            continue;
        }

        const CardDef& card = context_.cards.get(inst.cardId);

        if (card.target == TargetType::Enemy) {
            // 敌人目标牌：只选中，等待玩家再点敌人
            selectedHandIndex_ = static_cast<int>(i);
        } else {
            // 自身/无目标牌：可以直接打出
            ErrorCode result =
                combatSystem_.playCard(static_cast<int>(i), NoTarget);

            if (result == ErrorCode::OK) {
                selectedHandIndex_ = -1;
            } else {
                context_.failureToast.show(combatFailureMessage(result));
            }

        }

        updateBattleTransition();
        return;
    }
}

    updateBattleTransition();
}

void CombatScene::handlePileButtons(
    const sf::Event& event,
    const sf::RenderWindow& window
)
{
    (void)event;
    (void)window;

    if (drawPileButton_.wasClicked()) {
        pileOverlay_.open(
            "抽牌堆",
            buildPileCardViewData(combatSystem_.getDeck().drawPile)
        );
        drawPileButton_.reset();
    }

    if (discardPileButton_.wasClicked()) {
        pileOverlay_.open(
            "弃牌堆",
            buildPileCardViewData(combatSystem_.getDeck().discardPile)
        );
        discardPileButton_.reset();
    }

    if (exhaustPileButton_.wasClicked()) {
        pileOverlay_.open(
            "消耗堆",
            buildPileCardViewData(combatSystem_.getDeck().exhaustPile)
        );
        exhaustPileButton_.reset();
    }
}

void CombatScene::update(float dt)
{
    (void)dt;
}

void CombatScene::draw(sf::RenderWindow& window)
{
    layoutPileButtons(window);

    window.clear(sf::Color(50, 50, 50));

    sf::Font& font =
        context.resources.getFont("zh-R");

    drawBackground(window);
    TopInfoBar::draw(
    window,
    context,
    font,
    std::optional<int>{combatSystem_.getPlayer().hp},
    true,
    true,
    false // map 图标由按钮绘制
);


    TopInfoBar::layoutMapButton(mapIconButton_, window);
    mapIconButton_.draw(window);


    drawPileButton_.draw(window);
    discardPileButton_.draw(window);
    exhaustPileButton_.draw(window);
    endTurnButton_.draw(window);
    drawPlayerInfo(window);
    drawPlayerStatus(window, font);

    drawBurningBlood(window);
    drawEnemies(window, font);
    drawHand(window, font);
    drawEnergy(window, font);

    pileOverlay_.draw(
        window,
        font
    );
}

void CombatScene::updateBattleTransition()
{
    BattleResult result =
        combatSystem_.getBattleResult();

    if (result == BattleResult::Ongoing) {
        return;
    }

    if (!combatSystem_.hasCommittedResult()) {
        combatSystem_.commitResultToRunState(
            context.runState
        );
    }

    transition_.battleResult = result;

    if (result == BattleResult::Victory) {
        transition_.target = SceneType::Reward;
        return;
    }

    if (result == BattleResult::Defeat) {
        transition_.target = SceneType::End;
    }
}

SceneTransition CombatScene::getTransition() const
{
    return transition_;
}

void CombatScene::drawPlayerInfo(sf::RenderWindow& window) const
{
    sf::RectangleShape player({kEnemySpriteWidth, kEnemySpriteHeight});
    player.setScale({kPlayerDisplayScale, kPlayerDisplayScale});
    const float x = 113.f * window.getSize().x / 1920.f;
    const float y = 488.f * window.getSize().y / 1080.f;
    player.setPosition({x, y});

    player.setTexture(&context_.resources.getTexture("IronClad"));
    window.draw(player);
}

void CombatScene::drawPlayerStatus(
    sf::RenderWindow& window,
    sf::Font& font
) const {
    const Player& player = combatSystem_.getPlayer();

    const float scaleX =
        static_cast<float>(window.getSize().x) / 1920.f;
    const float scaleY =
        static_cast<float>(window.getSize().y) / 1080.f;

    const float designHealthX =
        kPlayerDesignX +
        (kPlayerDisplayWidth - kHealthBarWidth) * 0.5f;
    const float designHealthY = kPlayerHealthBarDesignY;
    const float designStateY =
        designHealthY + kHealthBarHeight + kStateGapBelowHealthBar - 110.f;

    const sf::Vector2f healthPos = toScreenPosition(
        designHealthX,
        designHealthY,
        scaleX,
        scaleY
    );
    const sf::Vector2f statePos = toScreenPosition(
        designHealthX,
        designStateY,
        scaleX,
        scaleY
    );

    HealthBar healthBar(healthPos);
    healthBar.draw(
        window,
        &context_.resources.getTexture("hp"),
        font,
        player.hp,
        player.maxHp
    );



    drawStatusIcons(
        window,
        font,
        context_.resources,
        statePos.x,
        statePos.y,
        CombatUnitStatus{
            player.block,
            player.strength,
            player.vulnerable,
            player.weak
        }
    );
}

void CombatScene::drawBackground(sf::RenderWindow& window) const {
    sf::RectangleShape background({1920.f, 1136.f});
    background.setTexture(&context_.resources.getTexture("background"));
    background.setScale({1.f, 1080.f / 1136.f});
    background.setPosition({0.f, 0.f});
    window.draw(background);
}

void CombatScene::drawPanelHeart(
    sf::RenderWindow& window,
    sf::Font& font
) const {
    const float kIconDesignX = 265.f;
    const float kIconSize = 64.f;

    const float scaleX =
        static_cast<float>(window.getSize().x) / kDesignWidth;
    const float x = kIconDesignX * scaleX;

    sf::RectangleShape panelHeart({kIconSize, kIconSize});
    panelHeart.setTexture(&context_.resources.getTexture("panelHeart"));
    panelHeart.setPosition({x, 0.f});
    window.draw(panelHeart);

    const int hp = combatSystem_.getPlayer().hp;
    sf::Text hpText = TextUtils::createWhiteText(
        font,
        std::to_string(hp),
        28,
        {x + kIconSize + 6.f, 18.f}
    );
    window.draw(hpText);
}

void CombatScene::drawPanelGoldBag(
    sf::RenderWindow& window,
    sf::Font& font
) const {
    const float kIconDesignX = 425.f;
    const float kIconSize = 64.f;

    const float scaleX =
        static_cast<float>(window.getSize().x) / kDesignWidth;
    const float x = kIconDesignX * scaleX;

    sf::RectangleShape panelGoldBag({kIconSize, kIconSize});
    panelGoldBag.setTexture(
        &context_.resources.getTexture("panelGoldBag")
    );
    panelGoldBag.setPosition({x, 0.f});
    window.draw(panelGoldBag);

    sf::Text goldText = TextUtils::createWhiteText(
        font,
        std::to_string(context_.runState.gold),
        28,
        {x + kIconSize + 6.f, 18.f}
    );
    window.draw(goldText);
}


void CombatScene::drawDeck(sf::RenderWindow& window) const{
    sf::RectangleShape deck({64.f, 64.f});
    deck.setTexture(&context_.resources.getTexture("deck"));
    const float x = 1773.f / 1920.f * window.getSize().x;
    deck.setPosition({x, 0.f});
    window.draw(deck);
}

void CombatScene::drawMap(sf::RenderWindow& window) const{
    sf::RectangleShape map({64.f, 64.f});
    map.setTexture(&context_.resources.getTexture("map"));
    const float x = 1700.f / 1920.f * window.getSize().x;
    map.setPosition({x, 0.f});
    window.draw(map);
}

void CombatScene::drawFloor(
    sf::RenderWindow& window,
    sf::Font& font
) const {
    const float kIconDesignX = 890.f;
    const float kIconSize = 64.f;

    const float scaleX =
        static_cast<float>(window.getSize().x) / kDesignWidth;
    const float x = kIconDesignX * scaleX;

    sf::RectangleShape floorIcon({kIconSize, kIconSize});
    floorIcon.setTexture(&context_.resources.getTexture("floor"));
    floorIcon.setPosition({x, 0.f});
    window.draw(floorIcon);

    sf::Text floorText = TextUtils::createWhiteText(
        font,
        std::to_string(context_.runState.floorInAct),
        28,
        {x + kIconSize + 6.f, 18.f}
    );
    window.draw(floorText);
}

void CombatScene::drawSettings(sf::RenderWindow& window) const{
    sf::RectangleShape settings({64.f, 64.f});
    settings.setTexture(&context_.resources.getTexture("settings"));
    const float x = 1846.f / 1920.f * window.getSize().x;
    settings.setPosition({x, 0.f});
    window.draw(settings);
}

void CombatScene::drawBurningBlood(sf::RenderWindow& window) const{
    sf::RectangleShape burningBlood({256.f, 256.f});
    burningBlood.setScale({115.f / 256.f, 115.f / 256.f});
    burningBlood.setTexture(&context_.resources.getTexture("burningBlood"));
    const float x = 10.f / 1920.f * window.getSize().x;
    const float y = 45.f / 1080.f * window.getSize().y;
    burningBlood.setPosition({x, y});
    window.draw(burningBlood);
}


void CombatScene::drawTop(sf::RenderWindow& window) const {
    sf::RectangleShape top({1920.f, 70.f});
    top.setFillColor({128,128,128});
    top.setPosition({0,0});
    window.draw(top);
}

void CombatScene::syncHandCardViews() const
{
    const auto& hand = combatSystem_.getDeck().hand;

    cardViews_.clear();
    cardViews_.reserve(hand.size());

    for (size_t i = 0; i < hand.size(); ++i) {
        const float slotLeft =
            CARD_START_X + static_cast<float>(i) * CARD_SPACING;

        cardViews_.emplace_back(
            hand[i].cardId,
            sf::Vector2f{
                slotLeft,
                CARD_START_Y
            },
            sf::Vector2f{kHandCardWidth, kHandCardHeight}
        );

        if (static_cast<int>(i) == selectedHandIndex_) {
            cardViews_.back().setSelected(true);
        }
    }
}


const sf::Texture& CombatScene::getCardTemplateTexture(
    CardType type
) const
{
    switch (type) {
        case CardType::Attack:
            return context_.resources.getTexture("Attack");

        case CardType::Skill:
            return context_.resources.getTexture("Skill");

        case CardType::Curse:
            return context_.resources.getTexture("Curse");
    }

    return context_.resources.getTexture("Attack");
}

const sf::Texture* CombatScene::getCardArtTexture(
    const CardDef& cardDef
) const
{
    if (
        cardDef.textureId.empty() ||
        !context_.resources.hasTexture(cardDef.textureId)
    ) {
        return nullptr;
    }

    return &context_.resources.getTexture(cardDef.textureId);
}

CardRenderTextures CombatScene::getCardRenderTextures(
    const CardDef& cardDef
) const
{
    CardRenderTextures textures;

    textures.templateTexture =
        &getCardTemplateTexture(cardDef.type);

    textures.artTexture =
        getCardArtTexture(cardDef);

    return textures;
}

std::vector<PileCardViewData> CombatScene::buildPileCardViewData(
    const std::vector<CardInstance>& cards
) const
{
    std::vector<PileCardViewData> result;
    result.reserve(cards.size());

    for (const CardInstance& card : cards) {
        if (!context_.cards.exists(card.cardId)) {
            continue;
        }

        const CardDef& def = context_.cards.get(card.cardId);

        PileCardViewData data;
        data.cardId = card.cardId;
        data.instanceId = card.instanceId;
        data.cardDef = &def;
        data.textures = getCardRenderTextures(def);
        result.push_back(data);
    }

    return result;
}

void CombatScene::drawHand(
    sf::RenderWindow& window,
    sf::Font& font
) const {
    syncHandCardViews();

    const auto& hand = combatSystem_.getDeck().hand;

    for (size_t i = 0; i < cardViews_.size(); ++i) {
        if (i >= hand.size()) {
            break;
        }

        const CardInstance& card = hand[i];

        if (!context_.cards.exists(card.cardId)) {
            continue;
        }

        const CardDef& def = context_.cards.get(card.cardId);
        const CardRenderTextures textures = getCardRenderTextures(def);

        cardViews_[i].draw(window, def, textures, font);
    }
}

void CombatScene::drawEnemies(
    sf::RenderWindow& window,
    sf::Font& font
) const {
    const std::vector<Enemy>& enemies = combatSystem_.getEnemies();
    const int slotCount = std::min(static_cast<int>(enemies.size()), 3);

    if (slotCount <= 0) {
        return;
    }

    std::vector<EnemyRenderData> renderData;
    renderData.reserve(slotCount);

    for (int enemyIndex = 0; enemyIndex < slotCount; ++enemyIndex) {
        const Enemy& enemy = enemies[enemyIndex];

        EnemyRenderData data;

        if (
            enemyIndex < static_cast<int>(enemyTextureIds_.size()) &&
            !enemyTextureIds_[enemyIndex].empty() &&
            context_.resources.hasTexture(enemyTextureIds_[enemyIndex])
        ) {
            data.enemyTexture =
                &context_.resources.getTexture(enemyTextureIds_[enemyIndex]);
        }

        if (context_.resources.hasTexture("hp")) {
            data.hpTexture = &context_.resources.getTexture("hp");
        }

        data.intent = IntentViewData{
            getIntentTexture(context_.resources, enemy.intent.type),
            enemy.intent.value
        };

        auto tryAddState = [&](int value, const char* textureId) {
            if (value <= 0 || !context_.resources.hasTexture(textureId)) {
                return;
            }

            data.states.push_back(StateIconViewData{
                value,
                &context_.resources.getTexture(textureId)
            });
        };

        tryAddState(enemy.block, "block");
        tryAddState(enemy.strength, "strength");
        tryAddState(enemy.vulnerable, "vulnerable");
        tryAddState(enemy.weak, "weak");

        renderData.push_back(data);
    }

    enemyGroupView_.draw(
        window,
        enemies,
        renderData,
        font
    );
}


void CombatScene::drawEnergy(
    sf::RenderWindow& window,
    sf::Font& font
) const {
    EnergyView energy;
    sf::Texture& energyTexture =
        context_.resources.getTexture("energy");

    const int nowEnergy = combatSystem_.getEnergy();
    const int maxEnergy = 3;

    energy.draw(
        window,
        energyTexture,
        font,
        nowEnergy,
        maxEnergy
    );
}

void CombatScene::resetTransition()
{
    transition_ = SceneTransition{};
}
