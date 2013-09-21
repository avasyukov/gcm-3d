#include <cmath>
#include <ctime>
#include <sys/time.h>
#include <stdlib.h>
#include <stdio.h>
#include <map>
#include <stdexcept>
#include <assert.h>

using namespace std;

int cmp(const void *a, const void *b) {
    int aa = *((int*)a);
    int bb = *((int*)b);
    #ifndef NDEBUG
    if (aa == bb)
        throw invalid_argument("Equal map keys are not allowed!");
    #endif
    return aa < bb ? -1 : 1;
}


template<typename key_type, typename value_type>
class FastMap {
    private:

        typedef struct {
            key_type key;
            value_type value;
        } map_struct;

        int *data;
        int size;
        int sz;
        bool modified=false;

/*
        int lookup(int key, int start, int stop) {
            if (start == stop)
                return data[2*start+1];
            int idx = (start + stop)/2;
            if (data[2*idx] >= key)
                return lookup(key, start, idx);
            else
                return lookup(key, idx+1, stop);
        }

*/        
    int lookup(int key, int start, int stop) {
        if (start == stop)
            return data[2*start+1];
        float fidx = start+1.0*(key-data[2*start])/(data[2*stop]-data[2*start])*(stop-start);
        int idx = fidx;
        if (data[2*idx] == key)
            return data[2*idx+1];
        else if (data[2*idx] < key)
            return lookup(key, idx+1, stop);
        else
            return lookup(key, start, idx-1);
    }

    public:
    FastMap(int sz) {
        data = new int[sz*2];
        size = 0;
        this->sz = sz;
    }

    ~FastMap() {
        delete[] data;
    }

    void put(int key, int value) {
        if (size < sz) {
            int idx = size*2;
            data[idx] = key;
            data[idx+1] = value;
            size++;
            modified = true;
        } else
            throw out_of_range("");

    }

    void sort() {
        qsort(data, size, 2*sizeof(int), cmp);
        modified = false;
    }

    int get(int key) {
        #ifndef NDEBUG
        if (modified)
            throw logic_error("Map get operation invoked without sorting being performed");
        #endif
        return lookup(key, 0, size-1);
    }
};

