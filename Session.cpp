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
