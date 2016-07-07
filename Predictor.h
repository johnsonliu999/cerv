#ifndef PREDICTOR
#define PREDICTOR

#include <QByteArray>

/**
  domain object:  represent a record in database table predictor.
 */
struct  Predictor
{
   int userId;
   QByteArray xml;

    Predictor(const int& userId, const QByteArray& xml) : userId(userId), xml(xml) {}
//   explicit Predictor(int _id =0):id(_id){}
};
#endif // PREDICTOR

