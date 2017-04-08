#ifndef PATH_H
#define PATH_H

#include <string>
#include <vector>

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
