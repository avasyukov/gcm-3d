#include <stdio.h>
#include <gtest/gtest.h>

#include "utils.h"

#include "Exception.h"
#include "node/CalcNode.h"

/*
 * Major test parameters
 */

// Number of time steps
#define STEPS 10

// Number of points per 'in-memmory line snapshot'
#define POINTS 20

// Thresholds
#define ALLOWED_VALUE_DEVIATION_PERCENT 0.1
#define ALLOWED_NUMBER_OF_BAD_NODES 4 // 2 fronts x 2 nodes per front

/*
 * Analytics for the test described in tasks/tests/p-wave-test.xml
 */

// Initial state
#define LEFT_MARK_START 1.0
#define RIGHT_MARK_START 3.0

/*
 * Sets analytical values for CalcNode object provided
 * Node should have coordinates (0; 0; z), where -5 < z < 5
 */
void setPWaveAnalytics(CalcNode& node, float t, Engine& engine)
{
	Material* mat = engine.getMaterial("testMaterial");
	float la = mat->getLambda();
	float mu = mat->getMu();
	float rho = mat->getRho();
	
	if( node.z < -5 || node.z > 5 )
		THROW_INVALID_INPUT("Z is out of acceptable range");
	if( t < 0 || t > 0.02 )
		THROW_INVALID_INPUT("T is out of acceptable range");
	
	float pWaveVelocity = sqrt( la + 2 * mu / rho );
	float leftMark = LEFT_MARK_START - t * pWaveVelocity;
	float rightMark = RIGHT_MARK_START - t * pWaveVelocity;
	
	node.x = node.y = 0;
	node.vx = node.vy = node.vz = 0;
	node.sxx = node.sxy = node.sxz = node.syy = node.syz = node.szz = 0;
	
	if( node.z >= leftMark && node.z <= rightMark )
	{
		node.vz = pWaveVelocity;
		node.sxx = node.syy = la;
		node.szz = la + 2 * mu;
	}
}

TEST(Waves, PWavePropagation)
{
	runTaskAsTest("tasks/tests/p-wave-test.xml", setPWaveAnalytics, STEPS, POINTS, 
			ALLOWED_VALUE_DEVIATION_PERCENT, ALLOWED_NUMBER_OF_BAD_NODES);
}