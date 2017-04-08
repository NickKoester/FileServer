#include "Session.h"
using namespace std;

unsigned int Session::nextSession = 0;

Session::Session(unsigned int seq, const char *user) {
    sessionNum = nextSession++;
    prevSequenceNum = seq;
    username = string(user);
}

unsigned int Session::getNum() {
    return sessionNum;
}

bool Session::validate(unsigned int seq, const char *name) {
    if (username.compare(name)) return false;
    if (!(prevSequenceNum < seq)) return false;
    return true;
}

void Session::updateSequenceNum(unsigned int seq) {
    prevSequenceNum = seq;
}
