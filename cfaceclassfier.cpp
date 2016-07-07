#include "cfaceclassfier.h"
#include "ccamerareader.h"
#include "ccamerareader.h"
#include "cdatabase.h"

/* cv */
#include "cv.h"
#include "objdetect.hpp"
#include "imgproc.hpp"


#include <QString>
#include <QDebug>
#include <QTime>
#include <QList>

using namespace cv;

const String FACE_FILENMAE = "haarcascade_frontalface_alt.xml";
const String PROFILE_FILENAME ="lbpcascade_profileface.xml";
const String EYES_FILENAME = "haarcascade_eye_tree_eyeglasses.xml";
const String MOUTH_FILENAME ="haarcascade_mcs_mouth.xml";

CFaceClassfier::CFaceClassfier() :
    mp_db(new CDatabase("face_predictor", DBParams("MYSQL", "localhost", "neck", "root", "qq452977491", 3306))),
    mp_faceClassfier(new CascadeClassifier),
    mp_eyesClassfier(new CascadeClassifier),
    mp_mouthClassfier(new CascadeClassifier),
    mp_profileClassfier(new CascadeClassifier),
    mp_coordinate(new OrgansCoordinate)
{
}

CFaceClassfier::~CFaceClassfier()
{

}

QString CFaceClassfier::Enum2String(Face::FaceType faceType)
{
    switch (faceType)
    {
        case Face::NORMAL:
            return "Normal";
            break;
        case Face::BACKWARD:
            return "Backward";
            break;
        case Face::FORWARD:
            return "Forward";
            break;
        case Face::LEFTWARD:
            return "Leftward";
            break;
        case Face::RIGHTWARD:
            return "Rightward";
            break;
        case Face::UNKNOWN:
            return "Unknown";
            break;
    }
}

///
/// \brief CFaceClassfier::clarrify
/// will circle out the position
/// \param frame
/// \return
///
Face::FaceType CFaceClassfier::clarrify(const cv::Mat &frame, QList<QPoint> &points)
{
    Face::FaceType res;
    Mat grayFrame;
    try {
        grayFrame = framePreproc(frame);
    } catch (const Exception &e)
    {
        qDebug() << "CFaceClassfier::clarrify : frame preprocess failed.";
        return Face::UNKNOWN;
    }

    if ( (res = clarrifyProfile(grayFrame, points)) != Face::UNKNOWN) return res;
    else res = clarrifyFace(grayFrame, points);
    return res;
}

cv::Mat CFaceClassfier::framePreproc(const cv::Mat frame)
{
    Mat grayFrame;
    try {
        cvtColor(frame, grayFrame, CV_BGR2GRAY);
    } catch(const Exception &e)
    {
        qDebug() << "CFaceClassifier::framPreproc : cvtColor exception.\n"
                    "Maybe due to incomplete frame.";
        throw e;
    }

    equalizeHist(grayFrame, grayFrame);

    return grayFrame;
}

///
/// \brief CFaceClassfier::clarrifyProfile
/// clarrifies profile.
///
/// * First tell right or left, because it's more convincing.
///
/// \param grayFrame
/// \param points insert the center to the first point
/// \return
///
Face::FaceType CFaceClassfier::clarrifyProfile(const cv::Mat grayFrame, QList<QPoint> &points)
{
    std::vector<Rect> profiles;
    Rect profileRec;
    Face::FaceType res = Face::UNKNOWN;
    mp_profileClassfier->detectMultiScale(grayFrame, profiles, 1.1, 2, 0|CV_HAAR_SCALE_IMAGE, Size(30,30));
    if(profiles.size() > 0)
    {
        // find max area profile as the main face
        profileRec = getMaxRect(profiles);
        QPoint center(profileRec.x + profileRec.width * 0.5,
                      profileRec.y + profileRec.height * 0.5);
        points << center;
        /* begin classification */
        res = Face::NORMAL;

        int maxArea = profileRec.width*profileRec.height;
        // forward or backward
        int framArea = grayFrame.size().width * grayFrame.size().height;
        if (maxArea > framArea*0.5) res = Face::FORWARD;
        else if (maxArea < framArea*0.1) res = Face::BACKWARD;

        // left or right
        if (center.x() < grayFrame.size().width*0.25) res = Face::LEFTWARD;
        else if (center.x() > grayFrame.size().width*0.75) res = Face::RIGHTWARD;
    }

    return res;
}

