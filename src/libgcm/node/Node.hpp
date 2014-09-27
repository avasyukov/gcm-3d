#ifndef GCM_NODE_H_
#define GCM_NODE_H_

#include "libgcm/util/Types.hpp"

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
        /**
         * Node coords.
         */
        vector3r coords;
        /**
         * Default node constructor. Creates new node with all members
         * set to default values;
         */
        Node();
        /**
         * Copy constructor
         */
        Node(const Node& src);
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
         * @param coords Node coords
         */
        Node(int num, const vector3r& coords);
        /**
         * Destructor
         */
        virtual ~Node() = 0;
    };
}

#endif
