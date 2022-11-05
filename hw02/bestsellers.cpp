#ifndef __PROGTEST__
#include <algorithm>
#include <array>
#include <bitset>
#include <cassert>
#include <deque>
#include <iostream>
#include <limits>
#include <list>
#include <memory>
#include <optional>
#include <queue>
#include <random>
#include <stack>
#include <unordered_map>
#include <unordered_set>
#include <vector>

#endif

// TODO implement
template <typename Product>
struct Bestsellers {
    // The total number of tracked products
    size_t products() const;

    void sell(const Product& p, size_t amount);

    // The most sold product has rank 1
    size_t rank(const Product& p) const;
    const Product& product(size_t rank) const;

    // How many copies of product with given rank were sold
    size_t sold(size_t rank) const;
    // The same but sum over interval of products (including from and to)
    // It must hold: sold(x) == sold(x, x)
    size_t sold(size_t from, size_t to) const;

    // Bonus only, ignore if you are not interested in bonus
    // The smallest (resp. largest) rank with sold(rank) == sold(r)
    size_t first_same(size_t r) const {
        return 0;
    }

    size_t last_same(size_t r) const {
        return 0;
    }
};
