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
    pCapture = cvCaptureFromCAM(0);			//����Ĭ������ͷ

	const char* WIN_SRC = "Source";			//ԭ���洰��
    const char* WIN_RESULT = "Result";		//���չʾ����

	namedWindow(WIN_SRC, CV_WINDOW_AUTOSIZE );  
    namedWindow(WIN_RESULT, CV_WINDOW_AUTOSIZE);  

	int framecount = 0;
	Mat frame;								// ������Ƶ֡����
	Mat frame0,dstframe;

	while(true)
	{
		framecount++;
		//��ȡ֡
		frame = cvQueryFrame( pCapture );  
        if( frame.empty() )  
        {  
            cout << " �޷���������ͷ  "<<endl;  
            break;  
        }  
		//��ʾ����ͷ����
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