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
	public:
		Material(string _id);
		~Material();
	
		void setRho(float _rho);
		void setLame(float _la, float _mu);
		
		string getId();
		float getLambda();
		float getMu();
		float getRho();
	};
}

#endif
