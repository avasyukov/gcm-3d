#ifndef _GCM_MATERIAL_H
#define _GCM_MATERIAL_H 1

#include <string>

using namespace std;

namespace gcm {

	class Material
	{
	protected:
		string id;
		float la;
		float mu;
		float rho;
		float crackThreshold;
		float adhesionThreshold;
	public:
		Material(string _id);
		~Material();
	
		void setRho(float _rho);
		void setLame(float _la, float _mu);
		void setCrackThreshold(float _ct);
		void setAdhesionThreshold(float _at);
		
		string getId();
		float getLambda();
		float getMu();
		float getRho();
		float getCrackThreshold();
		float getAdhesionThreshold();
	};
}

#endif
