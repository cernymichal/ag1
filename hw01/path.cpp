#ifndef __PROGTEST__
#include <algorithm>
#include <array>
#include <bitset>
#include <cassert>
#include <deque>
#include <iostream>
#include <limits>
#include <list>
#include <map>
#include <memory>
#include <optional>
#include <queue>
#include <set>
#include <stack>
#include <unordered_map>
#include <unordered_set>
#include <vector>

using Place = size_t;

struct Map {
    size_t places;
    Place start, end;
    std::vector<std::pair<Place, Place>> connections;
    std::vector<std::vector<Place>> items;
};

template <typename F, typename S>
struct std::hash<std::pair<F, S>> {
    std::size_t operator()(const std::pair<F, S>& p) const noexcept {
        // something like boost::combine would be much better
        return std::hash<F>()(p.first) ^ (std::hash<S>()(p.second) << 1);
    }
};

template <typename T>
void printList(const std::list<T>& list) {
    for (const auto& el : list)
        std::cout << el << ", ";
    std::cout << "\b\b" << std::endl;
}

#endif

#define MAX_ITEMS 12
#define MAX_BFS_ITEMS 6
#define MAX_BFS_ROOMS 10000

template <typename T>
inline void orderVariables(T& left, T& right) {
    if (left > right) {
        T temp = left;
        left = right;
        right = temp;
    }
}

class GrinchBFS {
    using VertexID = std::pair<Place, std::bitset<MAX_BFS_ITEMS>>;
    using Vertex = std::pair<VertexID, std::list<Place>>;

public:
    GrinchBFS(const Map& map) : map(map), rooms(map.places), itemCount(map.items.size()), allItems((1 << itemCount) - 1) {
    }

    std::list<Place> operator()() {
        if (!initializeRooms())
            return std::list<Place>();

        std::queue<Vertex> queue;
        auto queued = std::make_unique<std::bitset<MAX_BFS_ROOMS*(2UL << MAX_BFS_ITEMS)>>();
        std::bitset<MAX_BFS_ITEMS> allItems = (1 << map.items.size()) - 1;

        queue.emplace(VertexID(map.start, rooms[map.start].items), std::list<Place>());
        (*queued)[getVertexIDIndex(queue.front().first)] = true;

        for (; !queue.empty(); queue.pop()) {
            auto& current = queue.front();

            current.second.push_back(current.first.first);

            if (current.first.first == map.end && current.first.second == allItems)
                return current.second;

            for (const auto& neighbor : rooms[current.first.first].neighbors) {
                auto items = current.first.second | rooms[neighbor].items;
                VertexID neighborID(neighbor, items);

                if (!(*queued)[getVertexIDIndex(neighborID)]) {
                    queue.emplace(neighborID, current.second);
                    (*queued)[getVertexIDIndex(neighborID)] = true;
                }
            }
        }

        return std::list<Place>();
    }

private:
    struct Room {
        std::vector<Place> neighbors;
        std::bitset<MAX_BFS_ITEMS> items;
    };

    const Map& map;
    std::vector<Room> rooms;
    size_t itemCount;
    std::bitset<MAX_BFS_ITEMS> allItems;

    inline size_t getVertexIDIndex(const VertexID& vertexID) const {
        return (vertexID.first << itemCount) | vertexID.second.to_ulong();
    }

    bool initializeRooms() {
        for (const auto& connection : map.connections) {
            rooms[connection.first].neighbors.push_back(connection.second);
            rooms[connection.second].neighbors.push_back(connection.first);
        }

        for (size_t i = 0; i < itemCount; i++) {
            if (map.items[i].size() == 0)
                return false;

            for (const auto& place : map.items[i])
                rooms[place].items[i] = true;
        }

        return true;
    }
};

class GrinchDijkstra {
    using VertexID = std::pair<Place, std::bitset<MAX_ITEMS>>;
    using Vertex = std::pair<VertexID, size_t>;

public:
    GrinchDijkstra(const Map& map) : map(map), rooms(map.places), itemCount(map.items.size()), allItems((1 << itemCount) - 1) {
    }

