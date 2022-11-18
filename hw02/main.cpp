#include <cassert>
#include <iostream>

#include "bestsellers.cpp"

#define CATCH(expr)                    \
    try {                              \
        expr;                          \
        assert(0);                     \
    }                                  \
    catch (const std::out_of_range&) { \
        assert(1);                     \
    };

void test1() {
    Bestsellers<std::string> T;
    T.sell("coke", 32);
    T.sell("bread", 1);
    assert(T.products() == 2);
    T.sell("ham", 2);
    T.sell("mushrooms", 12);

    assert(T.products() == 4);

    assert(T.rank("ham") == 3);
    assert(T.rank("coke") == 1);
    assert(T.sold(1, 3) == 46);
    assert(T.product(2) == "mushrooms");

    T.sell("ham", 11);
    assert(T.products() == 4);
    assert(T.product(2) == "ham");
    assert(T.sold(2) == 13);
    assert(T.sold(2, 2) == 13);
    assert(T.sold(1, 2) == 45);
}

void test2() {
    Bestsellers<std::string> T;
    T.sell("coke", 32);
    T.sell("bread", 1);

    CATCH(T.rank("ham"));
    CATCH(T.product(3));
    CATCH(T.sold(0));
    CATCH(T.sold(9));
    CATCH(T.sold(0, 1));
    CATCH(T.sold(3, 2));
    CATCH(T.sold(1, 9));
}

void testRandom() {
    Bestsellers<std::string> T;
    #include "test"
}

int main() {
    test1();
    test2();
    testRandom();

    std::cout << "All tests completed" << std::endl;
}
