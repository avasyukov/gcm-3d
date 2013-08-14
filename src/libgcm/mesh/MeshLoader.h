#ifndef GCM_MESH_LOADER_H_
#define GCM_MESH_LOADER_H_

#include <map>
#include <string>

#include "../Interfaces.h"
#include "../Body.h"
#include "Mesh.h"
#include "../util/AABB.h"
#include "../Utils.h"

using namespace std;

namespace gcm {
	/*
	 * Base class for al mesh loaders.
	 */
	class MeshLoader {
	protected:
		/*
		 * Engine instance.
		 */
		IEngine* engine;		
	public:
		/*
		 * Sets engine
		 */
		void setEngine(IEngine* engine);
		/*
		 * Returns mesh loader type.
		 */
		virtual string getType() = 0;
		virtual void cleanUp() = 0;

		/*
		 * Loads mesh using passed configuration.
		 */
		virtual Mesh* load(Body* body, Params params) = 0;
		virtual void preLoadMesh(Params params, AABB* scene) = 0;
		
		/*
		 * Constants
		 */
		static const string PARAM_CALC;
		static const string PARAM_FILE;
		static const string PARAM_ID;
		static const string PARAM_ZONES;
		static const string PARAM_GENERATE_FROM;
		static const string PARAM_VALUE_TRUE;		
	};

	/*
	 * Templated class to simplify new loader creation.
	 */
	template<typename MeshType>
	class TemplatedMeshLoader: public MeshLoader {
	protected:
		/*
		 * Loads mesh using specified params (type-specific implementation).
		 */
		virtual void loadMesh(Params params, MeshType* mesh, GCMDispatcher* dispatcher) = 0;
	public:
		/*
		 * Loads mesh using passed configuration.
		 */
		MeshType* load(Body* body, Params params) {
			// create mesh
			MeshType* mesh = new MeshType();
			// set mesh id
			if (params.find(PARAM_ID) != params.end())
				mesh->setId(params[PARAM_ID]);
			// set calc
			if (params.find(PARAM_CALC) != params.end())
				mesh->setCalc(params[PARAM_CALC] == PARAM_VALUE_TRUE);
			// set body
			mesh->setBody(body);
			assert( body->getEngine() != NULL );
			assert( body->getEngine()->getDispatcher() != NULL );
			// invoke mesh type-specific code
			loadMesh(params, mesh, body->getEngine()->getDispatcher());
			return mesh;
		}
		
		void cleanUp() {
		}
	};
}

#endif
