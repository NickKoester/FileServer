#ifndef REQUESTS_H
#define REQUESTS_H

void sessionRequest(unsigned int seq, const char *username);
void sendResponse(unsigned int sesh, unsigned int seq);

#endif
