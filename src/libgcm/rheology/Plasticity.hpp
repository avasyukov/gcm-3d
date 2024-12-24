#ifndef PLASTICITY_HPP
#define PLASTICITY_HPP 

#include <string>

namespace gcm
{
    const std::string PLASTICITY_TYPE_NONE = "none";
    const std::string PLASTICITY_TYPE_PRANDTL_RAUSS = "PrandtlRauss";
    const std::string PLASTICITY_TYPE_PRANDTL_RAUSS_CORRECTOR = "PrandtlRaussCorrector";
    const std::string PLASTICITY_PROP_YIELD_STRENGTH = "yieldStrength";
    const std::string PLASTICITY_TYPE_MAXWELL = "maxwell";
    const std::string PLASTICITY_PROP_MAXWELL_TAU = "tau";
    const std::string PLASTICITY_TYPE_FOIGHT = "foight";
    const std::string PLASTICITY_PROP_FOIGHT_ETA = "eta";
    const std::string PLASTICITY_TYPE_PIEZO = "piezo";
    const std::string PLASTICITY_PROP_PIEZO_S = "s";
    const std::string PLASTICITY_PROP_PIEZO_FX = "fx";
    const std::string PLASTICITY_PROP_PIEZO_FY = "fy";
    const std::string PLASTICITY_PROP_PIEZO_FZ = "fz";
    const std::string PLASTICITY_PROP_PIEZO_START = "start";
    const std::string PLASTICITY_PROP_PIEZO_DURATION = "duration";
}
#endif /* PLASTICITY_HPP */
