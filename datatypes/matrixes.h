#include <iostream>
#include <boost/numeric/ublas/matrix.hpp>
#include <boost/numeric/ublas/io.hpp>

typedef boost::numeric::ublas::matrix<float> ublas_matrix;

/* Solution from http://www.anderswallin.net/2010/05/matrix-determinant-with-boostublas/  */

#include <boost/numeric/ublas/matrix.hpp>
#include <boost/numeric/ublas/io.hpp>
#include <boost/numeric/ublas/lu.hpp>

namespace bnu = boost::numeric::ublas;

int determinant_sign(const bnu::permutation_matrix<std ::size_t>& pm)
{
    int pm_sign=1;
    std::size_t size = pm.size();
    for (std::size_t i = 0; i < size; ++i)
        if (i != pm(i))
            pm_sign *= -1.0; // swap_rows would swap a pair of rows here, so we change sign
    return pm_sign;
}
 
float determinant( ublas_matrix& m ) {
    ublas_matrix tm(m); // tmp matrix to preserve m values
    bnu::permutation_matrix<std::size_t> pm(tm.size1());
    float det = 1.0;
    if( bnu::lu_factorize(tm,pm) ) {
        det = 0.0;
    } else {
        for(int i = 0; i < tm.size1(); i++) 
            det *= tm(i,i); // multiply by elements on diagonal
        det = det * determinant_sign( pm );
    }
    return det;
}
