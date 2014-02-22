#include <cmath>
#include <cstdlib>
#include <string>

#include <irrlicht.h>
#include <vector3d.h>

#include "mesh/markers/MarkeredMesh.h"
#include "mesh/markers/MarkeredBoxMeshGenerator.h"
#include "Utils.h"
 
using namespace std;
using namespace irr;
using namespace irr::core;
using namespace irr::scene;
using namespace irr::video;

class MarkeredBoxMeshGeneratorWrapper: public MarkeredBoxMeshGenerator {
public:
	void generate(MarkeredMesh* mesh, std::string mesh_edge, std::string cube_edge, std::string num) {
		Params p;
		p[MarkeredBoxMeshGenerator::PARAM_CUBE_EDGE] = cube_edge;
		p[MarkeredBoxMeshGenerator::PARAM_MESH_EDGE] = mesh_edge;
		p[MarkeredBoxMeshGenerator::PARAM_CELL_NUM] = num;

		loadMesh(p, mesh, nullptr);
	}
};


class GEngine : public IEventReceiver
{
protected:
    IMeshSceneNode* cubes[8];
    IrrlichtDevice *device;
    ICameraSceneNode *camera;
    IVideoDriver* driver;
    ISceneManager* scenemgr;

    MarkeredBoxMeshGeneratorWrapper mg;
    MarkeredMesh m;

    f32 phi = 0;
    f32 theta = M_PI/2;
    f32 r = 5;

    float nx, ny, nz;
public:
    GEngine() {
        device = createDevice( EDT_OPENGL, dimension2d<u32>(640,480));

        device->setEventReceiver(this);

        scenemgr = device->getSceneManager();
        driver = device->getVideoDriver();

        camera = scenemgr->addCameraSceneNode(0, vector3df(5, 5, 5), vector3df(0,0,0));

        int n = 0;
        for (int k = 0; k <= 1; k++)
            for (int j = 0; j <= 1; j++)
                for (int i = 0; i <= 1; i++) {
                    cubes[n] = scenemgr->addCubeSceneNode(1,0,-1,vector3df(-0.5+i,-0.5+j,-0.5+k),vector3df(0,0,0),vector3df(1,1,1));
                    cubes[n]->setMaterialFlag(EMF_LIGHTING, false);
                    scenemgr->getMeshManipulator()->setVertexColors(cubes[n]->getMesh(), SColor(255, 0, 0, 100*(1+1.5*n/7.0)));
                    n++;
                }

        scenemgr->addSphereSceneNode(0.1)->setMaterialFlag(EMF_LIGHTING, false);

        updateCamPosition();
        camera->setUpVector(vector3df(0,0,1));

        mg.generate(&m, "3.0", "1.0", "2");

        for (int i = 0; i < 8; i++)
        	m.setCellInnerFlag(i, true);

        m.getNodeByLocalIndex(13).setIsBorder(true);

        updateNormal();
    }

    virtual bool OnEvent(const SEvent& event)
    {
        if (event.EventType == EET_KEY_INPUT_EVENT && event.KeyInput.PressedDown) {
            if (event.KeyInput.Key >= '1' && event.KeyInput.Key <= '8') {
            	auto n = event.KeyInput.Key-'1';
                auto cube = cubes[n];
                cube->setVisible(!cube->isVisible());
                m.setCellInnerFlag(n, !m.getCellInnerFlag(n));
                updateNormal();
            } else if (event.KeyInput.Key == KEY_UP) {
                 if (theta > M_PI/15) 
                    theta -= M_PI/16;
                updateCamPosition();
            }
            else if (event.KeyInput.Key == KEY_DOWN) {
                if (theta < 14*M_PI/15) 
                    theta += M_PI/16;
                updateCamPosition();
            } else if (event.KeyInput.Key == KEY_LEFT) {
                phi += M_PI/16;
                updateCamPosition();
            } else if (event.KeyInput.Key == KEY_RIGHT) {
                phi -= M_PI/16;
                updateCamPosition();
            }
        }

        return false;
    }

    void run() {
        while(device->run() && driver)
        {
            driver->beginScene(true, true, SColor(255,100,100,100));
            scenemgr->drawAll();
            driver->setTransform(ETS_WORLD, IdentityMatrix);            
            driver->draw3DLine(vector3df(0,0,0), 3*vector3df(nx,ny,nz), SColor(255, 255, 255, 255));
            driver->endScene();
        }

        device->drop();
    }

    void updateCamPosition()
    {
        float x = r*sin(theta)*cos(phi);
        float y = r*sin(theta)*sin(phi);
        float z = r*cos(theta);
        camera->setPosition(vector3df(x, y, z));
        camera->setTarget(vector3df(0,0,0));
    }

    void updateNormal() {
    	m.findBorderNodeNormal(13, &nx, &ny, &nz, false);
    }
};


 int main()
 {

    GEngine engine;
    engine.run();

    return 0;
 }
