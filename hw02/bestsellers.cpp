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

#include "treeviz.cpp"

#define PRINT_TREE(x) x->print()
#define PRINT(x) std::cout << x << std::endl

#endif

#ifdef __PROGTEST__

#define PRINT_TREE()
#define PRINT(x)

#endif

template <typename T>
inline T max(const T& a, const T& b) {
    if (a >= b)
        return a;
    return b;
}

template <typename Product>
class Bestsellers {
    struct TreeNode;

public:
    ~Bestsellers() {
        delete m_treeTop;
    }

    // The total number of tracked products
    size_t products() const {
        return m_products.size();

        if (!m_treeTop)
            return 0;

        return m_treeTop->m_subTreeCount;
    }

    void sell(const Product& product, size_t amount) {
        const auto productIter = m_products.find(product);
        TreeNode* node = nullptr;

        if (productIter == m_products.end()) {
            node = new TreeNode(product, amount);
            m_products[product] = node;
            if (m_treeTop)
                insertNode(m_treeTop, node);
            else
                m_treeTop = node;
        }
        else {
            node = (*productIter).second;
            node->m_sold += amount;
            removeNode(node);
            insertNode(m_treeTop, node);
        }
    }

    // The most sold product has rank 1
    size_t rank(const Product& product) const {
        const TreeNode* productNode = m_products.at(product);
        return productNode->leftSubTreeCount() + 1 + countRank(productNode);
    }

    const Product& product(size_t rank) const {
        return findByRank(rank)->m_product;
    }

    // How many copies of product with given rank were sold
    size_t sold(size_t rank) const {
        return findByRank(rank)->m_sold;
    }

    // The same but sum over interval of products (including from and to)
    // It must hold: sold(x) == sold(x, x)
    size_t sold(size_t from, size_t to) const {
        // TODO optimize

        if (to < from)
            throw std::out_of_range("rank interval ill formed");

        size_t sum = 0;
        for (size_t i = from; i <= to; i++) {
            sum += findByRank(i)->m_sold;
        }

        return sum;
    }

    // Bonus only, ignore if you are not interested in bonus
    // The smallest (resp. largest) rank r with sold(rank) == sold(r)
    size_t first_same(size_t rank) const {
        // TODO

        return 0;
    }

    size_t last_same(size_t rank) const {
        // TODO

        return 0;
    }

#ifndef __PROGTEST__

    void print() const {
        printTree<TreeNode>("", m_treeTop, false);
    }

#endif

private:
    struct TreeNode {
        const Product m_product;
        size_t m_sold;
        TreeNode* m_parent = nullptr;
        TreeNode* m_leftChild = nullptr;
        TreeNode* m_rightChild = nullptr;
        size_t m_subTreeSumSold;
        int m_subTreeDepth = 1;       // with self
        unsigned m_subTreeCount = 1;  // with self

        TreeNode(const Product& product, size_t sold) : m_product(product), m_sold(sold) {
        }

        ~TreeNode() {
            delete m_leftChild;
            delete m_rightChild;
        }

        size_t leftSubTreeCount() const {
            if (!m_leftChild)
                return 0;
            return m_leftChild->m_subTreeCount;
        }

        size_t rightSubTreeCount() const {
            if (!m_rightChild)
                return 0;
            return m_rightChild->m_subTreeCount;
        }

        size_t leftSubTreeDepth() const {
            if (!m_leftChild)
                return 0;
            return m_leftChild->m_subTreeDepth;
        }

        size_t rightSubTreeDepth() const {
            if (!m_rightChild)
                return 0;
            return m_rightChild->m_subTreeDepth;
        }

        int sign() const {
            return rightSubTreeDepth() - leftSubTreeDepth();
        }

        void clear() {
            m_parent = nullptr;
            m_leftChild = nullptr;
            m_rightChild = nullptr;
            m_subTreeCount = 1;
            m_subTreeDepth = 1;
            // TODO sum
        }

        void replaceWithPredecessor(TreeNode* node) {
            if (node->m_parent == this) {
                node->m_parent = node;
                m_leftChild = node->m_leftChild;
            }

            if (node->m_leftChild) {
                node->m_leftChild->m_parent = node->m_parent;
                node->m_leftChild->fixParentChild(node);
            }
            else if (node->m_parent != this) {
                node->m_parent->m_rightChild = nullptr;
            }

            node->m_parent = m_parent;
            node->m_leftChild = m_leftChild;
            node->m_rightChild = m_rightChild;
            node->m_subTreeCount = m_subTreeCount;
            node->m_subTreeDepth = m_subTreeDepth;

            node->fixChildrenParent();
            node->fixParentChild(this);
        }

        void rotateR() {
            m_parent->m_leftChild = m_rightChild;
            m_rightChild = m_parent;
            m_parent = m_parent->m_parent;

            fixChildrenParent();
            m_rightChild->fixChildrenParent();
            fixParentChild(m_rightChild);

            m_rightChild->recalculate();
            recalculate();
        }

        void rotateL() {
            m_parent->m_rightChild = m_leftChild;
            m_leftChild = m_parent;
            m_parent = m_parent->m_parent;

            fixChildrenParent();
            m_leftChild->fixChildrenParent();
            fixParentChild(m_leftChild);

            m_leftChild->recalculate();
            recalculate();
        }

        void rotateLR() {
            rotateL();
            rotateR();
        }

        void rotateRL() {
            rotateR();
            rotateL();
        }

