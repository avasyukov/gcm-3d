#ifndef GCM_NODE_H_
#define GCM_NODE_H_

#include <iostream>
#include <log4cxx/logger.h>

using namespace std;

namespace gcm {

    /**
     * Base class that contains node data. All other node implementations 
     * should derive from this one. Contains basic node information such
     * as number and coordinates.
     * 
     */
    class Node {
    public:
        /**
         * Number of the node.
         */
        int number;

        union {
            float coords[3];

            struct {
                /**
                 * Position vector x component.
                 */
                float x;
                /**
                 * Position vector y component.
                 */
                float y;
                /**
                 * Position vector z component.
                 */
                float z;
            };
        };
        /**
         * Default node constructor. Creates new node with all members
         * set to default values;
         */
        Node();
        /**
         * Constructor. Creates node with specified number assigned.
         *
         * @param num Number of the node
         */
        Node(int num);
        /**
         * Constructor. Creates node with specified number assigned
         * and set coordinates.
         *
         * @param num Number of the node
         * @param x X coordinate of the node
         * @param y Y coordinate of the node
         * @param z Z coordinate of the node
         */
        Node(int num, float x, float y, float z);
        /**
         * Destructor
         */
        virtual ~Node() = 0;
    };
}

#endif