#include <iostream>
#include "libgcm/config.hpp"

#include "tests/perf/util.hpp"

#include <vector>
#include <algorithm>

#if CONFIG_ENABLE_LOGGING
#include <log4cxx/propertyconfigurator.h>
#endif

#include "libgcm/mesh/cube/BasicCubicMesh.hpp"
#include "libgcm/BruteforceCollisionDetector.hpp"

#include "launcher/util/FileFolderLookupService.hpp"
#include "launcher/launcher.hpp"
#include "libgcm/util/Types.hpp"

using namespace gcm;
using namespace std;

int main() {
    MPI::Init();

    auto& ffls = launcher::FileFolderLookupService::getInstance();

    #if CONFIG_ENABLE_LOGGING
    ffls.addPath("src/tests");
    log4cxx::PropertyConfigurator::configure(ffls.lookupFile("log4cxx.properties"));
    #endif

    ffls.addPath(".");

    cout << "Generating meshes...";
    cout.flush();

    launcher::Launcher launcher;
    launcher.loadSceneFromFile("tasks/tests/cubic-interp-border-node-test.xml");

    auto& engine = Engine::getInstance();

    auto m1 = engine.getBodyById("cube1")->getMesh("cube1");
    auto m2 = dynamic_cast<BasicCubicMesh*>(engine.getBodyById("cube2")->getMesh("cube2"));


    assert_true(m1);
    assert_true(m2);

    cout << "done" << endl;

    AABB intersection;

    BruteforceCollisionDetector bcd;
    bcd.set_threshold(0.002);

    assert_false(bcd.is_static());

    auto o1 = m1->getOutline();
    auto o2 = m2->getOutline();

    if( !bcd.find_intersection(o1, o2, intersection) )
    {
    	cout << "Can't find intersection\n";
    	return -1;
    }

    vector<CalcNode> virt_nodes1;
    vector<CalcNode> virt_nodes2;

    auto t = measure_time2(
        [&](){ virt_nodes1.clear(); },
        [&](){
            vector<CalcNode> nodes;
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

        	        CalcNode new_node;
        	        if( m2->interpolateBorderNode(
        	        		nodes[k].coords[0], nodes[k].coords[1], nodes[k].coords[2],
        	                direction[0] * bcd.get_threshold(), direction[1] * bcd.get_threshold(),
        					direction[2] * bcd.get_threshold(), new_node) )
        	        {
        	        	new_node.setIsBorder(true);
        	            new_node.setInContact(true);
        	            new_node.contactNodeNum = 2;
        	            new_node.setCustomFlag(CalcNode::FLAG_1, 1);
        	            (m1->getNode(nodes[k].number)).setInContact(true);
        	            (m1->getNode(nodes[k].number)).contactNodeNum = virt_nodes1.size();
        	            (m1->getNode(nodes[k].number)).contactDirection = m;
        	            virt_nodes1.push_back(new_node);
        	            break;
        	        }
        	    }
        	}
        },
        [&](){ virt_nodes2.clear(); },
        [&](){
            vector<CalcNode> nodes;
            bcd.find_nodes_in_intersection(dynamic_cast<BasicCubicMesh*>(m1), intersection, nodes);

        	vector3r direction;
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

        	        CalcNode new_node;
        	        if( m2->interpolateBorderNode(nodes[k].coords, bcd.get_threshold() * direction, new_node) )
        	        {
        	        	new_node.setIsBorder(true);
        	            new_node.setInContact(true);
        	            new_node.contactNodeNum = 2;
        	            new_node.setCustomFlag(CalcNode::FLAG_1, 1);
        	            (m1->getNode(nodes[k].number)).setInContact(true);
        	            (m1->getNode(nodes[k].number)).contactNodeNum = virt_nodes2.size();
        	            (m1->getNode(nodes[k].number)).contactDirection = m;
        	            virt_nodes2.push_back(new_node);
        	            break;
        	        }
        	    }
        	}
         }
    );


    // Comparison of virtual nodes
    cout << "Starting comparison" << endl;
    cout << "virt_nodes1 size: " << virt_nodes1.size() << endl;
    cout << "virt_nodes2 size: " << virt_nodes2.size() << endl;

    vector<CalcNode>::iterator it;
    for(int i = 0; i < virt_nodes2.size(); i++) {
		it = find(virt_nodes1.begin(), virt_nodes1.end(), virt_nodes2[i]);
		if(it == virt_nodes1.end()) {
			cout << "Results differ !!!" << endl;
			return -1;
		} else
			virt_nodes1.erase(it);
	}
    cout << "Vectors are the same\n";

    print_test_results("Current variant", t.first, "New variant", t.second);

    return 0;
}
