#include <stdio.h>

#include "../datatypes/ElasticNode.h"
#include "../datatypes/Tetrahedron_1st_order.h"

ElasticNode ext_nodes[4];
Tetrahedron_1st_order tetrs[1];

#include <iostream>

using std::cout;
using std::ios;
using std::endl;

bool point_in_tetr(float x, float y, float z, Tetrahedron* tetr);

float determinant(float x1, float y1, float z1, float x2, float y2, float z2, float x3, float y3, float z3)
{
	cout.precision(10);
        cout.setf(ios::fixed,ios::floatfield);
//	cout << "\t\tDet:" << endl;
//	cout << "\t\t" << x1 << " " << y1 << " " << z1 << endl;
//	cout << "\t\t" << x2 << " " << y2 << " " << z2 << endl;
//	cout << "\t\t" << x3 << " " << y3 << " " << z3 << endl;
        return (x1*(y2*z3-y3*z2) - x2*(y1*z3-y3*z1) + x3*(y1*z2-y2*z1));
};


float calcD2(Tetrahedron* tetr, int v1, int v2, int v3, int node_index, float dx, float dy, float dz)
{
	float x = ext_nodes[node_index].coords[0] + dx;
        float y = ext_nodes[node_index].coords[1] + dy;
        float z = ext_nodes[node_index].coords[2] + dz;

	if(tetr->vert[v1] == node_index) {
                return determinant(
                        - dx,
                        - dy,
                        - dz,
                        ext_nodes[tetr->vert[v2]].coords[0] - x,
                        ext_nodes[tetr->vert[v2]].coords[1] - y,
                        ext_nodes[tetr->vert[v2]].coords[2] - z,
                        ext_nodes[tetr->vert[v3]].coords[0] - x,
                        ext_nodes[tetr->vert[v3]].coords[1] - y,
                        ext_nodes[tetr->vert[v3]].coords[2] - z );

	} else if (tetr->vert[v2] == node_index) {
                return determinant(
                        ext_nodes[tetr->vert[v1]].coords[0] - x,
                        ext_nodes[tetr->vert[v1]].coords[1] - y,
                        ext_nodes[tetr->vert[v1]].coords[2] - z,
                        - dx,
                        - dy,
                        - dz,
                        ext_nodes[tetr->vert[v3]].coords[0] - x,
                        ext_nodes[tetr->vert[v3]].coords[1] - y,
                        ext_nodes[tetr->vert[v3]].coords[2] - z );

	} else if (tetr->vert[v3] == node_index) {
                return determinant(
                        ext_nodes[tetr->vert[v1]].coords[0] - x,
                        ext_nodes[tetr->vert[v1]].coords[1] - y,
                        ext_nodes[tetr->vert[v1]].coords[2] - z,
                        ext_nodes[tetr->vert[v2]].coords[0] - x,
                        ext_nodes[tetr->vert[v2]].coords[1] - y,
                        ext_nodes[tetr->vert[v2]].coords[2] - z,
                        - dx,
                        - dy,
                        - dz );

	} else {
		return determinant(
        	        ext_nodes[tetr->vert[v1]].coords[0] - x,
                	ext_nodes[tetr->vert[v1]].coords[1] - y,
	                ext_nodes[tetr->vert[v1]].coords[2] - z,
        	        ext_nodes[tetr->vert[v2]].coords[0] - x,
                	ext_nodes[tetr->vert[v2]].coords[1] - y,
	                ext_nodes[tetr->vert[v2]].coords[2] - z,
        	        ext_nodes[tetr->vert[v3]].coords[0] - x,
                	ext_nodes[tetr->vert[v3]].coords[1] - y,
	                ext_nodes[tetr->vert[v3]].coords[2] - z );
	}
};


