#ifndef REQUESTS_H
#define REQUESTS_H

void sessionRequest(unsigned int seq, const char *username);
void createRequest(unsigned int sesh, unsigned int seq, const char *pathname const char *username, char type);
void sendResponse(unsigned int sesh, unsigned int seq);

#endif
