#include "libgcm/Utils.hpp"

#include <boost/filesystem.hpp>

namespace bfs = boost::filesystem;

gcm::Params::Params() {

}

gcm::Params::Params(map<string, string> attrs) {
    for(auto& attr: attrs)
    {
        (*this)[attr.first] = attr.second;
    }
}

bool gcm::Params::has(string param) {
    return find(param) != end();
}

bool gcm::Params::paramEquals(string param, string value) {
    if (has(param))
        if ((*this)[param] == value)
            return true;
    return false;
}

void gcm::FileFolderLookupService::addPath(string path) {
    paths.push_back(path);
}

string gcm::FileFolderLookupService::lookupFile(string fname) {
    for(auto& path: paths) {
        // FIXME should we use different separators for different
        // target platforms? Windows is ok with /-sep
        string fullName = path + "/" + fname;
        ifstream ifile(fullName.c_str());
        if (ifile)
            return fullName;
    }
    THROW_INVALID_ARG("File not found: " + fname);
}

string gcm::FileFolderLookupService::lookupFolder(string fname)
{
    for(auto path: paths) {
        bfs::path p(path);
        p /= fname;
        if (bfs::is_directory(p))
            return p.string();
    }
    THROW_INVALID_ARG("Folder not found: " + fname);
}

void checkStream(fstream &f) {
    if (f.eof() || f.fail() || f.bad())
        THROW_INVALID_INPUT("Input file is corrupted");
}

void replaceAll(std::string& str, const std::string& from, const std::string& to) {
    size_t start_pos = 0;
    while((start_pos = str.find(from, start_pos)) != std::string::npos) {
        str.replace(start_pos, from.length(), to);
        start_pos += to.length(); // In case 'to' contains 'from', like replacing 'x' with 'yx'
    }
}

void split (const std::string& str, std::vector<std::string>& dest, char delim)
{
    std::stringstream ss (str);
    std::string s;
    while (std::getline (ss, s, delim))
    {
        dest.push_back (s);
    }
}
