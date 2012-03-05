#include "matrixes.h"

gcm_matrix::gcm_matrix() { };
gcm_matrix::~gcm_matrix() { };

inline gcm_matrix& gcm_matrix::operator=(const gcm_matrix &A)
{
	for(int i = 0; i < 9; i++)
		for(int j = 0; j < 9; j++)
			p[i][j] = A.p[i][j];
	return *this;
};

float &gcm_matrix::operator()(int i, int j)
{
	return p[i][j];
};

void gcm_matrix::createE()
{
	for(int i = 0; i < 9; i++)
		for(int j = 0; j < 9; j++)
			if(i == j)
				p[i][j] = 1;
			else
				p[i][j] = 0;
};

bool gcm_matrix::operator==(const gcm_matrix &A) const
{
	float max1 = max_abs_value();
	float max2 = A.max_abs_value();

	float max = (max1 > max2) ? max1 : max2;

	for (int i = 0; i < 9; ++i)
		for (int j = 0; j < 9; ++j)
			if(fabs(p[i][j] - A.p[i][j]) > EQUALITY_TOLERANCE * max)
				return false;
	return true;
};

bool gcm_matrix::operator!=(const gcm_matrix &A) const
{
	return !(*this == A);
};

gcm_matrix gcm_matrix::operator*(const gcm_matrix &A) const
{
	gcm_matrix res_matrix;
	for (int r = 0; r < 9; r++) {
		for (int c = 0; c < 9; c++) {
			res_matrix.p[r][c] = 0;
			for (int i = 0; i < 9; i++) {
				res_matrix.p[r][c] += this->p[r][i] * A.p[i][c];
			}
		}
	}
	return res_matrix;
};

gcm_matrix gcm_matrix::operator+(const gcm_matrix &A) const
{
        gcm_matrix res_matrix;
        for (int r = 0; r < 9; r++)
                for (int c = 0; c < 9; c++)
			res_matrix.p[r][c] = this->p[r][c] + A.p[r][c];
        return res_matrix;
};

gcm_matrix gcm_matrix::operator-(const gcm_matrix &A) const
{
        gcm_matrix res_matrix;
        for (int r = 0; r < 9; r++)
                for (int c = 0; c < 9; c++)
                        res_matrix.p[r][c] = this->p[r][c] - A.p[r][c];
        return res_matrix;
};

std::ostream& operator<< (std::ostream &os, const gcm_matrix &matrix)
{
	for (int r = 0; r < 9; r++)
	{
		for (int c = 0; c < 9; c++)
		{
			os << matrix.p[r][c] << " ";
		}
		os << std::endl;
	}
	return os;
};

float gcm_matrix::max_abs_value() const
{
	float res = 0;
	for (int r = 0; r < 9; r++)
		for (int c = 0; c < 9; c++)
			if(fabs(p[r][c]) > res)
				res = fabs(p[r][c]);
	return res;
};
