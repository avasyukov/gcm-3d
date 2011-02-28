#include <iostream>
#include <fstream>
#include <sstream>
#include <stdlib.h>
#include <string.h>
#include <math.h>

using namespace std;

void print_usage()
{
	cout << "Usage: plot2d file.vtk x y z" << endl;
	cout << "\tfile.vtk is the file to plot" << endl;
	cout << "\t'x' 'y' 'z' set the value of corresponding coordinate" << endl;
	cout << "\tif the value is ? it means we will plot along this axis" << endl;
	cout << "\tExample: plot2d test.vtk 1 2 ?" << endl;
	cout << "\tWill plot values from test.vtk along axis (x=1,y=2,z=*)" << endl;
};

int main(int argc, char* argv[])
{
	if(argc < 5)
	{
		print_usage();
		return -1;
	}
	stringstream filename;
	filename << argv[1];

	ifstream infile;

	infile.open(filename.str().c_str());
	if(!infile.is_open())
	{
		cout << "Can not open file: " << filename.str() << endl;
		return -1;
	}

	int axis = -1;
	float base_coords[3];

	if(argv[2][0] == '?')
	{
		if(axis == -1)
		{
			axis = 0;
			base_coords[0] = 0;
		}
		else
		{
			cout << "Too many ?" << endl;
			return -1;
		}
	} else {
		sscanf(argv[2],"%f",&base_coords[0]);
	}

        if(argv[3][0] == '?')
        {
                if(axis == -1)
                {
                        axis = 1;
                        base_coords[1] = 0;
                }
                else
                {
                        cout << "Too many ?" << endl;
                        return -1;
                }
        } else {
                sscanf(argv[3],"%f",&base_coords[1]);
        }

        if(argv[4][0] == '?')
        {
                if(axis == -1)
                {
                        axis = 2;
                        base_coords[2] = 0;
                }
                else
                {
                        cout << "To many ?" << endl;
                        return -1;
                }
        } else {
                sscanf(argv[4],"%f",&base_coords[2]);
        }

	if(axis == -1)
	{
		cout << "Axis not set" << endl;
		return -1;
	}

	cout << "Axis: " << axis << ". Base coords: " << base_coords[0] << " " << base_coords[1] << " " << base_coords[2] << endl;

	char str[255];
	int dimensions[3];
	float spacing[3];
	float origin[3];

	int count = 0;
	while (strcmp(str,"LOOKUP_TABLE default") != 0)
	{
		infile.getline(str, 255, '\n');
		if(strncmp(str,"DIMENSIONS",10) == 0)
			sscanf(str, "DIMENSIONS %d %d %d", &dimensions[0], &dimensions[1], &dimensions[2]);
		if(strncmp(str,"SPACING",7) == 0)
			sscanf(str, "SPACING %f %f %f", &spacing[0], &spacing[1], &spacing[2]);
		if(strncmp(str,"ORIGIN",6) == 0)
			sscanf(str, "ORIGIN %f %f %f", &origin[0], &origin[1], &origin[2]);
		count++;
	}

	for(int k = 0; k < 3; k++)
		if(axis != k)
		{
			float new_coord = origin[k];
			float delta = fabs(base_coords[k] - new_coord);
			for(int i = 0; i < dimensions[k]; i++)
			{
				if(fabs(base_coords[k] - origin[k] - spacing[k]*i) < delta )
				{
					new_coord = origin[k] + spacing[k]*i;
					delta = fabs(base_coords[k] - origin[k] - spacing[k]*i);
				}
			}
			base_coords[k] = new_coord;
		}

	printf("DIMENSIONS %d %d %d\n", dimensions[0], dimensions[1], dimensions[2]);
        printf("SPACING %f %f %f\n", spacing[0], spacing[1], spacing[2]);
        printf("ORIGIN %f %f %f\n", origin[0], origin[1], origin[2]);

	cout << "Axis: " << axis << ". Base coords: " << base_coords[0] << " " << base_coords[1] << " " << base_coords[2] << endl;

	float data[dimensions[0]][dimensions[1]][dimensions[2]];

	for(int k = 0; k < dimensions[2]; k++)
		for(int j = 0; j < dimensions[1]; j++)
			for(int i = 0; i < dimensions[0]; i++)
				infile >> data[i][j][k];

	for(int k = 0; k < dimensions[2]; k++)
                for(int j = 0; j < dimensions[1]; j++)
                        for(int i = 0; i < dimensions[0]; i++)
			{
				if( ((axis == 0) || (fabs(base_coords[0] - origin[0] - spacing[0]*i) < fabs(spacing[0]/10)))
				&& ((axis == 1) || (fabs(base_coords[1] - origin[1] - spacing[1]*j) < fabs(spacing[1]/10)) )
				&& ((axis == 2) || (fabs(base_coords[2] - origin[2] - spacing[2]*k) < fabs(spacing[2]/10))) )
				{
					if (axis == 0) cout << origin[0] + spacing[0]*i;
					else if (axis == 1) cout << origin[1] + spacing[1]*j;
					else if (axis == 2) cout << origin[2] + spacing[2]*k;
					cout << " " << data[i][j][k] << endl;
				}
			}

	return 0;
}
