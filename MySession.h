#ifndef MYSESSION_H
#define MYSESSION_H

//#include "User.h"
#include <QString>

/**
  Session contains the User object,
  used to maintain the Login info.

*/


class Session
{
public:
    static QString Username;
    static int UserId;
};

#endif // SESSION_H
