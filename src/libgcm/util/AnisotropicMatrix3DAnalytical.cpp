#include "util/AnisotropicMatrix3DAnalytical.h"

gcm::AnisotropicMatrix3DAnalytical::AnisotropicMatrix3DAnalytical()
{
	zero_all();
};

gcm::AnisotropicMatrix3DAnalytical::~AnisotropicMatrix3DAnalytical() { };

void gcm::AnisotropicMatrix3DAnalytical::prepare_matrix(const AnisotropicNumbers &C, float rho, int stage)
{
	assert ( rho > 0 );
	for (int i = 0; i < 21; i++) {
		// TODO	- Are there any other limits on anisotropic coefficients?
		assert (C.c[i] >= 0) ; 
	}

	if (stage == 0) {
		CreateAx(C, rho);
	} else if (stage == 1) { 
		CreateAy(C, rho); 
	} else if (stage == 2) { 
		CreateAz(C, rho); 
	} else {
		THROW_BAD_CONFIG("Wrong stage number");
	}
};

void gcm::AnisotropicMatrix3DAnalytical::prepare_matrix(const AnisotropicNumbers &C, float rho, 
											float qjx, float qjy, float qjz)
{
	assert ( rho > 0 );
	for (int i = 0; i < 21; i++) {
		// TODO	- Are there any other limits on anisotropic coefficients?
		assert (C.c[i] >= 0) ; 
	}
	assert( fabs( vectorNorm(qjx, qjy, qjz) - 1 ) < EQUALITY_TOLERANCE );

	CreateGeneralizedMatrix(C, rho, qjx, qjy, qjz);
};

void gcm::AnisotropicMatrix3DAnalytical::self_check()
{
	gcm_matrix E;
	E.createE();
	if( (U1 * L * U != A) || (A * U1 != U1 * L) || (U * A != L * U) || (U1 * U !=  E) )
		THROW_BAD_CONFIG("Self check failed");
};

float gcm::AnisotropicMatrix3DAnalytical::max_lambda()
{
	float res = 0;
	for (unsigned i = 0; i < GCM_MATRIX_SIZE; ++i)
		if(L(i,i) > res)
			res = L(i,i);
	return res;
};

void gcm::AnisotropicMatrix3DAnalytical::zero_all()
{
	A.clear();
	L.clear();
	U.clear();
	U1.clear();
};

void gcm::AnisotropicMatrix3DAnalytical::CreateAx(const AnisotropicNumbers &C, float rho)
{
	zero_all();

	// Setting values of A
	A(0,3) = A(1,9) = A(2,8) = -1/rho;
	for (int i = 0; i < 6; i++)
		A(i+3,0) = -C.c[i];
	A(3,1) = -C.c16;
	A(4,1) = -C.c26;
	A(5,1) = -C.c36;
	A(6,1) = -C.c46;
	A(7,1) = -C.c56;
	A(8,1) = -C.c66;
	A(3,2) = -C.c15;
	A(4,2) = -C.c25;
	A(5,2) = -C.c35;
	A(6,2) = -C.c45;
	A(7,2) = -C.c55;
	A(8,2) = -C.c56;
	
	// Search eigenvalues and filling the diagonal matrix
	ThirdDegreePolynomial tdp (rho, C, 0);
	float roots[3];
	tdp.getRoots(roots);
	L(0,0) = sqrt(roots[0]);
	L(1,1) = -L(0,0);
	L(2,2) = sqrt(roots[1]);
	L(3,3) = -L(2,2);
	L(4,4) = sqrt(roots[2]);
	L(5,5) = -L(4,4);
	
	// Search eigenvectors and filling the transition matrix
	// (  A = U1 * L * U  and so eigenvectors are columns of the U1  )
	float eigenVec[9];
	for (int i = 0; i < 6; i++) {
		findEigenVec(eigenVec, L(i,i), rho, C, 0);
		U1.setColumn(eigenVec, i);
 	}
	U1(4,6) = U1(5,7) = U1(6,8) = 1;

	// Search U = U1^(-1)
	gsl_matrix* Z1 = gsl_matrix_alloc (GCM_MATRIX_SIZE, GCM_MATRIX_SIZE);
	for (int i = 0; i < GCM_MATRIX_SIZE; i++)
		for (int j = 0; j < GCM_MATRIX_SIZE; j++)
			gsl_matrix_set(Z1, i, j, U1(i,j));
	gsl_matrix* Z = gsl_matrix_alloc (GCM_MATRIX_SIZE, GCM_MATRIX_SIZE);
    gsl_permutation* perm = gsl_permutation_alloc (GCM_MATRIX_SIZE);  
	int j;
	gsl_linalg_LU_decomp (Z1, perm, &j);
	gsl_linalg_LU_invert (Z1, perm, Z);
};

