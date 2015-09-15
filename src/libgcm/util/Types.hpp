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

        // FIXME this operator is WA to ease migration from union to vector in CalcNode
        // This code should be removed immediately after refactoring is done
        operator T*()
        {
            return coords;
        }
        operator const T*() const
        {
            return coords;
        }
        //--

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
        
        vector3<T>& operator+=(const vector3<T>& t)
        {
            this->x += t.x;
            this->y += t.y;
            this->z += t.z;
            
            return *this;
        }

        vector3<T>& operator/=(T val)
        {
            this->x /= val;
            this->y /= val;
            this->z /= val;

            return *this;
        }

        T length() const 
        {
            return sqrt(x*x+y*y+z*z);
        }

        T normalize()
        {
            auto l = length();

            if (l == T(0.0))
            	return l;

            x /= l;
            y /= l;
            z /= l;

            return l;
        }

    };

    template<typename T, typename U>
    inline vector3<T> operator-(const vector3<T>& v1, const vector3<U>& v2)
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

    template<typename T, typename U>
    inline vector3<T> operator+(const vector3<T>& v1, const vector3<U>& v2)
    {
        vector3<T> v;
        for (int i = 0; i < 3; i++)
            v[i] = v1[i] + v2[i];

        return v;
    }
    
    template<typename T, typename U>
    inline T operator*(const vector3<T>& v1, const vector3<U>& v2)
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
    
    template<typename T, typename U>
    inline vector3<T> operator%(const vector3<T>& v1, vector3<U> v2)
    {
        vector3<T> v;
        for (int i = 0; i < 3; i++)
            v[i] = v1[i]*v2[i];
        return v;
    }

    template<typename T, typename U>
    inline vector3<T> operator/(const vector3<T>& v1, vector3<U> v2)
    {
        vector3<T> v;
        for (int i = 0; i < 3; i++)
            v[i] = v1[i]/v2[i];
        return v;
    }

    template<typename T>
    inline vector3<T> operator*( double factor, const vector3<T>& v1)
    {
        return v1*factor;
    }

    template<typename T>
    inline vector3<T> operator/(const vector3<T>& v1, double factor)
    {
        return v1*(1/factor);
    }

    template<typename T>
    inline bool operator==(const vector3<T>& v1, const vector3<T>& v2)
    {
    	return v1.x == v2.x && v1.y == v2.y && v1.z == v2.z;
    }
    
    template<typename T>
    inline bool operator!=(const vector3<T>& v1, const vector3<T>& v2)
    {
    	return !(v1 == v2);
    }

    template<typename T>
    inline bool operator>(const vector3<T>& v1, const vector3<T>& v2)
    {
        for (int i = 0; i < 3; i++)
            if (v1.coords[i] > v2.coords[i])
                return true;
        return false;
    }

    template<typename T>
    inline bool operator<(const vector3<T>& v1, const vector3<T>& v2)
    {
        return v2 > v1;
    }

    template<typename T>
    inline bool operator>=(const vector3<T>& v1, const vector3<T>& v2)
    {
        for (int i = 0; i < 3; i++)
            if (v1.coords[i] >= v2.coords[i])
                return true;
        return false;
    }

    template<typename T>
    inline bool operator<=(const vector3<T>& v1, const vector3<T>& v2)
    {
        return v2 >= v1;
    }

    template<typename T>
	inline bool operator>>(const vector3<T>& v1, const vector3<T>& v2)
	{
		for (int i = 0; i < 3; i++)
			if (v1.coords[i] <= v2.coords[i])
				return false;
		return true;
	}

    template<typename T>
	inline bool operator>>=(const vector3<T>& v1, const vector3<T>& v2)
	{
		for (int i = 0; i < 3; i++)
			if (v1.coords[i] < v2.coords[i])
				return false;
		return true;
	}

    template<typename T>
	inline bool operator<<(const vector3<T>& v1, const vector3<T>& v2)
    {
    	return v2 >> v1;
    }

    template<typename T>
	inline bool operator<<=(const vector3<T>& v1, const vector3<T>& v2)
	{
		return v2 >>= v1;
	}

    template<typename T>
    inline vector3<T> vmin(const vector3<T>& v1, const vector3<T>& v2)
    {
        return vector3<T>(std::min(v1.x, v2.x), std::min(v1.y, v2.y), std::min(v1.z, v2.z));
    }

    template<typename T>
    inline vector3<T> vmin(const vector3<T>& v1, const vector3<T>& v2, const vector3<T>& v3)
    {
        return vmin(v1, vmin(v2, v3));
    }

    template<typename T>
    inline vector3<T> vmax(const vector3<T>& v1, const vector3<T>& v2)
    {
        return vector3<T>(std::max(v1.x, v2.x), std::max(v1.y, v2.y), std::max(v1.z, v2.z));
    }

    template<typename T>
    inline vector3<T> vmax(const vector3<T>& v1, const vector3<T>& v2, const vector3<T>& v3)
    {
        return vmax(v1, vmax(v2, v3));
    }


    typedef vector3<real> vector3r;
    typedef vector3<uint> vector3u;
    typedef vector3<int> vector3i;

    template<typename T>
    inline std::ostream& operator<<(std::ostream &os, const vector3<T>& v) {
        os << "(" << v.x << ", " << v.y << ", " << v.z << ")";
        return os;
    }

    // For pair sorting
    struct sort_pred {
        bool operator()(const std::pair<int,int> &left, const std::pair<int,int> &right) {
            return left.second < right.second;
        }
    };

}
#endif
