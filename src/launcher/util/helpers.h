/*
 * File:   helpers.h
 * Author: Alexey Ermakov
 *
 * Created on March 9, 2014, 11:38 AM
 */

#ifndef HELPERS_H
#define    HELPERS_H

#include <cstdlib>

#include "util/Types.h"

inline gcm_real s2r(std::string s)
{
#ifndef GCM_DOUBLE_PRECISION
    return strtof(s.c_str(), nullptr);
#else
    return strtod(s.c_str(), nullptr);
#endif
}

#endif    /* HELPERS_H */

