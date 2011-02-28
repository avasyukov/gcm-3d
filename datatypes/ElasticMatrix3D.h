#ifndef _GCM_ELASTIC_MATRIX_3D_H
#define _GCM_ELASTIC_MATRIX_3D_H  1

#include "matrixes.h"
#include "../system/Logger.h"

class ElasticMatrix3D
{
public:
	ElasticMatrix3D();
	~ElasticMatrix3D();
	int prepare_matrix(float la, float mu, float ro, int stage);
	int prepare_matrix(float la, float mu, float ro, int stage, Logger* logger);
	float max_lambda();

	int self_check(float la, float mu, float ro, Logger* logger);
	int check_current(Logger* logger);

	gcm_matrix A;
	gcm_matrix L;
	gcm_matrix U;
	gcm_matrix U1;

private:
	void CreateAx(float la, float mu, float ro);
	void CreateAy(float la, float mu, float ro);
	void CreateAz(float la, float mu, float ro);
	void zero_all();
};

#include "ElasticMatrix3D.inl"

#endif
