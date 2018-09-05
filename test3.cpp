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
	double start,end;
	Mat frame;
	frame = imread("D:/1.png");
	resize(frame,frame,Size(320,240));
	imwrite("D:/2.png",frame);
	start = clock();	
	// ÖÐÖµÂË²¨£¬È¥³ý½·ÑÎÔëÉù  
    medianBlur(frame, frame, 5);  
	end = clock();
	printf("%fms\n",end-start);
}