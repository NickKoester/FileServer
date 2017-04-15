#include "SessionManager.h"
using namespace std;

void SessionManager::add(unsigned int key, Session *val) {
    mapLock.lock();

    sessionMap[key] = val;

    mapLock.unlock();
}

Session *SessionManager::find(unsigned int key) {
    mapLock.lock();

    Session *session = sessionMap[key];

    mapLock.unlock();
    return session;
}

bool SessionManager::exists(unsigned int key) {
    mapLock.lock();

    bool inMap = sessionMap.find(key) != sessionMap.end();

    mapLock.unlock();
    return inMap;
}

void SessionManager::validateRequest(unsigned int key, unsigned int seq, const char *username) {
    Session *session = nullptr;
    mapLock.lock();

    try { 
        session = sessionMap.at(key);
    } catch (...) {
        string message = string("Invalid session: ") + std::to_string(key) + string("\n");

        mapLock.unlock();
        throw std::runtime_error(message);
    }

    if (!session->belongsToUser(username)) {

        mapLock.unlock();
        throw std::runtime_error("Session does not belong to user\n");
    }

    if (!session->validateSequenceNumber(seq)) {

        mapLock.unlock();
        throw std::runtime_error("Sequence number must be strictly increasing\n");
    }

    session->updateSequenceNumber(seq);

    mapLock.unlock();
}

void SessionManager::remove(unsigned int key) {
    mapLock.lock();

    if(sessionMap.find(key) != sessionMap.end()) {
        delete sessionMap[key];
        sessionMap.erase(key);
    }

    mapLock.unlock();
}