void gcm::AnisotropicMatrix3DAnalytical::CreateAy(const AnisotropicNumbers &C, float rho)
{
	
};

void gcm::AnisotropicMatrix3DAnalytical::CreateAz(const AnisotropicNumbers &C, float rho)
{
	
};

void gcm::AnisotropicMatrix3DAnalytical::CreateGeneralizedMatrix(const AnisotropicNumbers &C, float rho, 
													float qjx, float qjy, float qjz)
{/*

	zero_all();

	A(0,0) = 0;					A(0,1) = 0;					A(0,2) = 0;
	A(0,3) = -qjx/rho;			A(0,4) = -qjy/rho;			A(0,5) = -qjz/rho;
	A(0,6) = 0;					A(0,7) = 0;					A(0,8) = 0;

	A(1,0) = 0;					A(1,1) = 0;					A(1,2) = 0;
	A(1,3) = 0;					A(1,4) = -qjx/rho;			A(1,5) = 0;
	A(1,6) = -qjy/rho;			A(1,7) = -qjz/rho;			A(1,8) = 0;

	A(2,0) = 0;					A(2,1) = 0;					A(2,2) = 0;
	A(2,3) = 0;					A(2,4) = 0;					A(2,5) = -qjx/rho;
	A(2,6) = 0;					A(2,7) = -qjy/rho;			A(2,8) = -qjz/rho;

	A(3,0) = -(la+2*mu)*qjx;	A(3,1) = -la*qjy;			A(3,2) = -la*qjz;
	A(3,3) = 0;					A(3,4) = 0;					A(3,5) = 0;
	A(3,6) = 0;					A(3,7) = 0;					A(3,8) = 0;

	A(4,0) = -mu*qjy;			A(4,1) = -mu*qjx;			A(4,2) = 0;
	A(4,3) = 0;					A(4,4) = 0;					A(4,5) = 0;
	A(4,6) = 0;					A(4,7) = 0;					A(4,8) = 0;

	A(5,0) = -mu*qjz;			A(5,1) = 0;					A(5,2) = -mu*qjx;
	A(5,3) = 0;					A(5,4) = 0;					A(5,5) = 0;
	A(5,6) = 0;					A(5,7) = 0;					A(5,8) = 0;

	A(6,0) = -la*qjx;			A(6,1) = -(la+2*mu)*qjy;	A(6,2) = -la*qjz;
	A(6,3) = 0;					A(6,4) = 0;					A(6,5) = 0;
	A(6,6) = 0;					A(6,7) = 0;					A(6,8) = 0;

	A(7,0) = 0;					A(7,1) = -mu*qjz;			A(7,2) = -mu*qjy;
	A(7,3) = 0;					A(7,4) = 0;					A(7,5) = 0;
	A(7,6) = 0;					A(7,7) = 0;					A(7,8) = 0;

	A(8,0) = -la*qjx;			A(8,1) = -la*qjy;			A(8,2) = -(la+2*mu)*qjz;
	A(8,3) = 0;					A(8,4) = 0;					A(8,5) = 0;
	A(8,6) = 0;					A(8,7) = 0;					A(8,8) = 0;

	float l = vectorNorm(qjx, qjy, qjz);
	float scale = 1 / l;

	n[0][0] = qjx * scale;
	n[0][1] = qjy * scale;
	n[0][2] = qjz * scale;

	createLocalBasis( n[0], n[1], n[2] );
	
	float c1 = sqrt((la+2*mu)/rho);
	float c2 = sqrt(mu/rho);
	float c3 = sqrt(la*la/(rho*(la+2*mu)));

	L(0,0) = c1 * l;
	L(1,1) = -c1 * l;
	L(2,2) = c2 * l;
	L(3,3) = -c2 * l;
	L(4,4) = c2 * l;
	L(5,5) = -c2 * l;
	L(6,6) = 0;
	L(7,7) = 0;
	L(8,8) = 0;

	float I[6];
	float N00[6];
	float N01[6];
	float N02[6];
	float N11[6];
	float N12[6];
	float N22[6];

	I[0] = 1; I[1] = 0; I[2] = 0; I[3] = 1; I[4] = 0; I[5] = 1;
	createMatrixN(0,0,N00);
	createMatrixN(0,1,N01);
	createMatrixN(0,2,N02);
	createMatrixN(1,1,N11);
	createMatrixN(1,2,N12);
	createMatrixN(2,2,N22);

	U1(0,0) = n[0][0];
	U1(1,0) = n[0][1];
	U1(2,0) = n[0][2];
	for(int i = 0; i < 6; i++)
		U1(3+i,0) = -rho*((c1-c3)*N00[i] + c3*I[i]);

	U1(0,1) = n[0][0];
	U1(1,1) = n[0][1];
	U1(2,1) = n[0][2];
	for(int i = 0; i < 6; i++)
		U1(3+i,1) = rho*((c1-c3)*N00[i] + c3*I[i]);

	U1(0,2) = n[1][0];
	U1(1,2) = n[1][1];
	U1(2,2) = n[1][2];
	for(int i = 0; i < 6; i++)
		U1(3+i,2) = -2*rho*c2*N01[i];

	U1(0,3) = n[1][0];
	U1(1,3) = n[1][1];
	U1(2,3) = n[1][2];
	for(int i = 0; i < 6; i++)
		U1(3+i,3) = 2*rho*c2*N01[i];

	U1(0,4) = n[2][0];
	U1(1,4) = n[2][1];
	U1(2,4) = n[2][2];
	for(int i = 0; i < 6; i++)
		U1(3+i,4) = -2*rho*c2*N02[i];

	U1(0,5) = n[2][0];
	U1(1,5) = n[2][1];
	U1(2,5) = n[2][2];
	for(int i = 0; i < 6; i++)
		U1(3+i,5) = 2*rho*c2*N02[i];

	U1(0,6) = 0;
	U1(1,6) = 0;
	U1(2,6) = 0;
	for(int i = 0; i < 6; i++)
		U1(3+i,6) = 4*N12[i];

	U1(0,7) = 0;
	U1(1,7) = 0;
	U1(2,7) = 0;
	for(int i = 0; i < 6; i++)
		U1(3+i,7) = N11[i] - N22[i];

	U1(0,8) = 0;
	U1(1,8) = 0;
	U1(2,8) = 0;
	for(int i = 0; i < 6; i++)
		U1(3+i,8) = I[i] - N00[i];
		
	for(int i = 0; i < GCM_MATRIX_SIZE; i++)
		for(int j = 0; j < GCM_MATRIX_SIZE; j++)
			U1(i,j) /= 2;

	U(0, 0) = n[0][0];
	U(0, 1) = n[0][1];
	U(0, 2) = n[0][2];
	U(0, 3) = -N00[0]/(c1*rho);
	U(0, 4) = -2*N00[1]/(c1*rho);
	U(0, 5) = -2*N00[2]/(c1*rho);
	U(0, 6) = -N00[3]/(c1*rho);
	U(0, 7) = -2*N00[4]/(c1*rho);
	U(0, 8) = -N00[5]/(c1*rho);

	U(1, 0) = n[0][0];
	U(1, 1) = n[0][1];
	U(1, 2) = n[0][2];
	U(1, 3) = N00[0]/(c1*rho);
	U(1, 4) = 2*N00[1]/(c1*rho);
	U(1, 5) = 2*N00[2]/(c1*rho);
	U(1, 6) = N00[3]/(c1*rho);
	U(1, 7) = 2*N00[4]/(c1*rho);
	U(1, 8) = N00[5]/(c1*rho);

	U(2, 0) = n[1][0];
	U(2, 1) = n[1][1];
	U(2, 2) = n[1][2];
	U(2, 3) = -N01[0]/(c2*rho);
	U(2, 4) = -2*N01[1]/(c2*rho);
	U(2, 5) = -2*N01[2]/(c2*rho);
	U(2, 6) = -N01[3]/(c2*rho);
	U(2, 7) = -2*N01[4]/(c2*rho);
	U(2, 8) = -N01[5]/(c2*rho);

	U(3, 0) = n[1][0];
	U(3, 1) = n[1][1];
	U(3, 2) = n[1][2];
	U(3, 3) = N01[0]/(c2*rho);
	U(3, 4) = 2*N01[1]/(c2*rho);
	U(3, 5) = 2*N01[2]/(c2*rho);
	U(3, 6) = N01[3]/(c2*rho);
	U(3, 7) = 2*N01[4]/(c2*rho);
	U(3, 8) = N01[5]/(c2*rho);

	U(4, 0) = n[2][0];
	U(4, 1) = n[2][1];
	U(4, 2) = n[2][2];
	U(4, 3) = -N02[0]/(c2*rho);
	U(4, 4) = -2*N02[1]/(c2*rho);
	U(4, 5) = -2*N02[2]/(c2*rho);
	U(4, 6) = -N02[3]/(c2*rho);
	U(4, 7) = -2*N02[4]/(c2*rho);
	U(4, 8) = -N02[5]/(c2*rho);

	U(5, 0) = n[2][0];
	U(5, 1) = n[2][1];
	U(5, 2) = n[2][2];
	U(5, 3) = N02[0]/(c2*rho);
	U(5, 4) = 2*N02[1]/(c2*rho);
	U(5, 5) = 2*N02[2]/(c2*rho);
	U(5, 6) = N02[3]/(c2*rho);
	U(5, 7) = 2*N02[4]/(c2*rho);
	U(5, 8) = N02[5]/(c2*rho);

	U(6, 0) = 0;
	U(6, 1) = 0;
	U(6, 2) = 0;
	U(6, 3) = N12[0];
	U(6, 4) = 2*N12[1];
	U(6, 5) = 2*N12[2];
	U(6, 6) = N12[3];
	U(6, 7) = 2*N12[4];
	U(6, 8) = N12[5];

	U(7, 0) = 0;
	U(7, 1) = 0;
	U(7, 2) = 0;
	U(7, 3) = (N11[0]-N22[0]);
	U(7, 4) = 2*(N11[1]-N22[1]);
	U(7, 5) = 2*(N11[2]-N22[2]);
	U(7, 6) = (N11[3]-N22[3]);
	U(7, 7) = 2*(N11[4]-N22[4]);
	U(7, 8) = (N11[5]-N22[5]);

	U(8, 0) = 0;
	U(8, 1) = 0;
	U(8, 2) = 0;
	U(8, 3) = (N11[0]+N22[0]-2*la*N00[0]/(la+2*mu));
	U(8, 4) = 2*(N11[1]+N22[1]-2*la*N00[1]/(la+2*mu));
	U(8, 5) = 2*(N11[2]+N22[2]-2*la*N00[2]/(la+2*mu));
	U(8, 6) = (N11[3]+N22[3]-2*la*N00[3]/(la+2*mu));
	U(8, 7) = 2*(N11[4]+N22[4]-2*la*N00[4]/(la+2*mu));
	U(8, 8) = (N11[5]+N22[5]-2*la*N00[5]/(la+2*mu));

*/};


