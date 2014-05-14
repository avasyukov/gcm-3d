#include "libgcm/rheology/setters/PrandtlRaussPlasticityRheologyMatrixSetter.hpp"
#include "libgcm/Exception.hpp"
#include "libgcm/rheology/Plasticity.hpp"
#include "libgcm/util/Types.hpp"

#include <algorithm>

using namespace gcm;

void PrandtlRaussPlasticityRheologyMatrixSetter::getPlasticityProps(MaterialPtr material, float& yieldStrength)
{
    auto props = material->getPlasticityProperties();
    yieldStrength = props[PLASTICITY_TYPE_PRANDTL_RAUSS][PLASTICITY_PROP_YIELD_STRENGTH];
}

unsigned int PrandtlRaussPlasticityRheologyMatrixSetter::getNumberOfStates() const
{
    return 0;
}

unsigned int PrandtlRaussPlasticityRheologyMatrixSetter::getStateForNode(const ICalcNode& node) const
{
    return 0;
};

void PrandtlRaussPlasticityRheologyMatrixSetter::computeQ(const MaterialPtr& material, const ICalcNode& node)
{
    float yieldStrength;
    getPlasticityProps(material, yieldStrength);

    auto mu = material->getMu();
    auto la = material->getLa();

	// Taking stress tensor components
	auto S = [&node](int i, int j) -> gcm_real
	{
		int _i = min(i, j);
		int _j = max(i, j);
		
		int m = (_i == 0 ? 0 : 1);
		
		return node.stress[m + _i + _j];
	};
	
	// Figuring out stress deviator tensor components
	auto D = [&S](int i, int j) -> gcm_real
	{
		// Hydrostatic stress
		gcm_real p = (S(0, 0) + S(1, 1) + S(2, 2)) / 3;
			
		return S(i, j) - p*delta(i, j);
	};
	
	float x = (D(0, 0)*D(0, 0) + D(1, 1)*D(1, 1) + D(2, 2)*D(2, 2) + 2 * (S(0, 1)*S(0, 1) + S(0, 2)*S(0, 2) + S(1, 2)*S(1, 2))) / (2 * yieldStrength * yieldStrength);
	int I = (x) >= 1.0 ? 1 : 0;
	
	for(int i = 0; i < 3; i++)
		for(int j = 0; j < 3; j++)
			for(int k = 0; k < 3; k++)
				for(int l = 0; l < 3; l++) {
					//if(I == 1) cout << "Plasticity x = " << x << endl; 
					q[i][j][k][l] = la * delta(i, j) * delta(k, l) + mu * (delta(i, k) * delta(j, l) + delta(i, l) * delta(j, k)) - mu*I*D(i, j)*D(k, l)/yieldStrength/yieldStrength;
				}
};

void PrandtlRaussPlasticityRheologyMatrixSetter::setX(gcm_matrix& a, const MaterialPtr& material, const ICalcNode& node)
{
	a.clear();
	
	computeQ(material, node);
	
	auto rho = node.getRho();
	
	a(0, 3) = -1.0 / rho;
    a(1, 4) = -1.0 / rho;
    a(2, 5) = -1.0 / rho;
	
	a(3, 0) = -q[0][0][0][0];	a(3, 1) = -(q[0][0][0][1] + q[0][0][1][0])/2.0;	a(3, 2) = -(q[0][0][0][2] + q[0][0][2][0])/2.0;
	a(4, 0) = -q[0][1][0][0];	a(4, 1) = -(q[0][1][0][1] + q[0][1][1][0])/2.0;	a(4, 2) = -(q[0][1][0][2] + q[0][1][2][0])/2.0;
	a(5, 0) = -q[0][2][0][0];	a(5, 1) = -(q[0][2][0][1] + q[0][2][1][0])/2.0;	a(5, 2) = -(q[0][2][0][2] + q[0][2][2][0])/2.0;
	a(6, 0) = -q[1][1][0][0];	a(6, 1) = -(q[1][1][0][1] + q[1][1][1][0])/2.0;	a(6, 2) = -(q[1][1][0][2] + q[1][1][2][0])/2.0;
	a(7, 0) = -q[1][2][0][0];	a(7, 1) = -(q[1][2][0][1] + q[1][2][1][0])/2.0;	a(7, 2) = -(q[1][2][0][2] + q[1][2][2][0])/2.0;
	a(8, 0) = -q[2][2][0][0];	a(8, 1) = -(q[2][2][0][1] + q[2][2][1][0])/2.0;	a(8, 2) = -(q[2][2][0][2] + q[2][2][2][0])/2.0;	

};

