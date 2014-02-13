#include <stdlib.h>
#include <time.h>
#include <map>

#include <gtest/gtest.h>

#include <util/FastMap.h>

#define MAP_SIZE 100000

TEST(FastMap, Fuzzy) {
    std::map<int, int> sm;
    FastMap<int, int> fm(MAP_SIZE);

    srand(time(NULL));

    int *data = new int[MAP_SIZE*5];

    for (int j = 0; j < MAP_SIZE*5; j++)
        data[j] = j;

    for (int j = 5*MAP_SIZE-1; j > 0; j--) {
        int idx = 1.0*(j-1)*rand()/RAND_MAX;
        int t = data[idx];
        data[idx] = data[j];
        data[j] = t;
    }
    

    for (int i = 0; i < MAP_SIZE; i++) {
        int k = data[i];
        int v = rand();
        sm[k] = v;
        fm.put(k, v);
    }

    fm.sort();

    for (auto &i: sm) {
        EXPECT_EQ(fm.get(i.first), i.second);
    }

}

#ifndef NDEBUG
TEST(FastMap, DuplicatedKey) {
    FastMap<int, int> fm;

    fm.put(1, 1);
    fm.put(1, 1);
    ASSERT_THROW(fm.get(1), invalid_argument);
}
#endif

#ifndef NDEBUG
TEST(FastMap, InvalidKey) {
    FastMap<int, int> fm;

    fm.put(1, 1);
    ASSERT_THROW(fm.get(2), invalid_argument);
}
#endif
