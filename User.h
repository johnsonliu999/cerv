#ifndef USER_H
#define USER_H

#include <QString>

/**
  domain object ,simple user,now we don't need user to provide a password.

*/
struct User
{
<<<<<<< HEAD
public:
    explicit User(int _userid=0,QString _username="")
=======
public:
    User(int _userid=0,QString _username="")
>>>>>>> update
        :userid(_userid),username(_username){}
    int userid;
    QString username;


};

#endif // USER_H
