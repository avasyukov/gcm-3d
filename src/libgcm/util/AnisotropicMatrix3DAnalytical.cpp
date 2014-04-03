#include "util/AnisotropicMatrix3DAnalytical.h"
#include "materials/AnisotropicElasticMaterial.h"

void gcm::AnisotropicMatrix3DAnalytical::fixValuesOrder()
{
	// TODO - swap the components not by matrix multyplication
	gcm_matrix P;
	P.clear();
	P(0,0) = P(1,1) = P(2,2) = P(3,3) = P(4,6) =
			P(5,8) = P(6,7) = P(7,5) = P(8,4) = 1;
	gcm_matrix P1;
	P1.clear();
	P1(0,0) = P1(1,1) = P1(2,2) = P1(3,3) = P1(6,4) =
			P1(8,5) = P1(7,6) = P1(5,7) = P1(4,8) = 1;

	A = P1 * A * P;
	U = U * P;
	U1 = P1 * U1;
};

void gcm::AnisotropicMatrix3DAnalytical::clear()
{
    A.clear();
    L.clear();
    U.clear();
    U1.clear();
};

void gcm::AnisotropicMatrix3DAnalytical::createAx(const ICalcNode& node)
{
    clear();

#ifdef NDEBUG
    auto mat = static_cast<IAnisotropicElasticMaterial*> (node.getMaterial());
#else
    auto mat = dynamic_cast<IAnisotropicElasticMaterial*> (node.getMaterial());
    assert(mat);
#endif

	auto rho = node.getRho();
	auto C = mat->getParameters();

	// Setting values of A
	A(0,3) = A(1,8) = A(2,7) = -1.0/rho;

	A(3,0) = -C.c11;	A(3,1) = -C.c16;	A(3,2) = -C.c15;
	A(4,0) = -C.c12;	A(4,1) = -C.c26;	A(4,2) = -C.c25;
	A(5,0) = -C.c13;	A(5,1) = -C.c36;	A(5,2) = -C.c35;
	A(6,0) = -C.c14;	A(6,1) = -C.c46;	A(6,2) = -C.c45;
	A(7,0) = -C.c15;	A(7,1) = -C.c56;	A(7,2) = -C.c55;
	A(8,0) = -C.c16;	A(8,1) = -C.c66;	A(8,2) = -C.c56;

	ThirdDegreePolynomial tdp (rho, C, 0);
	double roots[3];
	tdp.getRoots(roots);
	MatrixInverter matInv;
	
	if ( ! tdp.isMultiple() ) {
		// Search eigenvalues and filling the diagonal matrix
		L(0,0) = sqrt(roots[0]);
		L(1,1) = -L(0,0);
		L(2,2) = sqrt(roots[1]);
		L(3,3) = -L(2,2);
		L(4,4) = sqrt(roots[2]);
		L(5,5) = -L(4,4);

		// Search eigenvectors and filling the transition matrix
		// (  A = U1 * L * U  and so eigenvectors are columns of the U1  )
		double eigenVec[9];
		for (int i = 0; i < 6; i++) {
			findEigenVec(eigenVec, sqrt(roots[i/2]) * ( (i%2) ? -1 : 1 ), rho, C, 0);
			U1.setColumn(eigenVec, i);
			matInv.setColumn(eigenVec, i);
		}
	} else {
		// Search eigenvalues and filling the diagonal matrix
		// We hope L is diagonizable
		L(0, 0) = sqrt(roots[0]);
		L(1, 1) = -L(0, 0);
		L(2, 2) = L(3, 3) = sqrt(roots[1]);
		L(4, 4) = L(5, 5) = - sqrt(roots[1]);
		// Search eigenvectors and filling the transition matrix
		// (  A = U1 * L * U  and so eigenvectors are columns of the U1  )
		double eigenVec1[9];
		double eigenVec2[9];
		for (int i = 0; i < 2; i++) {
			findEigenVec(eigenVec1, sqrt(roots[0]) * ( (i%2) ? -1 : 1 ), rho, C, 0);
			U1.setColumn(eigenVec1, i);
			matInv.setColumn(eigenVec1, i);
		}
		for (int i = 2; i < 5; i+=2) {
			findEigenVec(eigenVec1, eigenVec2, sqrt(roots[1]) * ( ((i/2)%2) ? 1 : -1 ), rho, C, 0);
			U1.setColumn(eigenVec1, i);
			matInv.setColumn(eigenVec1, i);
			U1.setColumn(eigenVec2, i+1);
			matInv.setColumn(eigenVec2, i+1);
		}
	}

	U1(4,6) = U1(5,7) = U1(6,8) = 1;
	matInv.setUnity(4,6, 5,7, 6,8);

	// Search U = U1^(-1)
	matInv.inv(U);
	
	fixValuesOrder();
};

