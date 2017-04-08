#ifndef SESSION_MANAGER_H
#define SESSION_MANAGER_H

#include <unordered_map>
#include "Session.h"

class SessionManager {
public:
    void add(unsigned int key, Session *val);
    Session *find(unsigned int key);
    bool exists(unsigned int key);
    bool validateRequest(unsigned int key, unsigned int seq, const char *username);
    void remove(unsigned int key);
private:
    std::unordered_map<unsigned int, Session *> sessionMap;
};

#endif
