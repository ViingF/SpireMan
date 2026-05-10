#include "MapSystem.hpp"

#include "config/Constants.hpp"

#include <algorithm>
#include <cmath>
#include <random>
#include <vector>
#include <SFML/System/Vector2.hpp>

namespace {

    float clampFloat(float value, float minValue, float maxValue)
    {
        return std::max(minValue, std::min(value, maxValue));
    }

    float cross(
        sf::Vector2f a,
        sf::Vector2f b,
        sf::Vector2f c
    )
    {
        return (b.x - a.x) * (c.y - a.y) -
               (b.y - a.y) * (c.x - a.x);
    }

    bool segmentsCross(
        sf::Vector2f a,
        sf::Vector2f b,
        sf::Vector2f c,
        sf::Vector2f d
    )
    {
        const float c1 = cross(a, b, c);
        const float c2 = cross(a, b, d);
        const float c3 = cross(c, d, a);
        const float c4 = cross(c, d, b);

        return c1 * c2 < 0.0f && c3 * c4 < 0.0f;
    }

    bool wouldCrossExistingEdge(
        const std::vector<MapNode>& nodes,
        const std::vector<std::pair<int, int>>& edges,
        int from,
        int to
    )
    {
        const sf::Vector2f a(nodes[from].mapX, nodes[from].mapY);
        const sf::Vector2f b(nodes[to].mapX, nodes[to].mapY);

        for (const auto& edge : edges) {
            const int otherFrom = edge.first;
            const int otherTo = edge.second;

            if (
                from == otherFrom ||
                from == otherTo ||
                to == otherFrom ||
                to == otherTo
            ) {
                continue;
            }

            const sf::Vector2f c(
                nodes[otherFrom].mapX,
                nodes[otherFrom].mapY
            );

            const sf::Vector2f d(
                nodes[otherTo].mapX,
                nodes[otherTo].mapY
            );

            if (segmentsCross(a, b, c, d)) {
                return true;
            }
        }

        return false;
    }

void addUniqueEdge(std::vector<int>& edges, int target)
{
    if (std::find(edges.begin(), edges.end(), target) == edges.end()) {
        edges.push_back(target);
    }
}

int chooseNearestSlot(
    int fromSlot,
    int fromCount,
    int toCount
)
{
    if (toCount <= 1) {
        return 0;
    }

    if (fromCount <= 1) {
        return toCount / 2;
    }

    const float normalized =
        static_cast<float>(fromSlot) /
        static_cast<float>(fromCount - 1);

    int targetSlot = static_cast<int>(
        std::round(normalized * static_cast<float>(toCount - 1))
    );

    targetSlot = std::clamp(targetSlot, 0, toCount - 1);

    return targetSlot;
}

bool hasIncomingEdge(
    const std::vector<MapNode>& nodes,
    const std::vector<int>& fromLayer,
    int targetNodeIndex
)
{
    for (int fromIndex : fromLayer) {
        const std::vector<int>& edges =
            nodes[fromIndex].nextIndices;

        if (
            std::find(edges.begin(), edges.end(), targetNodeIndex)
            != edges.end()
        ) {
            return true;
        }
    }

    return false;
}

MapNodeType chooseMiddleNodeType(
    RunState& runState,
    const EventDatabase& eventDatabase
)
{
    std::uniform_int_distribution<int> percentDist(1, 100);
    const int roll = percentDist(runState.rng);

    if (roll <= MAP_CAMPFIRE_CHANCE_PERCENT) {
        return MapNodeType::Campfire;
    }

    if (
        roll <=
        MAP_CAMPFIRE_CHANCE_PERCENT +
        MAP_SHOP_CHANCE_PERCENT
    ) {
        return MapNodeType::Shop;
    }

    if (
        roll <=
        MAP_CAMPFIRE_CHANCE_PERCENT +
        MAP_SHOP_CHANCE_PERCENT +
        MAP_EVENT_CHANCE_PERCENT &&
        !eventDatabase.getAllEventIds().empty()
    ) {
        return MapNodeType::Event;
    }

    return MapNodeType::Combat;
}

}

