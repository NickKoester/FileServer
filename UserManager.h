#ifndef USER_MANAGER_H
#define USER_MANAGER_H

#include <unordered_map>
#include <string>
#include "RWLock.h"


class UserManager {
public:
    void initialize();
    std::string lookupPassword(std::string username);
private:
    std::unordered_map<std::string, std::string> users;
    RWLock lock;
};

#endif
