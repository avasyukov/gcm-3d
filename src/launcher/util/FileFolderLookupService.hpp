#ifndef FILEFOLDERLOOKUPSERVICE_HPP
#define FILEFOLDERLOOKUPSERVICE_HPP 

#include <vector>
#include <string>
#include <fstream>

#include "libgcm/util/Singleton.hpp"
#include "libgcm/Exception.hpp"

using namespace std;

namespace launcher
{
    class FileFolderLookupService: public gcm::Singleton<FileFolderLookupService>
    {
        private:
            vector<string> paths;
        public:
            /*
             * Adds path to lookup list
             */
            void addPath(string path);
            /*
             * Looks up file by relative name
             */
            string lookupFile(string fname);
            /**
             * Searches for folder by relative path
             * @param fname Folder name
             */
            string lookupFolder(string fname);
    };
}

#endif /* FILEFOLDERLOOKUPSERVICE_HPP */
