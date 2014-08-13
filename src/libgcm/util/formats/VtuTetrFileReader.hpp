#ifndef GCM_VTU_FILE_READER_H
#define    GCM_VTU_FILE_READER_H

#include <string>
#include <fstream>
#include <limits>

#ifdef CONFIG_VTK_5
#include <vtkstd/string>
#else
#include <vtkStdString.h>
#endif
#include <vtkUnstructuredGrid.h>
#include <vtkXMLUnstructuredGridReader.h>
#include <vtkUnstructuredGridReader.h>
#include <vtkTetra.h>
#include <vtkDoubleArray.h>
#include <vtkIntArray.h>
#include <vtkPointData.h>

#include "libgcm/mesh/tetr/TetrMeshFirstOrder.hpp"
#include "libgcm/Logging.hpp"

using namespace std;

namespace gcm {
    class GCMDispatcher;
    class AABB;

    class VtuTetrFileReader
    {
    protected:
         USE_LOGGER;
    public:
        VtuTetrFileReader();
        ~VtuTetrFileReader();
        void readFile( string file, TetrMeshFirstOrder* mesh, GCMDispatcher* dispatcher, int rank);
        void preReadFile(string file, AABB* scene, int& sliceDirection, int& numberOfNodes);
    };
}

#endif
