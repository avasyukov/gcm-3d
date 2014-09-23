#ifndef LAUNCHER_H_
#define LAUNCHER_H_

#include <string>

#include "launcher/util/xml.hpp"
#include "libgcm/Logging.hpp"
#include "libgcm/util/areas/Area.hpp"


namespace launcher {
    class Launcher
    {
    private:
        void loadMaterialsFromXml(xml::NodeList matNodes);
		gcm::Area* readCylinderArea(xml::Node areaNode);
		gcm::Area* readSphereArea(xml::Node areaNode);
		gcm::Area* readBoxArea(xml::Node areaNode);
		gcm::Area* readArea(xml::Node areaNode);

        std::vector<int> usedMaterialsIds;

        USE_LOGGER;
    public:
        Launcher();

        void loadMaterialLibrary(std::string path);
        void loadSceneFromFile(std::string fileName);
    };
}


#endif /* LAUNCHER_H_ */
