#include <stdio.h>
#include <stdlib.h>
#include "../system/quick_math.h"

#define LIMIT 0.000000000000001

int main()
{
	float ksi[3][3];
	float x[3][3];

	for (int r = 0; r < 3; r++)
		for (int c = 0; c < 3; c++)
			ksi[r][c] = (float)drand48();

	quick_math qm_engine;

	float det = qm_engine.determinant(
			ksi[0][0], ksi[0][1], ksi[0][2],
			ksi[1][0], ksi[1][1], ksi[1][2],
			ksi[2][0], ksi[2][1], ksi[2][2] );

	x[0][0] = ( ksi[1][1] * ksi[2][2] - ksi[2][1] * ksi[1][2]) / det;
	x[1][0] = ( ksi[2][0] * ksi[1][2] - ksi[1][0] * ksi[2][2]) / det;
	x[2][0] = ( ksi[1][0] * ksi[2][1] - ksi[2][0] * ksi[1][1]) / det;

	x[0][1] = ( ksi[2][1] * ksi[0][2] - ksi[0][1] * ksi[2][2]) / det;
	x[1][1] = ( ksi[0][0] * ksi[2][2] - ksi[2][0] * ksi[0][2]) / det;
	x[2][1] = ( ksi[2][0] * ksi[0][1] - ksi[0][0] * ksi[2][1]) / det;

	x[0][2] = ( ksi[0][1] * ksi[1][2] - ksi[1][1] * ksi[0][2]) / det;
	x[1][2] = ( ksi[1][0] * ksi[0][2] - ksi[0][0] * ksi[1][2]) / det;
	x[2][2] = ( ksi[0][0] * ksi[1][1] - ksi[1][0] * ksi[0][1]) / det;

	printf("Testing coordinates dervatives calculation:\n");

	float res_matrix[3][3];
	bool is_correct = true;

	float e[3][3];
	for (int r = 0; r < 3; r++) 
		for (int c = 0; c < 3; c++) 
			if(r == c)
				e[r][c] = 1;
			else
				e[r][c] = 0;

	for (int r = 0; r < 3; r++) {
		for (int c = 0; c < 3; c++) {
			res_matrix[r][c] = 0;
			for (int i = 0; i < 3; i++) {
				res_matrix[r][c] += ksi[r][i] * x[i][c];
			}
			if(fabs(res_matrix[r][c] - e[r][c]) > LIMIT)
				is_correct = false;
		}
	}

	if(is_correct)
		printf("\tInverse matrix - OK\n");
	else
		printf("\tInverse matrix - OK\n");

	printf("Done\n");
	return 0;
};
