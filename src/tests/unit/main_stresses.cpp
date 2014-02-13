#include <time.h>

#include <gtest/gtest.h>

#include "node/CalcNode.h"

#define ITERATIONS 1000

TEST(MainStresses, FuzzCalculate) {
	srand(time(NULL));
	
	CalcNode node;
	
	for( int i = 0; i < ITERATIONS; i++ )
	{
		for( int j = 0; j < 9; j++ )
			node.values[j]= (float) rand() / RAND_MAX;

		float s[3];
		node.clearState();
		node.getMainStressComponents(s[0], s[1], s[2]);

		float err = 0.01 * node.getJ1();

		EXPECT_NEAR(node.getJ1(), -3*node.getPressure(), err);
		EXPECT_NEAR(node.getJ1(), s[0] + s[1] + s[2], err);
		EXPECT_NEAR(node.getJ2(), s[0]*s[1] + s[0]*s[2] + s[1]*s[2], err);
		EXPECT_NEAR(node.getJ3(), s[0]*s[1]*s[2], err);
	}
}
