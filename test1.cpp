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

int FindMaxContours(vector< vector<Point> > contours, vector<Point> &maxContours);			//寻找最大轮廓
Point FindCenter(vector<Point> contour,int step = 10);										//计算轮廓的中心点
float GetAngel(Point a, Point center, Point b);												//计算三个点之间的夹角
void polyPoint(vector<Point> &points, Point center);										//删除距离较近的点
//
//void main(int argc, char *argv[]) 
//{
//	printf("%f\n",GetAngel(Point(0,0),Point(1,1),Point(2,0)));
//}
void main(int argc, char *argv[])  
{
	CvCapture* pCapture = NULL;//  
    pCapture = cvCaptureFromCAM(0);  //开启默认摄像头

	const char* WIN_SRC = "Source";  //原画面窗口
    const char* WIN_RESULT = "Result"; //结果窗口

	 // Windows  
    namedWindow(WIN_SRC, CV_WINDOW_AUTOSIZE );  
    namedWindow(WIN_RESULT, CV_WINDOW_AUTOSIZE);  

	Mat frame;  // 输入视频帧序列
	Mat frameCrCb;	//Crcb图像
	Mat framedst;
	Mat dst;

	vector< vector<Point> > contours;   // 轮廓  
    vector< Point > maxContour;			// 筛选后的最大轮廓  
    vector< Vec4i > hierarchy;			// 轮廓的结构信息  
    vector< Point > hull;				// 凸包络的点集  


	//开始读帧
	cout << "程序开始运行，按Esc键可结束..." <<endl;
	while(true)
	{
		double start,end;
		//获取帧
		frame = cvQueryFrame( pCapture );  
        if( frame.empty() )  
        {  
            cout << " 无法开启摄像头  "<<endl;  
            break;  
        }  
		//显示摄像头画面
        imshow( WIN_SRC, frame);
		start = clock();
		
		// 中值滤波，去除椒盐噪声  
        medianBlur(frame, frame, 5);  
		
		//转换成CrCb图像
        cvtColor(frame, frameCrCb, CV_BGR2YCrCb);
		inRange(frameCrCb, Scalar(0,130,90), Scalar(255,175,140), framedst);
		Mat element = getStructuringElement(MORPH_RECT, Size(3, 3));
		dilate(framedst, framedst, element);//白色区域膨胀
		morphologyEx(framedst, framedst, MORPH_CLOSE, element);
		erode(framedst, framedst, element);//白色区域腐蚀
		morphologyEx(framedst, framedst, MORPH_OPEN, element); 
		//imshow( WIN_RESULT, framedst);
		//framedst.copyTo(dst);  
		frame.copyTo(dst,framedst);
		contours.clear();  
        hierarchy.clear();  
        // 得到手的轮廓  
        findContours(framedst, contours, hierarchy, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_SIMPLE);  
		
        // 去除伪轮廓  
        int find = FindMaxContours(contours, maxContour);
		if(find >= 0)  
		{
			drawContours(dst, contours, find, Scalar(255,255,255), 3/*, 8, hierarchy*/); 
			//寻找凸包
            convexHull(Mat(maxContour), hull, true);  
			Point center = FindCenter(maxContour);
			circle(dst, center,3,Scalar(100,255,0));
			polyPoint(hull,center);
			int hullcount = (int)hull.size();  
            for (int i=0; i<hullcount; i++)  
            {  
                line(dst, center, hull[i], Scalar(255,0,0), 2, CV_AA);//白色
			}
			//line(dst, hull[hullcount-1], hull[0], Scalar(0,255,0), 2, CV_AA);//绿色，最后一条
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
        if (fabs(contourArea(Mat(contours[i]))) > 30000 && fabs(contourArea(Mat(contours[i]))) > maxc) //判断手进入区域的阈值  
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