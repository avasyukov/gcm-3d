#ifndef GCM_ELASTIC_NODE_H_
#define GCM_ELASTIC_NODE_H_

#include "CalcNode.h"

using namespace std;

namespace gcm {
	class ElasticNode: public CalcNode {
		
		friend class DataBus;
		friend class VtuTetrFileReader;
		friend class Vtu2TetrFileReader;
		
		public:
			ElasticNode();
			ElasticNode(int _num);
			ElasticNode(int _num, float _x, float _y, float _z);
			ElasticNode(const ElasticNode& src);
			~ElasticNode();
			
			void fastCopy(const ElasticNode& src);
			
			ElasticNode &operator=(const ElasticNode &src);
			/*
			 * Elastic rheology parameters.
			 */
			union {
				float elasticRheologyProperties[3];
				struct {
					float la;
					float mu;
					float rho;
				};
			};
			
			inline bool rheologyIsValid() {
				return ( la > 0 && mu > 0 && rho > 0 );
			}
	};
}

namespace std {
	inline std::ostream& operator<< (std::ostream &os, const gcm::ElasticNode &node) {
		os << "\n\tElasticNode number: " << node.number << "\n";
		os << "\tCoords:";
		for( int i = 0; i < 3; i++ )
			os << " " << node.coords[i];
		os << "\n\tVelocity:";
		for( int i = 0; i < 3; i++ )
			os << " " << node.values[i];
		os << "\n\tStress:";
		for( int i = 3; i < 9; i++ )
			os << " " << node.values[i];
		os << "\n\tLambda: " << node.la << " Mu: " << node.mu << " Rho: " << node.rho;
		return os;
	}
}

#endif