void gcm::AnisotropicMatrix3DAnalytical::createMatrixN(int i, int j, float *res)
{
	*res = (n[i][0]*n[j][0] + n[j][0]*n[i][0])/2;
	*(res+1) = (n[i][0]*n[j][1] + n[j][0]*n[i][1])/2;
	*(res+2) = (n[i][0]*n[j][2] + n[j][0]*n[i][2])/2;
	*(res+3) = (n[i][1]*n[j][1] + n[j][1]*n[i][1])/2;
	*(res+4) = (n[i][1]*n[j][2] + n[j][1]*n[i][2])/2;
	*(res+5) = (n[i][2]*n[j][2] + n[j][2]*n[i][2])/2;
};

void gcm::AnisotropicMatrix3DAnalytical::findNonZeroSolution(float **M, float *x)
{
	// Range ( M ) = 2, one of x[i] is random
	float det = M[0][0]*M[1][1]*M[2][2] + M[2][0]*M[0][1]*M[1][2] + M[1][0]*M[0][2]*M[2][1] -
				M[2][0]*M[1][1]*M[0][2] - M[0][0]*M[1][2]*M[2][1] - M[1][0]*M[0][1]*M[2][2];
	if (abs(det) > 1e-3)
		THROW_INVALID_ARG("M isn't degenerate (from findNonZeroSolution)");
	
	det = M[0][0]*M[1][1] - M[0][1]*M[1][0];
	int i = 2;
	float det2 = M[0][0]*M[2][2] - M[0][2]*M[2][0];
	if (abs(det) < abs(det2)) {
		det = det2;
		i = 1;
	}
	det2 = M[1][1]*M[2][2] - M[1][2]*M[2][1];
	if (abs(det) < abs(det2)) {
		det = det2;
		i = 0;
	}
	if (abs(det) < 1e-3)
		THROW_INVALID_ARG("Range (M) isn't 2 (from findNonZeroSolution)");
	
	x[i] = 1;
	switch (i) {
		case (0) :
		{
			x[1] = (M[1][2]*M[2][0] - M[2][2]*M[1][0]) * x[0] / det;
			x[2] = (M[2][1]*M[1][0] - M[1][1]*M[2][0]) * x[0] / det;
		}
		case (1) :
		{
			x[0] = (M[0][2]*M[2][1] - M[2][2]*M[0][1]) * x[1] / det;
			x[2] = (M[2][0]*M[0][1] - M[0][0]*M[2][1]) * x[1] / det;
		}
		case (2) :
		{
			x[0] = (M[0][1]*M[1][2] - M[1][1]*M[0][2]) * x[2] / det;
			x[1] = (M[1][0]*M[0][2] - M[0][0]*M[1][2]) * x[2] / det;
		}
	}
	
};

