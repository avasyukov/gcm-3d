#ifndef GCM_UTILS_H_
#define GCM_UTILS_H_

#include <fstream>
#include <map>
#include <string>
#include <vector>

#include "libgcm/Exception.hpp"

using namespace std;

/*
 * Checks if previous read operation succeeded.
 */
void checkStream(fstream &f);

void replaceAll(std::string& str, const std::string& from, const std::string& to);

#endif
