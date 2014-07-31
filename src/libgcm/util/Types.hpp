#ifndef GCM_TYPES
#define GCM_TYPES

#ifndef GCM_DOUBLE_PRECISION
typedef float gcm_real;
#else
typedef double gcm_real;
#endif

#include <cmath>

namespace gcm
{
    class vector3
    {
    public:
        union
        {
            gcm_real coords[3];
            struct
            {
                gcm_real x;
                gcm_real y;
                gcm_real z;
            };
        };

        vector3(): vector3(0.0, 0.0, 0.0)
        {
        }

        vector3(gcm_real x, gcm_real y, gcm_real z): x(x), y(y), z(z)
        {
        }

        gcm_real operator[](int index) const
        {
            return coords[index];
        }
        
        gcm_real& operator[](int index)
        {
            return coords[index];
        }

        gcm_real length() const 
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

    typedef unsigned char uchar;
    typedef unsigned int uint;
    typedef unsigned long ulong;


    inline vector3 operator-(const vector3& v1, const vector3& v2)
    {
        vector3 v;
        for (int i = 0; i < 3; i++)
            v[i] = v1[i] - v2[i];

        return v;
    }
    
    inline vector3 operator-(const vector3& v)
    {
        return vector3(-v.x, -v.y, -v.z);
    }

    inline vector3 operator+(const vector3& v1, const vector3& v2)
    {
        vector3 v;
        for (int i = 0; i < 3; i++)
            v[i] = v1[i] + v2[i];

        return v;
    }

    inline gcm_real operator*(const vector3& v1, const vector3& v2)
    {
        gcm_real res = 0.0;
        for (int i = 0; i < 3; i++)
            res += v1[i]*v2[i];
        return res;
    }
    
    inline vector3 operator*(const vector3& v1, gcm_real factor)
    {
        vector3 v;
        for (int i = 0; i < 3; i++)
            v[i] = v1[i]*factor;
        return v;
    }
    
    inline vector3 operator/(const vector3& v1, gcm_real factor)
    {
        return v1*(1/factor);
    }
}
#endif
