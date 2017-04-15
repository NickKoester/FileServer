#include "Session.h"
using namespace std;

unsigned int Session::nextSession = 0;

Session::Session(unsigned int seq, const char *user) {
    sessionNum = nextSession++;
    prevSequenceNum = seq;
    username = string(user);
}

unsigned int Session::getNum() const {
    return sessionNum;
}

bool Session::validateSequenceNumber(unsigned int seq) const {
    return prevSequenceNum < seq;
}

bool Session::belongsToUser(const char *name) const {
    return !username.compare(name);
}

void Session::updateSequenceNumber(unsigned int seq) {
    prevSequenceNum = seq;
}
