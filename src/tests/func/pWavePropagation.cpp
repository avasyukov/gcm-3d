#include <stdio.h>

#include "Exception.h"
#include "node/CalcNode.h"

/*
 * Analytics for the test described in tasks/tests/p-wave-test.xml
 */

// Initial state
#define LEFT_MARK_START 1.0
#define RIGHT_MARK_START 3.0
#define LA 70000
#define MU 10000
#define RHO 1

/*
 * Sets analytical values for CalcNode object provided
 * Node should have coordinates (0; 0; z), where -5 < z < 5
 */
void setAnalytics(CalcNode& node, float t)
{
	if( node.z < -5 || node.z > 5 )
		THROW_INVALID_INPUT("Z is out of acceptable range");
	if( t < 0 || t > 0.02 )
		THROW_INVALID_INPUT("T is out of acceptable range");
	
	float pWaveVelocity = sqrt( LA + 2 * MU / RHO );
	float leftMark = LEFT_MARK_START - t * pWaveVelocity;
	float rightMark = RIGHT_MARK_START - t * pWaveVelocity;
	
	node.x = node.y = 0;
	node.vx = node.vy = node.vz = 0;
	node.sxx = node.sxy = node.sxz = node.syy = node.syz = node.szz = 0;
	
	if( node.z >= leftMark && node.z <= rightMark )
	{
		node.vz = pWaveVelocity;
		node.sxx = node.syy = LA;
		node.szz = LA + 2 * MU;
	}
}

int main()
{
	CalcNode node;
	float time = 0.0;
	for( int t = 0; t <= 10; t++ )
	{
		for( int i = 0; i < 20; i++ )
		{
			node.z = -5 + i * 0.5;
			setAnalytics(node, time);
			std::cout << node.vz << "\n";
		}
		std::cout << "=========================\n";
		time += 0.001;
	}
	return 0;
}