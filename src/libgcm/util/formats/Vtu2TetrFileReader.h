#ifndef GCM_VTU2_FILE_READER_H
#define    GCM_VTU2_FILE_READER_H

#include <string>
#include <fstream>
#include <limits>

#include <vtkstd/string>
#include <vtkUnstructuredGrid.h>
#include <vtkXMLUnstructuredGridReader.h>
#include <vtkUnstructuredGridReader.h>
#include <vtkTetra.h>
#include <vtkDoubleArray.h>
#include <vtkIntArray.h>
#include <vtkPointData.h>
#include <vtkCellData.h>

#include "mesh/tetr/TetrMeshSecondOrder.h"
#include "Logging.h"

using namespace std;

namespace gcm {
    class GCMDispatcher;
    class AABB;

    class Vtu2TetrFileReader
    {
    protected:
         USE_LOGGER;
    public:
        Vtu2TetrFileReader();
        ~Vtu2TetrFileReader();
        void readFile( string file, TetrMeshSecondOrder* mesh, GCMDispatcher* dispatcher, int rank);
        void readFile( string file, TetrMeshSecondOrder* mesh, GCMDispatcher* dispatcher, int rank, bool ignoreDispatcher);
        void preReadFile(string file, AABB* scene, int& sliceDirection, int& numberOfNodes);
    };
}

#endif
