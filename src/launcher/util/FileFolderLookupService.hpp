#ifndef FILEFOLDERLOOKUPSERVICE_HPP
#define FILEFOLDERLOOKUPSERVICE_HPP 

#include <vector>
#include <string>
#include <fstream>

#include "libgcm/util/Singleton.hpp"
#include "libgcm/Exception.hpp"


namespace launcher
{
    class FileFolderLookupService: public gcm::Singleton<FileFolderLookupService>
    {
        private:
            std::vector<std::string> paths;
        public:
            /*
             * Adds path to lookup list
             */
            void addPath(std::string path);
            /*
             * Looks up file by relative name
             */
            std::string lookupFile(std::string fname);
            /**
             * Searches for folder by relative path
             * @param fname Folder name
             */
            std::string lookupFolder(std::string fname);
    };
}

#endif /* FILEFOLDERLOOKUPSERVICE_HPP */
