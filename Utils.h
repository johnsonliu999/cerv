#ifndef UTILS_H
#define UTILS_H

#include <QString>
#include "FaceLogic.h"
using namespace std;

///
/// \brief The Utils class
/// provide general utilities.
///
class Utils
{
public:
    static QString int2str(int n) {

        char t[10];
        itoa(n,t,10);
        return QString(t);
     }


};

#endif // UTILS_H
