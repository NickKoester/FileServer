#include "Path.h"
#include "fs_server.h"
using namespace std;

Path::Path(const char *pathname) {
    int nameIdx = 0;
    bool done = false;

    if(!pathname || !pathname[0]) {
        return;
    }

    if (!pathname || pathname[0] != '/') {
        throw std::runtime_error("Malformed path\n");
    }

    while(!done) {
        char name[FS_MAXFILENAME + 1];
        uint32_t i = 0;

        ++nameIdx;
        while(pathname[nameIdx] != '/' && pathname[nameIdx] != '\0') {
            if (isspace(pathname[nameIdx])) {
                throw std::runtime_error("File/directory name must not contain whitespace\n");
            }

            if (i >= FS_MAXFILENAME) {
                throw std::runtime_error("File name exceeds maximum length\n");
            }

            name[i++] = pathname[nameIdx++];
        }

        if (!i) {
            throw std::runtime_error("File/directory name must be non-empty\n");
        }

        if(!pathname[nameIdx]) {
            done = true;
        }

        name[i++] = '\0';

        string nameStr(name);
        pathVector.push_back(nameStr);
    }

}

string Path::getNameString(int i) const {
    return pathVector.at(i);
}

const char *Path::getNameCString(int i) const {
    return pathVector.at(i).c_str();
}

int Path::depth() const {
    return pathVector.size();
}
