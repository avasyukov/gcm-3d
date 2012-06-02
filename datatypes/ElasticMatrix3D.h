#ifndef _GCM_ELASTIC_MATRIX_3D_H
#define _GCM_ELASTIC_MATRIX_3D_H  1

#include "matrixes.h"
#include "../system/GCMException.h"

class ElasticMatrix3D
{
public:
	ElasticMatrix3D();
	~ElasticMatrix3D();
	int prepare_matrix(float la, float mu, float ro, int stage);
	int prepare_matrix(float la, float mu, float ro, float qjx, float qjy, float qjz);
	float max_lambda();

	int self_check();

	gcm_matrix A;
	gcm_matrix L;
	gcm_matrix U;
	gcm_matrix U1;

private:
	void CreateAx(float la, float mu, float ro);
	void CreateAy(float la, float mu, float ro);
	void CreateAz(float la, float mu, float ro);
	void CreateGeneralizedMatrix(float la, float mu, float ro, float qjx, float qjy, float qjz);
	void createMatrixN(int i, int j, float *res);
	void zero_all();

	// TODO it is worth turning them into local vars
	float n[3][3];
};

#endif
