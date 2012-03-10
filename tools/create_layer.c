#include <stdio.h>

// Number of points in layer sample
#define Size 201
// Number of layers in zone
#define Width 10

#define ZonesPerEdge 2

// Example:
// If you set Size to 60 and Width to 10
// you will get layer 60x60x10 total

int get_num_in_zone(int i, int j, int k);
int write_tetrs(FILE* file, int num, int i, int j, int k);
int get_absolute_num(int i, int j, int k);

int main()
{
	FILE* file;
	char file_name[] = "layer.msh";

	file = fopen(file_name,"w");

	fprintf(file, "$MeshFormat\n2 0 8\n$EndMeshFormat\n$Nodes\n%d\n", Size*Size*Width);

	int cur_num;

	for(int k = 0; k < Width; k++)
		for(int j = 0; j < Size; j++)
			for(int i = 0; i < Size; i++)
			{
				cur_num = get_num_in_zone(i, j, k);
				if( cur_num >= 0 )
					fprintf(file, "%d %d %d %d\n", cur_num, i , j, k);
			}

	fprintf(file, "$EndNodes\n$Elements\n%d\n", (Size-1)*(Size-1)*(Width-1)*6);

	int tetr_num = 1;

	for(int k = 0; k < Width-1; k++)
		for(int j = 0; j < Size-1; j++)
			for(int i = 0; i < Size-1; i++)
			{
				write_tetrs(file, tetr_num, i, j, k);
				tetr_num += 6;
			}

	fprintf(file, "$EndElements\n");

	fprintf(file, "$NodeData\n1\n\"Mesh partitions indexes\"\n1\n0.0\n3\n0\n1\n%d\n", Size*Size*Width);

	int nodes_per_zone_edge;
	if(Size % ZonesPerEdge == 0)
		nodes_per_zone_edge = (int)(Size / ZonesPerEdge);
	else
		nodes_per_zone_edge = (int)(Size / ZonesPerEdge) + 1;

	int zone_num;

	for(int k = 0; k < Width; k++)
		for(int j = 0; j < Size; j++)
			for(int i = 0; i < Size; i++)
			{
				cur_num = get_num_in_zone(i, j, k);
				zone_num = (int)(i/nodes_per_zone_edge) + (int)(j/nodes_per_zone_edge) * ZonesPerEdge;
				fprintf(file, "%d %d\n", cur_num, zone_num);
			}
	
	fprintf(file, "$EndNodeData\n");

	fclose(file);

	return 0;
}

int get_num_in_zone(int i, int j, int k)
{
	return ( k * (Size) * (Size) + j * (Size) + i ) /* Just to start numbers from 1*/ + 1;
}

int write_tetrs(FILE* file, int num, int i, int j, int k) // i,j,k - 'local' i,j,k
{
	fprintf(file, "%d 4 3 0 1 0 %d %d %d %d\n", num,
		get_absolute_num(i, j, k), get_absolute_num(i, j+1, k+1), 
		get_absolute_num(i, j+1, k), get_absolute_num(i+1, j+1, k) );

	fprintf(file, "%d 4 3 0 1 0 %d %d %d %d\n", num+1,
		get_absolute_num(i, j, k+1), get_absolute_num(i, j+1, k+1),
		get_absolute_num(i+1, j+1, k+1), get_absolute_num(i, j, k) );

	fprintf(file, "%d 4 3 0 1 0 %d %d %d %d\n", num+2,
		get_absolute_num(i, j+1, k+1), get_absolute_num(i, j, k),
		get_absolute_num(i+1, j+1, k+1), get_absolute_num(i+1, j+1, k) );

	fprintf(file, "%d 4 3 0 1 0 %d %d %d %d\n", num+3,
		get_absolute_num(i, j, k), get_absolute_num(i+1, j, k),
		get_absolute_num(i+1, j+1, k), get_absolute_num(i+1, j, k+1) );

	fprintf(file, "%d 4 3 0 1 0 %d %d %d %d\n", num+4,
		get_absolute_num(i, j, k+1), get_absolute_num(i+1, j, k+1),
		get_absolute_num(i+1, j+1, k+1), get_absolute_num(i, j, k) );

	fprintf(file, "%d 4 3 0 1 0 %d %d %d %d\n", num+5,
		get_absolute_num(i+1, j, k+1), get_absolute_num(i+1, j+1, k+1),
		get_absolute_num(i, j, k), get_absolute_num(i+1, j+1, k) );

	return 0;
}

int get_absolute_num(int i, int j, int k)
{
	return 1 + k * (Size) * (Size) + j * (Size) + i;
}
