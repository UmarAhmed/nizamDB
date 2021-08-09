#include "bptree.h"

#include <random>


using namespace std;


int main() {
    BPTree<int, int> t;
    constexpr int N = 5175;

    std::random_device rd;  // used to obtain a seed for the random number engine
    const auto seed = rd();
    std::mt19937 gen(seed); //Standard mersenne_twister_engine seeded with rd()
    std::uniform_int_distribution<int> distrib(-2 * N, 2 * N);

    // Insert and check size
    for (int i = 0; i <= N; i++) {
        t.insert(i, i);

        if (t.size() != i + 1) {
            cout << "wrong size after insertion";
            return 1;
        }   
    }


    // Test search
    for (int i = 0; i <= N; i++) {
        auto x = t.search(i);

        if (x == nullptr || *x != i) {
            std::cout << "failed to find: " << i << std::endl;
            return 1;
        }
    }
    

    // Test range query: do 100 random queries
    for (int count = 0; count < 100; ++count) {
        auto left = distrib(gen);
        auto right = distrib(gen);
        auto r = t.range_query(left, right);

        auto from = max(0, left);
        auto until = min(N, right);

        int j = 0;
        for (int i = from; i <= until; ++i) {
            if (r[j] != i) {
                std::cout << "error with result from range query" << std::endl;
                return 1;
            }
            ++j;
        } 
    }
}
