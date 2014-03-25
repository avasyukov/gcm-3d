#include <iostream>
#include <sys/time.h>
#include <cmath>
#include <ctime>
#include <unordered_map>

#include <libgcm/util/FastMap.h>
#include "tests/perf/util.h"

using namespace std;

#define LEN 10000000

int main() {
    unordered_map<int, int> um;

    srand(time(NULL));

    int *tmp = new int[4*LEN];

    cout << "This test compares performance of custom map (FastMap) implementation and C++11 standard one (unordered_map)." << endl;
    cout << "There are two cases to test: \"bad data\" and \"good data\"." << endl;
    cout << "\"Good data\" means that map keys are all numbers from range [0; N)." << endl;
    cout << "\"Bad data\" means that map keys are exact N numbers from range [0; 4*N)." << endl;
    cout << "Custom map implementation expected to be faster than standard one on \"good data\" and slower on \"bad data\"." << endl;

    cout << endl;
    cout << endl;


    for (int k = 0; k < 2; k++) {

        string test_case = k == 0 ? "Good data" : "Bad data";

        FastMap<int, int> fm(LEN);

        auto do_before_1 = [&tmp, &um, k]()
        {
            int m = 1+3*k;
            for (int j = 0; j < m*LEN; j++)
                tmp[j] = j;

            for (int j = m*LEN-1; j > 0; j--) {
                int idx = 1.0*(j-1)*rand()/RAND_MAX;
                int t = tmp[idx];
                tmp[idx] = tmp[j];
                tmp[j] = t;
            }

            um.clear();
        };
        
        auto do_before_2 = [&fm]()
        {
            fm.clear();
        };

        print_test_title(test_case + " - fill");
        
        auto um_fill = [&um, &tmp]()
        {
            for (int j = 0; j < LEN; j++)
                um[tmp[j]] = j;
        };

        auto fm_fill = [&fm, &tmp]()
        {
            for (int j = 0; j < LEN; j++)
                fm.put(tmp[j], j);
            fm.sort();
        };

        auto t = measure_time2(do_before_1, um_fill, do_before_2, fm_fill);     

        print_test_results("unordered_map", t.first, "fast map", t.second);
        
        print_test_title(test_case + " - get");

        auto um_get = [&um, &tmp]()
        {
            for (int j = 0; j < LEN; j++)
                um.at(tmp[j]);
        };


        auto fm_get = [&fm, &tmp]()
        {
            for (int j = 0; j < LEN; j++)
                fm.get(tmp[j]);
        };

        t = measure_time2
        (
            [&]()
            {
                do_before_1();
                um_fill();
            },
            [&]()
            {
                do_before_2();
                fm_fill();
            },
            um_get,
            fm_get
        );
        
        print_test_results("unordered_map", t.first, "fast map", t.second);
    }

    return 0;
}
