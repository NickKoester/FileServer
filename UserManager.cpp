#include "UserManager.h"

#include <iostream>
using namespace std;

void UserManager::initialize() {
    lock.writerStart();

    string user;
    string password;
    while (cin >> user) {
        cin >> password;
        users[user] = password;
    }

    lock.writerFinish();
}

string UserManager::lookupPassword(string username) {
    lock.readerStart();

    if(users.find(username) == users.end()) {
       throw std::runtime_error("User does not exist.");
    }

    string password = users[username];

    lock.readerFinish();
    return password;
}
