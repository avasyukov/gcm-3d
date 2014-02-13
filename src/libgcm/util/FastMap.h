#include <sys/time.h>
#include <cmath>
#include <ctime>
#include <stdlib.h>
#include <vector>
#include <stdexcept>
#include <sstream>

using namespace std;

template<typename key_type, typename value_type>
class FastMap {
    private:

        typedef struct {
            key_type key;
            value_type value;
        } map_struct;

        static int cmp(const void *a, const void *b) {
            key_type aa = *((key_type*)a);
            key_type bb = *((key_type*)b);
            #ifndef NDEBUG
            if (aa == bb) {
                stringstream ss;
                ss << "Equal map keys are not allowed."
                   << " key: " << aa 
                   << " value1: " << ((map_struct*)a)->value 
                   << " value2: " << ((map_struct*)b)->value;
                throw invalid_argument(ss.str());
            }
            #endif
            return aa < bb ? -1 : 1;
        }

        vector<map_struct> data;
        bool modified=false;

/*
        int lookup(key_type key, int start, int stop) {
            if (start == stop)
                return data[2*start+1];
            int idx = (start + stop)/2;
            if (data[2*idx] >= key)
                return lookup(key, start, idx);
            else
                return lookup(key, idx+1, stop);
        }

*/
    value_type lookup(key_type key, int start, int stop) {
        if (start == stop) {
            #ifndef NDEBUG
            if (data[start].key != key) {
                stringstream ss;
                ss << "Key was not found. Seems map does not contain it."
                   << " key: " << key;
                throw invalid_argument(ss.str());
            }
            #endif
            return data[start].value;
        }
        float fidx = start+1.0*(key-data[start].key)/(data[stop].key-data[start].key)*(stop-start);
        int idx = fidx;
        if (data[idx].key == key)
            return data[idx].value;
        else if (data[idx].key < key)
            return lookup(key, idx+1, stop);
        else
            return lookup(key, start, idx-1);
    }

    public:
    FastMap(int sz = 0) {
        data.reserve(sz);
    }

    ~FastMap() {
    }

    void put(key_type key, value_type value) {
        map_struct d;
        d.key = key;
        d.value = value;
        data.push_back(d);
        modified = true;
    }

    void sort() {
        qsort(&data[0], data.size(), sizeof(map_struct), FastMap::cmp);
        modified = false;
    }

    value_type get(key_type key) {
        if (modified) {
            sort();
        }
        return lookup(key, 0, data.size()-1);
    }
};

