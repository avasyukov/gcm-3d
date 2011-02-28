#include <stdio.h>

// Number of zones to create
#define Nnum 1
// Number of layers in zone
#define Mnum 101

// Example:
// If you set Nnum to 10 and Mnum to 2
// you will get cube 20x20x20 total
// divided into 10 zones 20x20x2 each.

int get_num_in_zone(int i, int j, int k, int n, int N, int M);
int write_tetrs(FILE* file, int num, int i, int j, int k, int N, int M);
int get_absolute_num(int i, int j, int k, int N, int M);

int main()
{
	FILE* file;
	char file_name[50];

	int N = Nnum;
	int M = Mnum;

	for(int n = 0; n < N; n++)
	{
		sprintf(file_name, "tetr-element-%d.msh.final", n);
		file = fopen(file_name,"w");

		int last_k_num = -1;
                if(n == 0 || n == N-1) { last_k_num = M-1; }
                else { last_k_num = M; }

		// TODO Test if change of last_k_num to num_of_layers works for parallel data
		int num_of_layers = M;
		if(n != 0) num_of_layers++;
		if(n != N-1) num_of_layers++;

		fprintf(file, "$MeshFormat\n2.1 0 8\n$EndMeshFormat\n$Nodes\n%d\n", N*M*N*M*num_of_layers);

		int num = 0;
		int cur_num;
		int rem_num;

		int k;

		k = n*M-1;
		for(int j = 0; j < N*M; j++)
			for(int i = 0; i < N*M; i++)
			{
				// Low level
				rem_num = get_num_in_zone(i, j, k, n-1, N, M);
				cur_num = get_num_in_zone(i, j, k, n, N, M);
				if( cur_num >=0 && rem_num >= 0)
					fprintf(file, "%d %d %d\n", -cur_num, n-1, rem_num);
			}

		for(int k = n*M; k < (n+1)*M; k++)
			for(int j = 0; j < N*M; j++)
				for(int i = 0; i < N*M; i++)
				{
					// Real layer
					cur_num = get_num_in_zone(i, j, k, n, N, M);
					if( cur_num >= 0 )
						fprintf(file, "%d %d %d %d\n", cur_num, i , j, k);
				}

		k = (n+1)*M;
		for(int j = 0; j < N*M; j++)
			for(int i = 0; i < N*M; i++)
			{
				// Top level
                                rem_num = get_num_in_zone(i, j, k, n+1, N, M);
                                cur_num = get_num_in_zone(i, j, k, n, N, M);
                                if( cur_num >=0 && rem_num >= 0)
                                        fprintf(file, "%d %d %d\n", -cur_num, n+1, rem_num);
                        }

		fprintf(file, "$EndNodes\n$Elements\n%d\n", (N*M-1)*(N*M-1)*(num_of_layers-1)*6);

		int tetr_num = 1;

		for(int k = 0; k < num_of_layers-1; k++)
                        for(int j = 0; j < N*M-1; j++)
                                for(int i = 0; i < N*M-1; i++)
				{
					write_tetrs(file, tetr_num, i, j, k, N, M);
					tetr_num += 6;
				}

		fprintf(file, "$EndElements\n");
		fclose(file);
	}

	return 0;
}

int get_num_in_zone(int i, int j, int k, int n, int N, int M)
{
	if(n < 0) return -1;
	if(n > N-1) return -1;
	if(k < 0) return -1;
	if(k > N * M - 1) return -1;
	if(k < n * M - 1) return -1;
	if(k > (n + 1) * M) return -1;

	int low_layer_size = -1;
	if( n == 0 )
		low_layer_size = 0;
	else
		low_layer_size = (n*M - 1) * (N*M) * (N*M);
	
	return ( k * (N*M) * (N*M) + j * (N*M) + i ) - low_layer_size /* Just to start numbers from 1*/ + 1;
}

int write_tetrs(FILE* file, int num, int i, int j, int k, int N, int M) // i,j,k - 'local' i,j,k
{
	fprintf(file, "%d 4 3 0 1 0 %d %d %d %d\n", num,
		get_absolute_num(i, j, k, N, M), get_absolute_num(i, j+1, k+1, N, M), 
		get_absolute_num(i, j+1, k, N, M), get_absolute_num(i+1, j+1, k, N, M) );

	fprintf(file, "%d 4 3 0 1 0 %d %d %d %d\n", num+1,
		get_absolute_num(i, j, k+1, N, M), get_absolute_num(i, j+1, k+1, N, M),
		get_absolute_num(i+1, j+1, k+1, N, M), get_absolute_num(i, j, k, N, M) );

	fprintf(file, "%d 4 3 0 1 0 %d %d %d %d\n", num+2,
		get_absolute_num(i, j+1, k+1, N, M), get_absolute_num(i, j, k, N, M),
		get_absolute_num(i+1, j+1, k+1, N, M), get_absolute_num(i+1, j+1, k, N, M) );

	fprintf(file, "%d 4 3 0 1 0 %d %d %d %d\n", num+3,
		get_absolute_num(i, j, k, N, M), get_absolute_num(i+1, j, k, N, M),
		get_absolute_num(i+1, j+1, k, N, M), get_absolute_num(i+1, j, k+1, N, M) );

	fprintf(file, "%d 4 3 0 1 0 %d %d %d %d\n", num+4,
		get_absolute_num(i, j, k+1, N, M), get_absolute_num(i+1, j, k+1, N, M),
		get_absolute_num(i+1, j+1, k+1, N, M), get_absolute_num(i, j, k, N, M) );

	fprintf(file, "%d 4 3 0 1 0 %d %d %d %d\n", num+5,
		get_absolute_num(i+1, j, k+1, N, M), get_absolute_num(i+1, j+1, k+1, N, M),
		get_absolute_num(i, j, k, N, M), get_absolute_num(i+1, j+1, k, N, M) );

	return 0;
}

int get_absolute_num(int i, int j, int k, int N, int M)
{
	return 1 + k * (N*M) * (N*M) + j * (N*M) + i;
}
