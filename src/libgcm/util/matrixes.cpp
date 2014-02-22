#include "util/matrixes.h"

gcm::gcm_matrix::gcm_matrix() { };
gcm::gcm_matrix::~gcm_matrix() { };

inline gcm::gcm_matrix &gcm::gcm_matrix::operator=(const gcm_matrix &A)
{
	for(int i = 0; i < GCM_MATRIX_SIZE; i++)
		for(int j = 0; j < GCM_MATRIX_SIZE; j++)
			p[i][j] = A.p[i][j];
	return *this;
};

float &gcm::gcm_matrix::operator()(int i, int j)
{
	return p[i][j];
};

void gcm::gcm_matrix::createE()
{
	for(int i = 0; i < GCM_MATRIX_SIZE; i++)
		for(int j = 0; j < GCM_MATRIX_SIZE; j++)
			if(i == j)
				p[i][j] = 1;
			else
				p[i][j] = 0;
};

bool gcm::gcm_matrix::operator==(const gcm_matrix &A) const
{
	float max1 = max_abs_value();
	float max2 = A.max_abs_value();

	float max = (max1 > max2) ? max1 : max2;

	for (int i = 0; i < GCM_MATRIX_SIZE; ++i)
		for (int j = 0; j < GCM_MATRIX_SIZE; ++j)
			if(fabs(p[i][j] - A.p[i][j]) > EQUALITY_TOLERANCE * max)
				return false;
	return true;
};

bool gcm::gcm_matrix::operator!=(const gcm_matrix &A) const
{
	return !(*this == A);
};

gcm::gcm_matrix gcm::gcm_matrix::operator*(const gcm_matrix &A) const
{
	gcm::gcm_matrix res_matrix;
	for (int r = 0; r < GCM_MATRIX_SIZE; r++) {
		for (int c = 0; c < GCM_MATRIX_SIZE; c++) {
			res_matrix.p[r][c] = 0;
			for (int i = 0; i < GCM_MATRIX_SIZE; i++) {
				res_matrix.p[r][c] += this->p[r][i] * A.p[i][c];
			}
		}
	}
	return res_matrix;
};

gcm::gcm_matrix gcm::gcm_matrix::operator+(const gcm_matrix &A) const
{
        gcm::gcm_matrix res_matrix;
        for (int r = 0; r < GCM_MATRIX_SIZE; r++)
                for (int c = 0; c < GCM_MATRIX_SIZE; c++)
			res_matrix.p[r][c] = this->p[r][c] + A.p[r][c];
        return res_matrix;
};

gcm::gcm_matrix gcm::gcm_matrix::operator-(const gcm_matrix &A) const
{
        gcm_matrix res_matrix;
        for (int r = 0; r < GCM_MATRIX_SIZE; r++)
                for (int c = 0; c < GCM_MATRIX_SIZE; c++)
                        res_matrix.p[r][c] = this->p[r][c] - A.p[r][c];
        return res_matrix;
};

float gcm::gcm_matrix::max_abs_value() const
{
	float res = 0;
	for (int r = 0; r < GCM_MATRIX_SIZE; r++)
		for (int c = 0; c < GCM_MATRIX_SIZE; c++)
			if(fabs(p[r][c]) > res)
				res = fabs(p[r][c]);
	return res;
};

void gcm::gcm_matrix::clear()
{
	memset(p, 0, 81*sizeof(float));
};

void gcm::gcm_matrix::setColumn(float *Clmn, int num)
{
	for (int i = 0; i < GCM_MATRIX_SIZE; i++)
		p[i][num] = Clmn[i];
}

void gcm::gcm_matrix::inv()
{
	// Invert the matrix using gsl library
	
	gsl_set_error_handler_off();
	
	gsl_matrix* Z1 = gsl_matrix_alloc (GCM_MATRIX_SIZE, GCM_MATRIX_SIZE);
	gsl_matrix* Z = gsl_matrix_alloc (GCM_MATRIX_SIZE, GCM_MATRIX_SIZE);
    gsl_permutation* perm = gsl_permutation_alloc (GCM_MATRIX_SIZE);  
	int k;
	
	for (int i = 0; i < GCM_MATRIX_SIZE; i++)
		for (int j = 0; j < GCM_MATRIX_SIZE; j++)
			gsl_matrix_set(Z1, i, j, p[i][j]);
	
	int status = gsl_linalg_LU_decomp (Z1, perm, &k);
	if (status)
		THROW_INVALID_ARG("gsl_linalg_LU_decomp failed");
	status = gsl_linalg_LU_invert (Z1, perm, Z);
	if (status)
		THROW_INVALID_ARG("gsl_linalg_LU_invert failed");
	for (int i = 0; i < GCM_MATRIX_SIZE; i++)
		for (int j = 0; j < GCM_MATRIX_SIZE; j++)
			p[i][j] = gsl_matrix_get(Z, i, j);
}
