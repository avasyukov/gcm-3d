#include "AnalyticalRheologyMatrixDecomposer.hpp"

gcm::AnalyticalRheologyMatrixDecomposer::AnalyticalRheologyMatrixDecomposer() {
}

gcm::AnalyticalRheologyMatrixDecomposer::~AnalyticalRheologyMatrixDecomposer() {
}

void gcm::AnalyticalRheologyMatrixDecomposer::decomposeX(const gcm_matrix& a, 
							gcm_matrix& u, gcm_matrix& l, gcm_matrix& u1) const
{
	l.clear();
    u.clear();
    u1.clear();
	
	ThirdDegreePolynomial tdp (a, 0);
	double roots[3];
	tdp.getRoots(roots);
	MatrixInverter matInv;
	
	if ( ! tdp.isMultiple() ) {
		// Search eigenvalues and filling the diagonal matrix
		l(0,0) = sqrt(roots[0]);
		l(1,1) = -l(0,0);
		l(2,2) = sqrt(roots[1]);
		l(3,3) = -l(2,2);
		l(4,4) = sqrt(roots[2]);
		l(5,5) = -l(4,4);

		// Search eigenvectors and filling the transition matrix
		// (  A = U1 * L * U  and so eigenvectors are columns of the U1  )
		double eigenVec[9];
		for (int i = 0; i < 6; i++) {
			findEigenVec(eigenVec, sqrt(roots[i/2]) * ( (i%2) ? -1 : 1 ), a, 0);
			u1.setColumn(eigenVec, i);
			matInv.setColumn(eigenVec, i);
		}
	} else {
		// Search eigenvalues and filling the diagonal matrix
		// We hope L is diagonizable
		l(0, 0) = sqrt(roots[0]);
		l(1, 1) = -l(0, 0);
		l(2, 2) = l(3, 3) = sqrt(roots[1]);
		l(4, 4) = l(5, 5) = - sqrt(roots[1]);
		// Search eigenvectors and filling the transition matrix
		// (  A = U1 * L * U  and so eigenvectors are columns of the U1  )
		double eigenVec1[9];
		double eigenVec2[9];
		for (int i = 0; i < 2; i++) {
			findEigenVec(eigenVec1, sqrt(roots[0]) * ( (i%2) ? -1 : 1 ), a, 0);
			u1.setColumn(eigenVec1, i);
			matInv.setColumn(eigenVec1, i);
		}
		for (int i = 2; i < 5; i+=2) {
			findEigenVec(eigenVec1, eigenVec2, sqrt(roots[1]) * ( ((i/2)%2) ? 1 : -1 ), a, 0);
			u1.setColumn(eigenVec1, i);
			matInv.setColumn(eigenVec1, i);
			u1.setColumn(eigenVec2, i+1);
			matInv.setColumn(eigenVec2, i+1);
		}
	}

	u1(6,6) = u1(7,7) = u1(8,8) = 1;
	matInv.setUnity(6,6, 7,7, 8,8);

	// Search U = U1^(-1)
	matInv.inv(u);
};

