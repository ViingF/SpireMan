#ifndef SPIRELIKE_SCENE_HPP
#define SPIRELIKE_SCENE_HPP

enum class SceneType {
    None,
    Menu,
    Map,
    Combat,
    Reward,
    End
};

struct SceneTransition {
    SceneType target = SceneType::None;
    BattleResult battleResult = BattleResult::Ongoing;
    EnemyId enemyId = 0;
};


#endif //SPIRELIKE_SCENE_HPP