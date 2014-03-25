#include <time.h>

#include "libgcm/materials/IsotropicElasticMaterial.h"
#include "libgcm/util/ElasticMatrix3D.h"
#include "libgcm/node/CalcNode.h"
#include "libgcm/Engine.h"

#include <gtest/gtest.h>

#define ITERATIONS 1000

using namespace gcm;

TEST(IsotropicMatrix3D, FuzzyMultiplication)
{
    srand(time(NULL));

    CalcNode isotropicNode;

    for (int k = 0; k < ITERATIONS; k++) {
        gcm_real la = 1.0e+9 * (double) rand() / RAND_MAX;
        gcm_real mu = 1.0e+8 * (double) rand() / RAND_MAX;
        gcm_real rho = 1.0e+4 * (double) rand() / RAND_MAX;
        gcm_real crackThreshold = numeric_limits<gcm_real>::infinity();

        IsotropicElasticMaterial mat("IsotropicMatrix3D_IsotropicTransition_IEM_" + k, rho, crackThreshold, la, mu);

        isotropicNode.setMaterialId(Engine::getInstance().addMaterial(&mat));

        RheologyMatrix3D& isotropicMatrix = isotropicNode.getRheologyMatrix();

        for (int i = 0; i < 3; i++) {
            switch (i) {
            case 0: isotropicMatrix.createAx(isotropicNode);
                break;
            case 1: isotropicMatrix.createAy(isotropicNode);
                break;
            case 2: isotropicMatrix.createAz(isotropicNode);
                break;
            }

            // Test decomposition
            ASSERT_TRUE( isotropicMatrix.getU1() * isotropicMatrix.getL() * isotropicMatrix.getU() == isotropicMatrix.getA() );
            // Test eigen values and eigen rows
            ASSERT_TRUE( isotropicMatrix.getU1() * isotropicMatrix.getL() == isotropicMatrix.getA() * isotropicMatrix.getU1() );
        }
        Engine::getInstance().clear();
    }
};

TEST(IsotropicMatrix3D, FuzzyElasticVelocities)
{
    srand(time(NULL));

    CalcNode isotropicNode;

    for (int k = 0; k < ITERATIONS; k++) {
        gcm_real la = 1.0e+9 * (double) rand() / RAND_MAX;
        gcm_real mu = 1.0e+8 * (double) rand() / RAND_MAX;
        gcm_real rho = 1.0e+4 * (double) rand() / RAND_MAX;
        gcm_real crackThreshold = numeric_limits<gcm_real>::infinity();
        gcm_real pVel = sqrt( ( la + 2 * mu ) / rho );
        gcm_real sVel = sqrt( mu / rho );

        IsotropicElasticMaterial mat("IsotropicMatrix3D_IsotropicTransition_IEM_" + k, rho, crackThreshold, la, mu);

        isotropicNode.setMaterialId(Engine::getInstance().addMaterial(&mat));

        RheologyMatrix3D& isotropicMatrix = isotropicNode.getRheologyMatrix();

        for (int i = 0; i < 3; i++) {
            switch (i) {
            case 0: isotropicMatrix.createAx(isotropicNode);
                break;
            case 1: isotropicMatrix.createAy(isotropicNode);
                break;
            case 2: isotropicMatrix.createAz(isotropicNode);
                break;
            }

            ASSERT_NEAR( isotropicMatrix.getMaxEigenvalue(), pVel, EQUALITY_TOLERANCE );
            for(int j = 0; j < 9; j++)
            {
                gcm_real v = fabs(isotropicMatrix.getL(j,j));
                ASSERT_TRUE( ( fabs(v - pVel) < EQUALITY_TOLERANCE )
                                || ( fabs(v - sVel) < EQUALITY_TOLERANCE )
                                || ( fabs(v - 0) < EQUALITY_TOLERANCE ) );
            }
        }
        Engine::getInstance().clear();
    }
};
