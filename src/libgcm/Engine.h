#ifndef GCM_ENGINE_H
#define GCM_ENGINE_H

#include <string>
#include <stdexcept>
#include <vector>
#include <mpi.h>
#include <limits>

#include "calc/volume/VolumeCalculator.h"
#include "calc/border/BorderCalculator.h"
#include "calc/contact/ContactCalculator.h"
#include "util/forms/PulseForm.h"
#include "util/areas/BoxArea.h"
#include "mesh/MeshLoader.h"
#include "snapshot/SnapshotWriter.h"
#include "method/NumericalMethod.h"
// FIXME - do we need it here?
#include "interpolator/TetrInterpolator.h"
#include "interpolator/TetrFirstOrderInterpolator.h"
#include "interpolator/TetrSecondOrderMinMaxInterpolator.h"
#include "interpolator/LineFirstOrderInterpolator.h"
//
#include "materials/Material.h"
#include "rheology/RheologyCalculator.h"
#include "rheology/DummyRheologyCalculator.h"
#include "GCMDispatcher.h"
#include "DummyDispatcher.h"
#include "DataBus.h"
#include "BorderCondition.h"
#include "ContactCondition.h"
#include "CollisionDetector.h"
#include "Logging.h"
#include "Body.h"
#include "Interfaces.h"
#include "Utils.h"

#define CONTACT_THRESHOLD_BY_AVG_H 0
#define CONTACT_THRESHOLD_BY_MAX_LT 1
#define CONTACT_THRESHOLD_FIXED 2

using namespace std;
using namespace gcm;

namespace gcm
{
    class VTKSnapshotWriter;
    class VTK2SnapshotWriter;
    /*
     * Main class to operate calculation scene.
     */
    class Engine: public IEngine {

        static Engine* engineInstance;

    protected:
        /*
         * File lookup service
         */
         FileFolderLookupService fls;
        /*
         * Engines counter.
         */
        static int enginesNumber;
        /*
         * Process rank in MPI communicator.
         */
        int rank;
        int numberOfWorkers;
        /*
         * Mesh loaders.
         */
        map<string, MeshLoader*> meshLoaders;
        /*
         * Snapshot writers.
         */
        map<string, SnapshotWriter*> snapshotWriters;
        /*
         * Numerical methods
         */
        map<string, NumericalMethod*> numericalMethods;
        /*
         * Volume calculators
         */
        map<string, VolumeCalculator*> volumeCalculators;
        /*
         * Border calculators
         */
        map<string, BorderCalculator*> borderCalculators;
        /*
         * Contact calculators
         */
        map<string, ContactCalculator*> contactCalculators;

        map<string, RheologyCalculator*> rheologyCalculators;

        // FIXME - tetr-specific is bad
        map<string, TetrInterpolator*> interpolators;

        vector<BorderCondition*> borderConditions;
        vector<ContactCondition*> contactConditions;

        vector<Material*> materials;
        /*
         * Scene bodies.
         */
        vector<Body*> bodies;

        gcm::GCMDispatcher* dispatcher;

        DataBus* dataBus;
        CollisionDetector* colDet;
        vector<CalcNode> virtNodes;

        //VTKSnapshotWriter* vtkSnapshotWriter;
        //VTK2SnapshotWriter* vtkDumpWriter;

        unsigned char contactThresholdType;
        float contactThresholdFactor;
        float fixedTimeStep;
        float currentTime;
        int currentTimeStep;

        int numberOfSnaps;
        int stepsPerSnap;

        AABB scene;

        string defaultRheoCalcType;

        float gmshVerbosity;

        /*
         * Logger.
         */
        USE_LOGGER;

    protected:
        /*
         * Engine is a singletone, so constructors are private
         * Limitation of current design: single threaded only
         */

        /*
         * Default constructor. Engine uses log4cxx as logging subsystem, so
         * it must be initialized *before* creating engine instance.
         */
        Engine();
        Engine(Engine const& copy);    // Not implemented
        Engine& operator=(Engine const& copy); //Not implemented
        /*
         * Destructor.
         */
        ~Engine();

