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
	IplImage* src=cvLoadImage("D:/手势识别-测试数据/过曝光.png",1);
	IplImage* hsv_src=cvCreateImage(cvGetSize(src),8,3);
	cvCvtColor(src,hsv_src,CV_BGR2YCrCb);
	CvScalar s = cvGet2D(hsv_src, 0, 0);
	printf(" YCrCb : Y=%.2f, Cr=%.2f, Cb=%.2f \n" , s.val[0], s.val[1], s.val[2]);
}