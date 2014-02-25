#ifndef UTILS_H_
#define UTILS_H_

#include <string>

#include "mesh/tetr/TetrMesh.h"
#include "elem/TetrFirstOrder.h"
#include "Engine.h"

Engine& loadTaskScenario(std::string taskFile);

TetrFirstOrder* findTetr(TetrMesh* m, float x, float y, float z);

#endif /* UTILS_H_ */
