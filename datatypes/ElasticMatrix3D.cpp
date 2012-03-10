#include "ElasticMatrix3D.h"

ElasticMatrix3D::ElasticMatrix3D()
{
//	A.resize(9,9);
//	L.resize(9,9);
//	U.resize(9,9);
//	U1.resize(9,9);
	zero_all();
};

ElasticMatrix3D::~ElasticMatrix3D() { };

int ElasticMatrix3D::prepare_matrix(float la, float mu, float ro, int stage)
{
	return prepare_matrix(la, mu, ro, stage, NULL);
};

int ElasticMatrix3D::prepare_matrix(float la, float mu, float ro, int stage, Logger* logger)
{
	if((la <= 0) || (mu <= 0) || (ro <= 0))
		throw GCMException( GCMException::CONFIG_EXCEPTION, "Bad rheology");

	if (stage == 0) {
		CreateAx(la, mu, ro);
	} else if (stage == 1) { 
		CreateAy(la, mu, ro); 
	} else if (stage == 2) { 
		CreateAz(la, mu, ro); 
	} else {
		throw GCMException( GCMException::CONFIG_EXCEPTION, "Wrong stage number");
	}
	return 0;
};


int ElasticMatrix3D::prepare_matrix(float la, float mu, float ro, float qjx, float qjy, float qjz)
{
	return prepare_matrix(la, mu, ro, qjx, qjy, qjz, NULL);
};


int ElasticMatrix3D::prepare_matrix(float la, float mu, float ro, float qjx, float qjy, float qjz, Logger* logger)
{
	if((la <= 0) || (mu <= 0) || (ro <= 0))
		throw GCMException( GCMException::CONFIG_EXCEPTION, "Bad rheology");

	if( qjx*qjx + qjy*qjy + qjz*qjz <= 0 )
		throw GCMException( GCMException::CONFIG_EXCEPTION, "Bad vector q");

	CreateGeneralizedMatrix(la, mu, ro, qjx, qjy, qjz);

	return 0;
};

int ElasticMatrix3D::self_check(Logger* logger)
{
	gcm_matrix E;
	E.createE();
	if( (U1 * L * U != A) || (A * U1 != U1 * L) || (U * A != L * U) || (U1 * U !=  E) ) {
		if(logger != NULL) {
			throw GCMException( GCMException::CONFIG_EXCEPTION, "Self check failed");
			//cout << "A:\n";
			//cout << A;
			//cout << "U1 * L * U:\n";
			//cout << U1 * L * U;
			//cout << "Delta:\n";
			//cout << A - U1 * L * U;
			//cout << "L:\n";
			//cout << L;
			//cout << "Omega:\n";
			//cout << U;
			//cout << "Omega-1:\n";
			//cout << U1;
		}
		return -1;
	}
	return 0;
}

float ElasticMatrix3D::max_lambda()
{
	float res = 0;
	for (unsigned i = 0; i < 9; ++i)
		if(L(i,i) > res)
			res = L(i,i);
	return res;
};

void ElasticMatrix3D::zero_all()
{
	for (unsigned i = 0; i < 9; ++i)
		for (unsigned j = 0; j < 9; ++j)
		{
			A(i,j) = 0;
			L(i,j) = 0;
			U(i,j) = 0;
			U1(i,j) = 0;
		}
};

