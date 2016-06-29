#include "cfaceclassfier.h"
#include "ccamerareader.h"

/* cv */
#include "core/cvstd.hpp"
#include "objdetect.hpp"
#include "imgproc.hpp"


#include <QString>
#include <QDebug>
#include "cdatabase.h"

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

CFaceClassfier::FaceType CFaceClassfier::clarrify(const cv::Mat frame)
{
    FaceType res;
    Mat grayFrame = framePreproc(frame);

    if ( (res = clarrifyProfile(grayFrame)) != UNKNOWN) return res;
    else res = clarrifyFace(grayFrame);
    return res;
}

cv::Mat CFaceClassfier::framePreproc(const cv::Mat frame)
{
    Mat grayFrame;
    cvtColor(frame, grayFrame, CV_BGR2GRAY);
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
/// \return
///
CFaceClassfier::FaceType CFaceClassfier::clarrifyProfile(const cv::Mat grayFrame)
{
    std::vector<Rect> profiles;
    Rect profileRec;
    FaceType res = UNKNOWN;
    mp_profileClassfier->detectMultiScale(grayFrame, profiles, 1.1, 2, 0|CV_HAAR_SCALE_IMAGE, Size(30,30));
    if(profiles.size() > 0)
    {
        // find max area profile as the main face
        profileRec = getMaxRect(profiles);

        /* begin classification */
        res = NORMAL;

        int maxArea = profileRec.width*profileRec.height;
        // forward or backward
        int framArea = grayFrame.size().width * grayFrame.size().height;
        if (maxArea > framArea*0.5) res = FORWARD;
        else if (maxArea < framArea*0.1) res = BACKWARD;

        // left or right
        Point center;
        center.x = profileRec.x + profileRec.width * 0.5;
        center.y = profileRec.y + profileRec.height * 0.5;

        if (center.x < grayFrame.size().width*0.25) res = LEFTWARD;
        else if (center.x > grayFrame.size().width*0.75) res = RIGHTWARD;
    }

    return res;
}

///
/// \brief CFaceClassfier::clarrifyFace
/// clarrify face according to organs.
/// \param grayFrame
/// \return
///
CFaceClassfier::FaceType CFaceClassfier::clarrifyFace(const cv::Mat grayFrame)
{
    std::vector<Rect> faces;
    Rect faceRect;
    FaceType res = UNKNOWN;

    mp_faceClassfier->detectMultiScale(grayFrame, faces, 1.1, 2, 0|CV_HAAR_SCALE_IMAGE, Size(30,30));
    if (faces.size())
    {
        faceRect = getMaxRect(faces);

        /* begin classification */
        res = NORMAL;

        int maxArea = faceRect.width*faceRect.height;
        // forward or backward
        int framArea = grayFrame.size().width * grayFrame.size().height;
        if (maxArea > framArea*0.5) res = FORWARD;
        else if (maxArea < framArea*0.1) res = BACKWARD;

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
            leftEye = eyes[0];
            rightEye = eyes[1];

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

                QPoint leftEyeCenter(leftEye.x+leftEye.width/2, leftEye.y+leftEye.height/2);
                QPoint rightEyeCenter(rightEye.x+rightEye.width/2, rightEye.y+rightEye.height/2);
                QPoint mouthCenter(mouth.x+mouth.width/2, mouth.y+mouth.height/2);

                int totalDist = calcDist(leftEyeCenter, mp_coordinate->leftEye) +
                                calcDist(rightEyeCenter, mp_coordinate->rightEye) +
                                calcDist(mouthCenter, mp_coordinate->mouth);

                if (totalDist > 24000)
                {
                    if (mouthCenter.x()-mp_coordinate->mouth.x()>0) res = LEFTWARD;
                    else res = RIGHTWARD;
                }
            }
        }
    }

    return res;
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
            mp_faceClassfier->load(parentPath+FACE_FILENMAE) |
            mp_eyesClassfier->load(parentPath+EYES_FILENAME) |
            mp_mouthClassfier->load(parentPath+MOUTH_FILENAME))
        throw QString("Training file not found.");
    qDebug() << "Loading face training file succeed.";
}

#include "cdatabase.h"
#include "DAO.h"
#include "MySession.h"
void CFaceClassfier::loadFromDB()
{
    mp_db->openDB();
    try {
        *mp_coordinate = mp_db->selectCoordinate();
    } catch(const QString &e)
    {
        mp_db->closeDB();
        throw e;
    }
    mp_db->closeDB();
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
        else mp_db->updateCoordinate(*mp_coordinate);
        mp_db->closeDB();
    } catch (const QString &e)
    {
        mp_db->closeDB();
        throw e;
    }
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
