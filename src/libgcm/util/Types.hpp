#ifndef GCM_TYPES
#define GCM_TYPES

#ifndef GCM_DOUBLE_PRECISION
typedef float gcm_real;
#else
typedef double gcm_real;
#endif

typedef gcm_real vector3[3];

typedef unsigned char uchar;
typedef unsigned int uint;
typedef unsigned long ulong;

#endif