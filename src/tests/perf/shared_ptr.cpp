#include <iostream>
#include <iomanip>
#include <memory>
#include <chrono>
#include <vector>

#include "tests/perf/util.hpp"

#define ITERATIONS 1000000

using namespace std::chrono;
using namespace std;

class A
{
    protected:
        int _x;
    public:
        A(int _x): _x(_x) {}
        virtual ~A() {}
        virtual void x()
        {
            _x++;
        }
};

int main()
{
    vector<shared_ptr<A>> v1;
    vector<A*> v2;

    cout << "This test compares performace of raw pointers (A*) and c++11 smart pointers (shared_ptr<A>)." << endl;
    cout << "There are two cases to test: object creation and method invocation." << endl;
    cout << endl;
    cout << endl;

    print_test_title("Creating pointers");

    auto t1 = measure_time(
        [&v1]()
        {
            v1.clear();
            v1.reserve(ITERATIONS);
        },
        [&v1]()
        {
            for (int i = 0; i < ITERATIONS; i++)
                v1.push_back(make_shared<A>(i));
        }
    );

    auto t2 = measure_time(
        [&v2]()
        {
            for (auto v: v2)
                delete v;
            v2.clear();
            v2.reserve(ITERATIONS);
        },
        [&v2]()
        {
            for (int i = 0; i < ITERATIONS; i++)
                v2.push_back(new A(i));
        }
    );

    cout << "Number of pointers created: " << ITERATIONS << endl;
    print_test_results("shared_ptr", t1, "raw pointer", t2);


    print_test_title("Calling method");

    auto a1 = make_shared<A>(0);
    auto a2 = new A(0);

    t1 = measure_time ([&a1](){
        for (int i = 0; i < ITERATIONS; i++)
            a1->x();
    });

    t2 = measure_time ([&a2](){
        for (int i = 0; i < ITERATIONS; i++)
            a2->x();
    });

    cout << "Number of method invocations: " << ITERATIONS << endl;
    print_test_results("shared_ptr", t1, "raw pointer", t2);

    return 0;
}
