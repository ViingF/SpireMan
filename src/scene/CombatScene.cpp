#include "CombatScene.hpp"

#include "database/CardDatabase.hpp"
#include "ui/CardView.hpp"
#include "ui/EnemyView.hpp"
#include "ui/EnergyView.hpp"
#include "ui/HealthBar.hpp"
#include "ui/StateView.hpp"
#include "ui/TextUtils.hpp"

#include <algorithm>
#include <initializer_list>
#include <optional>
#include <string>
#include <vector>

namespace {

sf::Color kEnemyPlaceholderFill = sf::Color::White;
sf::Color kEnemyPlaceholderOutline = sf::Color(40, 40, 40);

constexpr float kEnemySpriteWidth = 1500.f;
constexpr float kEnemySpriteHeight = 1000.f;
constexpr float kEnemyDisplayScale = 0.5f;
constexpr float kEnemyDisplayWidth =
    kEnemySpriteWidth * kEnemyDisplayScale;
constexpr float kEnemyDisplayHeight =
    kEnemySpriteHeight * kEnemyDisplayScale;
constexpr float kIntentSize = 64.f;
constexpr float kHealthBarWidth = 240.f;
constexpr float kHealthBarHeight = 30.f;
constexpr float kStateIconSize = 48.f;
constexpr float kStateIconSpacing = 55.f;

constexpr float kIntentGapAboveEnemy = 10.f;
constexpr float kEnemyHealthBarOffsetY = 455.f;
constexpr float kStateGapBelowHealthBar = 8.f;

constexpr float kPlayerDesignX = 113.f;
constexpr float kPlayerDesignY = 488.f;
constexpr float kPlayerDisplayWidth = 750.f;
constexpr float kPlayerDisplayHeight = 500.f;

    constexpr float kEnemyHitboxWidth = 260.f;
    constexpr float kEnemyHitboxHeight = 360.f;

    // 这两个值需要按素材微调
    constexpr float kEnemyHitboxOffsetX =
        (kEnemyDisplayWidth - kEnemyHitboxWidth) * 0.5f;

    constexpr float kEnemyHitboxOffsetY = 90.f;


struct EnemyLayoutConfig {
    std::vector<float> xPositions;
    float yPosition = 0.f;
};

struct CombatUnitStatus {
    int block = 0;
    int strength = 0;
    int vulnerable = 0;
    int weak = 0;
};

void applyEnemyPlaceholderStyle(sf::RectangleShape& shape)
{
    shape.setFillColor(kEnemyPlaceholderFill);
    shape.setOutlineColor(kEnemyPlaceholderOutline);
    shape.setOutlineThickness(2.f);
}

const EnemyLayoutConfig& getEnemyLayoutConfig(int enemyCount)
{
    static const EnemyLayoutConfig oneEnemy = {{690.f}, 217.f};
    static const EnemyLayoutConfig twoEnemies = {{690.f, 970.f}, 217.f};
    static const EnemyLayoutConfig threeEnemies = {
        {400.f, 690.f, 970.f},
        217.f
    };

    switch (enemyCount) {
        case 1: return oneEnemy;
        case 2: return twoEnemies;
        case 3: return threeEnemies;
        default: return threeEnemies;
    }
}

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
    float screenX,
    float screenY,
    const CombatUnitStatus& status
)
{
    float currentX = screenX;

    auto drawIcon = [&](int value) {
        if (value <= 0) {
            return;
        }

        sf::RectangleShape icon({kStateIconSize, kStateIconSize});
        // icon.setTexture(&...);  // 状态纹理 48x48
        applyEnemyPlaceholderStyle(icon);
        icon.setPosition({currentX, screenY});
        window.draw(icon);

        sf::Text valueText = TextUtils::createWhiteText(
            font,
            std::to_string(value),
            15,
            {currentX + 10.f, screenY + 50.f}
        );
        window.draw(valueText);

        currentX += kStateIconSpacing;
    };

    drawIcon(status.block);
    drawIcon(status.strength);
    drawIcon(status.vulnerable);
    drawIcon(status.weak);
}

constexpr float CARD_START_X = 430.f;
constexpr float CARD_START_Y = 650.f;
constexpr float CARD_SPACING = 170.f;

std::vector<PileCardViewData> convertToPileCardViews(
    const std::vector<CardInstance>& cards,
    CardDatabase& cardDb
)
{
    std::vector<PileCardViewData> result;
    result.reserve(cards.size());

    for (const auto& card : cards) {
        PileCardViewData data;
        data.cardId = card.cardId;
        data.instanceId = card.instanceId;
        data.cardDef = &cardDb.get(card.cardId);
        // data.textures.templateTexture = ...
        // data.textures.artTexture = ...
        data.textures = {};
        result.push_back(data);
    }

    return result;
}

