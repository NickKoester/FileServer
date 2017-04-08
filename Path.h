#ifndef PATH_H
#define PATH_H

#include <string>
#include <vector>

//  Create an object by giving it the char *pathname and the constructor
// turns it into a vector of strings.  This way we dont have to parse the
// damn thing every time we want to do stuff with it.
class Path {
public:
   Path(const char *);
   std::string getNameString(int i) const;
   const char *getNameCString(int i) const;
   int depth() const;
private:
   std::vector<std::string> pathVector;
};

#endif
