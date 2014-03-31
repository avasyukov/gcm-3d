#ifndef GCM_UTILS_H_
#define GCM_UTILS_H_

#include <fstream>
#include <map>
#include <string>
#include <vector>

#include "libgcm/Exception.h"

using namespace std;

namespace gcm {
    /*
     * Class to hold params while loading tasks.
     */

    class Params: public map<string, string> {
        public:
            /*
             * Constructors
             */
            Params();
            Params(map<string, string> attrs);
            /*
             * Checks if specified key is in params.
             */
            bool has(string param);
            /*
             * Checks if value stored in map equals to specifie one.
             */
            bool paramEquals(string param, string value);
    };


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
