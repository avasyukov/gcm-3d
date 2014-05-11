#include <time.h>

#include "libgcm/rheology/Material.hpp"
#include "libgcm/rheology/setters/IsotropicRheologyMatrixSetter.hpp"
#include "libgcm/rheology/decomposers/IsotropicRheologyMatrixDecomposer.hpp"
#include "libgcm/node/CalcNode.hpp"
#include "libgcm/Engine.hpp"

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

        auto mat = makeMaterialPtr("material", rho, crackThreshold, la, mu);

        RheologyMatrixPtr isotropicMatrix = makeRheologyMatrixPtr<IsotropicRheologyMatrixSetter, IsotropicRheologyMatrixDecomposer>(mat);

        for (int i = 0; i < 3; i++) {
            switch (i) {
            case 0: isotropicMatrix->decomposeX(isotropicNode);
                break;
            case 1: isotropicMatrix->decomposeY(isotropicNode);
                break;
            case 2: isotropicMatrix->decomposeZ(isotropicNode);
                break;
            }

            // Test decomposition
            ASSERT_TRUE( isotropicMatrix->getU1() * isotropicMatrix->getL() * isotropicMatrix->getU() == isotropicMatrix->getA() );
            // Test eigen values and eigen rows
            ASSERT_TRUE( isotropicMatrix->getU1() * isotropicMatrix->getL() == isotropicMatrix->getA() * isotropicMatrix->getU1() );
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

        auto mat = makeMaterialPtr("material", rho, crackThreshold, la, mu);

        RheologyMatrixPtr isotropicMatrix = makeRheologyMatrixPtr<IsotropicRheologyMatrixSetter, IsotropicRheologyMatrixDecomposer>(mat);

        for (int i = 0; i < 3; i++) {
            switch (i) {
            case 0: isotropicMatrix->decomposeX(isotropicNode);
                break;
            case 1: isotropicMatrix->decomposeY(isotropicNode);
                break;
            case 2: isotropicMatrix->decomposeZ(isotropicNode);
                break;
            }

            ASSERT_NEAR( isotropicMatrix->getMaxEigenvalue(), pVel, pVel * EQUALITY_TOLERANCE );
            for(int j = 0; j < 9; j++)
            {
                gcm_real v = fabs(isotropicMatrix->getL(j,j));
                ASSERT_TRUE( ( fabs(v - pVel) < pVel * EQUALITY_TOLERANCE )
                                || ( fabs(v - sVel) < sVel * EQUALITY_TOLERANCE )
                                || ( fabs(v - 0) < EQUALITY_TOLERANCE ) );
            }
        }
        Engine::getInstance().clear();
    }
};
