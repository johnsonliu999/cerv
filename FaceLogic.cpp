#include "FaceLogic.h"
#include <fstream>
#include <math.h>
#include <QDebug>
#include <string>

const int RES_NUM = 5; ///< the number of results kept
const int TYPE_NUM = 5;

/*
///
/// \brief FaceLogic::isInitialized tell if position.txt exists or not.
/// \return Success or not.
///
bool FaceLogic::isInitialized()
{
    std::ifstream file("position.txt");
    return file.is_open();
}
*/

///
/// \brief FaceLogic::getRecentRes
/// finds the most common type in result list.
///
/// \return
///
CFaceClassfier::FaceType FaceLogic::getRecentRes()
{
    int ind = 0;
    for (int i = 0; i < TYPE_NUM; i++)
    {
        if ((*mp_statList)[ind] < (*mp_statList)[i])
            ind = i;
    }

    return CFaceClassfier::FaceType(ind);
}


///
/// \brief FaceLogic::storeNormalValue store position of eye and mouth.
/// \param lfeye Left eye position.
/// \param rteye Right eye position.
/// \param mouth Mouth position.
/// \return success or not.
///
FaceLogic::FaceLogic() :
    mp_resList(new QList<CFaceClassfier::FaceType>),
    mp_statList(new QList<int>),
    mp_reader(new CCameraReader),
    mp_classifier(new CFaceClassfier)
{
    for (int i = 0; i < TYPE_NUM; i++)
        *mp_statList << 0;
}

FaceLogic::~FaceLogic()
{

}

///
/// \brief FaceLogic::start
/// starts face logic process.
/// Including resetting all records and reloading models.
///
void FaceLogic::start()
{
    for (int i = 0; i < RES_NUM; i++)
        (*mp_statList)[i] = 0;

    mp_resList->clear();

    try{
        mp_reader->openCamera();
    } catch(const QString &e)
    {
        qDebug() << "Open camera failed.";
        throw e;
    }

    try{
        mp_classifier->loadModel("./train_file/");
    } catch(const QString& e)
    {
        mp_reader->closeCamera();
        qDebug() << "Failed to load face classifying model.";
        throw e;
    }
}

///
/// \brief FaceLogic::stop
/// stops face logic process.
///
void FaceLogic::stop()
{
    mp_reader->closeCamera();
}


// begin slots

///
/// \brief FaceLogic::updateFaceRes
/// update recent result of classification.
/// Wait for 'update' to use.
/// Current update 5 results each time.
///
void FaceLogic::updateFaceRes()
{
    for (int i = 0; i < RES_NUM; i++)
    {
        Mat frame = mp_reader->getFrame();
        CFaceClassfier::FaceType res = mp_classifier->clarrify(frame);

        // update statistics
        if (mp_resList->size() >= RES_NUM)
        {
            (*mp_statList)[(int)(*mp_resList)[0]]--;
            mp_resList->removeFirst();
        }
        mp_resList->append(res);
        (*mp_statList)[(int)res]++;
    }

    emit updateDisp(Enum2String(getRecentRes()));
}

#if NO
bool FaceLogic::storeNormalValue(const Point& lfeye,const Point& rteye,const Point& mouth)
{
    qDebug() << "Enter storeNormalValue";
    std::ofstream out("position.txt");
    if (out.is_open())   
    {
        qDebug() << "Open position.txt";
        out<<lfeye.x<<" "<<lfeye.y<<endl;
        out<<rteye.x<<" "<<rteye.y<<endl;
        out<<mouth.x<<" "<<mouth.y<<endl;
        out.close();
        return true;
    }
    return false;
}
#endif

/*
///
/// \brief FaceLogic::getNormalValue
/// get eye and mouth position.
/// If position.txt not exist, error will occur.
///
/// \return List of points.
///
QList<Point> FaceLogic::getNormalValue()
{
    std::ifstream file("position.txt");
    QList<Point> temp;
	if(file.is_open())
	{
        std::string line;
		while(getline(file,line))
		{
			int index = line.find_first_of(" ");
            std::string x =line.substr(0,index);
            std::string y =line.substr(index+1,line.length());
			Point p(atoi(x.c_str()),atoi(y.c_str()));
            temp.append(p);
		}
	}
	else{
        throw QString("File not found");
	}
    file.close();
	return temp;

}*/

