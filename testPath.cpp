#include "Path.h"
#include <iostream>
#include <string>
using namespace std;

int main() {
    string pathname("/main/nick/mark/andrew/end");


    Path path(pathname.c_str());


    cout << "Depth: " << path.depth() << '\n';
    cout << "Path: " << '\n';

    for(int i = 0; i < path.depth(); i++) {
        cout << path.getName(i) << '\n';
    }

    return 0;
}
