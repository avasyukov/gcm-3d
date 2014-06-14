#include "tests/perf/util.hpp"

#include <vector>
#include <algorithm>
#include <chrono>
#include <cmath>
#include <iostream>
#include <iomanip>

using namespace std;
using namespace std::chrono;

exec_time measure_time(std::function<void (void)> do_before, std::function<void (void)> func, int n)
{
    vector<microseconds::rep> t(n);
    
    for (int i = 0; i < n; i++)
    {
        do_before();
        auto t1 = high_resolution_clock::now();
        func();
        auto t2 = high_resolution_clock::now();

        t[i] = duration_cast<microseconds>(t2-t1).count();
    }

    double mean = 0;

    for_each(t.begin(), t.end(), [&mean](microseconds::rep v)
    {
        mean += v; 
    });

    mean /= n;
    
    long d = 0;
    
    for_each(t.begin(), t.end(), [&d, mean](microseconds::rep v)
    {
        d += (v-mean)*(v-mean); 
    });

    exec_time et;
    et.time = mean;
    et.std_dev = sqrt(d/n);
    et.iterations = n;

    return et;
}


exec_time measure_time(std::function<void (void)> func, int n)
{
    return measure_time(DUMMY_FUNC, func, n);
}

std::pair<exec_time, exec_time> measure_time2(std::function<void (void)> do_before_func1, std::function<void (void)> func1, std::function<void (void)> do_before_func2, std::function<void (void)> func2, int n)
{
    auto t1 = measure_time(do_before_func1, func1, n);
    auto t2 = measure_time(do_before_func2, func2, n);

    return make_pair(t1, t2);
}

pair<exec_time, exec_time> measure_time2(std::function<void (void)> func1, std::function<void (void)> func2, int n)
{
    return measure_time2(DUMMY_FUNC, DUMMY_FUNC, func1, func2, n);
}

void print_test_results(std::string test1, exec_time time1, std::string test2, exec_time time2)
{
    cout << "Test run " << time1.iterations << " times" << endl;

    auto flags = cout.flags();

    cout << fixed;
    cout << setprecision(2);

    cout << test1 << ": " << time1.time << " usec (standard deviation is " << 100.0*time1.std_dev/time1.time << "%)" << endl;
    cout << test2 << ": " << time2.time << " usec (standard deviation is " << 100.0*time2.std_dev/time2.time << "%)" << endl;
    float ratio = 1.0*time1.time/time2.time;
    
    if (ratio < 1.0)
        cout << test1 << " is " << 1/ratio << "x faster" << endl;
    else
        cout << test2 << " is " << ratio << "x faster" << endl;

    cout.setf(flags);
}

void print_test_title(string s)
{
    int l = 80 - s.length() - 2;
    int n1 = l / 2;
    int n2 = n1 + l % 2;

    for (int i = 0; i < n1; i++)
        cout << "-";
    
    cout << " " << s << " ";
    
    for (int i = 0; i < n2; i++)
        cout << "-";

    cout << endl;
}