void PrandtlRaussPlasticityRheologyMatrixSetter::setY(gcm_matrix& a, const MaterialPtr& material, const ICalcNode& node)
{
	a.clear();
	
	computeQ(material, node);
	
	auto rho = node.getRho();
	
    a(0, 4) = -1.0 / rho;
    a(1, 6) = -1.0 / rho;
    a(2, 7) = -1.0 / rho;
	
	a(3, 0) = -(q[0][0][0][1] + q[0][0][1][0])/2.0;	a(3, 1) = -q[0][0][1][1];	a(3, 2) = -(q[0][0][1][2] + q[0][0][2][1])/2.0;
	a(4, 0) = -(q[0][1][0][1] + q[0][1][1][0])/2.0;	a(4, 1) = -q[0][1][1][1];	a(4, 2) = -(q[0][1][1][2] + q[0][1][2][1])/2.0;
	a(5, 0) = -(q[0][2][0][1] + q[0][2][1][0])/2.0;	a(5, 1) = -q[0][2][1][1];	a(5, 2) = -(q[0][2][1][2] + q[0][2][2][1])/2.0;
	a(6, 0) = -(q[1][1][0][1] + q[1][1][1][0])/2.0;	a(6, 1) = -q[1][1][1][1];	a(6, 2) = -(q[1][1][1][2] + q[1][1][2][1])/2.0;
	a(7, 0) = -(q[1][2][0][1] + q[1][2][1][0])/2.0;	a(7, 1) = -q[1][2][1][1];	a(7, 2) = -(q[1][2][1][2] + q[1][2][2][1])/2.0;
	a(8, 0) = -(q[2][2][0][1] + q[2][2][1][0])/2.0;	a(8, 1) = -q[2][2][1][1];	a(8, 2) = -(q[2][2][1][2] + q[2][2][2][1])/2.0;
	
};

void PrandtlRaussPlasticityRheologyMatrixSetter::setZ(gcm_matrix& a, const MaterialPtr& material, const ICalcNode& node)
{
	a.clear();
	
	computeQ(material, node);
	
	auto rho = node.getRho();
	
    a(0, 5) = -1.0 / rho;
    a(1, 7) = -1.0 / rho;
    a(2, 8) = -1.0 / rho;
	
	a(3, 0) = -(q[0][0][0][2] + q[0][0][2][0])/2.0;	a(3, 1) = -(q[0][0][1][2] + q[0][0][2][1])/2.0;	a(3, 2) = -q[0][0][2][2];
	a(4, 0) = -(q[0][1][0][2] + q[0][1][2][0])/2.0;	a(4, 1) = -(q[0][1][1][2] + q[0][1][2][1])/2.0;	a(4, 2) = -q[0][1][2][2];
	a(5, 0) = -(q[0][2][0][2] + q[0][2][2][0])/2.0;	a(5, 1) = -(q[0][2][1][2] + q[0][2][2][1])/2.0;	a(5, 2) = -q[0][2][2][2];
	a(6, 0) = -(q[1][1][0][2] + q[1][1][2][0])/2.0;	a(6, 1) = -(q[1][1][1][2] + q[1][1][2][1])/2.0;	a(6, 2) = -q[1][1][2][2];
	a(7, 0) = -(q[1][2][0][2] + q[1][2][2][0])/2.0;	a(7, 1) = -(q[1][2][1][2] + q[1][2][2][1])/2.0;	a(7, 2) = -q[1][2][2][2];
	a(8, 0) = -(q[2][2][0][2] + q[2][2][2][0])/2.0;	a(8, 1) = -(q[2][2][1][2] + q[2][2][2][1])/2.0;	a(8, 2) = -q[2][2][2][2];
	
};
