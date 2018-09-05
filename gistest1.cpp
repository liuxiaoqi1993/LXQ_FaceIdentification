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

int FindMaxContours(vector< vector<Point> > contours, vector<Point> &maxContours);
int FindCenter(vector<Point> contour,Point &center,int step=5);
float GetAngel(Point a, Point center, Point b);
int MoveDirection(vector<Point> speeds);

void main(int argc, char *argv[])  
{
	CvCapture* pCapture = NULL;				//  
    pCapture = cvCaptureFromCAM(0);			//开启默认摄像头

	const char* WIN_SRC = "Source";			//原画面窗口
    const char* WIN_RESULT = "Result";		//结果展示窗口

	 // Windows  
    namedWindow(WIN_SRC, CV_WINDOW_AUTOSIZE );  
    namedWindow(WIN_RESULT, CV_WINDOW_AUTOSIZE);

	Mat frame;								//视频帧
	Mat frameCrCb;							//YCrCb图像
	Mat framedst,dst;

	vector< vector<Point> > contours,maxContours;		// 轮廓  及 筛选后的最大轮廓
	vector< Point > maxContour;							// 筛选后的最大轮廓  
	vector< vector<Point> > hulls;
	vector< Vec4i > hierarchy;				// 轮廓的结构信息 
	vector< Vec4i > defect;
	vector< Point > hull;					// 凸包络的点集  
	vector<int> hullsI;

	Point lasthandcenter(0,0);					//上一帧手心位置
	bool moveflag = false;					//运动标志位
	Point speed;
	vector<Point> speeds;

	//开始读帧
	cout << "程序开始运行，按Esc键可结束..." <<endl;
	while(true)
	{
		double start,end;
		start = clock();
		//获取帧
		frame = cvQueryFrame( pCapture );  
        if( frame.empty() )  
        {  
            cout << " 无法开启摄像头  "<<endl;  
            break;  
        }  
		//显示摄像头画面
        imshow( WIN_SRC, frame);
		//高斯滤波 
		GaussianBlur(frame,frame,Size(5, 5),0,0);
		//转换成CrCb图像
        cvtColor(frame, frameCrCb, CV_BGR2YCrCb);
		inRange(frameCrCb, Scalar(0,130,90), Scalar(255,175,140), framedst);
		Mat element = getStructuringElement(MORPH_RECT, Size(5, 5));
		erode(framedst, framedst, element);//白色区域腐蚀
		dilate(framedst, framedst, element);//白色区域膨胀
		frame.copyTo(dst,framedst);
		//清空上一帧提取的轮廓等信息
		contours.clear();
        hierarchy.clear();
		hulls.clear();
		maxContours.clear();
		// 计算图像中的轮廓  
        findContours(framedst, contours, hierarchy, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_SIMPLE);
		//找到最大的轮廓，作为手的轮廓
		
		int find = FindMaxContours(contours, maxContour);
		if(find >= 0)
		{
			approxPolyDP(Mat(maxContour), maxContour, maxContour.size()*0.005, true);
			maxContours.push_back(maxContour);
			drawContours(dst, maxContours, -1, Scalar(0,0,255), 2/*, 8, hierarchy*/);
			Point center;
			int handsize = FindCenter(maxContour,center);
			//printf("手掌半径%d\n",handsize);
			circle( dst, center,   4, Scalar(0,0,255), 2 );
			//凸包与凸缺陷
			convexHull(Mat(maxContour), hull, true);
			convexHull( Mat(maxContour), hullsI, false); 
			// get convexity defects
			convexityDefects(Mat(maxContour),hullsI, defect);
			hulls.push_back(hull);
			drawContours( dst, hulls, -1, Scalar(255,255,255), 1 );

			vector<Vec4i>::iterator d;
			int count1 = 0,count2 = 0;
			for(d=defect.begin();d!=defect.end();d++)
			{
				Vec4i& v=(*d);
				{
					int startidx=v[0]; 
					Point ptStart( maxContour[startidx] ); 
					int endidx=v[1]; 
					Point ptEnd( maxContour[endidx] ); 
					int faridx=v[2]; 
					Point ptFar( maxContour[faridx] );
					int depth = v[3] / 256;
 
					if(depth > 20)
					{
						line( dst, ptStart, ptFar, CV_RGB(0,255,0), 2 );
						line( dst, ptEnd, ptFar, CV_RGB(0,255,0), 2 );
						circle( dst, ptStart,   4, Scalar(255,0,100), 2 );
						circle( dst, ptEnd,   4, Scalar(255,0,100), 2 );
						circle( dst, ptFar,   4, Scalar(100,0,255), 2 );
						if(GetAngel(ptStart,ptFar,ptEnd)<= 85 && depth > handsize*0.9)
						{
							count1++;
						}
						else if(GetAngel(ptStart,ptFar,ptEnd)> 85 && depth > handsize*0.8)
						{
							count2++;
						}
					}
				}
			}
			//printf("count1 = %d,count2 = %d\n",count1,count2);
			//if(count1>0)
			//{
			//	if(count1 == 1)
			//	{
			//		printf("2\n");
			//	}
			//	else if(count1 == 2)
			//	{
			//		printf("3\n");
			//	}
			//	else if(count1 == 3)
			//	{
			//		printf("4\n");
			//	}
			//	else
			//	{
			//		printf("5\n");
			//	}
			//}
			//else
			//{
			//	if(count2 > 0)
			//	{
			//		printf("1\n");
			//	}
			//	else
			//	{
			//		printf("0\n");
			//	}
			//}
			speed = Point(center.x-lasthandcenter.x,center.y-lasthandcenter.y);
			if(!moveflag)
			{
				if(abs(speed.x) >= handsize*2 || abs(speed.y) >= handsize*2)
				{
					moveflag = true;
					speeds.push_back(speed);
				}
			}
			else
			{
				if(abs(speed.x) < handsize*2 && abs(speed.y) < handsize*2)
				{
					int dr = MoveDirection(speeds);
					if(dr == 0)
						printf("right\n");
					else if(dr == 1)
						printf("up\n");
					else if(dr == 2)
						printf("left\n");
					else if(dr == 3)
						printf("down\n");
					else
						printf("unkown\n");
					moveflag = false;
					speeds.clear();
				}

			}
			lasthandcenter = center;
		}
		imshow( WIN_RESULT, dst); 
		dst.release();
		end = clock();
		//printf("%f\n",end-start);
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
        if (fabs(contourArea(Mat(contours[i]))) > 10000 && fabs(contourArea(Mat(contours[i]))) > maxc) //判断手进入区域的阈值  
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

int FindCenter(vector<Point> contour,Point &center,int step)
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
		return maxdist;
	}
	else
	{
		return 0;
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

int MoveDirection(vector<Point> speeds)
{
	int up=0,down=0,left=0,right=0;
	int speedsize = speeds.size();
	for(int i=0;i<speedsize;i++)
	{
		if(abs(speeds[i].x)>=abs(speeds[i].y) && speeds[i].x<0)
			right++;
		else if(abs(speeds[i].x)>=abs(speeds[i].y) && speeds[i].x>0)
			left++;
		else if(abs(speeds[i].x)<abs(speeds[i].y) && speeds[i].y>0)
			down++;
		else if(abs(speeds[0].x)<abs(speeds[0].y) && speeds[0].y<0)
			up++;
	}
	speedsize++;
	if(up >= speedsize/2)
		return 1;
	if(down >= speedsize/2)
		return 3;
	if(left >= speedsize/2)
		return 2;
	if(right >= speedsize/2)
		return 0;
	return -1;
}