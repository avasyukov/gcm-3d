#include <stdio.h>


int main()
{
	FILE *f;
	char filename[] = "/home/amisto/gcm/gcm-3d/tasks/objects/phantom/phantom_0000.xml";
	char ph[] = "phantom_0000";
	for(int i=0; i<36; i++)
	{
		if (i < 10)
			sprintf(ph, "phantom_0%d", i);
		else
			sprintf(ph, "phantom_%d", i);
		sprintf(filename, "/home/amisto/gcm/gcm-3d/tasks/objects/phantom/%s.xml", ph);
		f = fopen(filename, "w");	
		fprintf(f, "<task numberOfSnaps=\"60\" stepsPerSnap=\"5\" xmlns:ndi=\"gcm3d.plugins.ndi\">\n    <system>\n        <meshMovement type=\"none\" />\n        <timeStep multiplier=\"0.5\"/>\n        <loadPlugin name=\"ndi\" />\n    </system>\n\n    <bodies>\n        <body id=\"%s\">\n            <rheology type=\"elastic\" />\n            <mesh id=\"%s\" type=\"geo2\" file=\"models/meat_cube.geo\" tetrSize=\"0.5\" />\n            <material id=\"muscle\">\n		        <area type=\"cylinder\" r=\"100\" x1=\"3\" y1=\"0\" z1=\"-100\" x2=\"3\" y2=\"0\" z2=\"100\" />\n	        </material>\n            <material id=\"aorta\">\n                <area type=\"cylinder\" r=\"1.5\" x1=\"3\" y1=\"0\" z1=\"-100\" x2=\"3\" y2=\"0\" z2=\"100\" />\n            </material>\n            <material id=\"blood\">\n                <area type=\"cylinder\" r=\"1\" x1=\"3\" y1=\"0\" z1=\"-100\" x2=\"3\" y2=\"0\" z2=\"100\" />\n            </material>\n        </body>\n    </bodies>\n\n    <materials>\n      <material name=\"aorta\" rheology=\"isotropic\">\n        <la>9210000</la>\n        <mu>190000</mu>\n        <rho>0.001</rho>\n        <crackThreshold>INF</crackThreshold>\n      </material>\n      <material name=\"blood\" rheology=\"isotropic\">\n        <la>239000</la>\n        <mu>500</mu>\n        <rho>0.001</rho>\n        <crackThreshold>INF</crackThreshold>\n      </material>\n      <material name=\"muscle\" rheology=\"isotropic\">\n        <la>3070000</la>\n        <mu>2050000</mu>\n        <rho>0.001</rho>\n        <crackThreshold>INF</crackThreshold>\n      </material>\n    </materials>\n\n    <borderCondition calculator=\"ConsumingBorderCalculator\">\n      <area type=\"box\" minX=\"-100\" maxX=\"114.99\" minY=\"-300\" maxY=\"250\" minZ=\"-240\" maxZ=\"270\"/>\n    </borderCondition>\n\n     <ndi:emitter name=\"%s_e\" sensor=\"true\" type=\"array\" axis=\"z\" n=\"30\" F=\"8\" dF=\"%lf\">\n                <borderCondition calculator=\"ExternalForceCalculator\" normalStress=\"10000\" tangentialStress=\"0.0\" startTime = \"0.0\" type=\"sinus_gauss\" omega=\"300000\" tau=\"0.2e-9\"/> \n                <area type=\"box\" minX=\"-200\" maxX=\"-4.99\" minY=\"-5\" maxY=\"5\" minZ=\"-5\" maxZ=\"5\"/>\n    </ndi:emitter>\n</task>", ph, ph, ph, -5 + i*10.0/36);
		fclose(f);
	}
	return 0;
}

