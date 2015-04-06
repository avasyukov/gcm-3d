/*
 * File:   Ani3DFileReader.h
 * Author: amisto
 *
 * Created on April 05, 2015
 */

#ifndef GCM_ANI3D_FILE_READER_H
#define    GCM_ANI3D_FILE_READER_H

#include <string>
#include <fstream>
#include <limits>

#include "libgcm/Logging.hpp"


namespace gcm {
    class TetrMeshFirstOrder;
    class GCMDispatcher;
    class AABB;

    class Ani3DTetrFileReader
    {
    protected:
         USE_LOGGER;
    public:
        Ani3DTetrFileReader();
        ~Ani3DTetrFileReader();
        void readFile( std::string file, TetrMeshFirstOrder* mesh, GCMDispatcher* dispatcher, int rank, bool ignoreDispatcher);
        void preReadFile(std::string file, AABB* scene, int& sliceDirection, int& numberOfNodes);
    };
}

#endif