void gcm::AnisotropicMatrix3DAnalytical::createAy(const ICalcNode& node)
{
    clear();

#ifdef NDEBUG
    auto mat = static_cast<IAnisotropicElasticMaterial*> (node.getMaterial());
#else
    auto mat = dynamic_cast<IAnisotropicElasticMaterial*> (node.getMaterial());
    assert(mat);
#endif

	auto rho = node.getRho();
	auto C = mat->getParameters();

	// Setting values of A
	A(0,8) = A(1,4) = A(2,6) = -1.0/rho;

	A(3,0) = -C.c16;	A(3,1) = -C.c12;	A(3,2) = -C.c14;
	A(4,0) = -C.c26;	A(4,1) = -C.c22;	A(4,2) = -C.c24;
	A(5,0) = -C.c36;	A(5,1) = -C.c23;	A(5,2) = -C.c34;
	A(6,0) = -C.c46;	A(6,1) = -C.c24;	A(6,2) = -C.c44;
	A(7,0) = -C.c56;	A(7,1) = -C.c25;	A(7,2) = -C.c45;
	A(8,0) = -C.c66;	A(8,1) = -C.c26;	A(8,2) = -C.c46;

	ThirdDegreePolynomial tdp (rho, C, 1);
	double roots[3];
	tdp.getRoots(roots);
	MatrixInverter matInv;
	
	if ( ! tdp.isMultiple() ) {
		// Search eigenvalues and filling the diagonal matrix
		L(0,0) = sqrt(roots[0]);
		L(1,1) = -L(0,0);
		L(2,2) = sqrt(roots[1]);
		L(3,3) = -L(2,2);
		L(4,4) = sqrt(roots[2]);
		L(5,5) = -L(4,4);
		// Search eigenvectors and filling the transition matrix
		// (  A = U1 * L * U  and so eigenvectors are columns of the U1  )
		double eigenVec[9];
		for (int i = 0; i < 6; i++) {
			findEigenVec (eigenVec, sqrt(roots[i/2]) * ( (i%2) ? -1 : 1 ), rho, C, 1);
			U1.setColumn(eigenVec, i);
			matInv.setColumn(eigenVec, i);
		}
	} else {
		// Search eigenvalues and filling the diagonal matrix
		// We hope L is diagonizable
		L(0, 0) = sqrt(roots[0]);
		L(1, 1) = -L(0, 0);
		L(2, 2) = L(3, 3) = sqrt(roots[1]);
		L(4, 4) = L(5, 5) = - sqrt(roots[1]);
		// Search eigenvectors and filling the transition matrix
		// (  A = U1 * L * U  and so eigenvectors are columns of the U1  )
		double eigenVec1[9];
		double eigenVec2[9];
		for (int i = 0; i < 2; i++) {
			findEigenVec(eigenVec1, sqrt(roots[0]) * ( (i%2) ? -1 : 1 ), rho, C, 1);
			U1.setColumn(eigenVec1, i);
			matInv.setColumn(eigenVec1, i);
		}
		for (int i = 2; i < 5; i+=2) {
			findEigenVec(eigenVec1, eigenVec2, sqrt(roots[1]) * ( ((i/2)%2) ? 1 : -1 ), rho, C, 1);
			U1.setColumn(eigenVec1, i);
			matInv.setColumn(eigenVec1, i);
			U1.setColumn(eigenVec2, i+1);
			matInv.setColumn(eigenVec2, i+1);
		}
	}

	U1(3, 6) = U1(5, 7) = U1(7, 8) = 1;
	matInv.setUnity(3,6, 5,7, 7,8);

	// Search U = U1^(-1)
	matInv.inv(U);

	fixValuesOrder();
};