bool point_in_tetr(int node_index, float dx, float dy, float dz, Tetrahedron* tetr)
{
	cout.precision(10);
	cout.setf(ios::fixed,ios::floatfield);

	float x = ext_nodes[node_index].coords[0] + dx;
	float y = ext_nodes[node_index].coords[1] + dy;
	float z = ext_nodes[node_index].coords[2] + dz;

	cout << "Point: x: " << x << " y: " << y << " z: " << z << endl;

	ElasticNode nodes[4];

	for(int i = 0; i < 4; i++) {
                cout << "Vert " << i << " x: " << ext_nodes[tetr->vert[i]].coords[0] << " y: "
                            << ext_nodes[tetr->vert[i]].coords[1] << " z: " << ext_nodes[tetr->vert[i]].coords[2] << endl;
        }

	for(int i = 0; i < 4; i++) {
		nodes[tetr->vert[i]].coords[0] = ext_nodes[tetr->vert[i]].coords[0];
		nodes[tetr->vert[i]].coords[1] = ext_nodes[tetr->vert[i]].coords[1];
		nodes[tetr->vert[i]].coords[2] = ext_nodes[tetr->vert[i]].coords[2];
	}

	float d1,d2;
	d1 = determinant(
		nodes[tetr->vert[1]].coords[0] - nodes[tetr->vert[0]].coords[0],
		nodes[tetr->vert[1]].coords[1] - nodes[tetr->vert[0]].coords[1],
		nodes[tetr->vert[1]].coords[2] - nodes[tetr->vert[0]].coords[2],
		nodes[tetr->vert[2]].coords[0] - nodes[tetr->vert[0]].coords[0],
		nodes[tetr->vert[2]].coords[1] - nodes[tetr->vert[0]].coords[1],
		nodes[tetr->vert[2]].coords[2] - nodes[tetr->vert[0]].coords[2],
		nodes[tetr->vert[3]].coords[0] - nodes[tetr->vert[0]].coords[0],
		nodes[tetr->vert[3]].coords[1] - nodes[tetr->vert[0]].coords[1],
		nodes[tetr->vert[3]].coords[2] - nodes[tetr->vert[0]].coords[2]
	);
	d2 = calcD2(tetr, 1, 2, 3, node_index, dx, dy, dz);
/*	d2 = determinant(
		nodes[tetr->vert[1]].coords[0] - x,
		nodes[tetr->vert[1]].coords[1] - y,
		nodes[tetr->vert[1]].coords[2] - z,
		nodes[tetr->vert[2]].coords[0] - x,
		nodes[tetr->vert[2]].coords[1] - y,
		nodes[tetr->vert[2]].coords[2] - z,
		nodes[tetr->vert[3]].coords[0] - x,
		nodes[tetr->vert[3]].coords[1] - y,
		nodes[tetr->vert[3]].coords[2] - z
	);*/
	cout << "Stage 1: " << d1 << " " << d2 << endl;
	if(d1*d2 < 0) { return false; }

	d1 = determinant(
		nodes[tetr->vert[0]].coords[0] - nodes[tetr->vert[1]].coords[0],
		nodes[tetr->vert[0]].coords[1] - nodes[tetr->vert[1]].coords[1],
		nodes[tetr->vert[0]].coords[2] - nodes[tetr->vert[1]].coords[2],
		nodes[tetr->vert[2]].coords[0] - nodes[tetr->vert[1]].coords[0],
		nodes[tetr->vert[2]].coords[1] - nodes[tetr->vert[1]].coords[1],
		nodes[tetr->vert[2]].coords[2] - nodes[tetr->vert[1]].coords[2],
		nodes[tetr->vert[3]].coords[0] - nodes[tetr->vert[1]].coords[0],
		nodes[tetr->vert[3]].coords[1] - nodes[tetr->vert[1]].coords[1],
		nodes[tetr->vert[3]].coords[2] - nodes[tetr->vert[1]].coords[2]
	);
	d2 = calcD2(tetr, 0, 2, 3, node_index, dx, dy, dz);
/*	d2 = determinant(
		nodes[tetr->vert[0]].coords[0] - x,
		nodes[tetr->vert[0]].coords[1] - y,
		nodes[tetr->vert[0]].coords[2] - z,
		nodes[tetr->vert[2]].coords[0] - x,
		nodes[tetr->vert[2]].coords[1] - y,
		nodes[tetr->vert[2]].coords[2] - z,
		nodes[tetr->vert[3]].coords[0] - x,
		nodes[tetr->vert[3]].coords[1] - y,
		nodes[tetr->vert[3]].coords[2] - z
	);*/
	cout << "Stage 2: " << d1 << " " << d2 << endl;
	if(d1*d2 < 0) { return false; }

	d1 = determinant(
		nodes[tetr->vert[0]].coords[0] - nodes[tetr->vert[2]].coords[0],
		nodes[tetr->vert[0]].coords[1] - nodes[tetr->vert[2]].coords[1],
		nodes[tetr->vert[0]].coords[2] - nodes[tetr->vert[2]].coords[2],
		nodes[tetr->vert[1]].coords[0] - nodes[tetr->vert[2]].coords[0],
		nodes[tetr->vert[1]].coords[1] - nodes[tetr->vert[2]].coords[1],
		nodes[tetr->vert[1]].coords[2] - nodes[tetr->vert[2]].coords[2],
		nodes[tetr->vert[3]].coords[0] - nodes[tetr->vert[2]].coords[0],
		nodes[tetr->vert[3]].coords[1] - nodes[tetr->vert[2]].coords[1],
		nodes[tetr->vert[3]].coords[2] - nodes[tetr->vert[2]].coords[2]
	);
	d2 = calcD2(tetr, 0, 1, 3, node_index, dx, dy, dz);
/*	d2 = determinant(
		nodes[tetr->vert[0]].coords[0] - x,
		nodes[tetr->vert[0]].coords[1] - y,
		nodes[tetr->vert[0]].coords[2] - z,
		nodes[tetr->vert[1]].coords[0] - x,
		nodes[tetr->vert[1]].coords[1] - y,
		nodes[tetr->vert[1]].coords[2] - z,
		nodes[tetr->vert[3]].coords[0] - x,
		nodes[tetr->vert[3]].coords[1] - y,
		nodes[tetr->vert[3]].coords[2] - z
	);*/
	cout << "Stage 3: " << d1 << " " << d2 << endl;
	if(d1*d2 < 0) { return false; }

	d1 = determinant(
		nodes[tetr->vert[0]].coords[0] - nodes[tetr->vert[3]].coords[0],
		nodes[tetr->vert[0]].coords[1] - nodes[tetr->vert[3]].coords[1],
		nodes[tetr->vert[0]].coords[2] - nodes[tetr->vert[3]].coords[2],
		nodes[tetr->vert[1]].coords[0] - nodes[tetr->vert[3]].coords[0],
		nodes[tetr->vert[1]].coords[1] - nodes[tetr->vert[3]].coords[1],
		nodes[tetr->vert[1]].coords[2] - nodes[tetr->vert[3]].coords[2],
		nodes[tetr->vert[2]].coords[0] - nodes[tetr->vert[3]].coords[0],
		nodes[tetr->vert[2]].coords[1] - nodes[tetr->vert[3]].coords[1],
		nodes[tetr->vert[2]].coords[2] - nodes[tetr->vert[3]].coords[2]
	);
	d2 = calcD2(tetr, 0, 1, 2, node_index, dx, dy, dz);
/*	d2 = determinant(
		nodes[tetr->vert[0]].coords[0] - x,
		nodes[tetr->vert[0]].coords[1] - y,
		nodes[tetr->vert[0]].coords[2] - z,
		nodes[tetr->vert[1]].coords[0] - x,
		nodes[tetr->vert[1]].coords[1] - y,
		nodes[tetr->vert[1]].coords[2] - z,
		nodes[tetr->vert[2]].coords[0] - x,
		nodes[tetr->vert[2]].coords[1] - y,
		nodes[tetr->vert[2]].coords[2] - z
	);*/
	cout << "Stage 4: " << d1 << " " << d2 << endl;
	if(d1*d2 < 0) { return false; }

	return true;
};


