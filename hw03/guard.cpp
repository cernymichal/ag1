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

class Solver {
public:
    Solver(const TreeProblem& problem) : m_problem(problem), m_connections(problem.gifts.size()), m_memo(memoIndex(problem.gifts.size(), 0), 0) {
        for (const auto& connection : m_problem.connections) {
            m_connections[connection.first].push_back(connection.second);
            m_connections[connection.second].push_back(connection.first);
        }
    }

    uint64_t maxGuardedGifts() {
        if (m_problem.gifts.empty())
            return 0;

        std::queue<std::pair<ChristmasTree, ChristmasTree>> queue;
        queue.push({0, 0});

        for (; !queue.empty(); queue.pop()) {
            const auto& current = queue.front();
            m_toSolve.push(current.first);

            for (const auto& connection : m_connections[current.first]) {
                if (connection == current.second)
                    continue;

                queue.emplace(std::pair<ChristmasTree, ChristmasTree>(connection, current.first));
            }
        }

        solve();

        uint64_t max = 0;
        for (int i = 0; i <= m_problem.max_group_size; i++)
            max = std::max(max, m_memo[memoIndex(0, i)]);

        return max;
    }

private:
    const TreeProblem& m_problem;
    std::vector<std::vector<ChristmasTree>> m_connections;
    std::vector<uint64_t> m_memo;
    std::stack<ChristmasTree> m_toSolve;

    inline size_t memoIndex(const ChristmasTree& tree, const int currentGroupSize) const {
        return tree * (m_problem.max_group_size + 1) + currentGroupSize;
    }

    void solve() {
        for (; !m_toSolve.empty(); m_toSolve.pop()) {
            const auto& current = m_toSolve.top();

            // without guard
            for (const auto& connection : m_connections[current]) {
                uint64_t max = 0;
                for (int i = 0; i <= m_problem.max_group_size; i++)
                    max = std::max(max, m_memo[memoIndex(connection, i)]);

                m_memo[memoIndex(current, 0)] += max;
            }

            // with one guard
            m_memo[memoIndex(current, 1)] = m_problem.gifts[current];
            for (const auto& connection : m_connections[current])
                m_memo[memoIndex(current, 1)] += m_memo[memoIndex(connection, 0)];

            // with two guards
            if (m_problem.max_group_size == 2) {
                m_memo[memoIndex(current, 2)] = m_problem.gifts[current];

                ChristmasTree maxGuardedNeighbour = m_connections[current][0];
                __int128_t maxGuardedNeighbourDiff = 0;
                for (const auto& connection : m_connections[current]) {
                    m_memo[memoIndex(current, 2)] += m_memo[memoIndex(connection, 0)];

                    __int128_t diff = (__int128_t)m_memo[memoIndex(connection, 1)] - (__int128_t)m_memo[memoIndex(connection, 0)];

                    if (diff > maxGuardedNeighbourDiff) {
                        maxGuardedNeighbour = connection;
                        maxGuardedNeighbourDiff = diff;
                    }
                }

                m_memo[memoIndex(current, 2)] -= m_memo[memoIndex(maxGuardedNeighbour, 0)];
                m_memo[memoIndex(current, 2)] += m_memo[memoIndex(maxGuardedNeighbour, 1)];
            }
        }
    }
};

uint64_t solve(const TreeProblem& problem) {
    return Solver(problem).maxGuardedGifts();
}
