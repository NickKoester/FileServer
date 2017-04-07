#include "requests.h"
#include "Session.h"
#include "SessionManager.h"

void sessionRequest(unsigned int seq, const char *username) {
    Session *newSesh = new Session(seq, username);

    unsigned int seshNum = newSesh->getNum();

    sessionManager.add(seshNum, newSesh);

    sendResponse(seshNum, seq);
}