void gcm::AnisotropicMatrix3DAnalytical::createAz(const ICalcNode& node)
{
    clear();

#ifdef NDEBUG
    auto mat = static_cast<IAnisotropicElasticMaterial*> (node.getMaterial());
#else
    auto mat = dynamic_cast<IAnisotropicElasticMaterial*> (node.getMaterial());
    assert(mat);
#endif

	auto rho = node.getRho();
	auto C = mat->getParameters();

	// Setting values of A
	A(0,7) = A(1,6) = A(2,5) = -1.0/rho;

	A(3,0) = -C.c15;	A(3,1) = -C.c14;	A(3,2) = -C.c13;
	A(4,0) = -C.c25;	A(4,1) = -C.c24;	A(4,2) = -C.c23;
	A(5,0) = -C.c35;	A(5,1) = -C.c34;	A(5,2) = -C.c33;
	A(6,0) = -C.c45;	A(6,1) = -C.c44;	A(6,2) = -C.c34;
	A(7,0) = -C.c55;	A(7,1) = -C.c45;	A(7,2) = -C.c35;
	A(8,0) = -C.c56;	A(8,1) = -C.c46;	A(8,2) = -C.c36;

	ThirdDegreePolynomial tdp (rho, C, 2);
	double roots[3];
	tdp.getRoots(roots);
	MatrixInverter matInv;
	
	if ( ! tdp.isMultiple() ) {
		// Search eigenvalues and filling the diagonal matrix
		L(0,0) = sqrt(roots[0]);
		L(1,1) = -L(0,0);
		L(2,2) = sqrt(roots[1]);
		L(3,3) = -L(2,2);
		L(4,4) = sqrt(roots[2]);
		L(5,5) = -L(4,4);

		// Search eigenvectors and filling the transition matrix
		// (  A = U1 * L * U  and so eigenvectors are columns of the U1  )
		double eigenVec[9];
		for (int i = 0; i < 6; i++) {
			findEigenVec(eigenVec, sqrt(roots[i/2]) * ( (i%2) ? -1 : 1 ), rho, C, 2);
			U1.setColumn(eigenVec, i);
			matInv.setColumn(eigenVec, i);
		}
	} else {
		// Search eigenvalues and filling the diagonal matrix
		// We hope L is diagonizable
		L(0, 0) = sqrt(roots[0]);
		L(1, 1) = -L(0, 0);
		L(2, 2) = L(3, 3) = sqrt(roots[1]);
		L(4, 4) = L(5, 5) = - sqrt(roots[1]);
		// Search eigenvectors and filling the transition matrix
		// (  A = U1 * L * U  and so eigenvectors are columns of the U1  )
		double eigenVec1[9];
		double eigenVec2[9];
		for (int i = 0; i < 2; i++) {
			findEigenVec(eigenVec1, sqrt(roots[0]) * ( (i%2) ? -1 : 1 ), rho, C, 2);
			U1.setColumn(eigenVec1, i);
			matInv.setColumn(eigenVec1, i);
		}
		for (int i = 2; i < 5; i+=2) {
			findEigenVec(eigenVec1, eigenVec2, sqrt(roots[1]) * ( ((i/2)%2) ? 1 : -1 ), rho, C, 2);
			U1.setColumn(eigenVec1, i);
			matInv.setColumn(eigenVec1, i);
			U1.setColumn(eigenVec2, i+1);
			matInv.setColumn(eigenVec2, i+1);
		}
	}

	U1(3,6) = U1(4,7) = U1(8,8) = 1;
	matInv.setUnity(3,6, 4,7, 8,8);

	// Search U = U1^(-1)
	matInv.inv(U);

	fixValuesOrder();
};

void gcm::AnisotropicMatrix3DAnalytical::findNonZeroSolution(double **M, double *x)
{
    // Range ( M ) = 2, one of x[i] is random

    int I = 0, J = 1, P = 0, Q = 1;
    double det = 0;
    for(int i = 0; i < 2; i++)
    for(int j = i+1; j < 3; j++)
            for(int p = 0; p < 2; p++)
            for(int q = p+1; q < 3; q++)
                if ( abs (M[p][i]*M[q][j] - M[q][i]*M[p][j]) > abs (det) ) {
                    det = M[p][i]*M[q][j] - M[q][i]*M[p][j];
                    I = i; J = j; P = p; Q = q;
                }

    // unity = no I and no J
    int unity = ! I;
    unity += (unity == J) ? 1 : 0;

    x[unity] = 1;
    x[I] = (-M[P][unity]*M[Q][J] + M[Q][unity]*M[P][J]) / det;
    x[J] = (-M[P][I]*M[Q][unity] + M[Q][I]*M[P][unity]) / det;
};