void gcm::AnalyticalRheologyMatrixDecomposer::decomposeY(const gcm_matrix& a,
							gcm_matrix& u, gcm_matrix& l, gcm_matrix& u1) const
{
	l.clear();
    u.clear();
    u1.clear();
	
	ThirdDegreePolynomial tdp (a, 1);
	double roots[3];
	tdp.getRoots(roots);
	MatrixInverter matInv;
	
	if ( ! tdp.isMultiple() ) {
		// Search eigenvalues and filling the diagonal matrix
		l(0,0) = sqrt(roots[0]);
		l(1,1) = -l(0,0);
		l(2,2) = sqrt(roots[1]);
		l(3,3) = -l(2,2);
		l(4,4) = sqrt(roots[2]);
		l(5,5) = -l(4,4);
		// Search eigenvectors and filling the transition matrix
		// (  A = U1 * L * U  and so eigenvectors are columns of the U1  )
		double eigenVec[9];
		for (int i = 0; i < 6; i++) {
			findEigenVec (eigenVec, sqrt(roots[i/2]) * ( (i%2) ? -1 : 1 ), a, 1);
			u1.setColumn(eigenVec, i);
			matInv.setColumn(eigenVec, i);
		}
	} else {
		// Search eigenvalues and filling the diagonal matrix
		// We hope L is diagonizable
		l(0, 0) = sqrt(roots[0]);
		l(1, 1) = -l(0, 0);
		l(2, 2) = l(3, 3) = sqrt(roots[1]);
		l(4, 4) = l(5, 5) = - sqrt(roots[1]);
		// Search eigenvectors and filling the transition matrix
		// (  A = U1 * L * U  and so eigenvectors are columns of the U1  )
		double eigenVec1[9];
		double eigenVec2[9];
		for (int i = 0; i < 2; i++) {
			findEigenVec(eigenVec1, sqrt(roots[0]) * ( (i%2) ? -1 : 1 ), a, 1);
			u1.setColumn(eigenVec1, i);
			matInv.setColumn(eigenVec1, i);
		}
		for (int i = 2; i < 5; i+=2) {
			findEigenVec(eigenVec1, eigenVec2, sqrt(roots[1]) * ( ((i/2)%2) ? 1 : -1 ), a, 1);
			u1.setColumn(eigenVec1, i);
			matInv.setColumn(eigenVec1, i);
			u1.setColumn(eigenVec2, i+1);
			matInv.setColumn(eigenVec2, i+1);
		}
	}

	u1(3, 6) = u1(5, 7) = u1(8, 8) = 1;
	matInv.setUnity(3,6, 5,7, 8,8);

	// Search U = U1^(-1)
	matInv.inv(u);
};

void gcm::AnalyticalRheologyMatrixDecomposer::decomposeZ(const gcm_matrix& a,
							gcm_matrix& u, gcm_matrix& l, gcm_matrix& u1) const
{
	l.clear();
    u.clear();
    u1.clear();
	
	ThirdDegreePolynomial tdp (a, 2);
	double roots[3];
	tdp.getRoots(roots);
	MatrixInverter matInv;
	
	if ( ! tdp.isMultiple() ) {
		// Search eigenvalues and filling the diagonal matrix
		l(0,0) = sqrt(roots[0]);
		l(1,1) = -l(0,0);
		l(2,2) = sqrt(roots[1]);
		l(3,3) = -l(2,2);
		l(4,4) = sqrt(roots[2]);
		l(5,5) = -l(4,4);

		// Search eigenvectors and filling the transition matrix
		// (  A = U1 * L * U  and so eigenvectors are columns of the U1  )
		double eigenVec[9];
		for (int i = 0; i < 6; i++) {
			findEigenVec(eigenVec, sqrt(roots[i/2]) * ( (i%2) ? -1 : 1 ), a, 2);
			u1.setColumn(eigenVec, i);
			matInv.setColumn(eigenVec, i);
		}
	} else {
		// Search eigenvalues and filling the diagonal matrix
		// We hope L is diagonizable
		l(0, 0) = sqrt(roots[0]);
		l(1, 1) = -l(0, 0);
		l(2, 2) = l(3, 3) = sqrt(roots[1]);
		l(4, 4) = l(5, 5) = - sqrt(roots[1]);
		// Search eigenvectors and filling the transition matrix
		// (  a = U1 * L * U  and so eigenvectors are columns of the U1  )
		double eigenVec1[9];
		double eigenVec2[9];
		for (int i = 0; i < 2; i++) {
			findEigenVec(eigenVec1, sqrt(roots[0]) * ( (i%2) ? -1 : 1 ), a, 2);
			u1.setColumn(eigenVec1, i);
			matInv.setColumn(eigenVec1, i);
		}
		for (int i = 2; i < 5; i+=2) {
			findEigenVec(eigenVec1, eigenVec2, sqrt(roots[1]) * ( ((i/2)%2) ? 1 : -1 ), a, 2);
			u1.setColumn(eigenVec1, i);
			matInv.setColumn(eigenVec1, i);
			u1.setColumn(eigenVec2, i+1);
			matInv.setColumn(eigenVec2, i+1);
		}
	}

	u1(3,6) = u1(4,7) = u1(6,8) = 1;
	matInv.setUnity(3,6, 4,7, 6,8);

	// Search U = U1^(-1)
	matInv.inv(u);
};

