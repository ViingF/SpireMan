
#ifndef SPIRELIKE_MAPNODE_HPP
#define SPIRELIKE_MAPNODE_HPP

#include "Types.hpp"

struct MapNode {
    int index = 0;
    MapNodeType type = MapNodeType::Combat;
    MapNodeState state = MapNodeState::Locked;

    EventId eventId = 0;
    EnemyId enemyId = 0;
    int layer = 0;
    int slot = 0;
    int orderInLayer = 0;
    float mapX = 0.0f;
    float mapY = 0.0f;
    std::vector<int> nextIndices;
};

#endif //SPIRELIKE_MAPNODE_HPP