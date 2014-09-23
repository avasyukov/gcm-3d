
#include <gtest/gtest.h>
#include "libgcm/rheology/Material.hpp"
#include "libgcm/rheology/Plasticity.hpp"
#include "libgcm/rheology/setters/PrandtlRaussPlasticityRheologyMatrixSetter.hpp"
#include "libgcm/rheology/setters/IsotropicRheologyMatrixSetter.hpp"
#include "libgcm/node/CalcNode.hpp"


#define ITERATIONS 1000

using namespace gcm;
using std::numeric_limits;

TEST(Plasticity, PrandtlRaussElasticTransition)
{
    srand(time(NULL));

    CalcNode isotropicNode;

    for (int k = 0; k < ITERATIONS; k++)
    {
        real la = 1.0e+9 * (double) rand() / RAND_MAX;
        real mu = 1.0e+8 * (double) rand() / RAND_MAX;
        real rho = 1.0e+4 * (double) rand() / RAND_MAX;
        
        Material::PlasticityProperties props;
        props[PLASTICITY_TYPE_PRANDTL_RAUSS][PLASTICITY_PROP_YIELD_STRENGTH] = numeric_limits<real>::infinity();

        auto mat = makeMaterialPtr("material", rho, la, mu, props);
	
	    gcm_matrix a1, a2;
		IsotropicRheologyMatrixSetter elasticSetter;
		PrandtlRaussPlasticityRheologyMatrixSetter plasticSetter;
		CalcNode node;
		
		node.setRho(rho);


		for(int i = 0; i < 3; i++)
		{
			switch(i)
			{
				case 0:	elasticSetter.setX(a1, mat, node); plasticSetter.setX(a2, mat, node); break;
				case 1:	elasticSetter.setY(a1, mat, node); plasticSetter.setY(a2, mat, node); break;
				case 2:	elasticSetter.setZ(a1, mat, node); plasticSetter.setZ(a2, mat, node); break;
			}
			
		    ASSERT_TRUE( a1 == a2 );
		}
	}
};
