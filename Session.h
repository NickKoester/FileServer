#ifndef SESSION_H
#define SESSION_H
#include <string>

class Session {
public:
    Session(unsigned int seq, const char *username);
    unsigned int getNum();

private:
    std::string username;
    static unsigned int nextSession;
    unsigned int sessionNum;
    unsigned int prevSequenceNum;
};

#endif
