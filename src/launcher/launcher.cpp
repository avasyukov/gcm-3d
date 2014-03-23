#include "launcher.h"

#include <boost/lexical_cast.hpp>

#include "util/xml.h"

#include "loaders/material/AnisotropicElasticMaterialLoader.h"
#include "loaders/material/IsotropicElasticMaterialLoader.h"

#include "util/forms/StepPulseForm.h"
#include "mesh/Mesh.h"
#include "Engine.h"
#include "Utils.h"
#include "Logging.h"
#include "ContactCondition.h"

using namespace xml;
using boost::lexical_cast;

void launcher::loadSceneFromFile(Engine& engine, string fileName)
{
    USE_LOGGER;
    INIT_LOGGER("gcm.launcher.TaskLoader");
    // FIXME should we validate task file against xml schema?
    FileLookupService& fls =  engine.getFileLookupService();
    string fname = fls.lookupFile(fileName);
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
        int numberOfSnaps = lexical_cast<int>( getAttributeByName(taskNode.getAttributes(), "numberOfSnaps").c_str() );
        int stepsPerSnap = lexical_cast<int>( getAttributeByName(taskNode.getAttributes(), "stepsPerSnap").c_str() );
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
        string type = getAttributeByName(defaultContactCalculator.getAttributes(), "type");
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
        string type = getAttributeByName(defaultRheoCalculator.getAttributes(), "type");
        engine.setDefaultRheologyCalculatorType(type);
    }

    NodeList contactThresholdList = rootNode.xpath("/task/system/contactThreshold");
    if( contactThresholdList.size() > 1 )
        THROW_INVALID_INPUT("Config file can contain only one <contactThreshold/> element");
    if( contactThresholdList.size() == 1 )
    {
        xml::Node contactThreshold = contactThresholdList.front();
        string measure = getAttributeByName(contactThreshold.getAttributes(), "measure");
        gcm_real value = lexical_cast<gcm_real>(getAttributeByName(contactThreshold.getAttributes(), "value"));
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
        string isStatic = getAttributeByName(collisionDetector.getAttributes(), "static");
        if( isStatic == "true" )
        {
            engine.setCollisionDetectorStatic(true);
        }
        else if( isStatic == "false" )
        {
            engine.setCollisionDetectorStatic(false);
        }
    }

    // reading materials
    NodeList matNodes = rootNode.xpath("/task/materials/material");
    for(auto& matNode: matNodes)
    {
        string rheology = getAttributeByName(matNode.getAttributes(), "rheology");
        if (rheology == AnisotropicElasticMaterialLoader::RHEOLOGY_TYPE)
            engine.addMaterial(AnisotropicElasticMaterialLoader::getInstance().load(matNode));
        else if (rheology == IsotropicElasticMaterialLoader::RHEOLOGY_TYPE)
            engine.addMaterial(IsotropicElasticMaterialLoader::getInstance().load(matNode));
        else
            THROW_UNSUPPORTED("Unsupported rheology found: " + rheology);
    }

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
            Params params = Params(meshNode.getAttributes());
            if (!params.has("type"))
                THROW_INVALID_INPUT("Mesh type is not specified.");

            MeshLoader* meshLoader = engine.getMeshLoader(params["type"]);
            if (!meshLoader)
                THROW_INVALID_INPUT("Mesh loader not found.");

            LOG_INFO("Preparing mesh for body '" << id << "'");

            AABB localScene;
            int slicingDirection;
            int numberOfNodes;
            meshLoader->preLoadMesh(params, &localScene, slicingDirection, numberOfNodes);

            // transform meshes
            NodeList transformNodes = bodyNode.getChildrenByName("transform");
            for(auto& transformNode: transformNodes)
            {
                string transformType = getAttributeByName(transformNode.getAttributes(), "type");
                if( transformType == "translate" )
                {
                    gcm_real x = lexical_cast<gcm_real>(getAttributeByName(transformNode.getAttributes(), "moveX"));
                    gcm_real y = lexical_cast<gcm_real>(getAttributeByName(transformNode.getAttributes(), "moveY"));
                    gcm_real z = lexical_cast<gcm_real>(getAttributeByName(transformNode.getAttributes(), "moveZ"));
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
            string transformType = getAttributeByName(transformNode.getAttributes(), "type");
            if( transformType == "translate" )
            {
                dX += lexical_cast<gcm_real>(getAttributeByName(transformNode.getAttributes(), "moveX"));
                dY += lexical_cast<gcm_real>(getAttributeByName(transformNode.getAttributes(), "moveY"));
                dZ += lexical_cast<gcm_real>(getAttributeByName(transformNode.getAttributes(), "moveZ"));
            }
        }
        engine.getDispatcher()->setTransferVector(dX, dY, dZ, id);

        // load meshes
        NodeList meshNodes = bodyNode.getChildrenByName("mesh");
        for(auto& meshNode: meshNodes)
        {
            Params params = Params(meshNode.getAttributes());
            MeshLoader* meshLoader = engine.getMeshLoader(params["type"]);

            LOG_INFO("Loading mesh for body '" << id << "'");
            // use loader to load mesh
            Mesh* mesh = meshLoader->load(body, params);

            // attach mesh to body
            body->attachMesh(mesh);
            mesh->setBodyNum( engine.getBodyNum(id) );
            LOG_INFO("Mesh '" << mesh->getId() << "' of type '" <<  meshLoader->getType() << "' created. "
                        << "Number of nodes: " << mesh->getNodesNumber() << ".");
        }

        // transform meshes
        NodeList transformNodes = bodyNode.getChildrenByName("transform");
        for(auto& transformNode: transformNodes)
        {
            string transformType = getAttributeByName(transformNode.getAttributes(), "type");
            if( transformType == "translate" )
            {
                gcm_real x = lexical_cast<gcm_real>(getAttributeByName(transformNode.getAttributes(), "moveX"));
                gcm_real y = lexical_cast<gcm_real>(getAttributeByName(transformNode.getAttributes(), "moveY"));
                gcm_real z = lexical_cast<gcm_real>(getAttributeByName(transformNode.getAttributes(), "moveZ"));
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
            string id = getAttributeByName(matNode.getAttributes(), "id");
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
                string areaType = getAttributeByName(areaNodes.front().getAttributes(), "type");
                if( areaType == "box" )
                {
                    LOG_DEBUG("Material area: " << areaType);
                    gcm_real minX = lexical_cast<gcm_real>(getAttributeByName(areaNodes.front().getAttributes(), "minX"));
                    gcm_real maxX = lexical_cast<gcm_real>(getAttributeByName(areaNodes.front().getAttributes(), "maxX"));
                    gcm_real minY = lexical_cast<gcm_real>(getAttributeByName(areaNodes.front().getAttributes(), "minY"));
                    gcm_real maxY = lexical_cast<gcm_real>(getAttributeByName(areaNodes.front().getAttributes(), "maxY"));
                    gcm_real minZ = lexical_cast<gcm_real>(getAttributeByName(areaNodes.front().getAttributes(), "minZ"));
                    gcm_real maxZ = lexical_cast<gcm_real>(getAttributeByName(areaNodes.front().getAttributes(), "maxZ"));
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
            string areaType = getAttributeByName(areaNodes.front().getAttributes(), "type");
            if( areaType == "box" )
            {
                LOG_DEBUG("Initial state area: " << areaType);
                gcm_real minX = lexical_cast<gcm_real>(getAttributeByName(areaNodes[node].getAttributes(), "minX"));
                gcm_real maxX = lexical_cast<gcm_real>(getAttributeByName(areaNodes[node].getAttributes(), "maxX"));
                gcm_real minY = lexical_cast<gcm_real>(getAttributeByName(areaNodes[node].getAttributes(), "minY"));
                gcm_real maxY = lexical_cast<gcm_real>(getAttributeByName(areaNodes[node].getAttributes(), "maxY"));
                gcm_real minZ = lexical_cast<gcm_real>(getAttributeByName(areaNodes[node].getAttributes(), "minZ"));
                gcm_real maxZ = lexical_cast<gcm_real>(getAttributeByName(areaNodes[node].getAttributes(), "maxZ"));
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
