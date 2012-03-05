#ifndef _GCM_MATRIXES_H
#define _GCM_MATRIXES_H  1

#define EQUALITY_TOLERANCE 0.00001

#include <iostream>
#include <math.h>

class gcm_matrix
{
public:
	gcm_matrix();
	~gcm_matrix();
	gcm_matrix &operator=(const gcm_matrix &A);
	bool operator==(const gcm_matrix &A) const;
	bool operator!=(const gcm_matrix &A) const;
	float &operator()(int i, int j);
	gcm_matrix operator+(const gcm_matrix &A) const;
	gcm_matrix operator-(const gcm_matrix &A) const;
	gcm_matrix operator*(const gcm_matrix &A) const;

	float max_abs_value() const;
	void createE();

	float p[9][9];	 // Data
};

#endif
