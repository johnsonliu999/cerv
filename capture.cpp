#include "capture.h"
#include "FaceLogic.h"



#include <qdebug.h>
#include <vector>
#include <string>
#include <fstream>
#include "opencv2/objdetect/objdetect.hpp"
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"

using namespace std;
using namespace cv;

 /** Function Headers */
 QImage cvMat2QImage(const cv::Mat& mat);
 cv::Mat QImage2cvMat(QImage image);
 vector<string> split(const string& src, string delimit, string null_subst);
 void triangle(Mat& frame,const Point& x1,const Point& x2,const Point& x3);
 string positionStr(const Point& p);
 string int2str(int n);

 /** Global variables */
 String face_cascade_name = "D:/OpenCV/haarcascades/haarcascade_frontalface_alt.xml";
 String profile_cascade_name ="D:/OpenCV/lbpcascades/lbpcascade_profileface.xml";
 String eyes_cascade_name = "D:/OpenCV/haarcascades/haarcascade_eye_tree_eyeglasses.xml";
 String mouth_cascade_name ="D:/OpenCV/haarcascades/haarcascade_mcs_mouth.xml";
 CascadeClassifier face_cascade;
 CascadeClassifier eyes_cascade;
 CascadeClassifier mouth_cascade;
 CascadeClassifier profile_cascade;





 bool initial =FaceLogic::isInitialized();


 VideoCapture cap;
 void openCamera()
 {

     if(cap.isOpened())
         return;
     //-- 1. Load the cascades
     if( !face_cascade.load( face_cascade_name ) ||
         !eyes_cascade.load( eyes_cascade_name ) ||
         !mouth_cascade.load( mouth_cascade_name ) ||
         !profile_cascade.load( profile_cascade_name) )
     {
         qDebug() << "Xml file not found";
         throw "Xml file not found";

     }

     qDebug() << "Xml fild found";
     if(cap.open(0))
         qDebug() << "Open camera";
     else
     {
         qDebug() << "Open camera failed";
         throw "Open camera failed";
     }

 }

 void closeCamera()
 {
     if(cap.isOpened())
        cap.release();
 }





/** @function detect */


QImage detect()
{
    if(!cap.isOpened())
    {
        qDebug() << "Camera not open";
        throw "camera not open";
    }
    Mat frame;
    cap>>frame;

//     if(frame.empty() )
//       return NULL;
    Mat frame_gray;
	cvtColor( frame, frame_gray, CV_BGR2GRAY );
	equalizeHist( frame_gray, frame_gray );

    //-- Detect profile faces
    std::vector<Rect> profiles;
    profile_cascade.detectMultiScale( frame_gray, profiles, 1.1, 2, 0|CV_HAAR_SCALE_IMAGE, Size(30, 30) );
    if(profiles.size()>0)
    {
        FaceLogic::setRtType(Profile);
        Point center( profiles[0].x + profiles[0].width*0.5, profiles[0].y + profiles[0].height*0.5 );
        ellipse( frame, center, Size( profiles[0].width*0.5, profiles[0].height*0.5), 0, 0, 360, Scalar( 255, 0, 255 ), 4, 8, 0 );

        return cvMat2QImage(frame);
    }





    //-- Detect faces
    std::vector<Rect> faces;
    face_cascade.detectMultiScale( frame_gray, faces, 1.1, 2, 0|CV_HAAR_SCALE_IMAGE, Size(30, 30) );
    if(faces.size()==0)
    {
        FaceLogic::setRtType(NO_FACE);
        return cvMat2QImage(frame);
    }


	for( size_t i = 0; i < faces.size(); i++ )
	{

        //Face

		Point center( faces[i].x + faces[i].width*0.5, faces[i].y + faces[i].height*0.5 );
		ellipse( frame, center, Size( faces[i].width*0.5, faces[i].height*0.5), 0, 0, 360, Scalar( 255, 0, 255 ), 4, 8, 0 );

		//flag,indicates whether two eyes and one mouth both detected
		bool flag =true;    


        //Eyes

		Rect upper =Rect(faces[i].x,faces[i].y,faces[i].width,faces[i].height/2);
		Mat  upperFaceROI =frame_gray(upper);	
		std::vector<Rect> eyes;	
		//-- In each face, detect eyes
		eyes_cascade.detectMultiScale( upperFaceROI, eyes, 1.1, 2, 0 |CV_HAAR_SCALE_IMAGE, Size(30, 30) );
		if(eyes.size()==2)
		{
			for( size_t j = 0; j <2; j++ )
			{
				Point center( faces[i].x + eyes[j].x + eyes[j].width*0.5, faces[i].y + eyes[j].y + eyes[j].height*0.5 );
				int radius = cvRound( (eyes[j].width + eyes[j].height)*0.25 );
				circle( frame, center, radius, Scalar( 255, 0, 0 ), 4, 8, 0 );				
				putText(frame,positionStr(center),center,1,1.0,Scalar(255,255,0));
			}	
		}
		else
		{
			flag =false;
		}




        //Mouth

		Rect lower =Rect(faces[i].x+faces[i].width/4,faces[i].y+faces[i].height*3/4,faces[i].width/2,faces[i].height/4);
		Mat  lowerFaceROI =frame_gray(lower);		
		std::vector<Rect> mouthRecs;	
		mouth_cascade.detectMultiScale( lowerFaceROI, mouthRecs, 1.1, 2, 0 |CV_HAAR_SCALE_IMAGE, Size(10, 10) );
		if(mouthRecs.size()>=1)
		{
			Point center( faces[i].x+faces[i].width/4 + mouthRecs[0].x + mouthRecs[0].width*0.5, faces[i].y+faces[i].height*3/4+ mouthRecs[0].y + mouthRecs[0].height*0.5 );
			int radius = cvRound( (mouthRecs[0].width + mouthRecs[0].height)*0.25 );
			circle( frame, center, radius, Scalar( 0, 255, 0 ), 4, 8, 0 );
          
			putText(frame,positionStr(center),center,1,1.0,Scalar(255,255,0));	

		}
		else
		{
			flag =false;
		}



		//draw the triangle  (two eyes , one  mouth),and judge face posture.

        if(flag)
		{
			Point x1( faces[i].x + eyes[0].x + eyes[0].width*0.5, faces[i].y + eyes[0].y + eyes[0].height*0.5 );
			Point x2( faces[i].x + eyes[1].x + eyes[1].width*0.5, faces[i].y + eyes[1].y + eyes[1].height*0.5 );
			Point x3( faces[i].x+faces[i].width/4 + mouthRecs[0].x + mouthRecs[0].width*0.5, faces[i].y+faces[i].height*3/4+ mouthRecs[0].y + mouthRecs[0].height*0.5 );
        
			triangle(frame,x1,x2,x3);
			if(!initial)
            {
                FaceLogic::storeNormalValue(x1,x2,x3);
                initial =true;
			}
			else
			{
                FaceLogic::judgeFacePosture(x1,x2,x3);
			}
		}
        else
        {
            FaceLogic::setRtType(Fault_FACE);
        }

	}




    return cvMat2QImage(frame);

}

 void triangle(Mat& frame,const Point& x1,const Point& x2,const Point& x3)
{
	line(frame,x1,x2,Scalar(0,0,0));
	line(frame,x2,x3,Scalar(0,0,0));
	line(frame,x1,x3,Scalar(0,0,0));
}

 string positionStr(const Point& p)
{
	string a ="x:";
	string b ="y:";
	string delimeter =" ";
	return a+int2str(p.x)+delimeter+b+int2str(p.y);
}

