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
}
#endif /* PLASTICITY_HPP */
