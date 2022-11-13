#include <iostream>

// https://stackoverflow.com/a/51730733
template <typename nodeType>
void printTree(const std::string& prefix, const nodeType* node, bool isLeft) {
    if (node != nullptr) {
        std::cout << prefix;

        std::cout << (isLeft ? "├<─" : "└>─");

        // print the value of the node
        std::cout << node->m_product << " " << node->m_sold << " (" << node->m_subTreeLevels << ')' << std::endl;

        // enter the next tree level - left and right branch
        printTree(prefix + (isLeft && node->m_parent && node->m_parent->m_rightChild ? "│   " : "    "), node->m_leftChild, true);
        printTree(prefix + (isLeft && node->m_parent && node->m_parent->m_rightChild ? "│   " : "    "), node->m_rightChild, false);
    }
}
