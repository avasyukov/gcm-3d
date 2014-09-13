#ifndef GCM_ENGINE_H
#define GCM_ENGINE_H

#include <string>
#include <stdexcept>
#include <vector>
#include <mpi.h>
#include <limits>
#include <functional>

#include "libgcm/calc/volume/VolumeCalculator.hpp"
#include "libgcm/calc/border/BorderCalculator.hpp"
#include "libgcm/calc/contact/ContactCalculator.hpp"
#include "libgcm/util/forms/PulseForm.hpp"
#include "libgcm/util/areas/BoxArea.hpp"
#include "libgcm/snapshot/SnapshotWriter.hpp"
#include "libgcm/method/NumericalMethod.hpp"
// FIXME - do we need it here?
#include "libgcm/interpolator/TetrInterpolator.hpp"
#include "libgcm/interpolator/TetrFirstOrderInterpolator.hpp"
#include "libgcm/interpolator/TetrSecondOrderMinMaxInterpolator.hpp"
#include "libgcm/interpolator/LineFirstOrderInterpolator.hpp"

#include "libgcm/failure/FailureModel.hpp"
#include "libgcm/failure/NoFailureModel.hpp"
#include "libgcm/failure/CrackFailureModel.hpp"
#include "libgcm/failure/DebugFailureModel.hpp"

//
#include "libgcm/rheology/Material.hpp"
#include "libgcm/rheology/RheologyCalculator.hpp"
#include "libgcm/rheology/DummyRheologyCalculator.hpp"
#include "libgcm/GCMDispatcher.hpp"
#include "libgcm/DummyDispatcher.hpp"
#include "libgcm/DataBus.hpp"
#include "libgcm/BorderCondition.hpp"
#include "libgcm/ContactCondition.hpp"
#include "libgcm/CollisionDetector.hpp"
#include "libgcm/Logging.hpp"
#include "libgcm/Body.hpp"
#include "libgcm/Interfaces.hpp"
#include "libgcm/util/Singleton.hpp"

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
    class Engine: public IEngine, public  Singleton<Engine>
    {
    friend class Singleton;
    protected:
        /*
         * Process rank in MPI communicator.
         */
        int rank;
        int numberOfWorkers;
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
		
		map<string, FailureModel*> failureModels;

        // FIXME - tetr-specific is bad
        map<string, TetrInterpolator*> interpolators;

        vector<BorderCondition*> borderConditions;
        vector<ContactCondition*> contactConditions;

        vector<MaterialPtr> materials;
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
        float timeStepMultiplier;
        float currentTime;
        int currentTimeStep;

        int numberOfSnaps;
        int stepsPerSnap;

        bool meshesMovable;

        AABB scene;

        string defaultRheoCalcType;
        string defaultFailureModelType;

        float gmshVerbosity;

        /*
         * Logger.
         */
        USE_LOGGER;

        vector<tuple<unsigned int, string, string>> snapshots;
        vector<float> snapshotTimestamps;

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

        void registerFailureModel(FailureModel *model);
		FailureModel* getFailureModel(string modelType);
        void setDefaultFailureModelType(string modelType);
        string getDefaultFailureModelType();

        unsigned int addBorderCondition(BorderCondition *borderCondition);
        void replaceDefaultBorderCondition(BorderCondition *borderCondition);

        unsigned int addContactCondition(ContactCondition *contactCondition);
        void replaceDefaultContactCondition(ContactCondition *contactCondition);

        unsigned char addMaterial(MaterialPtr material);

        void setDefaultRheologyCalculatorType(string calcType);
        string getDefaultRheologyCalculatorType();
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
        const MaterialPtr& getMaterial(string name);
        const MaterialPtr& getMaterial(unsigned char index);

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
        void setTimeStepMultiplier(float m);
        float getTimeStepMultiplier();
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
		void scaleScene(float x0, float y0, float z0, 
				float scaleX, float scaleY, float scaleZ);

        DataBus* getDataBus();
        CalcNode& getVirtNode(unsigned int i);

        void setContactThresholdType(unsigned char type);
        unsigned char getContactThresholdType();
        void setContactThresholdFactor(float val);
        float getContactThresholdFactor();
        void setCollisionDetectorStatic(bool val);
        bool isCollisionDetectorStatic();
        void setMeshesMovable(bool val);
        bool getMeshesMovable();

        float getGmshVerbosity();
        void setGmshVerbosity(float verbosity);

        bool interpolateNode(CalcNode& node);

        void setRheologyMatrices(function<RheologyMatrixPtr (const CalcNode&)> getMatrixForNode);

        const vector<tuple<unsigned int, string, string>>& getSnapshotsList() const;
        const vector<float>& getSnapshotTimestamps() const;
    };
}

#endif
