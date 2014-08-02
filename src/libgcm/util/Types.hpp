#ifndef GCM_TYPES
#define GCM_TYPES

#include "libgcm/util/Assertion.hpp"

#include <cmath>

namespace gcm
{
    
    #ifndef GCM_DOUBLE_PRECISION
    typedef float real;
    #else
    typedef double real;
    #endif    
    
    typedef unsigned char uchar;
    typedef unsigned int uint;
    typedef unsigned long ulong;
    
    template<typename T>
    class vector3
    {
    public:
        union
        {
            T coords[3];
            struct
            {
                T x;
                T y;
                T z;
            };
        };

        vector3(): vector3(0.0, 0.0, 0.0)
        {
        }

        vector3(T x, T y, T z): x(x), y(y), z(z)
        {
        }

        T operator[](uint index) const
        {
            assert_ge(index, 0);
            assert_lt(index, 3);
            
            return coords[index];
        }
        
        T& operator[](uint index)
        {
            assert_ge(index, 0);
            assert_lt(index, 3);
            
            return coords[index];
        }
        
        vector3<T>& operator+=(const vector3<T> t)
        {
            this->x += t.x;
            this->y += t.y;
            this->z += t.z;
        }

        T length() const 
        {
            return sqrt(x*x+y*y+z*z);
        }

        void normalize()
        {
            auto l = length();

            x /= l;
            y /= l;
            z /= l;
        }

    };

    template<typename T>
    inline vector3<T> operator-(const vector3<T>& v1, const vector3<T>& v2)
    {
        vector3<T> v;
        for (int i = 0; i < 3; i++)
            v[i] = v1[i] - v2[i];

        return v;
    }
    
    template<typename T>
    inline vector3<T> operator-(const vector3<T>& v)
    {
        return vector3<T>(-v.x, -v.y, -v.z);
    }

    template<typename T>
    inline vector3<T> operator+(const vector3<T>& v1, const vector3<T>& v2)
    {
        vector3<T> v;
        for (int i = 0; i < 3; i++)
            v[i] = v1[i] + v2[i];

        return v;
    }
    
    template<typename T>
    inline T operator*(const vector3<T>& v1, const vector3<T>& v2)
    {
        T res = 0.0;
        for (int i = 0; i < 3; i++)
            res += v1[i]*v2[i];
        return res;
    }
    
    template<typename T>
    inline vector3<T> operator*(const vector3<T>& v1, double factor)
    {
        vector3<T> v;
        for (int i = 0; i < 3; i++)
            v[i] = v1[i]*factor;
        return v;
    }
    
    template<typename T>
    inline vector3<T> operator/(const vector3<T>& v1, double factor)
    {
        return v1*(1/factor);
    }
    
    typedef vector3<real> vector3r;
}
#endif
