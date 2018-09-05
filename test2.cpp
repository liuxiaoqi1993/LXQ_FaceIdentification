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
Point FindCenter(vector<Point> contour,int step = 10);									//计算轮廓的中心点
float GetAngel(Point a, Point center, Point b);

void main(int argc, char *argv[])  
{
	CvCapture* pCapture = NULL;				//  
    pCapture = cvCaptureFromCAM(0);			//开启默认摄像头

	const char* WIN_SRC = "Source";			//原画面窗口
    const char* WIN_RESULT = "Result";		//结果展示窗口

	 // Windows  
    namedWindow(WIN_SRC, CV_WINDOW_AUTOSIZE );  
    namedWindow(WIN_RESULT, CV_WINDOW_AUTOSIZE);  

	Mat frame;								// 输入视频帧序列
	Mat frameCrCb;							//Crcb图像
	Mat framedst;
	Mat mask(frame.rows, frame.cols, CV_8UC1);  // 2值掩膜
	Mat dst;

	vector< vector<Point> > contours;		// 轮廓  
	vector< Point > maxContour;				// 筛选后的最大轮廓  
	vector< vector<Point> > maxContours;			// 筛选后的最大轮廓  
    vector< Vec4i > hierarchy;				// 轮廓的结构信息 
	vector< Vec4i > defect;
	vector< vector<Point> > hulls;
    vector< Point > hull;					// 凸包络的点集  
	vector<int> hullsI;
	RotatedRect box;

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
        //medianBlur(frame, frame, 3);  
		GaussianBlur(frame,frame,Size(5, 5),0,0);
		end = clock();
		//printf("%fs\n",end-start);
		//转换成CrCb图像
        cvtColor(frame, frameCrCb, CV_BGR2YCrCb);
		inRange(frameCrCb, Scalar(0,130,90), Scalar(255,175,140), framedst);
		Mat element = getStructuringElement(MORPH_RECT, Size(5, 5));
		erode(framedst, framedst, element);//白色区域腐蚀
		dilate(framedst, framedst, element);//白色区域膨胀
		//morphologyEx(framedst, framedst, MORPH_CLOSE, element);
		
		//morphologyEx(framedst, framedst, MORPH_OPEN, element); 
		//imshow( WIN_RESULT, framedst);
		//framedst.copyTo(dst);  
		frame.copyTo(dst,framedst);
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
			//Point2f rect[4];
			//box = minAreaRect(Mat(maxContour));
			//box.points(rect);
			//for(int j=0; j<4; j++)
			//{
   //         line(dst, rect[j], rect[(j+1)%4], Scalar(0, 0, 255), 2, 8);  //绘制最小外接矩形每条边
			//}
			approxPolyDP(Mat(maxContour), maxContour, 5, true);
			maxContours.push_back(maxContour);
			Point p = FindCenter(maxContour);
			circle( dst, p,   4, Scalar(0,0,255), 2 );
			//drawContours(dst, contours, find, Scalar(255,255,255), 3/*, 8, hierarchy*/); 
			//寻找凸包
            convexHull(Mat(maxContour), hull, true);
			convexHull( Mat(maxContour), hullsI, false); 
			// get convexity defects
			convexityDefects(Mat(maxContour),hullsI, defect);
			drawContours( dst, maxContours, -1, Scalar(0,0,255), 2, 8, vector<Vec4i>(), 0, Point() );
			hulls.push_back(hull);
			drawContours( dst, hulls, -1, Scalar(255,255,255), 1, 8, vector<Vec4i>(), 0, Point() );
			
			vector<Vec4i>::iterator d =defect.begin();
			int count1 = 0,count2 = 0;
			while( d!=defect.end() ) {
				Vec4i& v=(*d);
				//if(IndexOfBiggestContour == i)
				{
 
					int startidx=v[0]; 
					Point ptStart( maxContour[startidx] ); // point of the contour where the defect begins
					int endidx=v[1]; 
					Point ptEnd( maxContour[endidx] ); // point of the contour where the defect ends
					int faridx=v[2]; 
					Point ptFar( maxContour[faridx] );// the farthest from the convex hull point within the defect
					int depth = v[3] / 256; // distance between the farthest point and the convex hull
 
					if(depth > 20)
					{
						line( dst, ptStart, ptFar, CV_RGB(0,255,0), 2 );
						line( dst, ptEnd, ptFar, CV_RGB(0,255,0), 2 );
						circle( dst, ptStart,   4, Scalar(255,0,100), 2 );
						circle( dst, ptEnd,   4, Scalar(255,0,100), 2 );
						circle( dst, ptFar,   4, Scalar(100,0,255), 2 );
						//printf("%f \n",GetAngel(ptStart,ptFar,ptEnd));
						if(GetAngel(ptStart,ptFar,ptEnd)<= 85 && depth > 80)
						{
							count1++;
						}
						else if(GetAngel(ptStart,ptFar,ptEnd)> 85 && depth > 80)
						{
							count2++;
						}
					}
 
					/*printf("start(%d,%d) end(%d,%d), far(%d,%d)\n",
						ptStart.x, ptStart.y, ptEnd.x, ptEnd.y, ptFar.x, ptFar.y);*/
				}
				d++;
			}
			if(count1>0)
			{
				if(count1 == 1)
				{
					printf("2\n");
				}
				else if(count1 == 2)
				{
					printf("3\n");
				}
				else if(count1 == 3)
				{
					printf("4\n");
				}
				else
				{
					printf("5\n");
				}
			}
			else
			{
				if(count2 > 0)
				{
					printf("1\n");
				}
				else
				{
					printf("0\n");
				}
			}
		}
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
		//printf("%d\n",maxdist);
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