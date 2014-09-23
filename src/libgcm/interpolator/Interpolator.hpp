#ifndef GCM_INTERPOLATOR_H
#define    GCM_INTERPOLATOR_H

#include <string>

#include "libgcm/Math.hpp"
#include "libgcm/Logging.hpp"


namespace gcm {

    class Interpolator {
    public:
        Interpolator();
        ~Interpolator();
        std::string getType();
    protected:
        std::string type;
    private:
        USE_LOGGER;
    };
}

#endif

