#ifndef GCM_ANI_VTU_FILE_READER_H
#define    GCM_ANI_VTU_FILE_READER_H

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
#include <vtkCellData.h>

#include "libgcm/mesh/tetr/TetrMeshSecondOrder.hpp"
#include "libgcm/Logging.hpp"


namespace gcm {
    class GCMDispatcher;
    class AABB;

    class AniVtuTetrFileReader
    {
    protected:
         USE_LOGGER;
    public:
        AniVtuTetrFileReader();
        ~AniVtuTetrFileReader();
        void readFile( std::string file, TetrMeshSecondOrder* mesh, GCMDispatcher* dispatcher, int rank);
        void readFile( std::string file, TetrMeshSecondOrder* mesh, GCMDispatcher* dispatcher, int rank, bool ignoreDispatcher);
        void preReadFile(std::string file, AABB* scene, int& sliceDirection, int& numberOfNodes);
    };
}

#endif