        void fixParentChild(TreeNode* oldNode) {
            if (!m_parent)
                return;

            if (m_parent->m_leftChild == oldNode)
                m_parent->m_leftChild = this;
            else if (m_parent->m_rightChild == oldNode)
                m_parent->m_rightChild = this;
        }

        void fixChildrenParent() {
            if (m_leftChild)
                m_leftChild->m_parent = this;
            if (m_rightChild)
                m_rightChild->m_parent = this;
        }

        void recalculate() {
            recalculateCount();
            recalculateDepth();
        }

        void recalculateCount() {
            m_subTreeCount = leftSubTreeCount() + rightSubTreeCount() + 1;
        }

        void recalculateDepth() {
            m_subTreeDepth = max(leftSubTreeDepth(), rightSubTreeDepth()) + 1;
        }
    };

    TreeNode* m_treeTop = nullptr;  // AVL BST
    std::unordered_map<Product, TreeNode*> m_products;

    void insertNode(TreeNode* current, TreeNode* toInsert) {
        TreeNode** next;

        if (current->m_sold <= toInsert->m_sold)
            next = &current->m_leftChild;
        else
            next = &current->m_rightChild;

        if (*next == nullptr) {
            *next = toInsert;
            toInsert->m_parent = current;
            rebalance(toInsert);
        }
        else
            insertNode(*next, toInsert);
    }

    /*

    void incrementDepth(TreeNode* current) {
        if (!current->m_parent)
            return;

        if (current->m_parent->m_subTreeDepth >= current->m_subTreeDepth + 1)
            return;

        AVLRotate(current->m_parent);

        if (!current->m_parent)
            return;

        incrementDepth(current->m_parent);
    }

    void decrementDepth(TreeNode* current) {
        if (!current->m_parent)
            return;

        if ((current->m_parent->m_leftChild == current && current->m_parent->m_rightChild && current->m_parent->m_rightChild->m_subTreeDepth >= current->m_subTreeDepth) || (current->m_parent->m_rightChild == current && current->m_parent->m_leftChild && current->m_parent->m_leftChild->m_subTreeDepth >= current->m_subTreeDepth))
            return;

        AVLRotate(current->m_parent);

        if (!current->m_parent)
            return;

        decrementDepth(current->m_parent);
    }

    */

    void rebalance(TreeNode* current) {
        if (!current)
            return;

        AVLRotate(current);
        rebalance(current->m_parent);
    }

    void AVLRotate(TreeNode* node) {
        switch (node->sign()) {
            case -2:
                switch (node->m_leftChild->sign()) {
                    case -1:
                        node->m_leftChild->rotateR();
                        break;
                    case +1:
                        node->m_leftChild->m_rightChild->rotateLR();
                        break;
                }
                break;
            case +2:
                switch (node->m_rightChild->sign()) {
                    case -1:
                        node->m_rightChild->m_leftChild->rotateRL();
                        break;
                    case +1:
                        node->m_rightChild->rotateL();
                        break;
                }
                break;
        }

        node->recalculate();

        if (!node->m_parent)
            m_treeTop = node;
        else if (!node->m_parent->m_parent)
            m_treeTop = node->m_parent;
    }

    // doesnt delete node!
    void removeNode(TreeNode* node) {
        auto predecessor = findPredecessor(node);
        TreeNode* rebalanceStart = nullptr;

        if (!predecessor)
            predecessor = node->m_parent;

        if (node == m_treeTop)
            m_treeTop = predecessor;

        if (predecessor != node->m_parent) {
            if (predecessor->m_parent == node)
                rebalanceStart = predecessor;
            else
                rebalanceStart = predecessor->m_parent == node ? predecessor : predecessor->m_parent;
            node->replaceWithPredecessor(predecessor);
        }

        else if (node->m_rightChild) {
            rebalanceStart = node->m_rightChild;
            node->m_rightChild->m_parent = node->m_parent;
            node->m_rightChild->fixParentChild(node);

            if (!m_treeTop)
                m_treeTop = node->m_rightChild;
        }
        else if (node->m_parent) {
            rebalanceStart = node->m_parent;
            if (node->m_parent->m_leftChild == node)
                node->m_parent->m_leftChild = nullptr;
            else if (node->m_parent->m_rightChild == node)
                node->m_parent->m_rightChild = nullptr;
        }

        rebalance(rebalanceStart);

        node->clear();
    }

    TreeNode* findPredecessor(TreeNode* node) const {
        if (node->m_leftChild)
            node = node->m_leftChild;
        else
            return nullptr;

        for (; node->m_rightChild; node = node->m_rightChild)
            ;

        return node;
    }

    size_t countRank(const TreeNode* node) const {
        if (!node->m_parent)
            return 0;

        if (node->m_parent->m_leftChild == node)
            return countRank(node->m_parent);

        return node->m_parent->leftSubTreeCount() + 1 + countRank(node->m_parent);
    }

    // throws std::out_of_range when theres no product with rank
    TreeNode* findByRank(size_t rank) const {
        if (rank == 0 || rank > products())
            throw std::out_of_range("rank out of range");

        return findByRank(rank, 0, m_treeTop);
    }

    TreeNode* findByRank(size_t rank, size_t leftCount, TreeNode* current) const {
        const auto left = leftCount + current->leftSubTreeCount() + 1;

        if (left == rank)
            return current;

        if (left > rank)
            return findByRank(rank, leftCount, current->m_leftChild);

        return findByRank(rank, left, current->m_rightChild);
    }
};
