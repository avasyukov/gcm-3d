#include <stdio.h>
#include <gtest/gtest.h>

#include "utils.h"

#include "Exception.h"
#include "node/CalcNode.h"

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
void setPWaveAnalytics(CalcNode& node, float t, float la, float mu, float rho)
{
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

TEST(Waves, PWavePropogation)
{
	float time = 0.0;
	float dt = 1e-5;
	CalcNode node;

	Engine& engine = loadTaskScenario("tasks/tests/p-wave-test.xml");
	engine.setTimeStep(dt);

	Material* mat = engine.getMaterial("testMaterial");

	for (int t = 0; t < 10; t++, time += dt) {
		engine.doNextStep();
		for( int i = 0; i < 20; i++ )
		{
			node.z = -5 + i * 0.5;
			setPWaveAnalytics(node, time, 	mat->getLambda(), mat->getMu(), mat->getRho());
			ASSERT_EQ(node.sxx, node.sxx);
		}
	}
}