///
/// \brief CFaceClassfier::clarrifyFace
/// clarrify face according to organs.
/// \param grayFrame
/// \return
///
Face::FaceType CFaceClassfier::clarrifyFace(const cv::Mat grayFrame, QList<QPoint> &points)
{
    std::vector<Rect> faces;
    Rect faceRect;
    Face::FaceType res = Face::UNKNOWN;
    mp_faceClassfier->detectMultiScale(grayFrame, faces, 1.1, 2, 0|CV_HAAR_SCALE_IMAGE, Size(30,30));
    if (faces.size())
    {
        faceRect = getMaxRect(faces);

        /* begin classification */
        res = Face::NORMAL;

        int maxArea = faceRect.width*faceRect.height;
        // forward or backward
        int framArea = grayFrame.size().width * grayFrame.size().height;
        if (maxArea > framArea*0.5) res = Face::FORWARD;
        else if (maxArea < framArea*0.1) res = Face::BACKWARD;

        // left or right
        // eyes
        Rect r = Rect(faceRect.x, faceRect.y,
                      faceRect.width, faceRect.height*0.5);
        Mat upperRegion = grayFrame(r);
        std::vector<Rect> eyes;
        mp_eyesClassfier->detectMultiScale(upperRegion, eyes, 1.1, 2, 0|CV_HAAR_SCALE_IMAGE, Size(30,30));
        if (eyes.size() == 2)
        {
            Rect leftEye, rightEye;
            if (eyes[0].x > eyes[1].x)
            {
                leftEye = eyes[0];
                rightEye = eyes[1];
            }
            else
            {
                leftEye = eyes[1];
                rightEye = eyes[0];
            }

            QPoint leftEyeCenter(leftEye.x+leftEye.width/2, leftEye.y+leftEye.height/2);
            QPoint rightEyeCenter(rightEye.x+rightEye.width/2, rightEye.y+rightEye.height/2);
            points << leftEyeCenter << rightEyeCenter;

            // mouth
            r = Rect(faceRect.x+faceRect.width/4,
                     faceRect.y+faceRect.height*3/4,
                     faceRect.width/2,
                     faceRect.height/4);
            Mat bottomRegion = grayFrame(r);
            std::vector<Rect> mouths;
            mp_mouthClassfier->detectMultiScale(bottomRegion, mouths, 1.1, 2, 0|CV_HAAR_SCALE_IMAGE, Size(30,30));
            if (mouths.size())
            {
                Rect mouth = getMaxRect(mouths);
                QPoint mouthCenter(mouth.x+mouth.width/2, mouth.y+mouth.height/2);
                points << mouthCenter;

                int totalDist = calcDist(leftEyeCenter, mp_coordinate->leftEye) +
                                calcDist(rightEyeCenter, mp_coordinate->rightEye) +
                                calcDist(mouthCenter, mp_coordinate->mouth);

                if (totalDist > 24000)
                {
                    if (mouthCenter.x()-mp_coordinate->mouth.x()>0) res = Face::LEFTWARD;
                    else res = Face::RIGHTWARD;
                }
            }
        }
    }

    return res;
}

QPoint CFaceClassfier::calcAverage(const QList<QPoint> &pointList)
{
    if (!pointList.size()) throw QString("CFaceClassfier::calcAverage : list empty");

    int x = 0;
    int y = 0;
    for (int i = 0; i < pointList.size(); i++)
    {
        x += pointList[i].x();
        y += pointList[i].y();
    }

    x /= pointList.size();
    y /= pointList.size();

    return QPoint(x, y);
}