void MapSystem::generateRouteMap(
    RunState& runState,
    const EventDatabase& eventDatabase
) const
{
    runState.mapNodes.clear();
    runState.currentMapNodeIndex = -1;
    runState.selectedMapNodeIndex = -1;

    constexpr int layerCount = MAP_ROUTE_LENGTH;

    constexpr float centerX = 960.0f;

    constexpr float minX = 640.0f;
    constexpr float maxX = 1280.0f;

    constexpr float bottomY = 1000.0f;
    constexpr float topY = 350.0f;

    std::uniform_int_distribution<int> middleNodeCountDist(2, 4);
    std::uniform_real_distribution<float> jitterDist(-45.0f, 45.0f);
    std::uniform_int_distribution<int> percentDist(1, 100);
    std::uniform_int_distribution<int> extraEdgeDist(0, 100);

    std::vector<std::vector<int>> layers;
    layers.resize(layerCount);

    int nextNodeIndex = 0;

    for (int layer = 0; layer < layerCount; ++layer) {
        int count = 1;

        const bool firstLayer = layer == 0;
        const bool lastLayer = layer == layerCount - 1;

        if (!firstLayer && !lastLayer) {
            count = middleNodeCountDist(runState.rng);
        }

        const float t = layerCount <= 1
            ? 0.0f
            : static_cast<float>(layer) /
              static_cast<float>(layerCount - 1);

        float y = bottomY + (topY - bottomY) * t;
        if (layer == layerCount - 1) {
            y -= 80.0f;
        }


        for (int order = 0; order < count; ++order) {
            MapNode node;
            node.index = nextNodeIndex;
            node.layer = layer;
            node.orderInLayer = order;
            node.state = MapNodeState::Locked;

            if (firstLayer || lastLayer) {
                node.type = MapNodeType::Combat;
            } else if (layer == layerCount - 2) {
                node.type = MapNodeType::Campfire;
            } else {
                node.type = MapNodeType::Combat;

                const int shopRoll = percentDist(runState.rng);

                if (shopRoll <= MAP_SHOP_CHANCE_PERCENT) {
                    node.type = MapNodeType::Shop;
                } else if (percentDist(runState.rng)<MAP_CAMPFIRE_CHANCE_PERCENT) {
                    node.type = MapNodeType::Campfire;
                } else {
                    const int eventRoll = percentDist(runState.rng);

                    const bool makeEvent =
                        eventRoll <= MAP_EVENT_CHANCE_PERCENT &&
                        !eventDatabase.getAllEventIds().empty();

                    if (makeEvent) {
                        node.type = MapNodeType::Event;
                        node.eventId =
                            eventDatabase.chooseRandomEventId(runState.rng);
                    }
                }
            }


            float x = centerX;

            if (count > 1) {
                const float compactWidth =
                    std::min(520.0f, 190.0f * static_cast<float>(count - 1));

                const float left = centerX - compactWidth * 0.5f;
                const float step =
                    compactWidth / static_cast<float>(count - 1);

                x = left + step * static_cast<float>(order);
            }

            const float layerOffset =
                layer % 2 == 0 ? -28.0f : 28.0f;

            x += layerOffset;
            x += jitterDist(runState.rng);

            node.mapX = clampFloat(x, minX, maxX);
            node.mapY = y;

            runState.mapNodes.push_back(node);
            layers[layer].push_back(nextNodeIndex);

            ++nextNodeIndex;
        }

        std::sort(
            layers[layer].begin(),
            layers[layer].end(),
            [&](int a, int b) {
                return runState.mapNodes[a].mapX <
                       runState.mapNodes[b].mapX;
            }
        );

        for (int order = 0; order < static_cast<int>(layers[layer].size()); ++order) {
            runState.mapNodes[layers[layer][order]].orderInLayer = order;
        }
    }

    for (int layer = 0; layer + 1 < layerCount; ++layer) {
        const std::vector<int>& current = layers[layer];
        const std::vector<int>& next = layers[layer + 1];

        std::vector<std::pair<int, int>> layerEdges;

        for (int i = 0; i < static_cast<int>(current.size()); ++i) {
            const int from = current[i];

            const float ratio = current.size() <= 1
                ? 0.5f
                : static_cast<float>(i) /
                  static_cast<float>(current.size() - 1);

            int targetOrder = static_cast<int>(
                std::round(ratio * static_cast<float>(next.size() - 1))
            );

            targetOrder = std::clamp(
                targetOrder,
                0,
                static_cast<int>(next.size()) - 1
            );

            const int to = next[targetOrder];

            runState.mapNodes[from].nextIndices.push_back(to);
            layerEdges.push_back({from, to});
        }

        for (int targetOrder = 0;
             targetOrder < static_cast<int>(next.size());
             ++targetOrder) {
            const int to = next[targetOrder];

            if (hasIncomingEdge(runState.mapNodes, current, to)) {
                continue;
            }

            const int nearestFromOrder = chooseNearestSlot(
                targetOrder,
                static_cast<int>(next.size()),
                static_cast<int>(current.size())
            );

            std::vector<int> candidateFromOrders;

            candidateFromOrders.push_back(nearestFromOrder);

            for (int offset = 1;
                 offset < static_cast<int>(current.size());
                 ++offset) {
                const int left = nearestFromOrder - offset;
                const int right = nearestFromOrder + offset;

                if (left >= 0) {
                    candidateFromOrders.push_back(left);
                }

                if (right < static_cast<int>(current.size())) {
                    candidateFromOrders.push_back(right);
                }
                 }

            bool added = false;

            for (int fromOrder : candidateFromOrders) {
                const int from = current[fromOrder];

                if (wouldCrossExistingEdge(
                        runState.mapNodes,
                        layerEdges,
                        from,
                        to
                    )) {
                    continue;
                    }

                addUniqueEdge(runState.mapNodes[from].nextIndices, to);
                layerEdges.push_back({from, to});
                added = true;
                break;
            }

            if (!added) {
                const int from = current[nearestFromOrder];

                addUniqueEdge(runState.mapNodes[from].nextIndices, to);
                layerEdges.push_back({from, to});
            }
             }

        for (int i = 0; i < static_cast<int>(current.size()); ++i) {
            if (extraEdgeDist(runState.rng) > 35) {
                continue;
            }

            const int from = current[i];

            const int baseOrder =
                runState.mapNodes[
                    runState.mapNodes[from].nextIndices.front()
                ].orderInLayer;

            std::vector<int> candidates;

            if (baseOrder - 1 >= 0) {
                candidates.push_back(baseOrder - 1);
            }

            if (baseOrder + 1 < static_cast<int>(next.size())) {
                candidates.push_back(baseOrder + 1);
            }

            std::shuffle(
                candidates.begin(),
                candidates.end(),
                runState.rng
            );

            for (int candidateOrder : candidates) {
                const int to = next[candidateOrder];

                auto& nextIndices = runState.mapNodes[from].nextIndices;

                if (
                    std::find(
                        nextIndices.begin(),
                        nextIndices.end(),
                        to
                    ) != nextIndices.end()
                ) {
                    continue;
                }

                if (
                    wouldCrossExistingEdge(
                        runState.mapNodes,
                        layerEdges,
                        from,
                        to
                    )
                ) {
                    continue;
                }

                nextIndices.push_back(to);
                layerEdges.push_back({from, to});
                break;
            }
        }
    }

    refreshNodeStates(runState);
}


