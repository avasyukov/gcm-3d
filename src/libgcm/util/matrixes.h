#ifndef _GCM_MATRIXES_H
#define _GCM_MATRIXES_H  1

#include <iostream>
#include <math.h>
#include <string.h>

#include <assert.h>
#include <gsl/gsl_math.h>
#include <gsl/gsl_linalg.h>

#include "Math.h"
#include "Exception.h"

#define GCM_MATRIX_SIZE 9

namespace gcm
{

    class gcm_matrix
    {
    public:
        gcm_matrix();
        ~gcm_matrix();
        gcm_matrix& operator=(const gcm_matrix &A);
        bool operator==(const gcm_matrix &A) const;
        bool operator!=(const gcm_matrix &A) const;
        float& operator()(int i, int j);
        gcm_matrix operator+(const gcm_matrix &A) const;
        gcm_matrix operator-(const gcm_matrix &A) const;
        gcm_matrix operator*(const gcm_matrix &A) const;

        float get(unsigned int i, unsigned int j) const;

        float max_abs_value() const;
        void clear();
        void createE();
        void setColumn(float *Clmn, int num);
        void inv();

        float p[GCM_MATRIX_SIZE][GCM_MATRIX_SIZE]; // Data
    private:
        USE_LOGGER;
    };
}

namespace std
{

    inline std::ostream& operator<<(std::ostream &os, const gcm::gcm_matrix &matrix)
    {
        for (int r = 0; r < GCM_MATRIX_SIZE; r++) {
            for (int c = 0; c < GCM_MATRIX_SIZE; c++)
                os << matrix.p[r][c] << " ";
            os << std::endl;
        }
        return os;
    };
}

#endif
