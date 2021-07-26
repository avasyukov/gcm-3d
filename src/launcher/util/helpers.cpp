#include "launcher/util/helpers.hpp"

#include <string>
#include <boost/lexical_cast.hpp>

#include "libgcm/util/areas/BoxArea.hpp"
#include "libgcm/util/areas/SphereArea.hpp"
#include "libgcm/util/areas/CylinderArea.hpp"
#include "libgcm/util/areas/AberratorArea.hpp"

#include <boost/filesystem.hpp>
#include <fstream>

using namespace gcm;
namespace bfs = boost::filesystem;
using std::string;
using boost::lexical_cast;

Area* launcher::readBoxArea(const xml::Node& areaNode)
{
    real minX = lexical_cast<real>(areaNode["minX"]);
    real maxX = lexical_cast<real>(areaNode["maxX"]);
    real minY = lexical_cast<real>(areaNode["minY"]);
    real maxY = lexical_cast<real>(areaNode["maxY"]);
    real minZ = lexical_cast<real>(areaNode["minZ"]);
    real maxZ = lexical_cast<real>(areaNode["maxZ"]);
    LOG_DEBUG("Box size: [" << minX << ", " << maxX << "] "
              << "[" << minY << ", " << maxY << "] "
              << "[" << minZ << ", " << maxZ << "]");
    return new BoxArea(minX, maxX, minY, maxY, minZ, maxZ);
}

Area* launcher::readSphereArea(const xml::Node& areaNode)
{
    real r = lexical_cast<real>(areaNode["r"]);
    real x = lexical_cast<real>(areaNode["x"]);
    real y = lexical_cast<real>(areaNode["y"]);
    real z = lexical_cast<real>(areaNode["z"]);
    LOG_DEBUG("Sphere R = " << r << ". Center: (" << x << ", " << y << ", " << z << ").");
    return new SphereArea(r, x, y, z);
}

Area* launcher::readCylinderArea(const xml::Node& areaNode)
{
    real r = lexical_cast<real>(areaNode["r"]);
    real x1 = lexical_cast<real>(areaNode["x1"]);
    real y1 = lexical_cast<real>(areaNode["y1"]);
    real z1 = lexical_cast<real>(areaNode["z1"]);
    real x2 = lexical_cast<real>(areaNode["x2"]);
    real y2 = lexical_cast<real>(areaNode["y2"]);
    real z2 = lexical_cast<real>(areaNode["z2"]);
    LOG_DEBUG("Cylinder R = " << r << "."
              << " Center1: (" << x1 << ", " << y1 << ", " << z1 << ")."
              << " Center2: (" << x2 << ", " << y2 << ", " << z2 << ").");
    return new CylinderArea(r, x1, y1, z1, x2, y2, z2);
}

Area* launcher::readAberratorArea(const xml::Node& areaNode)
{
    real minX = lexical_cast<real>(areaNode["minX"]);
    real maxX = lexical_cast<real>(areaNode["maxX"]);
    real minY = lexical_cast<real>(areaNode["minY"]);
    real maxY = lexical_cast<real>(areaNode["maxY"]);
    real flatZ = lexical_cast<real>(areaNode["flatZ"]);
    string fcurve = areaNode["curve"];
    LOG_DEBUG("Aberrator size: [" << minX << ", " << maxX << "] "
                            << "[" << minY << ", " << maxY << "] "
                            << "[" << flatZ << ", " << fcurve << "]");

    if ( ! bfs::is_regular_file(fcurve))
        THROW_INVALID_ARG("File not found: " + fcurve);
    std::ifstream istrm(fcurve, std::ios::in);
    std::vector<real> curve;
    real tmp;
    while (istrm >> tmp)
        curve.push_back(tmp);
    istrm.close();

    return new AberratorArea(minX, maxX, minY, maxY, flatZ, curve);
}

Area* launcher::readArea(const xml::Node& areaNode)
{
    string areaType = areaNode["type"];
    LOG_DEBUG("Material area: " << areaType);

    if (areaType == "box")
        return readBoxArea(areaNode);
    else if (areaType == "sphere")
        return readSphereArea(areaNode);
    else if (areaType == "cylinder")
        return readCylinderArea(areaNode);
    else if (areaType == "aberrator")
        return readAberratorArea(areaNode);

    LOG_ERROR("Unknown initial state area: " << areaType);
    return NULL;
}