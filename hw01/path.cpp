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
    std::vector<size_t> neighbors;
    std::vector<size_t> items;
};

#define MAX_ITEMS 12

struct VertexID {
    size_t place;
    std::bitset<MAX_ITEMS> items;

    bool operator<(const VertexID& right) const {
        return place < right.place || items.to_ulong() < right.items.to_ulong();
    }

    bool operator==(const VertexID& right) const {
        return place == right.place && items == right.items;
    }
};

// doesnt compile on PT for some reason

/*
template <>
struct std::hash<VertexID> {
    std::size_t operator()(const VertexID& vertexID) const noexcept {
        return vertexID.place | (vertexID.items.to_ulong() << 14);
    }
};
*/

using Vertex = std::pair<VertexID, std::list<size_t>>;

std::list<size_t> find_path(const Map& map) {
    std::vector<Room> rooms(map.places);

    for (const auto& connection : map.connections) {
        rooms[connection.first].neighbors.push_back(connection.second);
        rooms[connection.second].neighbors.push_back(connection.first);
    }

    for (size_t i = 0; i < map.items.size(); i++) {
        if (map.items[i].size() == 0)
            return std::list<size_t>();

        for (const auto& place : map.items[i])
            rooms[place].items.push_back(i);
    }

    std::queue<Vertex> queue;
    std::set<VertexID> visited;  // TODO unordered
    std::bitset<MAX_ITEMS> allItems = (1 << map.items.size()) - 1;

    queue.emplace(VertexID{map.start, 0}, std::list<size_t>());

    for (; !queue.empty(); queue.pop()) {
        auto& current = queue.front();

        if (visited.find(current.first) != visited.end())
            continue;

        if (current.second.size() == 0 || current.second.back() != current.first.place)
            current.second.push_back(current.first.place);

        if (current.first.place == map.end && (current.first.items ^ allItems).none())
            return current.second;

        for (const auto& neighbor : rooms[current.first.place].neighbors)
            queue.emplace(VertexID{neighbor, current.first.items}, current.second);

        for (const auto& item : rooms[current.first.place].items) {
            auto items = current.first.items;
            items[item] = true;
            queue.emplace(VertexID{current.first.place, items}, current.second);
        }

        visited.insert(current.first);
    }

    return std::list<size_t>();
}
