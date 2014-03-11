#include <stdio.h>
#include <gtest/gtest.h>

#include "utils.h"

#include "Exception.h"
#include "materials/IsotropicElasticMaterial.h"
#include "node/CalcNode.h"

/*
 * Analytics for the test described in tasks/tests/p-wave-test.xml
 */

// Initial state
#define LEFT_MARK_START 1.0
#define RIGHT_MARK_START 3.0

// Number of time steps
#define STEPS 2

// Number of points per 'in-memmory line snapshot'
#define N 20

// Thresholds
#define ALLOWED_VALUE_DEVIATION_PERCENT 0.1
#define ALLOWED_NUMBER_OF_BAD_NODES 4 // 2 fronts x 2 nodes per front

//USE_AND_INIT_LOGGER("gcm.tests.PWavePropagation");

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

TEST(Waves, PWavePropagation)
{
	float time = 0.0;
	CalcNode node, cnode;

	Engine& engine = loadTaskScenario("tasks/tests/p-wave-test.xml");
	
	float dt = engine.calculateRecommendedTimeStep();
	engine.setTimeStep(dt);
	
	IsotropicElasticMaterial* mat = dynamic_cast<IsotropicElasticMaterial*>(engine.getMaterial("testMaterial"));
        ASSERT_TRUE(mat);
	auto mesh = dynamic_cast<TetrMeshFirstOrder*>(engine.getBodyById("cube")->getMesh("main"));
        ASSERT_TRUE(mesh);

	TetrFirstOrder* tetrs[N];
	float z[N];

	for (int i = 0; i < N; i++)
	{
		z[i] = -5.0 + 10.0*i/(N-1);
		tetrs[i] = findTetr(mesh, 0.0, 0.0, z[i]);
		ASSERT_TRUE(tetrs[i]);
	}
	
	cnode.x = node.x = 0.0;
	cnode.y = node.y = 0.0;
	
	float velocityNorm = - numeric_limits<float>::infinity();
	float pressureNorm = - numeric_limits<float>::infinity();
	for (int i = 0; i < N; i++)
	{
		node.z = z[i];
		setPWaveAnalytics(node, 0.0, mat->getLambda(), mat->getMu(), mat->getRho());
		// Check velocity
		for(int v = 0; v < 3; v++ )
		{
			if( fabs(node.values[v]) > velocityNorm )
				velocityNorm = fabs(node.values[v]);
		}
		// Check pressure
		for(int v = 3; v < GCM_MATRIX_SIZE; v++ )
		{
			if( fabs(node.values[v]) > pressureNorm )
				pressureNorm = fabs(node.values[v]);
		}
	}
	
	for (int t = 0; t < STEPS; t++, time += dt)
	{
		int badNodes = 0;
		engine.doNextStep();
		for (int i = 0; i < N; i++)
		{
			cnode.z = node.z = z[i];

			setPWaveAnalytics(node, time, mat->getLambda(), mat->getMu(), mat->getRho());

			bool isInnerPoint;
			CalcNode& baseNode = mesh->getNode(tetrs[i]->verts[0]);
			mesh->interpolateNode(
					baseNode, 
					cnode.x - baseNode.x, 
					cnode.y - baseNode.y, 
					cnode.z - baseNode.z, 
					false, 
					cnode, 
					isInnerPoint
			);
			ASSERT_TRUE( isInnerPoint );
			
			// Check velocity
			for(int v = 0; v < 3; v++ )
			{
				float delta = fabs(node.values[v] - cnode.values[v]);
				if( delta > velocityNorm * ALLOWED_VALUE_DEVIATION_PERCENT )
				{
					badNodes++;
					LOG_INFO("Bad nodes: " << node << "\n" << "VS" << cnode);
					LOG_INFO("Compare values[" << v << "], delta " << delta << ", norm " << velocityNorm );
					break;
				}
			}
			// Check pressure
			for(int v = 3; v < GCM_MATRIX_SIZE; v++ )
			{
				float delta = fabs(node.values[v] - cnode.values[v]);
				if( delta > pressureNorm * ALLOWED_VALUE_DEVIATION_PERCENT )
				{
					badNodes++;
					LOG_INFO("Bad nodes: " << node << "\n" << "VS" << cnode);
					LOG_INFO("Compare values[" << v << "], delta " << delta << ", norm " << pressureNorm );
					break;
				}
			}
		}
		
		ASSERT_LE(badNodes, ALLOWED_NUMBER_OF_BAD_NODES);
	}
}