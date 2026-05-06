
#ifndef SPIRELIKE_SCENETRANSITION_HPP
#define SPIRELIKE_SCENETRANSITION_HPP

struct SceneTransition {
    SceneType target = SceneType::None;
    BattleResult battleResult = BattleResult::Ongoing;
    EnemyId enemyId = 0;
};
#endif //SPIRELIKE_SCENETRANSITION_HPP