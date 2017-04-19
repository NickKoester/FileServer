#ifndef SESSION_H
#define SESSION_H
#include <string>

//This class keeps track of the relevent information of a session
//Such as sequence number and session number
//This helps us identify bad requests
class Session {
public:
    //Contructs session when we get a session request
    Session(unsigned int seq, const char *username);

    //Returns the session number
    unsigned int getNum() const;

    //This validates the requests we get
    //returns false if invalid
    bool validateSequenceNumber(unsigned int seq) const;

    //Verifies that the session belongs to the user
    bool belongsToUser(const char *username) const;

    //Updates the previous sequence number
    void updateSequenceNumber(unsigned int seq);

private:
    std::string username;
    static unsigned int nextSession;
    unsigned int sessionNum;
    unsigned int prevSequenceNum;
};

#endif
