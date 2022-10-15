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

template <typename T>
inline void orderVariables(T& left, T& right) {
    if (left > right) {
        T temp = left;
        left = right;
        right = temp;
    }
}

#define MAX_ITEMS 12

class GrinchDijkstra {
    using VertexID = std::pair<Place, std::bitset<MAX_ITEMS>>;
    using Vertex = std::pair<VertexID, std::list<Place>>;

public:
    GrinchDijkstra(const Map& map) : map(map), rooms(map.places), itemCount(map.items.size()), allItems((1 << itemCount) - 1) {
    }

    std::list<Place> operator()() {
        if (!initializeRooms())
            return std::list<Place>();

        visited = std::vector<bool>(map.places * (1UL << itemCount));
        queue.reserve(64);
        paths.reserve(64);

        queue[{map.start, rooms[map.start].items}] = {map.start};

        while (!queue.empty()) {
            auto& current = *std::min_element(queue.begin(), queue.end(), vertexCompare());
            visited[getVertexIDIndex(current.first)] = true;

            if (current.first.first == map.end && current.first.second == allItems)
                return current.second;

            if ((current.first.second | rooms[map.end].items) == allItems)
                enqueue(current, {map.end, allItems});
            else {
                for (size_t i = 0; i < itemCount; i++) {
                    if (current.first.second[i])
                        continue;

                    for (const auto& place : map.items[i]) {
                        auto items = current.first.second | rooms[place].items;
                        VertexID placeID(place, items);

                        if (!visited[getVertexIDIndex(placeID)])
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
    };

    struct vertexCompare {
        bool operator()(const Vertex& left, const Vertex& right) const {
            return left.second.size() < right.second.size();
        }
    };

    const Map& map;
    std::vector<Room> rooms;
    size_t itemCount;

    // TODO optimize queuing
    std::unordered_map<VertexID, std::list<Place>> queue;
    std::vector<bool> visited;
    std::unordered_map<std::pair<Place, Place>, std::list<Place>> paths;
    std::bitset<MAX_ITEMS> allItems;

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

    std::list<Place> findPathBetween(const std::pair<Place, Place>& places) const {
        std::queue<std::pair<Place, std::list<Place>>> queue;
        auto queued = std::vector<bool>(rooms.size());

        queue.emplace(places.first, std::list<Place>());
        queued[places.first] = true;

        for (; !queue.empty(); queue.pop()) {
            auto& current = queue.front();

            current.second.push_back(current.first);

            if (current.first == places.second)
                return current.second;

            for (const auto& neighbor : rooms[current.first].neighbors) {
                if (!queued[neighbor]) {
                    queue.emplace(neighbor, current.second);
                    queued[neighbor] = true;
                }
            }
        }

        return std::list<Place>();
    }

    void enqueue(const Vertex& current, const VertexID& target) {
        auto endPoints = std::pair<Place, Place>(current.first.first, target.first);
        orderVariables(endPoints.first, endPoints.second);

        if (paths.find(endPoints) == paths.end())
            paths[endPoints] = findPathBetween(endPoints);

        auto path = paths[endPoints];

        if (path.size() == 0)
            return;

        if (queue.find(target) == queue.end() || queue[target].size() > path.size() + current.second.size() - 1) {
            if (endPoints.second != target.first)
                path.reverse();
            path.erase(path.begin());
            path.insert(path.begin(), current.second.begin(), current.second.end());
            queue[target] = std::move(path);
        }
    }
};

std::list<Place> find_path(const Map& map) {
    return GrinchDijkstra(map)();
}
