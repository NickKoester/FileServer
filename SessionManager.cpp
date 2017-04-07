#include "SessionManager.h"
using namespace std;

void SessionManager::add(unsigned int key, Session *val) {
    sessionMap[key] = val;
}

Session *SessionManager::find(unsigned int key) {
    return sessionMap[key];
}

bool SessionManager::exists(unsigned int key) {
    return sessionMap.find(key) != sessionMap.end();
}

void SessionManager::remove(unsigned int key) {
    if(exists(key)) {
        delete sessionMap[key];
        sessionMap.erase(key);
    }
}
