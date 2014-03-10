#include <stdio.h>
#include <gtest/gtest.h>

#include "utils.h"

#include "Exception.h"
#include "node/CalcNode.h"

/*
 * Analytics for the test described in tasks/tests/s-wave-test.xml
 * Sets analytical values for CalcNode object provided
 * Node should have coordinates (0; 0; z), where -5 < z < 5
 */
void setSWaveAnalytics(CalcNode& node, float t, Engine& engine)
{
	// Parameters from task file
	float LEFT_MARK_START = 1.0;
	float RIGHT_MARK_START = 3.0;
	float WAVE_AMPLITUDE_SCALE = 0.1;
	
	Material* mat = engine.getMaterial("testMaterial");
	float mu = mat->getMu();
	float rho = mat->getRho();
	
	if( node.z < -5 || node.z > 5 )
		THROW_INVALID_INPUT("Z is out of acceptable range");
	if( t < 0 || t > 0.02 )
		THROW_INVALID_INPUT("T is out of acceptable range");
	
	float sWaveVelocity = sqrt( mu / rho );
	float leftMark = LEFT_MARK_START - t * sWaveVelocity;
	float rightMark = RIGHT_MARK_START - t * sWaveVelocity;
	
	node.x = node.y = 0;
	node.vx = node.vy = node.vz = 0;
	node.sxx = node.sxy = node.sxz = node.syy = node.syz = node.szz = 0;
	
	if( node.z >= leftMark && node.z <= rightMark )
	{
		node.vx = sWaveVelocity * WAVE_AMPLITUDE_SCALE;
		node.sxz = mu * WAVE_AMPLITUDE_SCALE;
	}
}

TEST(Waves, SWavePropagation)
{
	// Major test parameters
	
	// Number of time steps
	int STEPS = 10;
	
	// Number of points per 'in-memmory line snapshot'
	int POINTS = 50;
	
	// Thresholds
	float ALLOWED_VALUE_DEVIATION_PERCENT = 0.1;
	int ALLOWED_NUMBER_OF_BAD_NODES = 8; // 2 fronts x 2 nodes per front x 2 without any reason
	
	runTaskAsTest("tasks/tests/s-wave-test.xml", setSWaveAnalytics, STEPS, POINTS, 
			ALLOWED_VALUE_DEVIATION_PERCENT, ALLOWED_NUMBER_OF_BAD_NODES, 
			{"vx", "sxz"} );
}