void gcm::AnisotropicMatrix3DAnalytical::findEigenVec(double *eigenVec,
					double l, float rho, const IAnisotropicElasticMaterial::RheologyParameters &C, int stage)
{
    // Analitycal search eigenvectors
    // M * x = 0, x = (x1, x2, x3)
    // then x[4-9] <= x[1-3]
    double** M = new double* [3];
    for (int i = 0; i < 3; i++)
        M[i] = new double [3];

    switch ( stage ) {
        case 0 :
        {
            M[0][0] = -C.c11 + rho*l*l;
            M[1][2] = -C.c55 + rho*l*l;
            M[2][1] = -C.c66 + rho*l*l;
            M[0][1] = M[2][0] = -C.c16;
            M[0][2] = M[1][0] = -C.c15;
            M[1][1] = M[2][2] = -C.c56;
            break;
        }
        case 1 :
        {
            M[0][1] = -C.c22 + rho*l*l;
            M[1][2] = -C.c44 + rho*l*l;
            M[2][0] = -C.c66 + rho*l*l;
            M[0][0] = M[2][1] = -C.c26;
            M[1][0] = M[2][2] = -C.c46;
            M[1][1] = M[0][2] = -C.c24;
            break;
        }
        case 2 :
        {
            M[0][2] = -C.c33 + rho*l*l;
            M[1][1] = -C.c44 + rho*l*l;
            M[2][0] = -C.c55 + rho*l*l;
            M[0][0] = M[2][2] = -C.c35;
            M[2][1] = M[1][0] = -C.c45;
            M[0][1] = M[1][2] = -C.c34;
            break;
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
            break;
        }
        case 1 :
        {
            eigenVec[4] = -rho*l*eigenVec[1];
            eigenVec[8] = -rho*l*eigenVec[0];
            eigenVec[6] = -rho*l*eigenVec[2];
            eigenVec[3] = -(C.c16*eigenVec[0] + C.c12*eigenVec[1] + C.c14*eigenVec[2])/l;
            eigenVec[5] = -(C.c36*eigenVec[0] + C.c23*eigenVec[1] + C.c34*eigenVec[2])/l;
            eigenVec[7] = -(C.c56*eigenVec[0] + C.c25*eigenVec[1] + C.c45*eigenVec[2])/l;
            break;
        }
        case 2 :
        {
            eigenVec[7] = -rho*l*eigenVec[0];
            eigenVec[6] = -rho*l*eigenVec[1];
            eigenVec[5] = -rho*l*eigenVec[2];
            eigenVec[3] = -(C.c15*eigenVec[0] + C.c14*eigenVec[1] + C.c13*eigenVec[2])/l;
            eigenVec[4] = -(C.c25*eigenVec[0] + C.c24*eigenVec[1] + C.c23*eigenVec[2])/l;
            eigenVec[8] = -(C.c56*eigenVec[0] + C.c46*eigenVec[1] + C.c36*eigenVec[2])/l;
            break;
        }
    }
};

void gcm::AnisotropicMatrix3DAnalytical::findNonZeroSolution(double **M, double *x, double *y)
{
    // Range ( M ) = 1, two of x[i] are random

    int I = 0, J = 0, p, q;
    double det = 0;
    for (int i = 0; i < 3; i++)
        for (int j = 0; j < 3; j++)
            if ( abs(det) < abs(M[i][j]) ) {
                det = M[i][j];
                I = i; J = j;
            }
    // p and q = no J
    if (J == 0) { p = 1; q = 2; }
    else if (J == 1) { p = 0; q = 2; }
    else { p = 0; q = 1; }

    x[p] = y[q] = 1;
    x[q] = y[p] = 0;
    x[J] = -M[I][p] / det;
    y[J] = -M[I][q] / det;
};


