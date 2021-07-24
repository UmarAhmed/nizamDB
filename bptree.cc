#include "bptree.h"


using namespace std;


int main() {
    BPTree<int, int> t;

    for (int i = 0; i < 12; i++) {
        t.insert(i, i);

        if (t.size() != i + 1) {
            cout << "wrong size after insertion";
        }   
    }

    t.print();

    for (int i = 0; i < 12; i++) {
        auto x = t.search(i);

        if (x == nullptr || *x != i) {
            std::cout << "failed to find: " << i << std::endl;
        }
    }

    auto r = t.range_query(4, 9);
    for (const auto x: r) {
        std::cout << x << ", ";
    }
    std::cout << std::endl;
}
