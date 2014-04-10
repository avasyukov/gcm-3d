#include "launcher/launcher.hpp"

#include <boost/algorithm/string/predicate.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/filesystem.hpp>

#include "launcher/loaders/material/AnisotropicElasticMaterialLoader.hpp"
#include "launcher/loaders/material/IsotropicElasticMaterialLoader.hpp"
#include "launcher/loaders/mesh/Geo2MeshLoader.hpp"
#include "launcher/loaders/mesh/Msh2MeshLoader.hpp"
#include "launcher/loaders/mesh/Vtu2MeshLoader.hpp"
#include "launcher/loaders/mesh/Vtu2MeshZoneLoader.hpp"
#include "launcher/loaders/mesh/MarkeredBoxMeshLoader.hpp"
#include "launcher/loaders/mesh/CubicMeshLoader.hpp"
#include "launcher/util/FileFolderLookupService.hpp"

#include "libgcm/util/forms/StepPulseForm.hpp"
#include "libgcm/mesh/Mesh.hpp"
#include "libgcm/Engine.hpp"
#include "libgcm/Logging.hpp"
#include "libgcm/ContactCondition.hpp"
#include "libgcm/Exception.hpp"

namespace ba = boost::algorithm;
namespace bfs = boost::filesystem;

using namespace xml;
using boost::lexical_cast;


launcher::Launcher::Launcher()
{
    INIT_LOGGER("gcm.launcher");
}

void launcher::Launcher::loadMaterialsFromXml(NodeList matNodes)
{
    gcm::Engine& engine = gcm::Engine::getInstance();
    for(auto& matNode: matNodes)
    {
        string rheology = matNode["rheology"];
        Material* mat = nullptr;
        if (rheology == AnisotropicElasticMaterialLoader::RHEOLOGY_TYPE)
            mat = AnisotropicElasticMaterialLoader::getInstance().load(matNode);
        else if (rheology == IsotropicElasticMaterialLoader::RHEOLOGY_TYPE)
            mat = IsotropicElasticMaterialLoader::getInstance().load(matNode);
        else
            THROW_UNSUPPORTED("Unsupported rheology found: " + rheology);
        try
        {
            engine.getMaterial(mat->getName());
            LOG_WARN("Material \"" << mat->getName() << "\" already loaded. Ignoring duplicate.");
        }
        catch (gcm::Exception& e)
        {
            engine.addMaterial(mat);
        }
    }
}

void launcher::Launcher::loadMaterialLibrary(std::string path)
{
    LOG_INFO("Loading material library from " << path);

    bfs::recursive_directory_iterator dir(path), end;
    for (; dir != end; dir++)
    {
        if (bfs::is_regular_file(dir->path()))
            if (ba::iends_with(dir->path().string(), ".xml"))
            {
                string fname = dir->path().string();
                LOG_DEBUG("Loading material library item from " << fname);
                Doc doc = Doc::fromFile(fname);
                loadMaterialsFromXml(doc.getRootElement().xpath("/materials/material"));
            }
    }
}