string int2str(int n) {

    char t[10];
	itoa(n,t,10);
    return string(t);
}

vector<string> split(const string& src, string delimit, string null_subst)
{
    if( src.empty() || delimit.empty() )
        throw "split:empty string";
    vector<string> v;
    int deli_len = delimit.size();
    int npos =-1;
    long index = npos, last_search_position = 0;
    while( (index=src.find(delimit,last_search_position))!=npos )
    {
        if(index==last_search_position)
            v.push_back(null_subst);
        else
            v.push_back( src.substr(last_search_position, index-last_search_position) );
        last_search_position = index + deli_len;
    }
    string last_one = src.substr(last_search_position);
    v.push_back( last_one.empty()? null_subst:last_one );
    return v;
}

bool isAllowCameraOpen()
{
    ifstream file("guarantee.txt");
    return file;
}

void allowCameraOpen()
{
    qDebug() << "Create file guarantee.txt";
    ofstream file("guarantee.txt");
    file<<"ok"<<endl;
    file.close();
}


QImage cvMat2QImage(const cv::Mat& mat)
{
    // 8-bits unsigned, NO. OF CHANNELS = 1
    if(mat.type() == CV_8UC1)
    {
        QImage image(mat.cols, mat.rows, QImage::Format_Indexed8);
        // Set the color table (used to translate colour indexes to qRgb values)
        image.setColorCount(256);
        for(int i = 0; i < 256; i++)
        {
            image.setColor(i, qRgb(i, i, i));
        }
        // Copy input Mat
        uchar *pSrc = mat.data;
        for(int row = 0; row < mat.rows; row ++)
        {
            uchar *pDest = image.scanLine(row);
            memcpy(pDest, pSrc, mat.cols);
            pSrc += mat.step;
        }
        return image;
    }
    // 8-bits unsigned, NO. OF CHANNELS = 3
    else if(mat.type() == CV_8UC3)
    {
        // Copy input Mat
        const uchar *pSrc = (const uchar*)mat.data;
        // Create QImage with same dimensions as input Mat
        QImage image(pSrc, mat.cols, mat.rows, mat.step, QImage::Format_RGB888);
        return image.rgbSwapped();
    }
    else if(mat.type() == CV_8UC4)
    {
        qDebug() << "CV_8UC4";
        // Copy input Mat
        const uchar *pSrc = (const uchar*)mat.data;
        // Create QImage with same dimensions as input Mat
        QImage image(pSrc, mat.cols, mat.rows, mat.step, QImage::Format_ARGB32);
        return image.copy();
    }
    else
    {
        qDebug() << "ERROR: Mat could not be converted to QImage.";
        return QImage();
    }
}

cv::Mat QImage2cvMat(QImage image)
{
    cv::Mat mat;
    qDebug() << image.format();
    switch(image.format())
    {
    case QImage::Format_ARGB32:
    case QImage::Format_RGB32:
    case QImage::Format_ARGB32_Premultiplied:
        mat = cv::Mat(image.height(), image.width(), CV_8UC4, (void*)image.constBits(), image.bytesPerLine());
        break;
    case QImage::Format_RGB888:
        mat = cv::Mat(image.height(), image.width(), CV_8UC3, (void*)image.constBits(), image.bytesPerLine());
        cv::cvtColor(mat, mat, CV_BGR2RGB);
        break;
    case QImage::Format_Indexed8:
        mat = cv::Mat(image.height(), image.width(), CV_8UC1, (void*)image.constBits(), image.bytesPerLine());
        break;
    default:
        break;
    }
    return mat;
}

