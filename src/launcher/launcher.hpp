#ifndef LAUNCHER_H_
#define LAUNCHER_H_

#include <string>

#include "launcher/util/xml.hpp"
#include "libgcm/Logging.hpp"

using namespace std;

namespace launcher {
    class Launcher
    {
    private:
        void loadMaterialsFromXml(xml::NodeList matNodes);

        USE_LOGGER;
    public:
        Launcher();

        void loadMaterialLibrary(std::string path);
        void loadSceneFromFile(std::string fileName);
    };
}


#endif /* LAUNCHER_H_ */