void ElasticMatrix3D::CreateAx(float la, float mu, float ro)
{
	zero_all();

	A(0,3) = -1/ro;
	A(1,4) = -1/ro;
	A(2,5) = -1/ro;
	A(3,0) = -la - 2*mu;
	A(4,1) = -mu;
	A(5,2) = -mu;
	A(6,0) = -la;
	A(8,0) = -la;

	L(0,0) = sqrt(mu/ro);
	L(1,1) = sqrt(mu/ro);
	L(2,2) = -sqrt(mu/ro);
	L(3,3) = -sqrt(mu/ro);
	L(7,7) = sqrt((la+2*mu)/ro);
	L(8,8) = -sqrt((la+2*mu)/ro);

	U(0,1) = -sqrt(ro*mu)*0.5;
	U(0,4) = 0.5;
	U(1,2) = -sqrt(ro*mu)*0.5;
	U(1,5) = 0.5;
	U(2,1) = sqrt(ro*mu)*0.5;
	U(2,4) = 0.5;
	U(3,2) = sqrt(ro*mu)*0.5;
	U(3,5) = 0.5;
	U(4,7) = 1;
	U(5,3) = -la/(la+2*mu);
	U(5,6) = 1;
	U(6,3) = -la/(la+2*mu);
	U(6,8) = 1;
	U(7,0) = -0.5*la * sqrt(ro/(la+2*mu));
	U(7,3) = 0.5*la/(la+2*mu);
	U(8,0) = 0.5*la * sqrt(ro/(la+2*mu));
	U(8,3) = 0.5*la/(la+2*mu);

	U1(0,7) = -sqrt((la+2*mu)/ro)/la;
	U1(0,8) = sqrt((la+2*mu)/ro)/la;
	U1(1,0) = -1/sqrt(ro*mu);
	U1(1,2) = 1/sqrt(ro*mu);
	U1(2,1) = -1/sqrt(ro*mu);
	U1(2,3) = 1/sqrt(ro*mu);
	U1(3,7) = (la+2*mu)/la;
	U1(3,8) = (la+2*mu)/la;
	U1(4,0) = 1;
	U1(4,2) = 1;
	U1(5,1) = 1;
	U1(5,3) = 1;
	U1(6,5) = 1;
	U1(6,7) = 1;
	U1(6,8) = 1;
	U1(7,4) = 1;
	U1(8,6) = 1;
	U1(8,7) = 1;
	U1(8,8) = 1;
};

void ElasticMatrix3D::CreateAy(float la, float mu, float ro)
{
	zero_all();

	A(0,4) = -1/ro;
	A(1,6) = -1/ro;
	A(2,7) = -1/ro;
	A(3,1) = -la;
	A(4,0) = -mu;
	A(6,1) = -la-2*mu;
	A(7,2) = -mu;
	A(8,1) = -la;

	L(0,0) = sqrt((la+2*mu)/ro);
	L(1,1) = -sqrt((la+2*mu)/ro);
	L(2,2) = sqrt(mu/ro);
	L(3,3) = sqrt(mu/ro);
	L(4,4) = -sqrt(mu/ro);
	L(5,5) = -sqrt(mu/ro);

	U(0,1) = -0.5*la*sqrt(ro/(la+2*mu));
	U(0,6) = 0.5*la/(la+2*mu);
	U(1,1) = 0.5*la*sqrt(ro/(la+2*mu));
	U(1,6) = 0.5*la/(la+2*mu);
	U(2,0) = -sqrt(ro*mu)*0.5;
	U(2,4) = 0.5;
	U(3,2) = -sqrt(ro*mu)*0.5;
	U(3,7) = 0.5;
	U(4,0) = sqrt(ro*mu)*0.5;
	U(4,4) = 0.5;
	U(5,2) = sqrt(ro*mu)*0.5;
	U(5,7) = 0.5;
	U(6,3) = 1;
	U(6,6) = -la/(la+2*mu);
	U(7,5) = 1;
	U(8,6) = -la/(la+2*mu);
	U(8,8) = 1;

	U1(0,2) = -1/sqrt(ro*mu);
	U1(0,4) = 1/sqrt(ro*mu);
	U1(1,0) = -sqrt((la+2*mu)/ro)/la;
	U1(1,1) = sqrt((la+2*mu)/ro)/la;
	U1(2,3) = -1/sqrt(ro*mu);
	U1(2,5) = 1/sqrt(ro*mu);
	U1(3,0) = 1;
	U1(3,1) = 1;
	U1(3,6) = 1;
	U1(4,2) = 1;
	U1(4,4) = 1;
	U1(5,7) = 1;
	U1(6,0) = (la+2*mu)/la;
	U1(6,1) = (la+2*mu)/la;
	U1(7,3) = 1;
	U1(7,5) = 1;
	U1(8,0) = 1;
	U1(8,1) = 1;
	U1(8,8) = 1;
};