    public:
        /*
         * Returns singletone engine instance
         */
        static Engine& getInstance();
        static void initInstance() { new Engine; }
        void clear();
        void cleanUp();
        /*
         * Returns process rank.
         */
        int getRank();
        int getNumberOfWorkers();
        int getNumberOfBodies();
        int getNumberOfMaterials();
        /*
         * Performs calculation of one time step. If step is omitted
         * then actual time step is determined automatically.
         */
        void doStep(float step = 0.0);
        /*
         * Registers new mesh loader. Out-of-box mesh loaders are
         * registered automatically at engine creation. Note, that if mesh
         * loader with the same type is registered already it will be replaced.
         */
        void registerMeshLoader(MeshLoader *meshLoader);
        /*
         * Registers new snapshot writer. Out-of-box snapshot writers are
         * registered automatically at engine creation. Note, that if snapshot
         * writer with the same type is registered already it will be replaced.
         */
        void registerSnapshotWriter(SnapshotWriter *snapshotWriter);
        /*
         * Registers new volume calculator. Out-of-box calculators are
         * registered automatically at engine creation. Note, that if calculator
         * with the same type is registered already it will be replaced.
         */
        void registerVolumeCalculator(VolumeCalculator *volumeCalculator);
        /*
         * Registers new border calculator. Out-of-box calculators are
         * registered automatically at engine creation. Note, that if calculator
         * with the same type is registered already it will be replaced.
         */
        void registerBorderCalculator(BorderCalculator *borderCalculator);
        /*
         * Registers new contact calculator. Out-of-box calculators are
         * registered automatically at engine creation. Note, that if calculator
         * with the same type is registered already it will be replaced.
         */
        void registerContactCalculator(ContactCalculator *contactCalculator);

        void registerNumericalMethod(NumericalMethod *numericalMethod);

        void registerInterpolator(TetrInterpolator *interpolator);

        void registerRheologyCalculator(RheologyCalculator *rheologyCalculator);

        unsigned int addBorderCondition(BorderCondition *borderCondition);
        void replaceDefaultBorderCondition(BorderCondition *borderCondition);

        unsigned int addContactCondition(ContactCondition *contactCondition);
        void replaceDefaultContactCondition(ContactCondition *contactCondition);

        unsigned char addMaterial(Material *material);

        void setDefaultRheologyCalculatorType(string calcType);
        string getDefaultRheologyCalculatorType();

        /*
         * Returns mesh loader by type or NULL if not found.
         */
        MeshLoader* getMeshLoader(string type);
        /*
         * Returns snapshot writer by type or NULL if not found.
         */
        SnapshotWriter* getSnapshotWriter(string type);

        NumericalMethod* getNumericalMethod(string type);
        VolumeCalculator* getVolumeCalculator(string type);
        BorderCalculator* getBorderCalculator(string type);
        ContactCalculator* getContactCalculator(string type);
        BorderCondition* getBorderCondition(unsigned int num);
        ContactCondition* getContactCondition(unsigned int num);
        TetrFirstOrderInterpolator* getFirstOrderInterpolator(string type);
        TetrSecondOrderMinMaxInterpolator* getSecondOrderInterpolator(string type);
        LineFirstOrderInterpolator* getFirstOrderLineInterpolator(string type);
        RheologyCalculator* getRheologyCalculator(string type);
        GCMDispatcher* getDispatcher();

        /*
         * Returns body object by its id or NULL if not found
         */
        Body* getBodyById(string id);
        int getBodyNum(string id);

        unsigned char getMaterialIndex(string name);
        Material* getMaterial(string name);
        Material* getMaterial(unsigned char index);

        Body* getBody(unsigned int num);
        /*
         * Adds new body to scene.
         */
        void addBody(Body *body);

        float calculateRecommendedTimeStep();
        float calculateRecommendedContactTreshold(float tau);
        void createSnapshot(int number);
        void createDump(int number);
        void doNextStep();
        void doNextStepBeforeStages (const float maxAllowedStep, float& actualTimeStep);
        void doNextStepStages(const float time_step);
        void doNextStepAfterStages (const float time_step);

        void setTimeStep(float dt);
        float getTimeStep();
        float getCurrentTime();
        void setCurrentTime(float time);
        void syncNodes();
        void syncOutlines();
        void calculate();
        void setNumberOfSnaps(int number);
        void setStepsPerSnap(int number);
        AABB getScene();
        void setScene(AABB src);
        void transferScene(float x, float y, float z);

        DataBus* getDataBus();
        CalcNode& getVirtNode(unsigned int i);

        FileFolderLookupService& getFileFolderLookupService();
        void setContactThresholdType(unsigned char type);
        unsigned char getContactThresholdType();
        void setContactThresholdFactor(float val);
        float getContactThresholdFactor();
        void setCollisionDetectorStatic(bool val);
        bool isCollisionDetectorStatic();

        float getGmshVerbosity();
        void setGmshVerbosity(float verbosity);

        bool interpolateNode(CalcNode& node);
    };
}

#endif
