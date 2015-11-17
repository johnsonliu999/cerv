#ifndef MYSESSION_H
#define MYSESSION_H

#include "User.h"

/**
  Session contains the User object,
  used to maintain the Login info.

*/
class Session
{
public:

    static User user;
};

#endif // SESSION_H