sf::Texture& getFirstExistingTexture(
    ResourceManager& resources,
    std::initializer_list<const char*> textureIds
)
{
    for (const char* textureId : textureIds) {
        if (resources.hasTexture(textureId)) {
            return resources.getTexture(textureId);
        }
    }

    return resources.getTexture(*textureIds.begin());
}

    std::vector<int> getAliveEnemyIndices(const std::vector<Enemy>& enemies)
{
    std::vector<int> result;

    for (int i = 0; i < static_cast<int>(enemies.size()); ++i) {
        if (enemies[i].hp > 0) {
            result.push_back(i);
        }
    }

    return result;
}

    sf::FloatRect getEnemyScreenRect(
        const sf::RenderWindow& window,
        const EnemyLayoutConfig& config,
        int displayIndex
    )
{
    const float scaleX =
        static_cast<float>(window.getSize().x) / 1920.f;
    const float scaleY =
        static_cast<float>(window.getSize().y) / 1080.f;

    const float x = config.xPositions[displayIndex] * scaleX;
    const float y = config.yPosition * scaleY;

    const float width = kEnemyDisplayWidth * scaleX;
    const float height = kEnemyDisplayHeight * scaleY;

    return sf::FloatRect(
        {x, y},
        {width, height}
    );
}

    sf::FloatRect getEnemyHitRect(
    const sf::RenderWindow& window,
    const EnemyLayoutConfig& config,
    int displayIndex
)
{
    const float scaleX =
        static_cast<float>(window.getSize().x) / 1920.f;
    const float scaleY =
        static_cast<float>(window.getSize().y) / 1080.f;

    const float x =
        (config.xPositions[displayIndex] + kEnemyHitboxOffsetX) * scaleX;

    const float y =
        (config.yPosition + kEnemyHitboxOffsetY) * scaleY;

    const float width = kEnemyHitboxWidth * scaleX;
    const float height = kEnemyHitboxHeight * scaleY;

    return sf::FloatRect(
        {x, y},
        {width, height}
    );
}


    int getEnemyIndexAtPosition(
    sf::Vector2f mousePos,
    const sf::RenderWindow& window,
    const std::vector<Enemy>& enemies
)
{
    const int slotCount =
        std::min(static_cast<int>(enemies.size()), 3);

    if (slotCount <= 0) {
        return -1;
    }

    const EnemyLayoutConfig& config =
        getEnemyLayoutConfig(slotCount);

    // 从右往左检测，避免重叠区域优先命中左侧敌人
    for (int enemyIndex = slotCount - 1; enemyIndex >= 0; --enemyIndex) {
        if (enemies[enemyIndex].hp <= 0) {
            continue;
        }

        const sf::FloatRect enemyRect =
            getEnemyHitRect(window, config, enemyIndex);


        if (enemyRect.contains(mousePos)) {
            return enemyIndex;
        }
    }

    return -1;
}




    void drawHealthBar(
    sf::RenderWindow& window,
    const sf::Texture& texture,
    const sf::Font& font,
    sf::Vector2f position,
    int hp,
    int maxHp
)
{
    if (maxHp <= 0) {
        return;
    }

    const int clampedHp = std::clamp(hp, 0, maxHp);
    const float ratio =
        static_cast<float>(clampedHp) / static_cast<float>(maxHp);

    sf::RectangleShape background({
        kHealthBarWidth,
        kHealthBarHeight
    });
    background.setFillColor(sf::Color(40, 40, 40, 180));
    background.setPosition(position);
    window.draw(background);

    sf::RectangleShape foreground({
        kHealthBarWidth * ratio,
        kHealthBarHeight
    });
    foreground.setTexture(&texture);
    foreground.setPosition(position);
    window.draw(foreground);

    sf::Text hpText = TextUtils::createWhiteText(
        font,
        TextUtils::formatHpText(clampedHp, maxHp),
        15,
        {position.x + 60.f, position.y + 5.f}
    );
    window.draw(hpText);
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
      enemyDatabase_(),
      endTurnButton_(
          {1510.f, 743.f},
          {256.f, 256.f},
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
          {1780.f, 743.f},
          {128.f, 128.f},
          context.resources.getFont("zh-R"),
          "消耗堆"
      )
{
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
}

void CombatScene::handleEvent(
    const sf::Event& event,
    const sf::RenderWindow& window
)
{
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

    // 1. 如果已经选中了一张需要敌人目标的牌，优先检测是否点中了敌人
    if (
        selectedHandIndex_ >= 0 &&
        selectedHandIndex_ < static_cast<int>(hand.size())
    ) {
        const CardInstance& selectedInst = hand[selectedHandIndex_];

        if (context_.cards.exists(selectedInst.cardId)) {
            const CardDef& selectedCard =
                context_.cards.get(selectedInst.cardId);

            if (selectedCard.target == TargetType::Enemy) {
                const int enemyIndex = getEnemyIndexAtPosition(
                mousePos,
                window,
                combatSystem_.getEnemies()
                );


                if (enemyIndex >= 0) {
                    ErrorCode result =
                        combatSystem_.playCard(selectedHandIndex_, enemyIndex);

                    if (result == ErrorCode::OK) {
                        selectedHandIndex_ = -1;
                    }

                    updateBattleTransition();
                    return;
                }
            }
        }
    } else {
        selectedHandIndex_ = -1;
    }

    // 2. 检测是否点击了手牌
    for (size_t i = 0; i < hand.size(); ++i) {
        sf::FloatRect cardBounds(
            {
                CARD_START_X + static_cast<float>(i) * CARD_SPACING,
                CARD_START_Y
            },
            {
                150.f,
                220.f
            }
        );

        if (!cardBounds.contains(mousePos)) {
            continue;
        }

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
        auto pileCards = convertToPileCardViews(
            combatSystem_.getDeck().drawPile,
            context.cards
        );
        pileOverlay_.open("抽牌堆", pileCards);
        drawPileButton_.reset();
    }

    if (discardPileButton_.wasClicked()) {
        auto pileCards = convertToPileCardViews(
            combatSystem_.getDeck().discardPile,
            context.cards
        );
        pileOverlay_.open("弃牌堆", pileCards);
        discardPileButton_.reset();
    }

    if (exhaustPileButton_.wasClicked()) {
        auto pileCards = convertToPileCardViews(
            combatSystem_.getDeck().exhaustPile,
            context.cards
        );
        pileOverlay_.open("消耗堆", pileCards);
        exhaustPileButton_.reset();
    }
}

void CombatScene::update(float dt)
{
    (void)dt;
}

void CombatScene::draw(sf::RenderWindow& window)
{
    window.clear(sf::Color(50, 50, 50));

    sf::Font& font =
        context.resources.getFont("zh-R");

    drawBackground(window);
    drawTop(window);
    drawPileButton_.draw(window);
    discardPileButton_.draw(window);
    exhaustPileButton_.draw(window);
    endTurnButton_.draw(window);
    drawPlayerInfo(window);
    drawPlayerStatus(window, font);
    drawPanelHeart(window);
    drawPanelGoldBag(window) ;
    drawDeck(window);
    drawMap(window);
    drawFloor(window);
    drawSettings(window);
    drawBurningBlood(window);
    drawEnemies(window, font);
    drawHand(window, font);

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
        return;
    }
}

