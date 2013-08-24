#ifndef GCM_VTU2_FILE_READER_H
#define	GCM_VTU2_FILE_READER_H

#include "../../mesh/TetrMeshSecondOrder.h"
#include "../../Logging.h"
#include <string>
#include <fstream>
#include <limits>

#include <vtkUnstructuredGrid.h>
#include <vtkXMLUnstructuredGridReader.h>
#include <vtkUnstructuredGridReader.h>
#include <vtkTetra.h>
#include <vtkDoubleArray.h>
#include <vtkIntArray.h>
#include <vtkPointData.h>
#include <vtkCellData.h>
#include <vtkstd/string>

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
		int preReadFile( string file, AABB* scene);
	};
}

#endif