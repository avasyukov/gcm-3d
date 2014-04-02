#ifndef GCM_UTILS_H_
#define GCM_UTILS_H_

#include <fstream>
#include <map>
#include <string>
#include <vector>

#include "libgcm/Exception.hpp"

using namespace std;

namespace gcm {


    class FileFolderLookupService {
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

/*
 * Checks if previous read operation succeeded.
 */
void checkStream(fstream &f);

void replaceAll(std::string& str, const std::string& from, const std::string& to);

template<class T>
static std::string t_to_string(T i)
{
    std::stringstream ss;
    std::string s;
    ss << i;
    s = ss.str();
    return s;
}

void split (const std::string& str, std::vector<std::string>& dest, char delim);

#endif
