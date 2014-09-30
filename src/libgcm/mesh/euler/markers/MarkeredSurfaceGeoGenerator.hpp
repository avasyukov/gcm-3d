#ifndef MARKEREDSURFACEGEOGENERATOR_HPP
#define MARKEREDSURFACEGEOGENERATOR_HPP

#include <string>

#include "libgcm/util/Singleton.hpp"
#include "libgcm/util/Types.hpp"
#include "libgcm/Logging.hpp"

#include "libgcm/mesh/euler/markers/MarkeredSurface.hpp"

namespace gcm
{
    class MarkeredSurfaceGeoGenerator: public Singleton<MarkeredSurfaceGeoGenerator>
    {
        protected:
            USE_LOGGER;
        public:
            MarkeredSurfaceGeoGenerator();

            MarkeredSurface generate(std::string fileName, real size=0.0);

    };
};

#endif /* MARKEREDSURFACEGEOGENERATOR_HPP */
