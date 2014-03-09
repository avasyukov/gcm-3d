#ifndef UTILS_H_
#define UTILS_H_

#include <string>

#include "mesh/tetr/TetrMesh.h"
#include "elem/TetrFirstOrder.h"
#include "Engine.h"

#include <gtest/gtest.h>

Engine& loadTaskScenario(std::string taskFile);

TetrFirstOrder* findTetr(TetrMesh* m, float x, float y, float z);

void runTaskAsTest(std::string taskFile, void(*setAnalytical)(CalcNode&, float, Engine&), 
						int stepsNum, int pointsNum, 
						float ALLOWED_VALUE_DEVIATION_PERCENT, float ALLOWED_NUMBER_OF_BAD_NODES);

#endif /* UTILS_H_ */
