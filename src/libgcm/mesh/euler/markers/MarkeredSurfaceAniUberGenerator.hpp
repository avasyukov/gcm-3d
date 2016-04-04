#ifndef MarkeredSurfaceAniUberGenerator_HPP
#define MarkeredSurfaceAniUberGenerator_HPP

#include <string>
#include <utility>
#include <vector>

#include "libgcm/util/Singleton.hpp"
#include "libgcm/util/Types.hpp"
#include "libgcm/Logging.hpp"

#include "libgcm/mesh/euler/markers/MarkeredSurface.hpp"

namespace gcm
{
    class MarkeredSurfaceAniUberGenerator: public Singleton<MarkeredSurfaceAniUberGenerator>
    {
        protected:
            USE_LOGGER;
        public:
            MarkeredSurfaceAniUberGenerator();

            MarkeredSurface generate(std::string& source);

    };
};

#endif /* MarkeredSurfaceAniUberGenerator_HPP */
