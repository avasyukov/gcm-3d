#ifndef GCM_ENGINE_H
#define GCM_ENGINE_H

#include <string>
#include <map>
#include <tuple>
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
#include "libgcm/method/NumericalMethod.hpp"
// FIXME - do we need it here?
#include "libgcm/interpolator/TetrInterpolator.hpp"
#include "libgcm/interpolator/TetrFirstOrderInterpolator.hpp"
#include "libgcm/interpolator/TetrSecondOrderMinMaxInterpolator.hpp"
#include "libgcm/interpolator/LineFirstOrderInterpolator.hpp"

#include "libgcm/failure/FailureModel.hpp"
#include "libgcm/failure/NoFailureModel.hpp"
#include "libgcm/failure/CrackFailureModel.hpp"
#include "libgcm/failure/ScalarFailureModel.hpp"
#include "libgcm/failure/ContinualFailureModel.hpp"
#include "libgcm/failure/DebugFailureModel.hpp"
#include "libgcm/failure/HashinFailureModel.hpp"
#include "libgcm/failure/TsaiHillFailureModel.hpp"
#include "libgcm/failure/TsaiWuFailureModel.hpp"
#include "libgcm/failure/DruckerPragerFailureModel.hpp"
#include "libgcm/failure/PuckFailureModel.hpp"

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
#include "libgcm/util/Singleton.hpp"

#define CONTACT_THRESHOLD_BY_AVG_H 0
#define CONTACT_THRESHOLD_BY_MAX_LT 1
#define CONTACT_THRESHOLD_FIXED 2

namespace gcm
{
	class ContactCalculator;
	class ContactCondition;
    /*
     * Main class to operate calculation scene.
     */
    class Engine: public Singleton<Engine>
    {
     public:
        class Options
        {
         private:
            Options();
         public:
            const static std::string SNAPSHOT_OUTPUT_PATH_PATTERN;
        };

    friend class Singleton;
    protected:
        std::map<std::string, std::string> options;
        /*
         * Process rank in MPI communicator.
         */
        int rank;
        int numberOfWorkers;
        /*
         * Numerical methods
         */
        std::map<std::string, NumericalMethod*> numericalMethods;
        /*
         * Volume calculators
         */
        std::map<std::string, VolumeCalculator*> volumeCalculators;
        /*
         * Border calculators
         */
        std::map<std::string, BorderCalculator*> borderCalculators;
        /*
         * Contact calculators
         */
        std::map<std::string, ContactCalculator*> contactCalculators;

        std::map<std::string, RheologyCalculator*> rheologyCalculators;
		
		std::map<std::string, FailureModel*> failureModels;

        // FIXME - tetr-specific is bad
        std::map<std::string, TetrInterpolator*> interpolators;

        std::vector<BorderCondition*> borderConditions;
        std::vector<ContactCondition*> contactConditions;

        std::vector<MaterialPtr> materials;
        /*
         * Scene bodies.
         */
        std::vector<Body*> bodies;

        gcm::GCMDispatcher* dispatcher;

        DataBus* dataBus;
        CollisionDetector* colDet;
        std::vector<CalcNode> virtNodes;

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

        std::string defaultRheoCalcType;
        std::string defaultFailureModelType;

        float gmshVerbosity;

        /*
         * Logger.
         */
        USE_LOGGER;

        std::vector<std::tuple<unsigned int, std::string, std::string>> snapshots;
        std::vector<float> snapshotTimestamps;

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
		
		/**
		 * Decide what collision detector to use:
		 * When all meshes are cubic, use static
         */
		void determineTypeOfCollisionDetector();

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
		FailureModel* getFailureModel(std::string modelType);
        void setDefaultFailureModelType(std::string modelType);
        std::string getDefaultFailureModelType();

        unsigned int addBorderCondition(BorderCondition *borderCondition);
        void replaceDefaultBorderCondition(BorderCondition *borderCondition);

        unsigned int addContactCondition(ContactCondition *contactCondition);
        void replaceDefaultContactCondition(ContactCondition *contactCondition);

        unsigned char addMaterial(MaterialPtr material);

        void setDefaultRheologyCalculatorType(std::string calcType);
        std::string getDefaultRheologyCalculatorType();

        NumericalMethod* getNumericalMethod(std::string type);
        VolumeCalculator* getVolumeCalculator(std::string type);
        BorderCalculator* getBorderCalculator(std::string type);
        ContactCalculator* getContactCalculator(std::string type);
        BorderCondition* getBorderCondition(unsigned int num);
        ContactCondition* getContactCondition(unsigned int num);
        TetrFirstOrderInterpolator* getFirstOrderInterpolator(std::string type);
        TetrSecondOrderMinMaxInterpolator* getSecondOrderInterpolator(std::string type);
        LineFirstOrderInterpolator* getFirstOrderLineInterpolator(std::string type);
        RheologyCalculator* getRheologyCalculator(std::string type);
        GCMDispatcher* getDispatcher();

        /*
         * Returns body object by its id or NULL if not found
         */
        Body* getBodyById(std::string id);
        int getBodyNum(std::string id);

        unsigned char getMaterialIndex(std::string name);
        const MaterialPtr& getMaterial(std::string name);
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
        void calculate(bool save_snapshots = true);
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

        void setRheologyMatrices(std::function<RheologyMatrixPtr (const CalcNode&)> getMatrixForNode);

        const std::vector<std::tuple<unsigned int, std::string, std::string>>& getSnapshotsList() const;
        const std::vector<float>& getSnapshotTimestamps() const;

        void setOption(std::string option, std::string value);
        const std::string& getOption(std::string option) const;
        bool hasOption(std::string option) const;
    };
}

#endif
