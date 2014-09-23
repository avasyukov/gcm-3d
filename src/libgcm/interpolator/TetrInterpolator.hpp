#ifndef GCM_INTERPOLATOR_H
#define    GCM_INTERPOLATOR_H

#include <string>

#include "libgcm/Math.hpp"
#include "libgcm/Logging.hpp"


namespace gcm {

    class TetrInterpolator {
    public:
        TetrInterpolator();
        ~TetrInterpolator();
        std::string getType();
    protected:
        std::string type;
    private:
        USE_LOGGER;
    };
}

#endif