    std::list<Place> operator()() {
        if (!initializeRooms())
            return std::list<Place>();

        queue.reserve(64);
        paths.reserve(256);

        queue[{map.start, rooms[map.start].items}] = 1;

        while (!queue.empty()) {
            auto& current = *std::min_element(queue.begin(), queue.end(), vertexCompare());
            rooms[current.first.first].visited[current.first.second.to_ulong()] = true;

            if (current.first.first == map.end && current.first.second == allItems)
                return buildPath();

            if ((current.first.second | rooms[map.end].items) == allItems)
                enqueue(current, {map.end, allItems});
            else {
                for (size_t i = 0; i < itemCount; i++) {
                    if (current.first.second[i])
                        continue;

                    for (const auto& place : map.items[i]) {
                        auto items = current.first.second | rooms[place].items;
                        VertexID placeID(place, items);

                        if (!rooms[placeID.first].visited[placeID.second.to_ulong()])
                            enqueue(current, placeID);
                    }
                }
            }

            queue.erase(current.first);
        }

        return std::list<Place>();
    }

private:
    struct Room {
        std::vector<Place> neighbors;
        std::bitset<MAX_ITEMS> items;
        std::vector<VertexID> predecessors;
        std::vector<bool> visited;
    };

    struct vertexCompare {
        bool operator()(const Vertex& left, const Vertex& right) const {
            return left.second < right.second;
        }
    };

    const Map& map;
    std::vector<Room> rooms;
    size_t itemCount;

    std::unordered_map<VertexID, size_t> queue;
    std::unordered_map<std::pair<Place, Place>, std::list<Place>> paths;
    std::bitset<MAX_ITEMS> allItems;

    bool initializeRooms() {
        for (const auto& connection : map.connections) {
            rooms[connection.first].neighbors.push_back(connection.second);
            rooms[connection.second].neighbors.push_back(connection.first);
        }

        rooms[map.end].visited = std::vector<bool>(1UL << itemCount);
        rooms[map.end].predecessors = std::vector<VertexID>(1UL << itemCount, {-1, 0});

        for (size_t i = 0; i < itemCount; i++) {
            if (map.items[i].size() == 0)
                return false;

            for (const auto& place : map.items[i]) {
                rooms[place].items[i] = true;

                if (rooms[place].predecessors.size() == 0) {
                    rooms[place].visited = std::vector<bool>(1UL << itemCount);
                    rooms[place].predecessors = std::vector<VertexID>(1UL << itemCount, {-1, 0});
                }
            }
        }

        rooms[map.start].visited = std::vector<bool>(1UL << itemCount);
        rooms[map.start].predecessors = std::vector<VertexID>(1UL << itemCount, {0, rooms[map.start].items});

        return true;
    }

    const std::list<Place>& findPathBetween(const Place& placeA, const Place& placeB) {
        auto endPoints = std::pair<Place, Place>(placeA, placeB);
        orderVariables(endPoints.first, endPoints.second);

        if (paths.find(endPoints) != paths.end())
            return paths[endPoints];

        std::queue<Place> queue;
        auto predecessors = std::vector<Place>(rooms.size(), ~((size_t)0));

        queue.emplace(endPoints.first);
        predecessors[endPoints.first] = endPoints.first;

        for (; !queue.empty(); queue.pop()) {
            auto& current = queue.front();

            if (current == endPoints.second) {
                auto& path = paths[endPoints] = {endPoints.second};
                for (auto i = path.front(); i != endPoints.first; i = path.front())
                    path.push_front(predecessors[i]);

                return path;
            }

            for (const auto& neighbor : rooms[current].neighbors) {
                if (predecessors[neighbor] == ~((size_t)0)) {
                    queue.emplace(neighbor);
                    predecessors[neighbor] = current;
                }
            }
        }

        auto& path = paths[endPoints] = {};
        return path;
    }

    void enqueue(const Vertex& current, const VertexID& target) {
        auto& path = findPathBetween(current.first.first, target.first);

        if (path.size() == 0)
            return;

        auto newPathLength = path.size() + current.second - 1;

        if (queue.find(target) == queue.end() || queue[target] > newPathLength) {
            queue[target] = newPathLength;
            rooms[target.first].predecessors[target.second.to_ulong()] = current.first;
        }
    }

    std::list<Place> buildPath() {
        std::list<Place> path;
        const VertexID end(map.end, allItems);

        for (auto* to = &end; to->first != map.start || to->second != rooms[map.start].items;) {
            const auto& from = rooms[to->first].predecessors[to->second.to_ulong()];

            auto& connection = findPathBetween(from.first, to->first);
            if (connection.front() == from.first)
                path.insert(path.begin(), ++connection.begin(), connection.end());
            else
                path.insert(path.begin(), ++connection.rbegin(), connection.rend());

            to = &from;
        }

        path.push_front(map.start);

        return path;
    }
};

std::list<Place> find_path(const Map& map) {
    if (map.items.size() > 6 || map.places >= 6000 || true)
        return GrinchDijkstra(map)();
    else
        return GrinchBFS(map)();
}