void gcm::AnisotropicMatrix3DAnalytical::findEigenVec (double *eigenVec1,
		double *eigenVec2, double l, float rho, const IAnisotropicElasticMaterial::RheologyParameters &C, int stage)
{
    // Analitycal search eigenvectors
    // M * x = 0, x = (x1, x2, x3)
    // then x[4-9] <= x[1-3]
    double** M = new double* [3];
    for (int i = 0; i < 3; i++)
        M[i] = new double [3];

    switch ( stage ) {
        case 0 :
        {
            M[0][0] = -C.c11 + rho*l*l;
            M[1][2] = -C.c55 + rho*l*l;
            M[2][1] = -C.c66 + rho*l*l;
            M[0][1] = M[2][0] = -C.c16;
            M[0][2] = M[1][0] = -C.c15;
            M[1][1] = M[2][2] = -C.c56;
            break;
        }
        case 1 :
        {
            M[0][1] = -C.c22 + rho*l*l;
            M[1][2] = -C.c44 + rho*l*l;
            M[2][0] = -C.c66 + rho*l*l;
            M[0][0] = M[2][1] = -C.c26;
            M[1][0] = M[2][2] = -C.c46;
            M[1][1] = M[0][2] = -C.c24;
            break;
        }
        case 2 :
        {
            M[0][2] = -C.c33 + rho*l*l;
            M[1][1] = -C.c44 + rho*l*l;
            M[2][0] = -C.c55 + rho*l*l;
            M[0][0] = M[2][2] = -C.c35;
            M[2][1] = M[1][0] = -C.c45;
            M[0][1] = M[1][2] = -C.c34;
            break;
        }
        default:
            THROW_INVALID_ARG("Wrong stage number (from findEigenVec)");
    }

    findNonZeroSolution(M, eigenVec1, eigenVec2);

    switch ( stage ) {
        case 0 :
        {
            eigenVec1[3] = -rho*l*eigenVec1[0];
            eigenVec1[8] = -rho*l*eigenVec1[1];
            eigenVec1[7] = -rho*l*eigenVec1[2];
            eigenVec1[4] = -(C.c12*eigenVec1[0] + C.c26*eigenVec1[1] + C.c25*eigenVec1[2])/l;
            eigenVec1[5] = -(C.c13*eigenVec1[0] + C.c36*eigenVec1[1] + C.c35*eigenVec1[2])/l;
            eigenVec1[6] = -(C.c14*eigenVec1[0] + C.c46*eigenVec1[1] + C.c45*eigenVec1[2])/l;
            eigenVec2[3] = -rho*l*eigenVec2[0];
            eigenVec2[8] = -rho*l*eigenVec2[1];
            eigenVec2[7] = -rho*l*eigenVec2[2];
            eigenVec2[4] = -(C.c12*eigenVec2[0] + C.c26*eigenVec2[1] + C.c25*eigenVec2[2])/l;
            eigenVec2[5] = -(C.c13*eigenVec2[0] + C.c36*eigenVec2[1] + C.c35*eigenVec2[2])/l;
            eigenVec2[6] = -(C.c14*eigenVec2[0] + C.c46*eigenVec2[1] + C.c45*eigenVec2[2])/l;
            break;
        }
        case 1 :
        {
            eigenVec1[4] = -rho*l*eigenVec1[1];
            eigenVec1[8] = -rho*l*eigenVec1[0];
            eigenVec1[6] = -rho*l*eigenVec1[2];
            eigenVec1[3] = -(C.c16*eigenVec1[0] + C.c12*eigenVec1[1] + C.c14*eigenVec1[2])/l;
            eigenVec1[5] = -(C.c36*eigenVec1[0] + C.c23*eigenVec1[1] + C.c34*eigenVec1[2])/l;
            eigenVec1[7] = -(C.c56*eigenVec1[0] + C.c25*eigenVec1[1] + C.c45*eigenVec1[2])/l;
            eigenVec2[4] = -rho*l*eigenVec2[1];
            eigenVec2[8] = -rho*l*eigenVec2[0];
            eigenVec2[6] = -rho*l*eigenVec2[2];
            eigenVec2[3] = -(C.c16*eigenVec2[0] + C.c12*eigenVec2[1] + C.c14*eigenVec2[2])/l;
            eigenVec2[5] = -(C.c36*eigenVec2[0] + C.c23*eigenVec2[1] + C.c34*eigenVec2[2])/l;
            eigenVec2[7] = -(C.c56*eigenVec2[0] + C.c25*eigenVec2[1] + C.c45*eigenVec2[2])/l;
            break;
        }
        case 2 :
        {
            eigenVec1[7] = -rho*l*eigenVec1[0];
            eigenVec1[6] = -rho*l*eigenVec1[1];
            eigenVec1[5] = -rho*l*eigenVec1[2];
            eigenVec1[3] = -(C.c15*eigenVec1[0] + C.c14*eigenVec1[1] + C.c13*eigenVec1[2])/l;
            eigenVec1[4] = -(C.c25*eigenVec1[0] + C.c24*eigenVec1[1] + C.c23*eigenVec1[2])/l;
            eigenVec1[8] = -(C.c56*eigenVec1[0] + C.c46*eigenVec1[1] + C.c36*eigenVec1[2])/l;
            eigenVec2[7] = -rho*l*eigenVec2[0];
            eigenVec2[6] = -rho*l*eigenVec2[1];
            eigenVec2[5] = -rho*l*eigenVec2[2];
            eigenVec2[3] = -(C.c15*eigenVec2[0] + C.c14*eigenVec2[1] + C.c13*eigenVec2[2])/l;
            eigenVec2[4] = -(C.c25*eigenVec2[0] + C.c24*eigenVec2[1] + C.c23*eigenVec2[2])/l;
            eigenVec2[8] = -(C.c56*eigenVec2[0] + C.c46*eigenVec2[1] + C.c36*eigenVec2[2])/l;
            break;
        }
    }
};