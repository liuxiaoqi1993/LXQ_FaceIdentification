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

void main(int argc, char *argv[])  
{
	CvCapture* pCapture = NULL;				//  
    pCapture = cvCaptureFromCAM(0);			//开启默认摄像头

	const char* WIN_SRC = "Source";			//原画面窗口
    const char* WIN_RESULT = "Result";		//结果展示窗口

	namedWindow(WIN_SRC, CV_WINDOW_AUTOSIZE );  
    namedWindow(WIN_RESULT, CV_WINDOW_AUTOSIZE);  

	int framecount = 0;
	Mat frame;								// 输入视频帧序列
	Mat frame0,dstframe;

	while(true)
	{
		framecount++;
		//获取帧
		frame = cvQueryFrame( pCapture );  
        if( frame.empty() )  
        {  
            cout << " 无法开启摄像头  "<<endl;  
            break;  
        }  
		//显示摄像头画面
        imshow( WIN_SRC, frame);
		if(framecount > 1)
		{
			absdiff(frame,frame0,dstframe);
			frame.copyTo(frame0);
			inRange(dstframe, Scalar(40,40,40), Scalar(255,255,255), dstframe);
			imshow( WIN_RESULT, dstframe);
		}
		else
		{
			frame.copyTo(frame0);
		}
		
        if (cvWaitKey(1) == 27)
            break;  
	}

}