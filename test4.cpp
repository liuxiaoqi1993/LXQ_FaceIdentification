//--------------------------------------������˵����-------------------------------------------
//		����˵������OpenCV3������š�OpenCV2���鱾����ʾ������07
//		����������ʹ��VideoCapture���������ͷ������Ƶ����ʾ
//		�����������ò���ϵͳ�� Windows 7 64bit
//		������������IDE�汾��Visual Studio 2010
//		������������OpenCV�汾��	2.4.9
//		2014��03�� Created by @ǳī_ë����
//		2014��11�� Revised by @ǳī_ë����
//------------------------------------------------------------------------------------------------
//---------------------------------��ͷ�ļ��������ռ�������֡�----------------------------
//		����������������ʹ�õ�ͷ�ļ��������ռ�
//-------------------------------------------------------------------------------------------------
#include <opencv2\opencv.hpp>  
using namespace cv;  
using namespace std;
Mat frame;  //����һ��Mat���������ڴ洢ÿһ֡��ͼ��
double time0;
int exposure = -1;
//-----------------------------------��main( )������--------------------------------------------
//		����������̨Ӧ�ó������ں��������ǵĳ�������￪ʼ
//-------------------------------------------------------------------------------------------------
int main( )  
{  
	//��1��������ͷ������Ƶ
	VideoCapture capture(0);
	if(false == capture.isOpened())
	{
		return -1;
	}
	exposure = capture.get(CV_CAP_PROP_EXPOSURE);
	//��ʾ�ع�ֵ
	cout << ">����ǰ: �ع�ֵ= " << exposure << endl;
	//�����ع�ֵ
	capture.set(CV_CAP_PROP_EXPOSURE, exposure);
	exposure = capture.get(CV_CAP_PROP_EXPOSURE);
	//��ʾ�ع�ֵ
	cout << ">���ú�: �ع�ֵ= " << exposure << endl;
	double width = capture.get(CV_CAP_PROP_FRAME_WIDTH);
	double height = capture.get(CV_CAP_PROP_FRAME_HEIGHT);
	//��ʾ�ߴ�
	cout << ">��= " << width << ";  ��: =" << height << endl;
	//��2��ѭ����ʾÿһ֡
	while(1)  
	{  
		
		time0 = static_cast<double>(getTickCount( ));//��¼��ʼʱ��
		capture >> frame;  //��ȡ��ǰ֡
		//����Ƶ������ɣ��˳�ѭ��
		if (frame.empty())
		{
			break;
		}
		imshow("UVC",frame);  //��ʾ��ǰ֡
		//��ʾ֡��
		//cout << ">֡��= " << getTickFrequency() / (getTickCount() - time0) << endl;
		char c = (char)waitKey(10);
		if( c == 27 )
			break;
	}  
	return 0;     
}