SceneTransition CombatScene::getTransition() const
{
    return transition_;
}

void CombatScene::drawPlayerInfo(sf::RenderWindow& window) const
{
    sf::RectangleShape player({kEnemySpriteWidth, kEnemySpriteHeight});
    player.setScale({kEnemyDisplayScale, kEnemyDisplayScale});
    const float x = 113.f * window.getSize().x / 1920.f;
    const float y = 488.f * window.getSize().y / 1080.f;
    player.setPosition({x, y});

     player.setTexture(&context_.resources.getTexture("IronClad"));
    applyEnemyPlaceholderStyle(player);
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
    const float designHealthY =
        kPlayerDesignY + kPlayerDisplayHeight + kStateGapBelowHealthBar;
    const float designStateY =
        designHealthY + kHealthBarHeight + kStateGapBelowHealthBar;

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

    drawHealthBar(
    window,
    context_.resources.getTexture("hp"),
    font,
    healthPos,
    player.hp,
    player.maxHp
);


    drawStatusIcons(
        window,
        font,
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

void CombatScene::drawPanelHeart(sf::RenderWindow& window) const{
    sf::RectangleShape panelHeart({64.f, 64.f});
    panelHeart.setTexture(&context_.resources.getTexture("panelHeart"));
    const float x = 265.f / 1920.f * window.getSize().x;
    panelHeart.setPosition({x, 0.f});
    window.draw(panelHeart);
}

void CombatScene::drawPanelGoldBag(sf::RenderWindow& window) const{
    sf::RectangleShape panelGoldBag({64.f, 64.f});
    panelGoldBag.setTexture(&context_.resources.getTexture("panelGoldBag"));
    const float x = 425.f / 1920.f * window.getSize().x;
    panelGoldBag.setPosition({x, 0.f});
    window.draw(panelGoldBag);
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

void CombatScene::drawFloor(sf::RenderWindow& window) const{
    sf::RectangleShape floor({64.f, 64.f});
    floor.setTexture(&context_.resources.getTexture("floor"));
    const float x = 890.f / 1920.f * window.getSize().x;
    floor.setPosition({x, 0.f});
    window.draw(floor);
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

void CombatScene::drawHand(
    sf::RenderWindow& window,
    sf::Font& font
) const {
    const auto& hand = combatSystem_.getDeck().hand;
    const CardRenderTextures emptyTextures;

    for (size_t i = 0; i < hand.size(); ++i) {
        const CardInstance& card = hand[i];

        if (!context_.cards.exists(card.cardId)) {
            continue;
        }

        const CardDef& def = context_.cards.get(card.cardId);

        CardView cardView(
            card.cardId,
            {
                CARD_START_X + static_cast<float>(i) * CARD_SPACING,
                CARD_START_Y
            },
            {150.f, 220.f}
        );

        if (static_cast<int>(i) == selectedHandIndex_) {
            cardView.setSelected(true);
        }

        //CardRenderTextures textures;
        //textures.templateTexture = &getCardTemplateTexture(def.type);
        //textures.artTexture = getCardArtTexture(def);
        cardView.draw(window, def, emptyTextures, font);
    }
}

void CombatScene::drawEnemies(
    sf::RenderWindow& window,
    sf::Font& font
) const {
    const std::vector<Enemy>& enemies = combatSystem_.getEnemies();

    const int slotCount =
        std::min(static_cast<int>(enemies.size()), 3);

    if (slotCount <= 0) {
        return;
    }

    const float scaleX =
        static_cast<float>(window.getSize().x) / 1920.f;
    const float scaleY =
        static_cast<float>(window.getSize().y) / 1080.f;

    const EnemyLayoutConfig& config =
        getEnemyLayoutConfig(slotCount);

    for (int enemyIndex = 0; enemyIndex < slotCount; ++enemyIndex) {
        const Enemy& enemy = enemies[enemyIndex];

        // 死亡敌人不绘制，但不影响其他敌人的槽位
        if (enemy.hp <= 0) {
            continue;
        }

        const int displayIndex = enemyIndex;

        const float designEnemyX = config.xPositions[displayIndex];
        const float designEnemyY = config.yPosition;

        const float designIntentX =
            designEnemyX + (kEnemyDisplayWidth - kIntentSize) * 0.5f;
        const float designIntentY =
            designEnemyY - kIntentSize - kIntentGapAboveEnemy;

        const float designHealthX =
            designEnemyX + (kEnemyDisplayWidth - kHealthBarWidth) * 0.5f;
        const float designHealthY =
            designEnemyY + kEnemyHealthBarOffsetY;
        const float designStateY =
            designHealthY + kHealthBarHeight + kStateGapBelowHealthBar;

        const sf::Vector2f enemyPos = toScreenPosition(
            designEnemyX,
            designEnemyY,
            scaleX,
            scaleY
        );
        const sf::Vector2f intentPos = toScreenPosition(
            designIntentX,
            designIntentY,
            scaleX,
            scaleY
        );
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

        sf::RectangleShape intentRect({kIntentSize, kIntentSize});

        if (enemy.intent.type == EnemyIntentType::Attack &&
            context_.resources.hasTexture("attack")) {
            intentRect.setTexture(
                &context_.resources.getTexture("attack")
            );
        } else if (enemy.intent.type == EnemyIntentType::Block &&
                   context_.resources.hasTexture("block")) {
            intentRect.setTexture(
                &context_.resources.getTexture("block")
            );
        } else if (enemy.intent.type == EnemyIntentType::Buff &&
                   context_.resources.hasTexture("buff")) {
            intentRect.setTexture(
                &context_.resources.getTexture("buff")
            );
        }

        applyEnemyPlaceholderStyle(intentRect);
        intentRect.setPosition(intentPos);
        window.draw(intentRect);

        if (enemy.intent.value >= 0) {
            sf::Text intentText = TextUtils::createWhiteText(
                font,
                std::to_string(enemy.intent.value),
                25,
                {intentPos.x - 10.f, intentPos.y + kIntentSize + 5.f}
            );
            window.draw(intentText);
        }

        const sf::FloatRect enemyScreenRect =
            getEnemyScreenRect(window, config, displayIndex);

        sf::RectangleShape enemyRect(enemyScreenRect.size);
        enemyRect.setPosition(enemyScreenRect.position);

        if (
            enemyIndex < static_cast<int>(enemyTextureIds_.size()) &&
            !enemyTextureIds_[enemyIndex].empty() &&
            context_.resources.hasTexture(enemyTextureIds_[enemyIndex])
        ) {
            enemyRect.setTexture(
                &context_.resources.getTexture(enemyTextureIds_[enemyIndex])
            );
        }

        applyEnemyPlaceholderStyle(enemyRect);
        window.draw(enemyRect);

        drawHealthBar(
            window,
            context_.resources.getTexture("hp"),
            font,
            healthPos,
            enemy.hp,
            enemy.maxHp
        );

        drawStatusIcons(
            window,
            font,
            statePos.x,
            statePos.y,
            CombatUnitStatus{
                enemy.block,
                enemy.strength,
                enemy.vulnerable,
                enemy.weak
            }
        );
    }
}

