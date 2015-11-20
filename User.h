#ifndef USER_H
#define USER_H

#include <QString>

/**
  domain object ,simple user,now we don't need user to provide a password.

*/
struct User
{
public:
    explicit User(QString _username="",int _userid=0)
        :userid(_userid),username(_username){}
    int userid;
    QString username;
};

#endif // USER_H
