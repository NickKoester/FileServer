#include "Path.h"
#include "fs_server.h"
using namespace std;

Path::Path(const char *pathname) {
    int nameIdx = 0;
    bool done = false;

    if(!pathname || !pathname[0]) {
        return;
    }

    while(!done || pathname[nameIdx] != ' ') {
        char name[FS_MAXFILENAME + 1];
        int i = 0;

        ++nameIdx;
        while(pathname[nameIdx] != '/' && pathname[nameIdx] != '\0') {
            name[i++] = pathname[nameIdx++];
        }
        if(!pathname[nameIdx+1]) {
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
