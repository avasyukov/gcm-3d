#ifndef UTILS_H_
#define UTILS_H_

#include <string>

#include "mesh/tetr/TetrMesh.h"
#include "elem/TetrFirstOrder.h"
#include "Engine.h"

#include <gtest/gtest.h>

int mkpath(std::string s);

std::string getTestDataDirName();

Engine& loadTaskScenario(std::string taskFile);

TetrFirstOrder* findTetr(TetrMesh* m, float x, float y, float z);

void dumpPoint(CalcNode& analytical, CalcNode& numerical, int stepNum);

bool shouldDraw(std::string value, std::initializer_list<std::string> valuesToDraw);

void drawValues(std::initializer_list<std::string> valuesToDraw, int stepNum);

void runTaskAsTest(std::string taskFile, void(*setAnalytical)(CalcNode&, float, Engine&), 
						int stepsNum, int pointsNum, 
						float ALLOWED_VALUE_DEVIATION_PERCENT, int ALLOWED_NUMBER_OF_BAD_NODES, 
						std::initializer_list<std::string> valuesToDraw);

#endif /* UTILS_H_ */
