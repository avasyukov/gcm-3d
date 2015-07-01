#ifndef LAUNCHER_H_
#define LAUNCHER_H_

#include <string>

#include "launcher/util/xml.hpp"
#include "libgcm/Logging.hpp"
#include "libgcm/util/areas/Area.hpp"
#include "libgcm/linal/Vector3.hpp"
#include "libgcm/node/CalcNode.hpp"


namespace launcher {
    class Launcher
    {
    private:
		gcm::Area* readCylinderArea(xml::Node areaNode);
		gcm::Area* readSphereArea(xml::Node areaNode);
		gcm::Area* readBoxArea(xml::Node areaNode);
		gcm::Area* readArea(xml::Node areaNode);

        std::vector<int> usedMaterialsIds;

        USE_LOGGER;
    public:
        Launcher();
        
        void loadMaterialsFromXml(xml::NodeList matNodes);

        void loadMaterialLibrary(std::string path);
        void loadSceneFromFile(std::string fileName, std::string initialStateGroup="");
        
    };
        
    void setIsotropicElasticPWave(gcm::CalcNode& node, const gcm::linal::Vector3& direction, gcm::real amplitudeScale, bool compression);
}


#endif /* LAUNCHER_H_ */
