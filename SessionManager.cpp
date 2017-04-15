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

void SessionManager::validateRequest(unsigned int key, unsigned int seq, const char *username) {
    Session *session = nullptr;

    try { 
        session = sessionMap.at(key);
    } catch (...) {
        string message = string("Invalid session: ") + std::to_string(key) + string("\n");
        throw std::runtime_error(message);
    }

    if (!session->belongsToUser(username)) {
        throw std::runtime_error("Session does not belong to user\n");
    }

    if (!session->validateSequenceNumber(seq)) {
        throw std::runtime_error("Sequence number must be strictly increasing\n");
    }

    session->updateSequenceNumber(seq);
}

void SessionManager::remove(unsigned int key) {
    if(exists(key)) {
        delete sessionMap[key];
        sessionMap.erase(key);
    }
}