///
/// \brief CFaceClassfier::loadModel
/// loads classifier stored in file.
///
/// \param parentPath terminates with '/'
///
void CFaceClassfier::loadModel(const String &parentPath)
{
    if (!mp_profileClassfier->load(parentPath+PROFILE_FILENAME) |
            !mp_faceClassfier->load(parentPath+FACE_FILENMAE) |
            !mp_eyesClassfier->load(parentPath+EYES_FILENAME) |
            !mp_mouthClassfier->load(parentPath+MOUTH_FILENAME))
        throw QString("CFaceClassfier::loadModel : Training file not found.");
    qDebug() << "Loading face training file succeed.";
}

#include "cdatabase.h"
#include "MySession.h"
void CFaceClassfier::loadFromDB()
{
    try {
        mp_db->openDB();
    } catch (const QString &e)
    {
        qDebug() << "CFaceClassifire::loadFromDB : Cannot open DB";
        throw e;
    }

    try {
        *mp_coordinate = mp_db->selectCoordinate();
    } catch(const QString &e)
    {
        mp_db->closeDB();
        qDebug() << "CFaceClassifire::loadFromDB::Cannot select coordinate.";
        throw e;
    }
    mp_db->closeDB();
}

void CFaceClassfier::save2DB()
{
    try {
        mp_db->openDB();
    } catch(const QString &e)
    {
        qDebug() << "CFaceClassfier::save2DB : cannot open DB";
        throw e;
    }

    bool b_exist = false;
    try {
        mp_db->selectCoordinate();
        b_exist = true;
        mp_db->closeDB();
    } catch(const QString &e)
    {
        mp_db->closeDB();
        if (!e.contains("no record", Qt::CaseInsensitive))
        {
            qDebug() << "CFaceClassfier::save2DB() : can not select coordinate.";
            throw e;
        }
    }


    try {
        save2DB(b_exist);
    } catch(const QString &e)
    {
        qDebug() << "CFaceClassfier::save2DB():" << e;
        throw e;
    }
}

///
/// \brief CFaceClassfier::saveToDB
/// saves organs ordinate to DB.
///
/// \exception <> {}
///
void CFaceClassfier::save2DB(bool b_exist)
{

    mp_db->openDB();

    try{
        if(b_exist) mp_db->updateCoordinate(*mp_coordinate);
        else mp_db->insertCoordinate(*mp_coordinate);
        mp_db->closeDB();
    } catch (const QString &e)
    {
        mp_db->closeDB();
        throw e;
    }
}


