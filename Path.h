#ifndef PATH_H
#define PATH_H

#include <string>
#include <vector>

//  Create an object by giving it the char *pathname and the constructor
// turns it into a vector of strings.  This way we dont have to parse the
// damn thing every time we want to do stuff with it.
class Path {
public:
   //Takes the c string path name and converts it
   //to vector of strings containing the names
   Path(const char *);

   //This gets the name of an entity in the path
   //returns the string
   std::string getNameString(int i) const;

   //This gets the name at depth i
   //returns it as c string
   const char *getNameCString(int i) const;

   //This returns the 'depth' of the path.
   //0 is root directory and it increases from there
   int depth() const;
private:
   std::vector<std::string> pathVector;
};

#endif
