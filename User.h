#ifndef USER_H
#define USER_H

#include <QString>

/**
  domain object ,simple user,now we don't need user to provide a password.

*/
struct User
{
public:
    User(int _userid,QString _username)
        :userid(_userid),username(_username){}
    int userid;
    QString username;


};

#endif // USER_H
