#ifndef __PROGTEST__
#include <algorithm>
#include <array>
#include <bitset>
#include <cassert>
#include <cstdint>
#include <deque>
#include <iostream>
#include <limits>
#include <list>
#include <map>
#include <memory>
#include <optional>
#include <queue>
#include <random>
#include <set>
#include <stack>
#include <unordered_map>
#include <unordered_set>
#include <vector>

using ChristmasTree = size_t;

struct TreeProblem {
    int max_group_size;
    std::vector<uint64_t> gifts;
    std::vector<std::pair<ChristmasTree, ChristmasTree>> connections;
};

#endif

#define NULL_MEMO (uint64_t)(-1)

class Solver {
public:
    Solver(const TreeProblem& problem) : m_problem(problem), m_connections(problem.gifts.size()), m_memo(memoIndex(problem.gifts.size(), 0), NULL_MEMO) {
        for (const auto& connection : m_problem.connections) {
            m_connections[connection.first].push_back(connection.second);
            m_connections[connection.second].push_back(connection.first);
        }
    }

    uint64_t maxGuardedGifts() {
        if (m_problem.gifts.empty())
            return 0;

        return maxGuardedGifts(0, 0, 0);
    }

private:
    const TreeProblem& m_problem;
    std::vector<std::vector<ChristmasTree>> m_connections;
    std::vector<uint64_t> m_memo;

    inline size_t memoIndex(const ChristmasTree& tree, const int currentGroupSize) const {
        return tree * (m_problem.max_group_size + 1) + currentGroupSize;
    }

    uint64_t maxGuardedGifts(const ChristmasTree& tree, const ChristmasTree& cameFrom, const int currentGroupSize) {
        if (m_memo[memoIndex(tree, currentGroupSize)] != NULL_MEMO)
            return m_memo[memoIndex(tree, currentGroupSize)];

        uint64_t sumWithGuard = 0;
        uint64_t sumWithoutGuard = 0;

        // with guard
        if (currentGroupSize < m_problem.max_group_size) {
            sumWithGuard = m_problem.gifts[tree];
            for (const auto& connection : m_connections[tree]) {
                if (connection != cameFrom)
                    sumWithGuard += maxGuardedGifts(connection, tree, currentGroupSize + 1);
            }
        }

        // without guard
        for (const auto& connection : m_connections[tree]) {
            if (connection != cameFrom)
                sumWithoutGuard += maxGuardedGifts(connection, tree, 0);
        }

        auto max = std::max(sumWithGuard, sumWithoutGuard);
        m_memo[memoIndex(tree, currentGroupSize)] = max;
        return max;
    }
};

uint64_t solve(const TreeProblem& problem) {
    return Solver(problem).maxGuardedGifts();
}
