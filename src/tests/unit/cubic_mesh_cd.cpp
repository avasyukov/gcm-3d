#include <gtest/gtest.h>
#include <vector>
#include <algorithm>
#include <iostream>

#include "libgcm/mesh/cube/BasicCubicMesh.hpp"
#include "libgcm/BruteforceCollisionDetector.hpp"

#include "launcher/launcher.hpp"
#include "libgcm/util/Types.hpp"

using namespace gcm;
using namespace std;

TEST(CubicMeshCD, NodesInIntersectionTest)
{
    launcher::Launcher launcher;
    launcher.loadSceneFromFile("../../tasks/tests/cubic-interp-border-node-test.xml");

    auto& engine = Engine::getInstance();

    auto m1 = engine.getBodyById("cube1")->getMesh("cube1");
    auto m2 = engine.getBodyById("cube2")->getMesh("cube2");

    ASSERT_TRUE(m1);
    ASSERT_TRUE(m2);

    AABB intersection;
    BruteforceCollisionDetector bcd;
    bcd.set_threshold(0.002);

    ASSERT_FALSE(bcd.is_static());

    auto o1 = m1->getOutline();
    auto o2 = m2->getOutline();

    ASSERT_TRUE( bcd.find_intersection(o1, o2, intersection) );

    vector<CalcNode> nodes1;
    vector<CalcNode> nodes2;

    bcd.find_nodes_in_intersection(m1, intersection, nodes1);
    bcd.find_nodes_in_intersection(dynamic_cast<BasicCubicMesh*>(m1), intersection, nodes2);

    ASSERT_TRUE( nodes1.size() == nodes2.size() );
    vector<CalcNode>::iterator it;
    for(int i = 0; i < nodes2.size(); i++) {
    	it = find(nodes1.begin(), nodes1.end(), nodes2[i]);
    	if(it == nodes1.end())
    		ASSERT_TRUE(false);
    	else
    		nodes1.erase(it);
    }

    nodes1.clear();
    nodes2.clear();
};

TEST(CubicMeshCD, VirtualNodesTest)
{
    auto& engine = Engine::getInstance();

    auto m1 = engine.getBodyById("cube1")->getMesh("cube1");
    auto m2 = engine.getBodyById("cube2")->getMesh("cube2");
    auto m22 = dynamic_cast<BasicCubicMesh*>(m2);

    ASSERT_TRUE(m1);
    ASSERT_TRUE(m2);

    AABB intersection;
    BruteforceCollisionDetector bcd;
    bcd.set_threshold(0.002);

    ASSERT_FALSE(bcd.is_static());

    auto o1 = m1->getOutline();
    auto o2 = m2->getOutline();

    ASSERT_TRUE( bcd.find_intersection(o1, o2, intersection) );

    vector<CalcNode> nodes;
    vector<CalcNode> virt_nodes1;
    vector<CalcNode> virt_nodes2;
    bcd.find_nodes_in_intersection(m1, intersection, nodes);

	float direction[3];
	for (unsigned int k = 0; k < nodes.size(); k++)
	{
		for (int m = 0; m < 3; m++)
		{
			m1->findBorderNodeNormal(nodes[k], &direction[0], &direction[1], &direction[2], false);
			if (direction[m] > 0)
				direction[m] = 1;
			else
				direction[m] = -1;
			for (int z = 0; z < 3; z++)
				if (z != m)
					direction[z] = 0;

			CalcNode new_node1;
			if( m2->interpolateBorderNode(
					nodes[k].coords[0], nodes[k].coords[1], nodes[k].coords[2],
					direction[0] * bcd.get_threshold(), direction[1] * bcd.get_threshold(),
					direction[2] * bcd.get_threshold(), new_node1) )
			{
				new_node1.setIsBorder(true);
				new_node1.setInContact(true);
				new_node1.contactNodeNum = 2;
				new_node1.setCustomFlag(CalcNode::FLAG_1, 1);
				(m1->getNode(nodes[k].number)).setInContact(true);
				(m1->getNode(nodes[k].number)).contactNodeNum = virt_nodes1.size();
				(m1->getNode(nodes[k].number)).contactDirection = m;
				virt_nodes1.push_back(new_node1);
			}

	        CalcNode new_node2;
	        vector3r direction3r = vector3r(direction[0], direction[1], direction[2]);
	        if( m22->interpolateBorderNode(nodes[k].coords, bcd.get_threshold() * direction3r, new_node2) )
	        {
	        	new_node2.setIsBorder(true);
	            new_node2.setInContact(true);
	            new_node2.contactNodeNum = 2;
	            new_node2.setCustomFlag(CalcNode::FLAG_1, 1);
	            (m1->getNode(nodes[k].number)).setInContact(true);
	            (m1->getNode(nodes[k].number)).contactNodeNum = virt_nodes2.size();
	            (m1->getNode(nodes[k].number)).contactDirection = m;
	            virt_nodes2.push_back(new_node2);
	        }
		}
	}

    ASSERT_TRUE( virt_nodes1.size() == virt_nodes2.size() );
    vector<CalcNode>::iterator it;
    for(int i = 0; i < virt_nodes2.size(); i++) {
    	it = find(virt_nodes1.begin(), virt_nodes1.end(), virt_nodes2[i]);
    	if(it == virt_nodes1.end())
    		ASSERT_TRUE(false);
    	else
    		virt_nodes1.erase(it);
    }

    virt_nodes1.clear();
    virt_nodes2.clear();
    nodes.clear();
};

