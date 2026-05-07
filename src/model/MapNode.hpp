
#ifndef SPIRELIKE_MAPNODE_HPP
#define SPIRELIKE_MAPNODE_HPP

#include "Types.hpp"

struct MapNode {
    int index = 0;
    MapNodeType type = MapNodeType::Combat;
    MapNodeState state = MapNodeState::Locked;

    EventId eventId = 0;
    EnemyId enemyId = 0;
};

#endif //SPIRELIKE_MAPNODE_HPP