void gcm::AnisotropicMatrix3DAnalytical::findEigenVec(float *eigenVec,
					float l, float rho, const AnisotropicNumbers &C, int stage)
{
	// Analitycal search eigenvectors
	// M * x = 0, x = (x1, x2, x3)
	// then x[4-9] <= x[1-3]
	float** M = new float* [3];
	for (int i = 0; i < 3; i++)
		M[i] = new float [3];
	switch ( stage ) {
		case 0 :
		{
			M[0][0] = -C.c11 + rho*l*l;
			M[1][2] = -C.c55 + rho*l*l;
			M[2][1] = -C.c66 + rho*l*l;
			M[0][1] = M[2][0] = -C.c16;
			M[0][2] = M[1][0] = -C.c15;
			M[1][1] = M[2][2] = -C.c56;
		}
		case 1 :
		{
			M[0][1] = -C.c22 + rho*l*l;
			M[1][2] = -C.c44 + rho*l*l;
			M[2][0] = -C.c66 + rho*l*l;
			M[0][0] = M[2][1] = -C.c26;
			M[1][0] = M[2][2] = -C.c46;
			M[1][1] = M[0][2] = -C.c24;
		}
		case 2 :
		{
			M[0][2] = -C.c33 + rho*l*l;
			M[1][1] = -C.c44 + rho*l*l;
			M[2][0] = -C.c55 + rho*l*l;
			M[0][0] = M[2][2] = -C.c35;
			M[2][1] = M[1][0] = -C.c45;
			M[0][1] = M[1][2] = -C.c34;
		}
		default:
			THROW_INVALID_ARG("Wrong stage number (from findEigenVec)");	
	}
	
	findNonZeroSolution(M, eigenVec);

	switch ( stage ) {
		case 0 :
		{
			eigenVec[3] = -rho*l*eigenVec[0];
			eigenVec[8] = -rho*l*eigenVec[1];
			eigenVec[7] = -rho*l*eigenVec[2];
			eigenVec[4] = -(C.c12*eigenVec[0] + C.c26*eigenVec[1] + C.c25*eigenVec[2])/l;
			eigenVec[5] = -(C.c13*eigenVec[0] + C.c36*eigenVec[1] + C.c35*eigenVec[2])/l;
			eigenVec[6] = -(C.c14*eigenVec[0] + C.c46*eigenVec[1] + C.c45*eigenVec[2])/l;
		}
		case 1 :
		{
			eigenVec[4] = -rho*l*eigenVec[1];
			eigenVec[8] = -rho*l*eigenVec[0];
			eigenVec[6] = -rho*l*eigenVec[2];
			eigenVec[3] = -(C.c16*eigenVec[0] + C.c12*eigenVec[1] + C.c14*eigenVec[2])/l;
			eigenVec[5] = -(C.c36*eigenVec[0] + C.c23*eigenVec[1] + C.c34*eigenVec[2])/l;
			eigenVec[7] = -(C.c56*eigenVec[0] + C.c25*eigenVec[1] + C.c45*eigenVec[2])/l;
		}
		case 2 :
		{
			eigenVec[7] = -rho*l*eigenVec[0];
			eigenVec[6] = -rho*l*eigenVec[1];
			eigenVec[5] = -rho*l*eigenVec[2];
			eigenVec[3] = -(C.c15*eigenVec[0] + C.c14*eigenVec[1] + C.c13*eigenVec[2])/l;
			eigenVec[4] = -(C.c25*eigenVec[0] + C.c24*eigenVec[1] + C.c23*eigenVec[2])/l;
			eigenVec[8] = -(C.c56*eigenVec[0] + C.c46*eigenVec[1] + C.c36*eigenVec[2])/l;
		}
	}
};

