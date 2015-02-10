#ifndef GCM_BODY_H_
#define GCM_BODY_H_

#include <vector>
#include <string>
#include <functional>

#include "libgcm/mesh/Mesh.hpp"
#include "libgcm/Logging.hpp"


namespace gcm {
    /*
     * Calculation scene body. May contain few meshes of different types.
     */
    class Body {
    private:
        /*
         * All meshes associated with body. Meshes may have different types
         * like calculation, visualization etc.
         */
        std::vector<Mesh*> meshes;
        /*
         * Body id. May be empty or non-unique, but it's not recommended.
         */
        std::string id;

        std::string rheoCalcType;
        /*
         * Logger.
         */
        USE_LOGGER;
    public:
        /*
         * Constructor.
         */
        Body(std::string id = "");
        /*
         * Destructor.
         */
        virtual ~Body();
        /*
         * Returns all meshes associated with body.
         */
        Mesh* getMeshes();
        // FIXME merge with method above
        const std::vector<Mesh*>& getMeshesVector();
        /*
         * Returns mesh by id.
         */
        Mesh* getMesh(std::string id);
        /*
         * Attaches new mesh to body.
         */
        void attachMesh(Mesh* mesh);
        /*
         * Returns body id.
         */
        std::string getId();

        void setInitialState(Area* area, float values[9]);
        void setInitialState(Area* area, std::function<void(CalcNode&)> setter);
		void setBorderCondition(Area* area, unsigned int num);
		void setContactCondition(Area* area, unsigned int num);

        std::string getRheologyCalculatorType();
        void setRheologyCalculatorType(std::string calcType);
    };
}

#endif