void MapSystem::refreshNodeStates(RunState& runState) const
{
    for (MapNode& node : runState.mapNodes) {
        if (node.state != MapNodeState::Completed) {
            node.state = MapNodeState::Locked;
        }
    }

    if (runState.currentMapNodeIndex < 0) {
        for (MapNode& node : runState.mapNodes) {
            if (node.layer == 0 &&
                node.state != MapNodeState::Completed) {
                node.state = MapNodeState::Available;
                }
        }

        return;
    }

    if (
        runState.currentMapNodeIndex >=
        static_cast<int>(runState.mapNodes.size())
    ) {
        return;
    }

    const MapNode& current =
        runState.mapNodes[runState.currentMapNodeIndex];

    for (int nextIndex : current.nextIndices) {
        if (
            nextIndex >= 0 &&
            nextIndex < static_cast<int>(runState.mapNodes.size()) &&
            runState.mapNodes[nextIndex].state != MapNodeState::Completed
        ) {
            runState.mapNodes[nextIndex].state = MapNodeState::Available;
        }
    }
}


ErrorCode MapSystem::selectNode(
    RunState& runState,
    int nodeIndex
) const
{
    refreshNodeStates(runState);

    if (
        nodeIndex < 0 ||
        nodeIndex >= static_cast<int>(runState.mapNodes.size())
    ) {
        return ErrorCode::INVALID_SCENE_STATE;
    }

    if (runState.mapNodes[nodeIndex].state != MapNodeState::Available) {
        return ErrorCode::INVALID_SCENE_STATE;
    }

    runState.selectedMapNodeIndex = nodeIndex;

    return ErrorCode::OK;
}

void MapSystem::completeSelectedNode(RunState& runState) const
{
    const int index = runState.selectedMapNodeIndex;

    if (
        index < 0 ||
        index >= static_cast<int>(runState.mapNodes.size())
    ) {
        return;
    }

    runState.mapNodes[index].state = MapNodeState::Completed;
    runState.currentMapNodeIndex = index;
    runState.selectedMapNodeIndex = -1;

    refreshNodeStates(runState);
}

bool MapSystem::isRouteFinished(const RunState& runState) const
{
    if (
        runState.currentMapNodeIndex < 0 ||
        runState.currentMapNodeIndex >=
            static_cast<int>(runState.mapNodes.size())
    ) {
        return false;
    }

    const MapNode& currentNode =
        runState.mapNodes[runState.currentMapNodeIndex];

    return currentNode.nextIndices.empty();
}
