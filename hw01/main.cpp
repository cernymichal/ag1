#include <array>
#include <iostream>
#include <utility>

#include "path.cpp"

using TestCase = std::pair<size_t, Map>;

// Class template argument deduction exists since C++17 :-)
const std::array examples = {
    TestCase{1, Map{2, 0, 0, {{0, 1}}, {{0}}}},
    TestCase{3, Map{2, 0, 0, {{0, 1}}, {{1}}}},
    TestCase{3, Map{4, 0, 1, {{0, 2}, {2, 3}, {0, 3}, {3, 1}}, {}}},
    TestCase{4, Map{4, 0, 1, {{0, 2}, {2, 3}, {0, 3}, {3, 1}}, {{2}}}},
    TestCase{0, Map{4, 0, 1, {{0, 2}, {2, 3}, {0, 3}, {3, 1}}, {{2}, {}}}},
};

int main() {
    int fail = 0;
    for (size_t i = 0; i < examples.size(); i++) {
        auto sol = find_path(examples[i].second);
        if (sol.size() != examples[i].first) {
            std::cout << "Wrong answer for map " << i << std::endl;
            fail++;
        }
    }

    if (fail)
        std::cout << "Failed " << fail << " tests" << std::endl;
    else
        std::cout << "All tests completed" << std::endl;

    return 0;
}
