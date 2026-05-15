
#ifndef SPIRELIKE_SCENETRANSITION_HPP
#define SPIRELIKE_SCENETRANSITION_HPP
#include "model/Types.hpp"
struct SceneTransition {
    SceneType target = SceneType::None;
    BattleResult battleResult = BattleResult::Ongoing;

    EncounterId encounterId = 0;
    EventId eventId = 0;
    int mapNodeIndex = -1;
    bool loadGame = false;
    bool saveAndQuit = false;
};

#endif //SPIRELIKE_SCENETRANSITION_HPP