#ifndef LAUNCHER_H_
#define LAUNCHER_H_

#include <string>

#include "launcher/util/xml.hpp"
#include "libgcm/Logging.hpp"
#include "libgcm/util/areas/Area.hpp"
#include "libgcm/linal/Vector3.hpp"
#include "libgcm/node/CalcNode.hpp"
#include "libgcm/calc/contact/ClosedFractureContactCalculator.hpp"
#include "libgcm/calc/contact/OpenFractureContactCalculator.hpp"


namespace launcher {	
    class Launcher
    {
    private:
        void loadMaterialsFromXml(xml::NodeList matNodes);

        std::vector<int> usedMaterialsIds;

        USE_LOGGER;
    public:
        Launcher();

        void loadMaterialLibrary(std::string path);
        void loadSceneFromFile(std::string fileName, std::string initialStateGroup="");

    };
        
    void setIsotropicElasticPWave(gcm::CalcNode& node, const gcm::linal::Vector3& direction, gcm::real amplitudeScale, bool compression);
}


#endif /* LAUNCHER_H_ */
