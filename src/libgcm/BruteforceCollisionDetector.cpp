#include "BruteforceCollisionDetector.h"
#include "Engine.h"
#include "node/CalcNode.h"
#include "mesh/TetrMeshSecondOrder.h"

gcm::BruteforceCollisionDetector::BruteforceCollisionDetector() {
	INIT_LOGGER("gcm.BruteforceCollisionDetector");
}

gcm::BruteforceCollisionDetector::~BruteforceCollisionDetector() {
}

string gcm::BruteforceCollisionDetector::getType () const
{
	return "BruteforceCollisionDetector";
}

void gcm::BruteforceCollisionDetector::find_collisions(vector<CalcNode> &virt_nodes)
{
	Engine& e = Engine::getInstance();
	AABB intersection;
	vector<TriangleFirstOrder> local_faces;
	//data_bus->sync_outlines();
	vector<CalcNode> local_nodes;

	LOG_DEBUG("Processing local/local collisions");

	// process collisions between local nodes and local faces
	// we start both cycles from zero because collision should be 'symmetric'
	for (int i = 0; i < e.getNumberOfBodies(); i++)
		for (int j = 0; j < e.getNumberOfBodies(); j++)
		{
			TetrMeshSecondOrder* mesh1 = (TetrMeshSecondOrder*)e.getBody(i)->getMeshes();
			TetrMeshSecondOrder* mesh2 = (TetrMeshSecondOrder*)e.getBody(j)->getMeshes();
			AABB outl1 = mesh1->getOutline();
			AABB outl2 = mesh2->getOutline();
			
			if ( ( i != j ) && ( find_intersection(outl1, outl2, intersection) ) )
			{
				LOG_DEBUG( "Collision detected between local mesh zone #" << mesh1->getId()
						<< " and local mesh zone #" << mesh2->getId() << ". "
						<< "Intersection: " << intersection);
				// find local nodes inside intersection
				find_nodes_in_intersection(mesh1, intersection, local_nodes);
				// find local faces inside intersection
				find_faces_in_intersection(mesh2, intersection, local_faces);
				LOG_DEBUG("Got " << local_nodes.size() << " nodes and " << local_faces.size() << " local faces");

				LOG_DEBUG("Virt nodes size before processing: " << virt_nodes.size());

				// process collisions
				CalcNode new_node;
				float direction[3];
				for(int k = 0; k < local_nodes.size(); k++) {
					for(int l = 0; l < local_faces.size(); l++) {

						mesh1->find_border_node_normal(local_nodes[k].number, 
								&direction[0], &direction[1], &direction[2], false);

						if( vectorIntersectsTriangle( 
								mesh2->getNode( local_faces[l].verts[0] )->coords,
								mesh2->getNode( local_faces[l].verts[1] )->coords,
								mesh2->getNode( local_faces[l].verts[2] )->coords,
								local_nodes[k].coords,
								direction, get_treshold(), new_node.coords, false ) )
						{
							//float vel[3];
							//vel[0] = local_nodes[k].values[0] - mesh2->nodes[ local_faces[l].vert[0] ].values[0];
							//vel[1] = local_nodes[k].values[1] - mesh2->nodes[ local_faces[l].vert[0] ].values[1];
							//vel[2] = local_nodes[k].values[2] - mesh2->nodes[ local_faces[l].vert[0] ].values[2];
							//if( vel[0] * direction[0] + vel[1] * direction[1] + vel[2] * direction[2] > 0 )
							{
								mesh1->getNode( local_nodes[k].number )->setContactType (InContact);
								mesh1->getNode( local_nodes[k].number )->contactNodeNum = virt_nodes.size();

								interpolateTriangle(
									mesh2->nodes[ local_faces[l].verts[0] ].coords,
									mesh2->nodes[ local_faces[l].verts[1] ].coords,
									mesh2->nodes[ local_faces[l].verts[2] ].coords,
									new_node.coords,
									mesh2->nodes[ local_faces[l].verts[0] ].values,
									mesh2->nodes[ local_faces[l].verts[1] ].values,
									mesh2->nodes[ local_faces[l].verts[2] ].values,
									new_node.values, 9);
								new_node.setRho( mesh2->nodes[ local_faces[l].verts[0] ].getRho() );
								new_node.setMaterialId( mesh2->nodes[ local_faces[l].verts[0] ].getMaterialId() );

								// remote_num here should be remote face (!) num
								virt_nodes.push_back(new_node);

								break;
							}
						}
					}
				}

				LOG_DEBUG("Virt nodes size after processing: " << virt_nodes.size());

				// clear
				local_nodes.clear();
				local_faces.clear();
			}
		}

	LOG_DEBUG("Local/local collisions processed");

}