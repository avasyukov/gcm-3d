#ifndef GCM_NODE_H_
#define GCM_NODE_H_

#include <iostream>
#include <log4cxx/logger.h>

using namespace std;

namespace gcm {
	/*
	 * Base node class to inherit from.
	 */
	class Node {
		public:
			Node();
			Node(int _num);
			Node(int _num, float _x, float _y, float _z);
			~Node();
			/*
			 * Node number
			 */
			int number;
			/*
			 * Node characteristics.
			 */
			union
			{
				float coords[3];
				struct
				{
					/*
					 * Coordinates.
					 */
					float x;
					float y;
					float z;
				};
			};
	};
}

namespace std {
	inline std::ostream& operator<< (std::ostream &os, const gcm::Node &node) {
		os << "\n\tNode number: " << node.number << "\n";
		os << "\tCoords:";
		for( int i = 0; i < 3; i++ )
			os << " " << node.coords[i];
		return os;
	}
}

#endif
