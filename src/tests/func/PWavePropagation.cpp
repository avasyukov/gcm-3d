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

#define N 20

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
	CalcNode node, cnode;

	Engine& engine = loadTaskScenario("tasks/tests/p-wave-test.xml");
	engine.setTimeStep(dt);

	Material* mat = engine.getMaterial("testMaterial");
	auto mesh = dynamic_cast<TetrMeshFirstOrder*>(engine.getBodyById("cube")->getMesh("main"));

	TetrFirstOrder* tetrs[N];
	float z[N];

	for (int i = 0; i < N; i++) {
		z[i] = -5.0 + 10.0*i/(N-1);
		tetrs[i] = findTetr(mesh, 0.0, 0.0, z[i]);
		ASSERT_TRUE(tetrs[i]);
	}

	cnode.x = node.x = 0.0;
	cnode.y = node.y = 0.0;


	for (int t = 0; t < 10; t++, time += dt) {
		engine.doNextStep();
		for (int i = 0; i < N; i++) {
			cnode.z = node.z = z[i];

			setPWaveAnalytics(node, time, 	mat->getLambda(), mat->getMu(), mat->getRho());

			mesh->interpolate(&cnode, tetrs[i]);

			EXPECT_NEAR(node.sxx, cnode.sxx, 1e-3);
			EXPECT_NEAR(node.sxy, cnode.sxy, 1e-3);
			EXPECT_NEAR(node.sxz, cnode.sxz, 1e-3);
			EXPECT_NEAR(node.syy, cnode.syy, 1e-3);
			EXPECT_NEAR(node.syz, cnode.syz, 1e-3);
			EXPECT_NEAR(node.szz, cnode.szz, 1e-3);

			EXPECT_NEAR(node.vx, cnode.vx, 1e-3);
			EXPECT_NEAR(node.vy, cnode.vy, 1e-3);
			EXPECT_NEAR(node.vz, cnode.vz, 1e-3);
		}
	}
}