void launcher::Launcher::loadSceneFromFile(string fileName)
{
    Engine& engine = gcm::Engine::getInstance();

    // FIXME should we validate task file against xml schema?
    auto& ffls = FileFolderLookupService::getInstance();
    string fname = ffls.lookupFile(fileName);
    LOG_DEBUG("Loading scene from file " << fname);
    // parse file
    Doc doc = Doc::fromFile(fname);
    xml::Node rootNode = doc.getRootElement();
    // read task parameters
    NodeList taskNodes = rootNode.xpath("/task");
    if( taskNodes.size() != 1 )
        THROW_INVALID_INPUT("Config file should contain one <task/> element");
    for(auto& taskNode: taskNodes)
    {
        int numberOfSnaps = lexical_cast<int>(taskNode["numberOfSnaps"]);
        int stepsPerSnap = lexical_cast<int>(taskNode["stepsPerSnap"]);
        engine.setNumberOfSnaps(numberOfSnaps);
        engine.setStepsPerSnap(stepsPerSnap);
    }

    // reading system properties
    NodeList defaultContactCalculatorList = rootNode.xpath("/task/system/defaultContactCalculator");
    if( defaultContactCalculatorList.size() > 1 )
        THROW_INVALID_INPUT("Config file can contain only one <defaultContactCalculator/> element");
    if( defaultContactCalculatorList.size() == 1 )
    {
        xml::Node defaultContactCalculator = defaultContactCalculatorList.front();
        string type = defaultContactCalculator["type"];
        if( type == "SlidingContactCalculator" )
        {
            engine.replaceDefaultContactCondition( new ContactCondition(
                            NULL, new StepPulseForm(-1, -1),
                            engine.getContactCalculator("SlidingContactCalculator") ) );
        }
        else if( type == "AdhesionContactCalculator" )
        {
            engine.replaceDefaultContactCondition( new ContactCondition(
                            NULL, new StepPulseForm(-1, -1),
                            engine.getContactCalculator("AdhesionContactCalculator") ) );
        }
        else if (type == "AdhesionContactDestroyCalculator")
        {
            engine.replaceDefaultContactCondition( new ContactCondition( NULL, new StepPulseForm(-1,-1), engine.getContactCalculator("AdhesionContactDestroyCalculator")));
        }
    }

    NodeList defaultRheoCalculatorList = rootNode.xpath("/task/system/defaultRheologyCalculator");
    if( defaultRheoCalculatorList.size() > 1 )
        THROW_INVALID_INPUT("Config file can contain only one <defaultRheologyCalculator/> element");
    if( defaultRheoCalculatorList.size() == 1 )
    {
        xml::Node defaultRheoCalculator = defaultRheoCalculatorList.front();
        string type = defaultRheoCalculator["type"];
        engine.setDefaultRheologyCalculatorType(type);
    }

    NodeList contactThresholdList = rootNode.xpath("/task/system/contactThreshold");
    if( contactThresholdList.size() > 1 )
        THROW_INVALID_INPUT("Config file can contain only one <contactThreshold/> element");
    if( contactThresholdList.size() == 1 )
    {
        xml::Node contactThreshold = contactThresholdList.front();
        string measure = contactThreshold["measure"];
        gcm_real value = lexical_cast<gcm_real>(contactThreshold["value"]);
        if( measure == "avgH" )
        {
            engine.setContactThresholdType(CONTACT_THRESHOLD_BY_AVG_H);
            engine.setContactThresholdFactor(value);
        }
        else if( measure == "lambdaTau" )
        {
            engine.setContactThresholdType(CONTACT_THRESHOLD_BY_MAX_LT);
            engine.setContactThresholdFactor(value);
        }
        else if( measure == "abs" )
        {
            engine.setContactThresholdType(CONTACT_THRESHOLD_FIXED);
            engine.setContactThresholdFactor(value);
        }
        else
        {
            THROW_INVALID_INPUT("Unknown units of measure for <contactThreshold/>");
        }
    }

    NodeList collisionDetectorList = rootNode.xpath("/task/system/collisionDetector");
    if( collisionDetectorList.size() > 1 )
        THROW_INVALID_INPUT("Config file can contain only one <collisionDetector/> element");
    if( collisionDetectorList.size() == 1 )
    {
        xml::Node collisionDetector = collisionDetectorList.front();
        string isStatic = collisionDetector["static"];
        if( isStatic == "true" )
        {
            engine.setCollisionDetectorStatic(true);
        }
        else if( isStatic == "false" )
        {
            engine.setCollisionDetectorStatic(false);
        }
    }
    
    NodeList meshMovementList = rootNode.xpath("/task/system/meshMovement");
    if( meshMovementList.size() > 1 )
        THROW_INVALID_INPUT("Config file can contain only one <meshMovement/> element");
    if( meshMovementList.size() == 1 )
    {
        xml::Node meshMovement = meshMovementList.front();
        string meshMovementType = meshMovement["type"];
        if( meshMovementType == "none" )
        {
            engine.setMeshesMovable(false);
        }
    }

    string anisotropicMatrixImplementation = "numerical";
    NodeList anisotropicMatrixList = rootNode.xpath("/task/system/anisotropicMatrix");
    if( anisotropicMatrixList.size() > 1 )
        THROW_INVALID_INPUT("Config file can contain only one <anisotropicMatrix/> element");
    if( anisotropicMatrixList.size() == 1 )
    {
        xml::Node anisotropicMatrix = anisotropicMatrixList.front();
        anisotropicMatrixImplementation = anisotropicMatrix["implementation"];
    }

    AnisotropicElasticMaterialLoader::getInstance(anisotropicMatrixImplementation);

    loadMaterialLibrary("materials");
    
    // reading materials
    loadMaterialsFromXml(rootNode.xpath("/task/materials/material"));


    AABB globalScene;

    // search for bodies
    NodeList bodyNodes = rootNode.xpath("/task/bodies/body");

    // prepare basic bodies parameters
    for(auto& bodyNode: bodyNodes)
    {
        string id = bodyNode.getAttributes()["id"];
        LOG_DEBUG("Loading body '" << id << "'");
        // create body instance
        Body* body = new Body(id);
        body->setRheologyCalculatorType(engine.getDefaultRheologyCalculatorType());
        body->setEngine(engine);
        // set rheology
        NodeList rheologyNodes = bodyNode.getChildrenByName("rheology");
        if (rheologyNodes.size() > 1)
            THROW_INVALID_INPUT("Only one rheology element allowed for body declaration");
        if (rheologyNodes.size()) {
            // We can do smth here when we have more than one rheology calculators
        }

        // preload meshes for dispatcher
        NodeList meshNodes = bodyNode.getChildrenByName("mesh");
        for(auto& meshNode: meshNodes)
        {         
            string type = meshNode["type"];

            LOG_INFO("Preparing mesh for body '" << id << "'");

            AABB localScene;
            int slicingDirection;
            int numberOfNodes;

            if (type == Geo2MeshLoader::MESH_TYPE)
                Geo2MeshLoader::getInstance().preLoadMesh(meshNode, localScene, slicingDirection, numberOfNodes);
            else if (type == Msh2MeshLoader::MESH_TYPE)
                Msh2MeshLoader::getInstance().preLoadMesh(meshNode, localScene, slicingDirection, numberOfNodes);
            else if (type == Vtu2MeshLoader::MESH_TYPE)
                Vtu2MeshLoader::getInstance().preLoadMesh(meshNode, localScene, slicingDirection, numberOfNodes);
            else if (type == Vtu2MeshZoneLoader::MESH_TYPE)
                Vtu2MeshZoneLoader::getInstance().preLoadMesh(meshNode, localScene, slicingDirection, numberOfNodes);
            else if (type == MarkeredBoxMeshLoader::MESH_TYPE)
                MarkeredBoxMeshLoader::getInstance().preLoadMesh(meshNode, localScene, slicingDirection, numberOfNodes);
            else if (type == CubicMeshLoader::MESH_TYPE)
                CubicMeshLoader::getInstance().preLoadMesh(meshNode, localScene, slicingDirection, numberOfNodes);
            else
                THROW_UNSUPPORTED("Specified mesh loader is not supported");

            // transform meshes
            NodeList transformNodes = bodyNode.getChildrenByName("transform");
            for(auto& transformNode: transformNodes)
            {
                string transformType = transformNode["type"];
                if( transformType == "translate" )
                {
                    gcm_real x = lexical_cast<gcm_real>(transformNode["moveX"]);
                    gcm_real y = lexical_cast<gcm_real>(transformNode["moveY"]);
                    gcm_real z = lexical_cast<gcm_real>(transformNode["moveZ"]);
                    LOG_DEBUG("Moving body: [" << x << "; " << y << "; " << z << "]");
                    localScene.transfer(x, y, z);
                }
            }
            LOG_DEBUG("Mesh preloaded. Mesh size: " << localScene << " Number of nodes: " << numberOfNodes);

            engine.getDispatcher()->addBodyOutline(id, localScene);
            engine.getDispatcher()->addBodySlicingDirection(id, slicingDirection);
            engine.getDispatcher()->addBodyNodesNumber(id, numberOfNodes);

            if( isinf(globalScene.maxX) )
            {
                globalScene = localScene;
            }
            else
            {
                for( int k = 0; k < 3; k++ )
                {
                    if( globalScene.min_coords[k] > localScene.min_coords[k] )
                        globalScene.min_coords[k] = localScene.min_coords[k];
                    if( globalScene.max_coords[k] < localScene.max_coords[k] )
                        globalScene.max_coords[k] = localScene.max_coords[k];
                }
            }
        }

        // add body to scene
        engine.addBody(body);
    }

    engine.setScene(globalScene);
    LOG_DEBUG("Total scene: " << engine.getScene());

    // run dispatcher
    engine.getDispatcher()->prepare(engine.getNumberOfWorkers(), &globalScene);
    engine.getDataBus()->syncOutlines();
    for( int i = 0; i < engine.getNumberOfWorkers(); i++)
    {
        LOG_DEBUG("Area scheduled for worker " << i << ": " << *(engine.getDispatcher()->getOutline(i)));
    }

    // read meshes for all bodies
    for(auto& bodyNode: bodyNodes)
    {
        string id = bodyNode.getAttributes()["id"];
        LOG_DEBUG("Loading meshes for body '" << id << "'");
        // get body instance
        Body* body = engine.getBodyById(id);

        // FIXME - WA - we need this to determine isMine() correctly for moved points
        gcm_real dX = 0;
        gcm_real dY = 0;
        gcm_real dZ = 0;
        NodeList tmpTransformNodes = bodyNode.getChildrenByName("transform");
        for(auto& transformNode: tmpTransformNodes)
        {
            string transformType = transformNode["type"];
            if( transformType == "translate" )
            {
                dX += lexical_cast<gcm_real>(transformNode["moveX"]);
                dY += lexical_cast<gcm_real>(transformNode["moveY"]);
                dZ += lexical_cast<gcm_real>(transformNode["moveZ"]);
            }
        }
        engine.getDispatcher()->setTransferVector(dX, dY, dZ, id);

        // load meshes
        NodeList meshNodes = bodyNode.getChildrenByName("mesh");
        for(auto& meshNode: meshNodes)
        {
            LOG_INFO("Loading mesh for body '" << id << "'");

            string type = meshNode["type"];

            Mesh* mesh = nullptr;

            if (type == Geo2MeshLoader::MESH_TYPE)
                mesh = Geo2MeshLoader::getInstance().load(meshNode, body);
            else if (type == Msh2MeshLoader::MESH_TYPE)
                mesh = Msh2MeshLoader::getInstance().load(meshNode, body);
            else if (type == Vtu2MeshLoader::MESH_TYPE)
                mesh = Vtu2MeshLoader::getInstance().load(meshNode, body);
            else if (type == Vtu2MeshZoneLoader::MESH_TYPE)
                mesh = Vtu2MeshZoneLoader::getInstance().load(meshNode, body);
            else if (type == MarkeredBoxMeshLoader::MESH_TYPE)
                mesh = MarkeredBoxMeshLoader::getInstance().load(meshNode, body);
            else if (type == CubicMeshLoader::MESH_TYPE)
                mesh = CubicMeshLoader::getInstance().load(meshNode, body);

            // attach mesh to body
            body->attachMesh(mesh);
            mesh->setBodyNum( engine.getBodyNum(id) );
            LOG_INFO("Mesh '" << mesh->getId() << "' of type '" <<  type << "' created. "
                        << "Number of nodes: " << mesh->getNodesNumber() << ".");
        }

        // transform meshes
        NodeList transformNodes = bodyNode.getChildrenByName("transform");
        for(auto& transformNode: transformNodes)
        {
            string transformType = transformNode["type"];
            if( transformType == "translate" )
            {
                gcm_real x = lexical_cast<gcm_real>(transformNode["moveX"]);
                gcm_real y = lexical_cast<gcm_real>(transformNode["moveY"]);
                gcm_real z = lexical_cast<gcm_real>(transformNode["moveZ"]);
                LOG_DEBUG("Moving body: [" << x << "; " << y << "; " << z << "]");
                body->getMeshes()->transfer(x, y, z);
            }
        }

        // FIXME - Part of the WA above
        if( engine.getNumberOfWorkers() != 1 )
            engine.getDispatcher()->setTransferVector(/*-dX, -dY, -dZ,*/0, 0, 0, id);

        // set material properties
        NodeList matNodes = bodyNode.getChildrenByName("material");
        if (matNodes.size() < 1)
            THROW_INVALID_INPUT("Material not set");
        for(auto& matNode: matNodes)
        {
            string id = matNode["id"];
            // FIXME this code seems to be dead
            //Material* mat = engine.getMaterial(id);
            Mesh* mesh = body->getMeshes();

            NodeList areaNodes = matNode.getChildrenByName("area");
            if (areaNodes.size() == 0)
            {
                mesh->setRheology( engine.getMaterialIndex(id) );
            }
            else if (areaNodes.size() == 1)
            {
                Area* matArea = NULL;
                string areaType = areaNodes.front()["type"];
                if( areaType == "box" )
                {
                    LOG_DEBUG("Material area: " << areaType);
                    gcm_real minX = lexical_cast<gcm_real>(areaNodes.front()["minX"]);
                    gcm_real maxX = lexical_cast<gcm_real>(areaNodes.front()["maxX"]);
                    gcm_real minY = lexical_cast<gcm_real>(areaNodes.front()["minY"]);
                    gcm_real maxY = lexical_cast<gcm_real>(areaNodes.front()["maxY"]);
                    gcm_real minZ = lexical_cast<gcm_real>(areaNodes.front()["minZ"]);
                    gcm_real maxZ = lexical_cast<gcm_real>(areaNodes.front()["maxZ"]);
                    LOG_DEBUG("Box size: [" << minX << ", " << maxX << "] "
                                        << "[" << minY << ", " << maxY << "] "
                                        << "[" << minZ << ", " << maxZ << "]");
                    matArea = new BoxArea(minX, maxX, minY, maxY, minZ, maxZ);
                } else {
                    LOG_WARN("Unknown initial state area: " << areaType);
                }
                mesh->setRheology( engine.getMaterialIndex(id), matArea );
            }
            else
            {
                THROW_INVALID_INPUT("Only one or zero area elements are allowed for material");
            }
        }
        LOG_DEBUG("Body '" << id << "' loaded");
    }

    // FIXME - rewrite this indian style code
    NodeList initialStateNodes = rootNode.xpath("/task/initialState");
    for(auto& initialStateNode: initialStateNodes)
    {
        NodeList areaNodes = initialStateNode.getChildrenByName("area");
            NodeList valuesNodes = initialStateNode.getChildrenByName("values");
            if (areaNodes.size() != valuesNodes.size())
                //      THROW_INVALID_INPUT("Only one area element allowed for initial state");
                        THROW_INVALID_INPUT("Number of areas don't coincide with number of values");

        Area* stateArea = NULL;
        gcm_real values[9];
        if (areaNodes.size() != valuesNodes.size())
        //    THROW_INVALID_INPUT("Only one area element allowed for initial state");
            THROW_INVALID_INPUT("Number of areas don't coincide with number of values");
        for (unsigned int node=0; node<areaNodes.size(); node++)
    {
            string areaType = areaNodes.front()["type"];
            if( areaType == "box" )
            {
                LOG_DEBUG("Initial state area: " << areaType);
                gcm_real minX = lexical_cast<gcm_real>(areaNodes[node]["minX"]);
                gcm_real maxX = lexical_cast<gcm_real>(areaNodes[node]["maxX"]);
                gcm_real minY = lexical_cast<gcm_real>(areaNodes[node]["minY"]);
                gcm_real maxY = lexical_cast<gcm_real>(areaNodes[node]["maxY"]);
                gcm_real minZ = lexical_cast<gcm_real>(areaNodes[node]["minZ"]);
                gcm_real maxZ = lexical_cast<gcm_real>(areaNodes[node]["maxZ"]);
                LOG_DEBUG("Box size: [" << minX << ", " << maxX << "] "
                                    << "[" << minY << ", " << maxY << "] "
                                    << "[" << minZ << ", " << maxZ << "]");
                stateArea = new BoxArea(minX, maxX, minY, maxY, minZ, maxZ);
            } else {
                LOG_WARN("Unknown initial state area: " << areaType);
            }
        //if (valuesNodes.size() > 1)
        //    THROW_INVALID_INPUT("Only one values element allowed for initial state");
            memset(values, 0, 9*sizeof(gcm_real));
            string vx = valuesNodes[node].getAttributes()["vx"];
            if( !vx.empty() )
                values[0] = lexical_cast<gcm_real>(vx);
            string vy = valuesNodes[node].getAttributes()["vy"];
            if( !vy.empty() )
                values[1] = lexical_cast<gcm_real>(vy);
            string vz = valuesNodes[node].getAttributes()["vz"];
            if( !vz.empty() )
                values[2] = lexical_cast<gcm_real>(vz);
            string sxx = valuesNodes[node].getAttributes()["sxx"];
            if( !sxx.empty() )
                values[3] = lexical_cast<gcm_real>(sxx);
            string sxy = valuesNodes[node].getAttributes()["sxy"];
            if( !sxy.empty() )
                values[4] = lexical_cast<gcm_real>(sxy);
            string sxz = valuesNodes[node].getAttributes()["sxz"];
            if( !sxz.empty() )
                values[5] = lexical_cast<gcm_real>(sxz);
            string syy = valuesNodes[node].getAttributes()["syy"];
            if( !syy.empty() )
                values[6] = lexical_cast<gcm_real>(syy);
            string syz = valuesNodes[node].getAttributes()["syz"];
            if( !syz.empty() )
                values[7] = lexical_cast<gcm_real>(syz);
            string szz = valuesNodes[node].getAttributes()["szz"];
            if( !szz.empty() )
                values[8] = lexical_cast<gcm_real>(szz);
            LOG_DEBUG("Initial state values: "
                            << values[0] << " " << values[1] << " " << values[2] << " "
                            << values[3] << " " << values[4] << " " << values[5] << " "
                            << values[6] << " " << values[7] << " " << values[8] );
        for( int i = 0; i < engine.getNumberOfBodies(); i++ )
        {
            engine.getBody(i)->setInitialState(stateArea, values);
            engine.getBody(i)->getMeshes()->processStressState();
        }
    }
    }
    LOG_DEBUG("Scene loaded");
}