void CFaceClassfier::train()
{
    try{
        loadModel("./train_file/");
    } catch(const QString &e)
    {
        qDebug() << "CFaceClassfier::train : cannot find model files.";
        emit info("CFaceClassfier::train", e);
        return ;
    }

    CCameraReader reader;
    try {
        reader.openCamera();
    } catch(const QString &e)
    {
        qDebug() << "CFaceClassfier::train : open camera failed";
        emit info("CFaceClassfier::train", e);
        return ;
    }

    QTime t1 = QTime::currentTime();
    QList<QPoint> leftEyeList, rightEyeList, mouthList;
    int remainTime = 10*1000;
    Mat frame;
    while (remainTime > 0)
    {
        frame = reader.getFrame();
        Mat grayFrame;
        try{
            grayFrame = framePreproc(frame);
        } catch (const Exception &e)
        {
            qDebug() << "CFaceClassfier::train : frame preprocess exception";
            continue;
        }

        std::vector<Rect> faces;
        mp_faceClassfier->detectMultiScale(grayFrame, faces, 1.1, 2, 0|CV_HAAR_SCALE_IMAGE, Size(30,30));
        if (faces.size())
        {
            Rect face = getMaxRect(faces);
            Rect r = Rect(face.x, face.y,
                          face.width, face.height*0.5);
            // eyes
            Mat upperRegion = grayFrame(r);
            std::vector<Rect> eyes;
            mp_eyesClassfier->detectMultiScale(upperRegion, eyes, 1.1, 2, 0|CV_HAAR_SCALE_IMAGE, Size(30,30));
            if (eyes.size() == 2)
            {
                Rect leftEye, rightEye;
                if (eyes[0].x > eyes[1].x)
                {
                    leftEye = eyes[0];
                    rightEye = eyes[1];
                }
                else
                {
                    leftEye = eyes[1];
                    rightEye = eyes[0];
                }
                leftEyeList << QPoint(leftEye.x+leftEye.width/2, leftEye.y+leftEye.height/2);
                rightEyeList << QPoint(rightEye.x+leftEye.width/2, rightEye.y+leftEye.height/2);
            }

            // mouth
            r = Rect(face.x+face.width/4,
                     face.y+face.height*3/4,
                     face.width/2,
                     face.height/4);
            Mat bottomRegion = grayFrame(r);
            std::vector<Rect> mouths;
            mp_mouthClassfier->detectMultiScale(bottomRegion, mouths, 1.1, 2, 0|CV_HAAR_SCALE_IMAGE, Size(30,30));
            if (mouths.size())
            {
                Rect mouth = getMaxRect(mouths);
                mouthList << QPoint(mouth.x+mouth.width/2, mouth.y+mouth.height/2);
            }
        }

        emit updateDisp(CCameraReader::Mat2QImage(frame), QString::number(remainTime/1000.0, 'f', 2));
        remainTime -= t1.msecsTo(QTime::currentTime());
        QThread::msleep(500);
    }
    reader.closeCamera();
    emit updateDisp(CCameraReader::Mat2QImage(frame), QString::number(0, 'f', 2));

    qDebug() << leftEyeList.size() << rightEyeList.size() << mouthList.size();
    try {
        mp_coordinate->leftEye = calcAverage(leftEyeList);
        mp_coordinate->rightEye = calcAverage(rightEyeList);
        mp_coordinate->mouth = calcAverage(mouthList);
    } catch(const QString &e)
    {
        qDebug() << "CFaceClassfier::train : certain organ can not be detect.\nMay take off glasses";
        emit info("CFaceClassfier::train", e);
        return ;
    }

    qDebug() << "Left Eye : " << mp_coordinate->leftEye.x() << mp_coordinate->leftEye.y();
    qDebug() << "Right Eye : " << mp_coordinate->rightEye.x() << mp_coordinate->rightEye.y();
    qDebug() << "Mouth : " << mp_coordinate->mouth.x() << mp_coordinate->mouth.y();

    try {
        save2DB();
    } catch (const QString &e)
    {
        qDebug() << "CFaceClassfier::train : cannot save to DB.";
        emit info("CFaceClassfier::train", e);
        return ;
    }

    emit info("CFaceClassfier::train", "Train succeed.");
    qDebug() << "Face train succeed.";
}

Rect CFaceClassfier::getMaxRect(const std::vector<Rect> &rects)
{
    int maxAreaIndex = 0;
    int maxArea = 0;
    for (int i = 1; i < rects.size(); i++)
    {
        if (maxArea < rects[i].width * rects[i].height )
        {
            maxArea = rects[i].width * rects[i].height;
            maxAreaIndex = i;
        }
    }

    return rects[maxAreaIndex];
}

///
/// \brief CFaceClassfier::calcDist
/// calculate quadratic sum
/// \param p1
/// \param p2
/// \return
///
int CFaceClassfier::calcDist(const QPoint &p1, const QPoint &p2)
{
    int dist = qRound(qPow(p1.x()-p2.x(), 2)+qPow(p1.y()-p2.y(), 2));
    return dist;
}
