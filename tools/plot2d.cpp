#include <iostream>
#include <fstream>
#include <sstream>
#include <stdlib.h>
#include <string.h>

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
	int base_coords[3];

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
		base_coords[0] = atoi(argv[2]);
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
                base_coords[1] = atoi(argv[3]);
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
                base_coords[2] = atoi(argv[4]);
        }

	if(axis == -1)
	{
		cout << "Axis not set" << endl;
		return -1;
	}

	cout << "Axis: " << axis << ". Base coords: " << base_coords[0] << " " << base_coords[1] << " " << base_coords[2] << endl;

	char str[255];
	int dimensions[3];
	int spacing[3];
	int origin[3];
	int num_of_points;

	int count = 0;
	while (strcmp(str,"LOOKUP_TABLE default") != 0)
	{
		infile.getline(str, 255, '\n');
		sscanf(str, "DIMENSIONS %d %d %d", &dimensions[0], &dimensions[1], &dimensions[2]);
		sscanf(str, "SPACING %d %d %d", &spacing[0], &spacing[1], &spacing[2]);
		sscanf(str, "ORIGIN %d %d %d", &origin[0], &origin[1], &origin[2]);
		sscanf(str, "POINT_DATA %d", &num_of_points);
		count++;
	}

	float data[dimensions[0]][dimensions[1]][dimensions[2]];

	printf("DIMENSIONS %d %d %d\n", dimensions[0], dimensions[1], dimensions[2]);
	printf("SPACING %d %d %d\n", spacing[0], spacing[1], spacing[2]);
	printf("ORIGIN %d %d %d\n", origin[0], origin[1], origin[2]);
	printf("POINT_DATA %d\n", num_of_points);

	for(int k = 0; k < dimensions[2]; k++)
		for(int j = 0; j < dimensions[1]; j++)
			for(int i = 0; i < dimensions[0]; i++)
				infile >> data[i][j][k];

	for(int k = 0; k < dimensions[2]; k++)
                for(int j = 0; j < dimensions[1]; j++)
                        for(int i = 0; i < dimensions[0]; i++)
			{
				if( ((axis == 0) || (base_coords[0] == origin[0] + i) )
					&& ((axis == 1) || (base_coords[1] == origin[1] + j) )
					&& ((axis == 2) || (base_coords[2] == origin[2] + k) ) )
				{
					if (axis == 0) cout << origin[0] + i;
					else if (axis == 1) cout << origin[1] + j;
					else if (axis == 2) cout << origin[2] + k;
					cout << " " << data[i][j][k] << endl;
				}
			}

	return 0;
}
