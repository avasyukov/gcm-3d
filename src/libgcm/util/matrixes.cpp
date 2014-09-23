#include "libgcm/util/matrixes.hpp"

using namespace gcm;

gcm_matrix::gcm_matrix()
{
    INIT_LOGGER("gcm.matrixes");
};

gcm_matrix::~gcm_matrix()
{
};

gcm_matrix& gcm_matrix::operator=(const gcm_matrix &A)
{
    for (int i = 0; i < GCM_MATRIX_SIZE; i++)
        for (int j = 0; j < GCM_MATRIX_SIZE; j++)
            p[i][j] = A.p[i][j];
    return *this;
};

float& gcm_matrix::operator()(int i, int j)
{
    return p[i][j];
};

void gcm_matrix::createE()
{
    for (int i = 0; i < GCM_MATRIX_SIZE; i++)
        for (int j = 0; j < GCM_MATRIX_SIZE; j++)
            p[i][j] = (i == j ? 1 : 0);
};

bool gcm_matrix::operator==(const gcm_matrix &A) const
{
    for (int i = 0; i < GCM_MATRIX_SIZE; ++i)
        for (int j = 0; j < GCM_MATRIX_SIZE; ++j)
            // If element is greater than chosen floating point epsilon ...
            if ( (fabs(p[i][j]) > EQUALITY_TOLERANCE || fabs(A.p[i][j]) > EQUALITY_TOLERANCE )
                        // .. and difference is greater than 1% ...
                        && fabs(p[i][j] - A.p[i][j]) > 0.005 * fabs(p[i][j] + A.p[i][j]))
            {
                // .. matrices are not equal
                return false;
            }
    return true;
};

bool gcm_matrix::operator!=(const gcm_matrix &A) const
{
    return !(*this == A);
};

gcm_matrix gcm_matrix::operator*(const gcm_matrix &A) const
{
    gcm_matrix res_matrix;
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

gcm_matrix gcm_matrix::operator/(const gcm_matrix &A) const
{
    return (*this * A.inv());
};

gcm_matrix gcm_matrix::operator*(const real &a) const
{
    gcm_matrix res_matrix;
    for (int r = 0; r < GCM_MATRIX_SIZE; r++) {
        for (int c = 0; c < GCM_MATRIX_SIZE; c++) {
            res_matrix.p[r][c] = this->p[r][c] * a;
        }
    }
    return res_matrix;
};

gcm_matrix gcm_matrix::operator/(const real &a) const
{
    if( a == 0.0 )
        THROW_INVALID_ARG("Can not divide matrix by zero");
    return (*this * (1/a));
};

gcm_matrix gcm_matrix::operator%(const gcm_matrix &A) const
{
    gcm_matrix res_matrix;
    for (int r = 0; r < GCM_MATRIX_SIZE; r++) {
        for (int c = 0; c < GCM_MATRIX_SIZE; c++) {
            res_matrix.p[r][c] = this->p[r][c] / A.p[r][c];
        }
    }
    return res_matrix;
};

gcm_matrix gcm_matrix::operator+(const gcm_matrix &A) const
{
    gcm_matrix res_matrix;
    for (int r = 0; r < GCM_MATRIX_SIZE; r++)
        for (int c = 0; c < GCM_MATRIX_SIZE; c++)
            res_matrix.p[r][c] = this->p[r][c] + A.p[r][c];
    return res_matrix;
};

gcm_matrix gcm_matrix::operator-(const gcm_matrix &A) const
{
    gcm_matrix res_matrix;
    for (int r = 0; r < GCM_MATRIX_SIZE; r++)
        for (int c = 0; c < GCM_MATRIX_SIZE; c++)
            res_matrix.p[r][c] = this->p[r][c] - A.p[r][c];
    return res_matrix;
};

float gcm_matrix::max_abs_value() const
{
    float res = 0;
    for (int r = 0; r < GCM_MATRIX_SIZE; r++)
        for (int c = 0; c < GCM_MATRIX_SIZE; c++)
            if (fabs(p[r][c]) > res)
                res = fabs(p[r][c]);
    return res;
};

void gcm_matrix::clear()
{
    memset(p, 0, 81 * sizeof (float));
};

void gcm_matrix::setColumn(double *Clmn, int num)
{
    for (int i = 0; i < GCM_MATRIX_SIZE; i++)
        p[i][num] = Clmn[i];
};

float gcm_matrix::get(unsigned int i, unsigned int j) const
{
    return p[i][j];
};

gcm_matrix gcm_matrix::inv() const
{
    gcm_matrix res_matrix;
    // Invert the matrix using gsl library

    gsl_set_error_handler_off();

    gsl_matrix* Z1 = gsl_matrix_alloc(GCM_MATRIX_SIZE, GCM_MATRIX_SIZE);
    gsl_matrix* Z = gsl_matrix_alloc(GCM_MATRIX_SIZE, GCM_MATRIX_SIZE);
    gsl_permutation* perm = gsl_permutation_alloc(GCM_MATRIX_SIZE);
    int k;

    for (int i = 0; i < GCM_MATRIX_SIZE; i++)
        for (int j = 0; j < GCM_MATRIX_SIZE; j++)
            gsl_matrix_set(Z1, i, j, p[i][j]);

    int status = gsl_linalg_LU_decomp(Z1, perm, &k);
    if (status) {
        LOG_DEBUG("gsl_linalg_LU_decomp failed");
        THROW_INVALID_ARG("gsl_linalg_LU_decomp failed");
    }
    status = gsl_linalg_LU_invert(Z1, perm, Z);
    if (status) {
        LOG_DEBUG("gsl_linalg_LU_invert failed");
        THROW_INVALID_ARG("gsl_linalg_LU_invert failed");
    }
    for (int i = 0; i < GCM_MATRIX_SIZE; i++)
        for (int j = 0; j < GCM_MATRIX_SIZE; j++)
            res_matrix.p[i][j] = gsl_matrix_get(Z, i, j);

    return res_matrix;
};