int main()
{
        cout.precision(10);
        cout.setf(ios::fixed,ios::floatfield);


	printf("Testing owner tetr lookup:\n");

	ext_nodes[0].coords[0] = 5.3074893951;
	ext_nodes[0].coords[1] = 1.6371172667;
	ext_nodes[0].coords[2] = -2.3064794540;

	ext_nodes[1].coords[0] = 5.2448787689;
	ext_nodes[1].coords[1] = 1.4480520487;
	ext_nodes[1].coords[2] = -2.4296388626;

	ext_nodes[2].coords[0] = 5.4918704033;
	ext_nodes[2].coords[1] = 1.6011626720;
	ext_nodes[2].coords[2] = -2.3315827847;

	ext_nodes[3].coords[0] = 5.4362969398;
	ext_nodes[3].coords[1] = 1.4015516043;
	ext_nodes[3].coords[2] = -2.4567565918;

	tetrs[0].vert[0] = 0;
	tetrs[0].vert[1] = 1;
	tetrs[0].vert[2] = 2;
	tetrs[0].vert[3] = 3;

	bool res[1];

	float dx,dy,dz;
	float ksi, eta, dzeta;
	float l;

	ksi = -0.4218390584;
	eta = 0.7677641511;
	dzeta = 0.4822759330;

	l = -0.0000101252;

	for(int i = 0; i < 1; i++) {

		dx = - l*ksi*3;
		dy = - l*eta*3;
		dz = - l*dzeta*3;

		cout << "Point: " << 3 << " dx: " << dx << " dy: " << dy << " dz: " << dz << endl;
		res[i] = point_in_tetr(3, dx, dy, dz, &tetrs[i]);
		if(res[i])
			printf("\tIN\n");
		else
			printf("\tOUT\n");

		dx = - l*ksi/1000000;
		dy = - l*eta/1000000;
		dz = - l*dzeta/1000000;

		cout << "Point: " << 3 << " dx: " << dx << " dy: " << dy << " dz: " << dz << endl;
		res[i] = point_in_tetr(3, dx, dy, dz, &tetrs[i]);
		if(res[i])
			printf("\tIN\n");
		else
			printf("\tOUT\n");
	}

	printf("Done\n");

/*	float fafa = determinant( -0.1914138794, 0.0464926958, 0.0271129608,
					0.0555777550, 0.1996033192, 0.1251690388,
					0.0000042712048, -0.0000077737656, -0.0000048831403 );

	cout << "FAFA: " << fafa << endl;
*/
	return 0;
}