/*
///
/// \brief FaceLogic::judgeFacePosture tell type of face,
/// and store in rtType.
/// \param lfeye Left eye position.
/// \param rteye Right eye position.
/// \param mouth Mouth position.
///
void FaceLogic::judgeFacePosture(const Point& lfeye,const Point& rteye,const Point& mouth)
{
    if(normal.size()==0)
    {
       normal = getNormalValue();
       if(normal.size()!=3)
           throw "File not Found";
    }

    if(lfeye.x < normal[0].x)
		rtType =Left;
	if(rteye.x > normal[1].x)
        rtType =Right;

	int distance1 =(lfeye.x - normal[0].x)*(lfeye.x - normal[0].x) +(lfeye.y -normal[0].y)*(lfeye.y -normal[0].y);
	int distance2 =(rteye.x - normal[1].x)*(rteye.x - normal[1].x) +(rteye.y -normal[1].y)*(rteye.y -normal[1].y);
	int distance3 =(mouth.x - normal[2].x)*(mouth.x - normal[2].x) +(mouth.y -normal[2].y)*(mouth.y -normal[2].y);

    //cout<<distance1<<"   "<<distance2<<"  "<<distance3;

	int dis_lr = (normal[0].x-normal[1].x)*(normal[0].x-normal[1].x) + (normal[0].y-normal[1].y)*(normal[0].y-normal[1].y);
	int dis_lm = (normal[0].x-normal[2].x)*(normal[0].x-normal[2].x) + (normal[0].y-normal[2].y)*(normal[0].y-normal[2].y);
	int dis_rm = (normal[1].x-normal[2].x)*(normal[1].x-normal[2].x) + (normal[1].y-normal[2].y)*(normal[1].y-normal[2].y);
	int dis_lr_realtime = (lfeye.x - rteye.x)*(lfeye.x - rteye.x)+(lfeye.y-rteye.y)*(lfeye.y-rteye.y);
	int dis_lm_realtime = (lfeye.x - mouth.x)*(lfeye.x - mouth.x) + (lfeye.y-mouth.y)*(lfeye.y-mouth.y);
	int dis_rm_realtime = (rteye.x-mouth.x)*(rteye.x-mouth.x) + (rteye.y-mouth.y)*(rteye.y-mouth.y);
	if(dis_rm==0||dis_lm==0||dis_lr==0)
	{
        rtType =Normal;
	}
    else if(dis_rm_realtime/dis_rm>1.2&&dis_lm_realtime/dis_lm>1.2&&dis_lr_realtime/dis_lr>1.2)
    {
        rtType =Near;
    }
    else if(dis_rm_realtime/dis_rm<0.8&&dis_lm_realtime/dis_lm<0.8&&dis_lr_realtime/dis_lr<0.8)
    {
        rtType =Far;
    }
    else
    {
        rtType =Normal;
    }
    if(distance1+distance2+distance3 > 24000)
    {
        if(abs(lfeye.x-normal[0].x)==0||abs(rteye.x-normal[1].x)==0||abs(mouth.x-normal[2].x)==0)
        {
            rtType = Normal;
        }
        else if(lfeye.x-normal[0].x>0&&rteye.x-normal[1].x>0&&mouth.x-normal[2].x>0)
        {
            if(abs(lfeye.y-normal[0].y)/abs(lfeye.x-normal[0].x)+abs(rteye.y-normal[1].y)/abs(rteye.x-normal[1].x)+abs(mouth.y-normal[2].y)/abs(mouth.x-normal[2].x) > 3)
            {
                if(lfeye.y-normal[0].y>0&&rteye.y-normal[1].y>0&&mouth.y-normal[2].y>0)
                {
                    rtType = Down;
                }
                else
                {
                    rtType = Up;
                }
            }
            else
            {
                rtType = Left;
            }
        }
        else if(abs(lfeye.y-normal[0].y)/abs(lfeye.x-normal[0].x)+abs(rteye.y-normal[1].y)/abs(rteye.x-normal[1].x)+abs(mouth.y-normal[2].y)/abs(mouth.x-normal[2].x) > 3)
        {
            if( (lfeye.y-normal[0].y>0 && rteye.y-normal[1].y>0) ||mouth.y-normal[2].y>0)
            {
                rtType = Down;
            }
            else
            {
                rtType = Up;
            }
        }
        else
        {
            rtType = Right;
        }
    }
}*/

///
/// \brief FaceLogic::fetchJudgedMessage provide string format information.
/// \param faceType
/// \return Type in string format.
///
QString FaceLogic::Enum2String(const CFaceClassfier::FaceType &faceType)
{
    switch (faceType)
	{
        case CFaceClassfier::NORMAL:
            return "Normal";
			break;
        case CFaceClassfier::BACKWARD:
            return "Backward";
			break;
        case CFaceClassfier::FORWARD:
            return "Forward";
			break;
        case CFaceClassfier::LEFTWARD:
            return "Leftward";
			break;
        case CFaceClassfier::RIGHTWARD:
            return "Rightward";
			break;
        case CFaceClassfier::UNKNOWN:
            return "Unknown";
			break;
    }
}