void gcm::AnalyticalRheologyMatrixDecomposer::findNonZeroSolution
										(double **M, double *x) const
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

void gcm::AnalyticalRheologyMatrixDecomposer::findEigenVec(double *eigenVec,
								double l, const gcm_matrix &A, int stage) const
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
			double r = A.get(0,3);
            M[0][0] = A.get(3,0) - l*l/r;	M[0][1] = A.get(3,1);	M[0][2] = A.get(3,2);
            M[1][0] = A.get(4,0);	M[1][1] = A.get(4,1) - l*l/r;	M[1][2] = A.get(4,2);
			M[2][0] = A.get(5,0);	M[2][1] = A.get(5,1);	M[2][2] = A.get(5,2) - l*l/r;
            break;
        }
        case 1 :
        {
			double r = A.get(0,4);
            M[0][0] = A.get(4,0) - l*l/r;	M[0][1] = A.get(4,1);	M[0][2] = A.get(4,2);
            M[1][0] = A.get(6,0);	M[1][1] = A.get(6,1) - l*l/r;	M[1][2] = A.get(6,2);
			M[2][0] = A.get(7,0);	M[2][1] = A.get(7,1);	M[2][2] = A.get(7,2) - l*l/r;
            break;
        }
        case 2 :
        {
			double r = A.get(0,5);
            M[0][0] = A.get(5,0) - l*l/r;	M[0][1] = A.get(5,1);	M[0][2] = A.get(5,2);
            M[1][0] = A.get(7,0);	M[1][1] = A.get(7,1) - l*l/r;	M[1][2] = A.get(7,2);
			M[2][0] = A.get(8,0);	M[2][1] = A.get(8,1);	M[2][2] = A.get(8,2) - l*l/r;
            break;
        }
        default:
            THROW_INVALID_ARG("Wrong stage number (from findEigenVec)");
    }

    findNonZeroSolution(M, eigenVec);

    switch ( stage ) {
        case 0 :
        {
			double r = A.get(0,3);
            eigenVec[3] = l/r*eigenVec[0];
            eigenVec[4] = l/r*eigenVec[1];
            eigenVec[5] = l/r*eigenVec[2];
            eigenVec[6] = (A.get(6,0)*eigenVec[0] + A.get(6,1)*eigenVec[1] + A.get(6,2)*eigenVec[2])/l;
            eigenVec[7] = (A.get(7,0)*eigenVec[0] + A.get(7,1)*eigenVec[1] + A.get(7,2)*eigenVec[2])/l;
            eigenVec[8] = (A.get(8,0)*eigenVec[0] + A.get(8,1)*eigenVec[1] + A.get(8,2)*eigenVec[2])/l;
            break;
        }
        case 1 :
        {
			double r = A.get(0,4);
            eigenVec[4] = l/r*eigenVec[0];
            eigenVec[6] = l/r*eigenVec[1];
            eigenVec[7] = l/r*eigenVec[2];
            eigenVec[3] = (A.get(3,0)*eigenVec[0] + A.get(3,1)*eigenVec[1] + A.get(3,2)*eigenVec[2])/l;
            eigenVec[5] = (A.get(5,0)*eigenVec[0] + A.get(5,1)*eigenVec[1] + A.get(5,2)*eigenVec[2])/l;
            eigenVec[8] = (A.get(8,0)*eigenVec[0] + A.get(8,1)*eigenVec[1] + A.get(8,2)*eigenVec[2])/l;
            break;
        }
        case 2 :
        {
			double r = A.get(0,5);
            eigenVec[5] = l/r*eigenVec[0];
            eigenVec[7] = l/r*eigenVec[1];
            eigenVec[8] = l/r*eigenVec[2];
            eigenVec[6] = (A.get(6,0)*eigenVec[0] + A.get(6,1)*eigenVec[1] + A.get(6,2)*eigenVec[2])/l;
            eigenVec[3] = (A.get(3,0)*eigenVec[0] + A.get(3,1)*eigenVec[1] + A.get(3,2)*eigenVec[2])/l;
            eigenVec[4] = (A.get(4,0)*eigenVec[0] + A.get(4,1)*eigenVec[1] + A.get(4,2)*eigenVec[2])/l;
            break;
        }
    }
    
    for (int i = 0; i < 3; i++)
        delete M[i];
    delete M;
};

