#ifndef _GCM_MATRIXES_H
#define _GCM_MATRIXES_H  1

#include <iostream>

class gcm_matrix
{
public:
	gcm_matrix();
	~gcm_matrix();
	gcm_matrix &operator=(const gcm_matrix &A); 
	float &operator()(int i, int j);
	gcm_matrix operator+(const gcm_matrix &A) const;
	gcm_matrix operator-(const gcm_matrix &A) const;
	gcm_matrix operator*(const gcm_matrix &A) const;

	float p[9][9];	 // Data
};

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

//float gcm_matrix::determinant() {
//};

#endif
