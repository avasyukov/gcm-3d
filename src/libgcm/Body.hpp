#ifndef GCM_BODY_H_
#define GCM_BODY_H_

#include <vector>
#include <string>

#include "libgcm/mesh/Mesh.hpp"
#include "libgcm/Logging.hpp"

using namespace std;
using namespace gcm;

namespace gcm {
    /*
     * Calculation scene body. May contain few meshes of different types.
     */
    class Body: public IBody {
    private:
        /*
         * All meshes associated with body. Meshes may have different types
         * like calculation, visualization etc.
         */
        vector<Mesh*> meshes;
        /*
         * Body id. May be empty or non-unique, but it's not recommended.
         */
        string id;

        string rheoCalcType;
        /*
         * Logger.
         */
        USE_LOGGER;
    public:
        /*
         * Constructor.
         */
        Body(string id = "");
        /*
         * Destructor.
         */
        virtual ~Body();
        /*
         * Returns all meshes associated with body.
         */
        Mesh* getMeshes();
        // FIXME merge with method above
        const vector<Mesh*>& getMeshesVector();
        /*
         * Returns mesh by id.
         */
        Mesh* getMesh(string id);
        /*
         * Attaches new mesh to body.
         */
        void attachMesh(Mesh* mesh);
        /*
         * Returns body id.
         */
        string getId();

        void setInitialState(Area* area, float values[9]);
		void setBorderCondition(Area* area, unsigned int num);
		void setContactCondition(Area* area, unsigned int num);

        string getRheologyCalculatorType();
        void setRheologyCalculatorType(string calcType);
    };
}

#endif
