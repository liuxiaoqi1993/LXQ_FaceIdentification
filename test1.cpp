#include <iostream>   // for standard I/O  
#include <string>   // for strings  
#include <iomanip>  // for controlling float print precision  
#include <sstream>  // string to number conversion  
#include<windows.h>  
#include "cv.h"    
#include "highgui.h"    
#include "cxcore.h"  
#include <opencv2/imgproc/imgproc.hpp>  // Gaussian Blur  
#include <opencv2/core/core.hpp>        // Basic OpenCV structures (cv::Mat, Scalar)  
#include <opencv2/highgui/highgui.hpp>  // OpenCV window I/O  

using namespace cv;  
using namespace std;  

int FindMaxContours(vector< vector<Point> > contours, vector<Point> &maxContours);			//Ѱ���������
Point FindCenter(vector<Point> contour,int step = 10);										//�������������ĵ�
float GetAngel(Point a, Point center, Point b);												//����������֮��ļн�
void polyPoint(vector<Point> &points, Point center);										//ɾ������Ͻ��ĵ�
//
//void main(int argc, char *argv[]) 
//{
//	printf("%f\n",GetAngel(Point(0,0),Point(1,1),Point(2,0)));
//}
void main(int argc, char *argv[])  
{
	CvCapture* pCapture = NULL;//  
    pCapture = cvCaptureFromCAM(0);  //����Ĭ������ͷ

	const char* WIN_SRC = "Source";  //ԭ���洰��
    const char* WIN_RESULT = "Result"; //�������

	 // Windows  
    namedWindow(WIN_SRC, CV_WINDOW_AUTOSIZE );  
    namedWindow(WIN_RESULT, CV_WINDOW_AUTOSIZE);  

	Mat frame;  // ������Ƶ֡����
	Mat frameCrCb;	//Crcbͼ��
	Mat framedst;
	Mat dst;

	vector< vector<Point> > contours;   // ����  
    vector< Point > maxContour;			// ɸѡ����������  
    vector< Vec4i > hierarchy;			// �����Ľṹ��Ϣ  
    vector< Point > hull;				// ͹����ĵ㼯  


	//��ʼ��֡
	cout << "����ʼ���У���Esc���ɽ���..." <<endl;
	while(true)
	{
		double start,end;
		//��ȡ֡
		frame = cvQueryFrame( pCapture );  
        if( frame.empty() )  
        {  
            cout << " �޷���������ͷ  "<<endl;  
            break;  
        }  
		//��ʾ����ͷ����
        imshow( WIN_SRC, frame);
		start = clock();
		
		// ��ֵ�˲���ȥ����������  
        medianBlur(frame, frame, 5);  
		
		//ת����CrCbͼ��
        cvtColor(frame, frameCrCb, CV_BGR2YCrCb);
		inRange(frameCrCb, Scalar(0,130,90), Scalar(255,175,140), framedst);
		Mat element = getStructuringElement(MORPH_RECT, Size(3, 3));
		dilate(framedst, framedst, element);//��ɫ��������
		morphologyEx(framedst, framedst, MORPH_CLOSE, element);
		erode(framedst, framedst, element);//��ɫ����ʴ
		morphologyEx(framedst, framedst, MORPH_OPEN, element); 
		//imshow( WIN_RESULT, framedst);
		//framedst.copyTo(dst);  
		frame.copyTo(dst,framedst);
		contours.clear();  
        hierarchy.clear();  
        // �õ��ֵ�����  
        findContours(framedst, contours, hierarchy, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_SIMPLE);  
		
        // ȥ��α����  
        int find = FindMaxContours(contours, maxContour);
		if(find >= 0)  
		{
			drawContours(dst, contours, find, Scalar(255,255,255), 3/*, 8, hierarchy*/); 
			//Ѱ��͹��
            convexHull(Mat(maxContour), hull, true);  
			Point center = FindCenter(maxContour);
			circle(dst, center,3,Scalar(100,255,0));
			polyPoint(hull,center);
			int hullcount = (int)hull.size();  
            for (int i=0; i<hullcount; i++)  
            {  
                line(dst, center, hull[i], Scalar(255,0,0), 2, CV_AA);//��ɫ
			}
			//line(dst, hull[hullcount-1], hull[0], Scalar(0,255,0), 2, CV_AA);//��ɫ�����һ��
		}
		end = clock();
		printf("%fs\n",end-start);
		//
		imshow( WIN_RESULT, dst);
		dst.release();
        if (cvWaitKey(1) == 27)
            break;  
	}
}

int FindMaxContours(vector< vector<Point> > contours, vector<Point> &maxContours)
{
	int maxc = 0,maxindex = -1;
    for (size_t i = 0; i < contours.size(); i++)  
    {             
    //  approxPolyDP(Mat(contours[i]), Mat(approxContours[i]), arcLength(Mat(contours[i]), true)*0.02, true);  
        if (fabs(contourArea(Mat(contours[i]))) > 30000 && fabs(contourArea(Mat(contours[i]))) > maxc) //�ж��ֽ����������ֵ  
        {  
			maxindex = i;  
        }  
    }
	if(maxindex >= 0)
	{
		maxContours = contours[maxindex];
	}
	return maxindex;
}

Point FindCenter(vector<Point> contour,int step)
{
	int minX=10000,maxX=0;
	int minY=10000,maxY=0;
	for(int i = 0;i < contour.size();i++)
	{
		if(contour[i].x < minX)
			minX = contour[i].x;
		if(contour[i].y < minY)
			minY = contour[i].y;
		if(contour[i].x > maxX)
			maxX = contour[i].x;
		if(contour[i].y > maxY)
			maxY = contour[i].y;
	}
	if(contour.size())
	{
		int dist=0,maxdist=0;
		Point center;
		for(int x = minX; x <= maxX; x = x+step)
		{
			for(int y = minY; y <= maxY; y = y+step)
			{
				dist = pointPolygonTest(contour,cv::Point(x,y),true);
				if(dist>maxdist)
				{
					maxdist = dist;
					center = cv::Point(x,y);
				}
			}
		}
		return center;
	}
	else
	{
		return Point(0,0);
	}
}

float GetAngel(Point a, Point center, Point b)
{
	int dxCA,dyCA,dxCB,dyCB;
	dxCA = a.x - center.x;
	dyCA = a.y - center.y;
	dxCB = b.x - center.x;
	dyCB = b.y - center.y;

	float costhta = (dxCA*dxCB + dyCA*dyCB)*1.0 / (sqrt((dxCA*dxCA + dyCA*dyCA)*1.0)*sqrt((dxCB*dxCB + dyCB*dyCB)*1.0));
	float thta = acos(costhta);
	thta = thta*180/3.1415926;
	return thta;
}

void polyPoint(vector<Point> &points, Point center)
{
	for (vector<Point>::iterator it = points.begin(); it != points.end()-1;)
	{
		if(GetAngel(*it,center,*(it+1)) <= 10)
		{
			it = points.erase(it);
		}
		else
		{
			it++;
		}
	}
}