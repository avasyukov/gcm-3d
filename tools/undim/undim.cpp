#include "libgcm/rheology/RheologyMatrix.hpp"
#include "libgcm/rheology/setters/IsotropicRheologyMatrixSetter.hpp"
#include "libgcm/rheology/decomposers/IsotropicRheologyMatrixDecomposer.hpp"
#include "libgcm/util/matrixes.hpp"
#include "libgcm/node/CalcNode.hpp"

#include "launcher/util/xml.hpp"
#include "launcher/launcher.hpp"

#if CONFIG_ENABLE_LOGGING
#include <log4cxx/propertyconfigurator.h>
#include "launcher/util/FileFolderLookupService.hpp"
#endif

#include <iostream>
#include <algorithm>
#include <string>
#include <cmath>
#include <mpi.h>

using namespace gcm;
using namespace xml;
using namespace launcher;
using std::cout;
using std::endl;
using std::string;
using std::max;
using std::pow;

int main(int argc, char** argv)
{
    if (argc != 2)
        cout << "Usage: undim <task file>" << endl;

    MPI_Init(nullptr, nullptr);

    #if CONFIG_ENABLE_LOGGING
    auto& ffls = FileFolderLookupService::getInstance();
    ffls.addPath("src/tests");
    log4cxx::PropertyConfigurator::configure(ffls.lookupFile("log4cxx.properties"));
    #endif

    Launcher launcher;

    Doc doc = Doc::fromFile(string(argv[1]));
    launcher.loadMaterialsFromXml(doc.getRootElement().xpath("/task/materials/material"));



    auto getCond = [](MaterialPtr mat){
        gcm_matrix a, u, l, u1;
        CalcNode node;
        auto setter = makeSetterPtr<IsotropicRheologyMatrixSetter>();
        auto decomposer = makeDecomposerPtr<IsotropicRheologyMatrixDecomposer>();

        setter->setX(a, mat, node);
        decomposer->decomposeX(a, u, l, u1);
        float c1 = u.getCond();

        setter->setY(a, mat, node);
        decomposer->decomposeY(a, u, l, u1);
        float c2 = u.getCond();

        setter->setZ(a, mat, node);
        decomposer->decomposeZ(a, u, l, u1);
        float c3 = u.getCond();

        return max({c1, c2, c3});
    };

    auto& engine = Engine::getInstance();

    float minCond = 1e9;
    int _s0 = -100, _r0 = -100;

    for (int s0 = -9; s0 <= 9; s0++)
        for (int r0 = -9; r0 <= 9; r0++)
        {
            float s = pow(10, s0);
            float r = pow(10, r0);
            for (int i = 0; i < engine.getNumberOfMaterials(); i++)
            {
                auto mat = engine.getMaterial(i);
                auto undimMat = makeMaterialPtr("", mat->getRho()/r, mat->getLa()/s, mat->getMu()/s);
                float cond = getCond(undimMat);
                if (cond < minCond)
                {
                    minCond = cond;
                    _r0 = r0;
                    _s0 = s0;
                }
            }
        }

    cout << "r0 = 1e" << _r0 << "; s0 = 1e" << _s0 << endl;
    cout << "minCond = " << minCond << endl;

    MPI_Finalize();
}
