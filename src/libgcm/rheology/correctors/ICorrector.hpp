#ifndef ICORRECTOR_HPP
#define ICORRECTOR_HPP 

#include "libgcm/node/ICalcNode.hpp"
#include "libgcm/mesh/Mesh.hpp"

#include <memory>

namespace gcm
{
    /**
     * Interface to implement for rheology matrix corrector. Corrector is supposed to
     * perform node state correction after the main step is done 
     */
    class ICorrector
    {
        public:
            /**
             * Perform correction for the node
             *
             * @param node Node to perform correction on
             */
            virtual void correctNodeState(ICalcNode& node, Mesh& mesh, const MaterialPtr& material, float time_step) = 0;
    };

    typedef std::shared_ptr<ICorrector> CorrectorPtr;
    
    template<typename T, typename...Args>
    std::shared_ptr<T> makeCorrectorPtr(Args...args)
    {
        return std::make_shared<T>(args...);
    }
};

#endif /* ICORRECTOR_HPP */