void ElasticMatrix3D::CreateAz(float la, float mu, float ro)
{
	zero_all();

	A(0,5) = -1/ro;
	A(1,7) = -1/ro;
	A(2,8) = -1/ro;
	A(3,2) = -la;
	A(5,0) = -mu;
	A(6,2) = -la;
	A(7,1) = -mu;
	A(8,2) = -la-2*mu;

	L(3,3) = sqrt((la+2*mu)/ro);
	L(4,4) =-sqrt((la+2*mu)/ro);
	L(5,5) = sqrt(mu/ro);
	L(6,6) = sqrt(mu/ro);
	L(7,7) = - sqrt(mu/ro);
	L(8,8) = - sqrt(mu/ro);

	U(0,6) = 1;
	U(0,8) = -la/(la+2*mu);
	U(1,3) = 1;
	U(1,8) = -la/(la+2*mu);
	U(2,4) = 1;
	U(3,2) = -0.5*la*sqrt(ro/(la+2*mu));
	U(3,8) = 0.5*la/(la+2*mu);
	U(4,2) = 0.5*la*sqrt(ro/(la+2*mu));
	U(4,8) = 0.5*la/(la+2*mu);
	U(5,0) = 0.5;
	U(5,5) = -0.5/sqrt(ro*mu);
	U(6,1) = 0.5;
	U(6,7) = -0.5/sqrt(ro*mu);
	U(7,0) = 0.5;
	U(7,5) = 0.5/sqrt(ro*mu);
	U(8,1) = 0.5;
	U(8,7) = 0.5/sqrt(ro*mu);

	U1(0,5) = 1;
	U1(0,7) = 1;
	U1(1,6) = 1;
	U1(1,8) = 1;
	U1(2,3) = -sqrt((la+2*mu)/ro)/la;
	U1(2,4) = sqrt((la+2*mu)/ro)/la;
	U1(3,1) = 1;
	U1(3,3) = 1;
	U1(3,4) = 1;
	U1(4,2) = 1;
	U1(5,5) = -sqrt(ro*mu);
	U1(5,7) = sqrt(ro*mu);
	U1(6,0) = 1;
	U1(6,3) = 1;
	U1(6,4) = 1;
	U1(7,6) = -sqrt(ro*mu);
	U1(7,8) = sqrt(ro*mu);
	U1(8,3) = (la+2*mu)/la;
	U1(8,4) = (la+2*mu)/la;
};

