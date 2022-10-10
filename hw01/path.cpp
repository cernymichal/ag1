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

#endif

struct Room {
    std::vector<Place> neighbors;
    std::vector<size_t> items;
};

#define MAX_ITEMS 12
#define MAX_ROOMS 10000

using VertexID = std::pair<Place, std::bitset<MAX_ITEMS>>;
using Vertex = std::pair<VertexID, std::list<Place>>;

size_t getVertexIDIndex(const VertexID& vertexID) {
    return (vertexID.first << MAX_ITEMS) | vertexID.second.to_ulong();
}

std::list<Place> find_path(const Map& map) {
    std::vector<Room> rooms(map.places);

    for (const auto& connection : map.connections) {
        rooms[connection.first].neighbors.push_back(connection.second);
        rooms[connection.second].neighbors.push_back(connection.first);
    }

    for (size_t i = 0; i < map.items.size(); i++) {
        if (map.items[i].size() == 0)
            return std::list<Place>();

        for (const auto& place : map.items[i])
            rooms[place].items.push_back(i);
    }

    std::queue<Vertex> queue;
    // std::bitset<MAX_ROOMS * (2 << MAX_ITEMS)> queued;
    std::vector<bool> queued(map.places * (2 << MAX_ITEMS));
    std::bitset<MAX_ITEMS> allItems = (1 << map.items.size()) - 1;

    queue.emplace(VertexID(map.start, 0), std::list<Place>());
    queued[getVertexIDIndex(queue.front().first)] = true;

    for (; !queue.empty(); queue.pop()) {
        auto& current = queue.front();

        if (current.second.size() == 0 || current.second.back() != current.first.first)
            current.second.push_back(current.first.first);

        if (current.first.first == map.end && (current.first.second ^ allItems).none())
            return current.second;

        for (const auto& neighbor : rooms[current.first.first].neighbors) {
            VertexID neighborID(neighbor, current.first.second);

            if (!queued[getVertexIDIndex(neighborID)]) {
                queue.emplace(neighborID, current.second);
                queued[getVertexIDIndex(neighborID)] = true;
            }
        }

        for (const auto& item : rooms[current.first.first].items) {
            /*
            if (current.first.second[item])
                continue;
            */

            auto items = current.first.second;
            items[item] = true;

            VertexID neighborID(current.first.first, items);

            if (!queued[getVertexIDIndex(neighborID)]) {
                queue.emplace(neighborID, current.second);
                queued[getVertexIDIndex(neighborID)] = true;
            }
        }
    }

    return std::list<Place>();
}
