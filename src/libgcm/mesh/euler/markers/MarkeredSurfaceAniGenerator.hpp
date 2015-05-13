#ifndef MARKEREDSURFACEANIGENERATOR_HPP
#define MARKEREDSURFACEANIGENERATOR_HPP

#include <string>
#include <utility>
#include <vector>

#include "libgcm/util/Singleton.hpp"
#include "libgcm/util/Types.hpp"
#include "libgcm/Logging.hpp"

#include "libgcm/mesh/euler/markers/MarkeredSurface.hpp"

namespace gcm
{
    class MarkeredSurfaceAniGenerator: public Singleton<MarkeredSurfaceAniGenerator>
    {
        protected:
            USE_LOGGER;
        public:
            MarkeredSurfaceAniGenerator();

            MarkeredSurface generate(std::vector<std::pair<std::string, std::string>> source);

    };
};

#endif /* MARKEREDSURFACEANIGENERATOR_HPP */