void ElasticMatrix3D::CreateGeneralizedMatrix(float la, float mu, float ro, float qjx, float qjy, float qjz)
{

	zero_all();

	A(0,0) = 0;			A(0,1) = 0;			A(0,2) = 0;
	A(0,3) = -qjx/ro;		A(0,4) = -qjy/ro;		A(0,5) = -qjz/ro;
	A(0,6) = 0;			A(0,7) = 0;			A(0,8) = 0;

	A(1,0) = 0;			A(1,1) = 0;			A(1,2) = 0;
	A(1,3) = 0;			A(1,4) = -qjx/ro;		A(1,5) = 0;
	A(1,6) = -qjy/ro;		A(1,7) = -qjz/ro;		A(1,8) = 0;

	A(2,0) = 0;			A(2,1) = 0;			A(2,2) = 0;
	A(2,3) = 0;			A(2,4) = 0;			A(2,5) = -qjx/ro;
	A(2,6) = 0;			A(2,7) = -qjy/ro;		A(2,8) = -qjz/ro;

	A(3,0) = -(la+2*mu)*qjx;	A(3,1) = -la*qjy;		A(3,2) = -la*qjz;
	A(3,3) = 0;			A(3,4) = 0;			A(3,5) = 0;
	A(3,6) = 0;			A(3,7) = 0;			A(3,8) = 0;

	A(4,0) = -mu*qjy;		A(4,1) = -mu*qjx;		A(4,2) = 0;
	A(4,3) = 0;			A(4,4) = 0;			A(4,5) = 0;
	A(4,6) = 0;			A(4,7) = 0;			A(4,8) = 0;

	A(5,0) = -mu*qjz;		A(5,1) = 0;			A(5,2) = -mu*qjx;
	A(5,3) = 0;			A(5,4) = 0;			A(5,5) = 0;
	A(5,6) = 0;			A(5,7) = 0;			A(5,8) = 0;

	A(6,0) = -la*qjx;		A(6,1) = -(la+2*mu)*qjy;	A(6,2) = -la*qjz;
	A(6,3) = 0;			A(6,4) = 0;			A(6,5) = 0;
	A(6,6) = 0;			A(6,7) = 0;			A(6,8) = 0;

	A(7,0) = 0;			A(7,1) = -mu*qjz;		A(7,2) = -mu*qjy;
	A(7,3) = 0;			A(7,4) = 0;			A(7,5) = 0;
	A(7,6) = 0;			A(7,7) = 0;			A(7,8) = 0;

	A(8,0) = -la*qjx;		A(8,1) = -la*qjy;		A(8,2) = -(la+2*mu)*qjz;
	A(8,3) = 0;			A(8,4) = 0;			A(8,5) = 0;
	A(8,6) = 0;			A(8,7) = 0;			A(8,8) = 0;

	float l = sqrt(qjx*qjx + qjy*qjy + qjz*qjz);

	n[0][0] = qjx/l;
	n[0][1] = qjy/l;
	n[0][2] = qjz/l;

	/*if(fabs(n[0][0]) <= fabs(n[0][1]) && fabs(n[0][0]) <= fabs(n[0][2]))
	{
		n[1][0] = 0;
		n[1][1] = -n[0][2];
		n[1][2] = n[0][1];
	}
	else if(fabs(n[0][1]) <= fabs(n[0][0]) && fabs(n[0][1]) <= fabs(n[0][2]))
	{
		n[1][0] = -n[0][2];
		n[1][1] = 0;
		n[1][2] = n[0][0];
	}
	else if(fabs(n[0][2]) <= fabs(n[0][0]) && fabs(n[0][2]) <= fabs(n[0][1]))
	{
		n[1][0] = -n[0][1];
		n[1][1] = n[0][0];
		n[1][2] = 0;
	}*/
	if(fabs(n[0][0]) <= fabs(n[0][1])) {
		if(fabs(n[0][0]) <= fabs(n[0][2])) {
			n[1][0] = 0;
			n[1][1] = -n[0][2];
			n[1][2] = n[0][1];
		} else {
			n[1][0] = -n[0][1];
			n[1][1] = n[0][0];
			n[1][2] = 0;
		}
	} else {
		if(fabs(n[0][1]) <= fabs(n[0][2])) {
			n[1][0] = -n[0][2];
			n[1][1] = 0;
			n[1][2] = n[0][0];
		} else {
			n[1][0] = -n[0][1];
			n[1][1] = n[0][0];
			n[1][2] = 0;
		}
	}
	float dtmp = sqrt(n[1][0]*n[1][0] + n[1][1]*n[1][1] + n[1][2]*n[1][2]);
	n[1][0] /= dtmp;
	n[1][1] /= dtmp;
	n[1][2] /= dtmp;

	n[2][0] = n[0][1]*n[1][2] - n[0][2]*n[1][1];
	n[2][1] = n[0][2]*n[1][0] - n[0][0]*n[1][2];
	n[2][2] = n[0][0]*n[1][1] - n[0][1]*n[1][0];

	float c1 = sqrt((la+2*mu)/ro);
	float c2 = sqrt(mu/ro);
	float c3 = sqrt(la*la/(ro*(la+2*mu)));

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
		U1(3+i,0) = -ro*((c1-c3)*N00[i] + c3*I[i]);

	U1(0,1) = n[0][0];
	U1(1,1) = n[0][1];
	U1(2,1) = n[0][2];
	for(int i = 0; i < 6; i++)
		U1(3+i,1) = ro*((c1-c3)*N00[i] + c3*I[i]);

	U1(0,2) = n[1][0];
	U1(1,2) = n[1][1];
	U1(2,2) = n[1][2];
	for(int i = 0; i < 6; i++)
		U1(3+i,2) = -2*ro*c2*N01[i];

	U1(0,3) = n[1][0];
	U1(1,3) = n[1][1];
	U1(2,3) = n[1][2];
	for(int i = 0; i < 6; i++)
		U1(3+i,3) = 2*ro*c2*N01[i];

	U1(0,4) = n[2][0];
	U1(1,4) = n[2][1];
	U1(2,4) = n[2][2];
	for(int i = 0; i < 6; i++)
		U1(3+i,4) = -2*ro*c2*N02[i];

	U1(0,5) = n[2][0];
	U1(1,5) = n[2][1];
	U1(2,5) = n[2][2];
	for(int i = 0; i < 6; i++)
		U1(3+i,5) = 2*ro*c2*N02[i];

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
		
	for(int i = 0; i < 9; i++)
		for(int j = 0; j < 9; j++)
			U1(i,j) /= 2;

	U(0, 0) = n[0][0];
	U(0, 1) = n[0][1];
	U(0, 2) = n[0][2];
	U(0, 3) = -N00[0]/(c1*ro);
	U(0, 4) = -2*N00[1]/(c1*ro);
	U(0, 5) = -2*N00[2]/(c1*ro);
	U(0, 6) = -N00[3]/(c1*ro);
	U(0, 7) = -2*N00[4]/(c1*ro);
	U(0, 8) = -N00[5]/(c1*ro);

	U(1, 0) = n[0][0];
	U(1, 1) = n[0][1];
	U(1, 2) = n[0][2];
	U(1, 3) = N00[0]/(c1*ro);
	U(1, 4) = 2*N00[1]/(c1*ro);
	U(1, 5) = 2*N00[2]/(c1*ro);
	U(1, 6) = N00[3]/(c1*ro);
	U(1, 7) = 2*N00[4]/(c1*ro);
	U(1, 8) = N00[5]/(c1*ro);

	U(2, 0) = n[1][0];
	U(2, 1) = n[1][1];
	U(2, 2) = n[1][2];
	U(2, 3) = -N01[0]/(c2*ro);
	U(2, 4) = -2*N01[1]/(c2*ro);
	U(2, 5) = -2*N01[2]/(c2*ro);
	U(2, 6) = -N01[3]/(c2*ro);
	U(2, 7) = -2*N01[4]/(c2*ro);
	U(2, 8) = -N01[5]/(c2*ro);

	U(3, 0) = n[1][0];
	U(3, 1) = n[1][1];
	U(3, 2) = n[1][2];
	U(3, 3) = N01[0]/(c2*ro);
	U(3, 4) = 2*N01[1]/(c2*ro);
	U(3, 5) = 2*N01[2]/(c2*ro);
	U(3, 6) = N01[3]/(c2*ro);
	U(3, 7) = 2*N01[4]/(c2*ro);
	U(3, 8) = N01[5]/(c2*ro);

	U(4, 0) = n[2][0];
	U(4, 1) = n[2][1];
	U(4, 2) = n[2][2];
	U(4, 3) = -N02[0]/(c2*ro);
	U(4, 4) = -2*N02[1]/(c2*ro);
	U(4, 5) = -2*N02[2]/(c2*ro);
	U(4, 6) = -N02[3]/(c2*ro);
	U(4, 7) = -2*N02[4]/(c2*ro);
	U(4, 8) = -N02[5]/(c2*ro);

	U(5, 0) = n[2][0];
	U(5, 1) = n[2][1];
	U(5, 2) = n[2][2];
	U(5, 3) = N02[0]/(c2*ro);
	U(5, 4) = 2*N02[1]/(c2*ro);
	U(5, 5) = 2*N02[2]/(c2*ro);
	U(5, 6) = N02[3]/(c2*ro);
	U(5, 7) = 2*N02[4]/(c2*ro);
	U(5, 8) = N02[5]/(c2*ro);

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

};


void ElasticMatrix3D::createMatrixN(int i, int j, float *res)
{
	*res = (n[i][0]*n[j][0] + n[j][0]*n[i][0])/2;
	*(res+1) = (n[i][0]*n[j][1] + n[j][0]*n[i][1])/2;
	*(res+2) = (n[i][0]*n[j][2] + n[j][0]*n[i][2])/2;
	*(res+3) = (n[i][1]*n[j][1] + n[j][1]*n[i][1])/2;
	*(res+4) = (n[i][1]*n[j][2] + n[j][1]*n[i][2])/2;
	*(res+5) = (n[i][2]*n[j][2] + n[j][2]*n[i][2])/2;
};
