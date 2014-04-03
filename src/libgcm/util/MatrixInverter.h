#ifndef MATRIXINVERTER_H
#define	MATRIXINVERTER_H

#include "util/matrixes.h"
#include <gsl/gsl_linalg.h>
#include <gsl/gsl_matrix.h>
#include <gsl/gsl_vector.h>


namespace gcm
{
	class MatrixInverter {
		gsl_matrix *U;
	public:
		void inv(gcm_matrix &result);
		void setColumn(double *Clmn, int num);
		void setUnity(int i1, int j1, int i2, int j2, int i3, int j3); 
		MatrixInverter();
		~MatrixInverter();
	};

}
#endif	/* MATRIXINVERTER_H */

