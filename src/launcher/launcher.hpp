#ifndef LAUNCHER_H_
#define LAUNCHER_H_

#include <string>

#include "launcher/util/xml.hpp"
#include "libgcm/Logging.hpp"
#include "libgcm/util/areas/Area.hpp"

using namespace std;

namespace launcher {
    class Launcher
    {
    private:
        void loadMaterialsFromXml(xml::NodeList matNodes);
		Area* readCylinderArea(xml::Node areaNode);
		Area* readSphereArea(xml::Node areaNode);
		Area* readBoxArea(xml::Node areaNode);
		Area* readArea(xml::Node areaNode);

        USE_LOGGER;
    public:
        Launcher();

        void loadMaterialLibrary(std::string path);
        void loadSceneFromFile(std::string fileName);
    };
}


#endif /* LAUNCHER_H_ */
