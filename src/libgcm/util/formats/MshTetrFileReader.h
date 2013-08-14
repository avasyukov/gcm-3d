/* 
 * File:   MshFileReader.h
 * Author: anganar
 *
 * Created on May 13, 2013, 6:33 PM
 */

#ifndef GCM_MSH_FILE_READER_H
#define	GCM_MSH_FILE_READER_H

#include "../../Logging.h"
#include <string>
#include <fstream>
#include <limits>

using namespace std;

namespace gcm {
	class TetrMeshFirstOrder;
	class GCMDispatcher;
	class AABB;
	
	class MshTetrFileReader
	{
	protected:
		 USE_LOGGER;
	public:
		MshTetrFileReader();
		~MshTetrFileReader();
		void readFile( string file, TetrMeshFirstOrder* mesh, GCMDispatcher* dispatcher, int rank);
		void preReadFile( string file, AABB* scene);
	};
}

#endif