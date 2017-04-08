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

bool SessionManager::validateRequest(unsigned int key, unsigned int seq, const char *username) {
    Session *session = nullptr;

    try { 
        session = sessionMap.at(key);
    } catch (...) {
        return false;
    }

    if (!session->validate(seq, username)) {
        return false;
    }

    session->updateSequenceNum(seq);

    return true;
}

void SessionManager::remove(unsigned int key) {
    if(exists(key)) {
        delete sessionMap[key];
        sessionMap.erase(key);
    }
}