void gcm::AnalyticalRheologyMatrixDecomposer::findNonZeroSolution
								(double **M, double *x, double *y) const
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


void gcm::AnalyticalRheologyMatrixDecomposer::findEigenVec (double *eigenVec1,
			double *eigenVec2, double l, const gcm_matrix &A, int stage) const
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
			double r = A.get(0,3);
            M[0][0] = A.get(3,0) - l*l/r;	M[0][1] = A.get(3,1);	M[0][2] = A.get(3,2);
            M[1][0] = A.get(4,0);	M[1][1] = A.get(4,1) - l*l/r;	M[1][2] = A.get(4,2);
			M[2][0] = A.get(5,0);	M[2][1] = A.get(5,1);	M[2][2] = A.get(5,2) - l*l/r;
            break;
        }
        case 1 :
        {
			double r = A.get(0,4);
            M[0][0] = A.get(4,0) - l*l/r;	M[0][1] = A.get(4,1);	M[0][2] = A.get(4,2);
            M[1][0] = A.get(6,0);	M[1][1] = A.get(6,1) - l*l/r;	M[1][2] = A.get(6,2);
			M[2][0] = A.get(7,0);	M[2][1] = A.get(7,1);	M[2][2] = A.get(7,2) - l*l/r;
            break;
        }
        case 2 :
        {
			double r = A.get(0,5);
            M[0][0] = A.get(5,0) - l*l/r;	M[0][1] = A.get(5,1);	M[0][2] = A.get(5,2);
            M[1][0] = A.get(7,0);	M[1][1] = A.get(7,1) - l*l/r;	M[1][2] = A.get(7,2);
			M[2][0] = A.get(8,0);	M[2][1] = A.get(8,1);	M[2][2] = A.get(8,2) - l*l/r;
            break;
        }
        default:
            THROW_INVALID_ARG("Wrong stage number (from findEigenVec)");
    }

    findNonZeroSolution(M, eigenVec1, eigenVec2);

    switch ( stage ) {
        case 0 :
        {
			double r = A.get(0,3);
            eigenVec1[3] = l/r*eigenVec1[0];
            eigenVec1[4] = l/r*eigenVec1[1];
            eigenVec1[5] = l/r*eigenVec1[2];
            eigenVec1[6] = (A.get(6,0)*eigenVec1[0] + A.get(6,1)*eigenVec1[1] + A.get(6,2)*eigenVec1[2])/l;
            eigenVec1[7] = (A.get(7,0)*eigenVec1[0] + A.get(7,1)*eigenVec1[1] + A.get(7,2)*eigenVec1[2])/l;
            eigenVec1[8] = (A.get(8,0)*eigenVec1[0] + A.get(8,1)*eigenVec1[1] + A.get(8,2)*eigenVec1[2])/l;

            eigenVec2[3] = l/r*eigenVec2[0];
            eigenVec2[4] = l/r*eigenVec2[1];
            eigenVec2[5] = l/r*eigenVec2[2];
            eigenVec2[6] = (A.get(6,0)*eigenVec2[0] + A.get(6,1)*eigenVec2[1] + A.get(6,2)*eigenVec2[2])/l;
            eigenVec2[7] = (A.get(7,0)*eigenVec2[0] + A.get(7,1)*eigenVec2[1] + A.get(7,2)*eigenVec2[2])/l;
            eigenVec2[8] = (A.get(8,0)*eigenVec2[0] + A.get(8,1)*eigenVec2[1] + A.get(8,2)*eigenVec2[2])/l;          
			break;
        }
        case 1 :
        {
			double r = A.get(0,4);
            eigenVec1[4] = l/r*eigenVec1[0];
            eigenVec1[6] = l/r*eigenVec1[1];
            eigenVec1[7] = l/r*eigenVec1[2];
            eigenVec1[3] = (A.get(3,0)*eigenVec1[0] + A.get(3,1)*eigenVec1[1] + A.get(3,2)*eigenVec1[2])/l;
            eigenVec1[5] = (A.get(5,0)*eigenVec1[0] + A.get(5,1)*eigenVec1[1] + A.get(5,2)*eigenVec1[2])/l;
            eigenVec1[8] = (A.get(8,0)*eigenVec1[0] + A.get(8,1)*eigenVec1[1] + A.get(8,2)*eigenVec1[2])/l;
   
			eigenVec2[4] = l/r*eigenVec2[0];
            eigenVec2[6] = l/r*eigenVec2[1];
            eigenVec2[7] = l/r*eigenVec2[2];
            eigenVec2[3] = (A.get(3,0)*eigenVec2[0] + A.get(3,1)*eigenVec2[1] + A.get(3,2)*eigenVec2[2])/l;
            eigenVec2[5] = (A.get(5,0)*eigenVec2[0] + A.get(5,1)*eigenVec2[1] + A.get(5,2)*eigenVec2[2])/l;
            eigenVec2[8] = (A.get(8,0)*eigenVec2[0] + A.get(8,1)*eigenVec2[1] + A.get(8,2)*eigenVec2[2])/l;
			break;
        }
        case 2 :
        {
			double r = A.get(0,5);
            eigenVec1[5] = l/r*eigenVec1[0];
            eigenVec1[7] = l/r*eigenVec1[1];
            eigenVec1[8] = l/r*eigenVec1[2];
            eigenVec1[6] = (A.get(6,0)*eigenVec1[0] + A.get(6,1)*eigenVec1[1] + A.get(6,2)*eigenVec1[2])/l;
            eigenVec1[3] = (A.get(3,0)*eigenVec1[0] + A.get(3,1)*eigenVec1[1] + A.get(3,2)*eigenVec1[2])/l;
            eigenVec1[4] = (A.get(4,0)*eigenVec1[0] + A.get(4,1)*eigenVec1[1] + A.get(4,2)*eigenVec1[2])/l;

            eigenVec2[5] = l/r*eigenVec2[0];
            eigenVec2[7] = l/r*eigenVec2[1];
            eigenVec2[8] = l/r*eigenVec2[2];
            eigenVec2[6] = (A.get(6,0)*eigenVec2[0] + A.get(6,1)*eigenVec2[1] + A.get(6,2)*eigenVec2[2])/l;
            eigenVec2[3] = (A.get(3,0)*eigenVec2[0] + A.get(3,1)*eigenVec2[1] + A.get(3,2)*eigenVec2[2])/l;
            eigenVec2[4] = (A.get(4,0)*eigenVec2[0] + A.get(4,1)*eigenVec2[1] + A.get(4,2)*eigenVec2[2])/l;
            break;
        }
    }
    
    for (int i = 0; i < 3; i++)
        delete M[i];
    delete M;
};
