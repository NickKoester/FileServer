#ifndef SESSION_MANAGER_H
#define SESSION_MANAGER_H

#include <unordered_map>
#include <mutex>

#include "Session.h"

//Helps us keep track of all the sessions we've seen before
//Maps the session numbers to our session objects
class SessionManager {
public:
    //Adds a session to the map
    void add(unsigned int key, Session *val);

    //Returns a pointer to the session object
    //Given the session number
    Session *find(unsigned int key);

    //Returns true if the session number exists
    //in the data structure
    bool exists(unsigned int key);

    //Verifies everything about a session for error checking
    //Such as session number and sequence number
    void validateRequest(unsigned int key, unsigned int seq, const char *username);

    //Removes session from map
    void remove(unsigned int key);
private:
    std::unordered_map<unsigned int, Session *> sessionMap;
    std::mutex mapLock;
};

#endif
