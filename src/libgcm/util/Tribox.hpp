#ifndef TRIBOX_HPP
#define TRIBOX_HPP

#include "libgcm/node/Node.hpp"
#include "libgcm/util/Types.hpp"

namespace gcm
{
    bool triangleIntersectsBox(const Node& boxCenter, const vector3r& boxEdges, const Node& v1, const Node& v2, const Node& v3);
    bool triangleIntersectsCube(const Node& cubeCenter, real cubeEdge, const Node& v1, const Node& v2, const Node& v3);
}
#endif /* TRIBOX_HPP */
