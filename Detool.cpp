#include "StdAfx.h"
#include "Detool.h"
using namespace std;
using namespace seeta;

#define TEST(major, minor) major##_##minor##_Tester()
#define EXPECT_NE(a, b) if ((a) == (b)) std::cout << "ERROR: "
#define EXPECT_EQ(a, b) if ((a) != (b)) std::cout << "ERROR: "


#ifdef _WIN32
#pragma once
#include <opencv2/core/version.hpp>

#define CV_VERSION_ID CVAUX_STR(CV_MAJOR_VERSION) CVAUX_STR(CV_MINOR_VERSION) \
  CVAUX_STR(CV_SUBMINOR_VERSION)

#ifdef _DEBUG
#define cvLIB(name) "opencv_" name CV_VERSION_ID "d"
#else
#define cvLIB(name) "opencv_" name CV_VERSION_ID
#endif //_DEBUG
#endif //_WIN32

#if defined(__unix__) || defined(__APPLE__)

#ifndef fopen_s

#define fopen_s(pFile,filename,mode) ((*(pFile))=fopen((filename),(mode)))==NULL

#endif //fopen_s

#endif //__unix
#include "common.h"
#include "math.h"
#include "time.h"
#include <fstream>
#include <vector>
#include <stdlib.h>

#include "ctime"

using namespace seeta;

#define TEST(major, minor) major##_##minor##_Tester()
#define EXPECT_NE(a, b) if ((a) == (b)) std::cout << "ERROR: "
#define EXPECT_EQ(a, b) if ((a) != (b)) std::cout << "ERROR: "

#ifdef _WIN32
//std::string DATA_DIR = "D:/SeetaFaceEngine-master/FaceIdentification/data/";
//std::string MODEL_DIR = "D:/SeetaFaceEngine-master/FaceIdentification/model/";
#else
std::string DATA_DIR = "./data/";
std::string MODEL_DIR = "./model/";
#endif


Detool::Detool(void)
{
}


Detool::~Detool(void)
{
}
cv::Mat Detool::startDetection(Mat img){
    // const char* img_path = "D:SeetaFaceEngine-master/FaceDetection/data/0_1_1.jpg";
    const char* model_path = "seeta_fd_frontal_v1.0.bin";
    seeta::FaceDetection detector(model_path);

    detector.SetMinFaceSize(40);
    detector.SetScoreThresh(2.f);
    detector.SetImagePyramidScaleFactor(0.8f);
    detector.SetWindowStep(4, 4);

    //cv::Mat img = cv::imread(img_path, cv::IMREAD_UNCHANGED);
    cv::Mat img_gray;

    if (img.channels() != 1)
        cv::cvtColor(img, img_gray, cv::COLOR_BGR2GRAY);
    else
        img_gray = img;

    seeta::ImageData img_data;
    img_data.data = img_gray.data;
    img_data.width = img_gray.cols;
    img_data.height = img_gray.rows;
    img_data.num_channels = 1;

    long t0 = cv::getTickCount();
    std::vector<seeta::FaceInfo> faces = detector.Detect(img_data);
    printf("123");
    long t1 = cv::getTickCount();
    double secs = (t1 - t0)/cv::getTickFrequency();

    cout << "Detections takes " << secs << " seconds " << endl;
#ifdef USE_OPENMP
    cout << "OpenMP is used." << endl;
#else
    cout << "OpenMP is not used. " << endl;
#endif

#ifdef USE_SSE
    cout << "SSE is used." << endl;
#else
    cout << "SSE is not used." << endl;
#endif

    cout << "Image size (wxh): " << img_data.width << "x" 
        << img_data.height << endl;

    cv::Rect face_rect;
    int32_t num_face = static_cast<int32_t>(faces.size());

    for (int32_t i = 0; i < num_face; i++) {
        face_rect.x = faces[i].bbox.x;
        face_rect.y = faces[i].bbox.y;
        face_rect.width = faces[i].bbox.width;
        face_rect.height = faces[i].bbox.height;
        cv::rectangle(img, face_rect, CV_RGB(0, 0, 255), 4, 8, 0);
    }
/*
    cv::namedWindow("Test", cv::WINDOW_AUTOSIZE);
    cv::imshow("Test", img);
    cv::waitKey(0);
    cv::destroyAllWindows();
*/
    return img;
}

void Detool::startalignment(cv::Mat img){
    seeta::FaceDetection detector("seeta_fd_frontal_v1.0.bin");
    detector.SetMinFaceSize(40);
    detector.SetScoreThresh(2.f);
    detector.SetImagePyramidScaleFactor(0.8f);
    detector.SetWindowStep(4, 4);

    // Initialize face alignment model 
    seeta::FaceAlignment point_detector("seeta_fa_v1.1.bin");

    //load image
    Mat Matgray;
    cvtColor(img, Matgray, CV_BGR2GRAY);
    IplImage *img_grayscale = NULL;
    img_grayscale = &IplImage(Matgray);     //&IplImage(img);     
    if (img_grayscale == NULL)
    {
        return ;
    }
    //转换
    IplImage *img_color =  &IplImage(img);
    int pts_num = 5;
    int im_width = img_grayscale->width;
    int im_height = img_grayscale->height;
    unsigned char* data = new unsigned char[im_width * im_height];
    unsigned char* data_ptr = data;
    unsigned char* image_data_ptr = (unsigned char*)img_grayscale->imageData;
    int h = 0;
    for (h = 0; h < im_height; h++) {
        memcpy(data_ptr, image_data_ptr, im_width);
        data_ptr += im_width;
        image_data_ptr += img_grayscale->widthStep;
    }

    seeta::ImageData image_data;
    image_data.data = data;
    image_data.width = im_width;
    image_data.height = im_height;
    image_data.num_channels = 1;

    // Detect faces
    std::vector<seeta::FaceInfo> faces = detector.Detect(image_data);
    int32_t face_num = static_cast<int32_t>(faces.size());

    if (face_num == 0)
    {
        delete[]data;
        cvReleaseImage(&img_grayscale);
        cvReleaseImage(&img_color);
        return ;
    }

    // Detect 5 facial landmarks
    seeta::FacialLandmark points[5];
    point_detector.PointDetectLandmarks(image_data, faces[0], points);

    // Visualize the results
    cvRectangle(img_color, cvPoint(faces[0].bbox.x, faces[0].bbox.y), cvPoint(faces[0].bbox.x + faces[0].bbox.width - 1, faces[0].bbox.y + faces[0].bbox.height - 1), CV_RGB(255, 0, 0));
    for (int i = 0; i<pts_num; i++)
    {
        cvCircle(img_color, cvPoint(points[i].x, points[i].y), 2, CV_RGB(0, 255, 0), CV_FILLED);
    }

    //cvSaveImage("result.jpg", img_color);

    // Release memory

    cvNamedWindow("Image:",1);
    cvShowImage("Image:",img_color);
    cvWaitKey();
    cvDestroyWindow("Image:");

    //局部变量不需要释放
    //cvReleaseImage(&img_color);
    //cvReleaseImage(&img_grayscale);
    //delete[]data;

}

float Detool::startIdentification(cv::Mat img){
    float sim;
    float Threshold = 0.65;
    seeta::FaceDetection detector("seeta_fd_frontal_v1.0.bin");
    detector.SetMinFaceSize(40);
    detector.SetScoreThresh(2.f);
    detector.SetImagePyramidScaleFactor(0.8f);
    detector.SetWindowStep(4, 4);

    // Initialize face alignment model 
    seeta::FaceAlignment point_detector("seeta_fa_v1.1.bin");

    // Initialize face Identification model 
    FaceIdentification face_recognizer("seeta_fr_v1.0.bin");
    //std::string test_dir = DATA_DIR + "test_face_recognizer/";

    //load image
    cv::Mat gallery_img_color = img; //cv::imread("C:\\Users\\Administrator\\Desktop\\789.jpg", 1);
    cv::Mat gallery_img_gray;
    cv::cvtColor(gallery_img_color, gallery_img_gray, CV_BGR2GRAY);
    
    ImageData gallery_img_data_color(gallery_img_color.cols, gallery_img_color.rows, gallery_img_color.channels());
    gallery_img_data_color.data = gallery_img_color.data;
    
    ImageData gallery_img_data_gray(gallery_img_gray.cols, gallery_img_gray.rows, gallery_img_gray.channels());
    gallery_img_data_gray.data = gallery_img_gray.data;
    
    std::vector<seeta::FaceInfo> gallery_faces = detector.Detect(gallery_img_data_gray);
    int32_t gallery_face_num = static_cast<int32_t>(gallery_faces.size());
     
    seeta::FacialLandmark gallery_points[5];
    point_detector.PointDetectLandmarks(gallery_img_data_gray, gallery_faces[0], gallery_points);
    if (gallery_face_num == 0)
    {
        std::cout << "Faces are not detected.";
        return 0;
    }

    for (int i = 0; i<5; i++)
    {
        cv::circle(gallery_img_color, cv::Point(gallery_points[i].x, gallery_points[i].y), 2,
            CV_RGB(0, 255, 0));
    }

     float gallery_fea[2048];
     face_recognizer.ExtractFeatureWithCrop(gallery_img_data_color, gallery_points, gallery_fea);
    ifstream fp("C:\\Users\\Administrator\\Desktop\\UerInfo.txt");
    string a;
    while(getline(fp,a)){
        //getline(fp,a);
        cv::Mat probe_img_color = cv::imread("C:\\Users\\Administrator\\Desktop\\Userpicture\\"+ a, 1);
        cv::Mat probe_img_gray;
        cv::cvtColor(probe_img_color, probe_img_gray, CV_BGR2GRAY);

        ImageData probe_img_data_color(probe_img_color.cols, probe_img_color.rows, probe_img_color.channels());
        probe_img_data_color.data = probe_img_color.data;

        ImageData probe_img_data_gray(probe_img_gray.cols, probe_img_gray.rows, probe_img_gray.channels());
        probe_img_data_gray.data = probe_img_gray.data;

        // Detect faces
        std::vector<seeta::FaceInfo> probe_faces = detector.Detect(probe_img_data_gray);
        int32_t probe_face_num = static_cast<int32_t>(probe_faces.size());

        if (probe_face_num==0)
        {
            std::cout << "Faces are not detected.";
            return 0;
        }

        // Detect 5 facial landmarks
        seeta::FacialLandmark probe_points[5];
        point_detector.PointDetectLandmarks(probe_img_data_gray, probe_faces[0], probe_points);

        for (int i = 0; i<5; i++)
        {
            cv::circle(gallery_img_color, cv::Point(gallery_points[i].x, gallery_points[i].y), 2,
                CV_RGB(0, 255, 0));
            cv::circle(probe_img_color, cv::Point(probe_points[i].x, probe_points[i].y), 2,
                CV_RGB(0, 255, 0));
        }
        cv::imwrite("gallery_point_result.jpg", gallery_img_color);
        cv::imwrite("probe_point_result.jpg", probe_img_color);

        // Extract face identity feature

        float probe_fea[2048]; 
        face_recognizer.ExtractFeatureWithCrop(probe_img_data_color, probe_points, probe_fea);

        // Caculate similarity of two faces
        sim = face_recognizer.CalcSimilarity(gallery_fea, probe_fea);
        if(sim >= Threshold){
            MessageBox(GetForegroundWindow(),"匹配成功","error",1);
            fp.close();
            return sim;
        }
        //std::cout << sim <<endl;
        //char s[10];
        //sprintf(s, "%f", sim);
        //MessageBox(NULL,s, TEXT("操作提示"), 1);
        //MessageBox(GetForegroundWindow(),s,"sim value",1);
    }
         MessageBox(GetForegroundWindow(),"匹配失败","error",1);
         fp.close();
    return sim;
}


#ifdef _flag
void TEST(FaceRecognizerTest, CropFace) {
 FaceIdentification face_recognizer((MODEL_DIR + "seeta_fr_v1.0.bin").c_str());
  std::string test_dir = DATA_DIR + "test_face_recognizer/";
  /* data initialize */
  std::ifstream ifs;
  std::string img_name;
  FacialLandmark pt5[5];
  ifs.open(test_dir + "test_file_list.txt", std::ifstream::in);
  clock_t start, count = 0;
  int img_num = 0;
  while (ifs >> img_name) {
    img_num ++ ;
    // read image
    cv::Mat src_img = cv::imread(test_dir + img_name, 1);
    EXPECT_NE(src_img.data, nullptr) << "Load image error!";

    // ImageData store data of an image without memory alignment.
    ImageData src_img_data(src_img.cols, src_img.rows, src_img.channels());
    src_img_data.data = src_img.data;

    // 5 located landmark points (left eye, right eye, nose, left and right 
    // corner of mouse).
    for (int i = 0; i < 5; ++ i) {
      ifs >> pt5[i].x >> pt5[i].y;
    }

    // Create a image to store crop face.
    cv::Mat dst_img(face_recognizer.crop_height(),
      face_recognizer.crop_width(),
      CV_8UC(face_recognizer.crop_channels()));
    ImageData dst_img_data(dst_img.cols, dst_img.rows, dst_img.channels());
    dst_img_data.data = dst_img.data;
    /* Crop Face */
    start = clock();
    face_recognizer.CropFace(src_img_data, pt5, dst_img_data);
    count += clock() - start;
    // Show crop face
    //    cv::imshow("Crop Face", dst_img);
    //    cv::waitKey(0);
    //    cv::destroyWindow("Crop Face");
  }
  ifs.close();
  std::cout << "Test successful! \nAverage crop face time: "
    << 1000.0 * count / CLOCKS_PER_SEC / img_num << "ms" << std::endl;
}

void TEST(FaceRecognizerTest, ExtractFeature) {
  FaceIdentification face_recognizer((MODEL_DIR + "seeta_fr_v1.0.bin").c_str());
  std::string test_dir = DATA_DIR + "test_face_recognizer/";

  int feat_size = face_recognizer.feature_size();
  EXPECT_EQ(feat_size, 2048);

  FILE* feat_file = NULL;

  // Load features extract from caffe
  fopen_s(&feat_file, (test_dir + "feats.dat").c_str(), "rb");
  int n, c, h, w;
  EXPECT_EQ(fread(&n, sizeof(int), 1, feat_file), (unsigned int)1);
  EXPECT_EQ(fread(&c, sizeof(int), 1, feat_file), (unsigned int)1);
  EXPECT_EQ(fread(&h, sizeof(int), 1, feat_file), (unsigned int)1);
  EXPECT_EQ(fread(&w, sizeof(int), 1, feat_file), (unsigned int)1);
  float* feat_caffe = new float[n * c * h * w];
  float* feat_sdk = new float[n * c * h * w];
  EXPECT_EQ(fread(feat_caffe, sizeof(float), n * c * h * w, feat_file),
    n * c * h * w);
  EXPECT_EQ(feat_size, c * h * w);

  int cnt = 0;

  /* Data initialize */
  std::ifstream ifs(test_dir + "crop_file_list.txt");
  std::string img_name;

  clock_t start, count = 0;
  int img_num = 0, lb;
  double average_sim = 0.0;
  while (ifs >> img_name >> lb) {
    // read image
    cv::Mat src_img = cv::imread(test_dir + img_name, 1);
    EXPECT_NE(src_img.data, nullptr) << "Load image error!";
    cv::resize(src_img, src_img, cv::Size(face_recognizer.crop_height(),
      face_recognizer.crop_width()));

    // ImageData store data of an image without memory alignment.
    ImageData src_img_data(src_img.cols, src_img.rows, src_img.channels());
    src_img_data.data = src_img.data;

    /* Extract feature */
    start = clock();
    face_recognizer.ExtractFeature(src_img_data,
      feat_sdk + img_num * feat_size);
    count += clock() - start;

    /* Caculate similarity*/
    float* feat1 = feat_caffe + img_num * feat_size;
    float* feat2 = feat_sdk + img_num * feat_size;
    float sim = face_recognizer.CalcSimilarity(feat1, feat2);
    average_sim += sim;
    img_num ++ ;
  }
  ifs.close();
  average_sim /= img_num;
  if (1.0 - average_sim >  0.01) {
    std::cout<< "average similarity: " << average_sim << std::endl;
  }
  else {
    std::cout << "Test successful!\nAverage extract feature time: "
      << 1000.0 * count / CLOCKS_PER_SEC / img_num << "ms" << std::endl;
  }
  delete []feat_caffe;
  delete []feat_sdk;
}

void TEST(FaceRecognizerTest, ExtractFeatureWithCrop) {
  FaceIdentification face_recognizer((MODEL_DIR + "seeta_fr_v1.0.bin").c_str());
  //std::string test_dir = DATA_DIR + "test_face_recognizer/";

  int feat_size = face_recognizer.feature_size();
  EXPECT_EQ(feat_size, 2048);

  FILE* feat_file = NULL;

  // Load features extract from caffe
  fopen_s(&feat_file, (test_dir + "feats.dat").c_str(), "rb");
  int n, c, h, w;
  EXPECT_EQ(fread(&n, sizeof(int), 1, feat_file), (unsigned int)1);
  EXPECT_EQ(fread(&c, sizeof(int), 1, feat_file), (unsigned int)1);
  EXPECT_EQ(fread(&h, sizeof(int), 1, feat_file), (unsigned int)1);
  EXPECT_EQ(fread(&w, sizeof(int), 1, feat_file), (unsigned int)1);
  float* feat_caffe = new float[n * c * h * w];
  float* feat_sdk = new float[n * c * h * w];
  EXPECT_EQ(fread(feat_caffe, sizeof(float), n * c * h * w, feat_file),
    n * c * h * w);
  EXPECT_EQ(feat_size, c * h * w);

  int cnt = 0;

  /* Data initialize */
  std::ifstream ifs(test_dir + "test_file_list.txt");
  std::string img_name;
  FacialLandmark pt5[5];

  clock_t start, count = 0;
  int img_num = 0;
  double average_sim = 0.0;
  while (ifs >> img_name) {
    // read image
    cv::Mat src_img = cv::imread(test_dir + img_name, 1);
    EXPECT_NE(src_img.data, nullptr) << "Load image error!";

    // ImageData store data of an image without memory alignment.
    ImageData src_img_data(src_img.cols, src_img.rows, src_img.channels());
    src_img_data.data = src_img.data;

    // 5 located landmark points (left eye, right eye, nose, left and right 
    // corner of mouse).
    for (int i = 0; i < 5; ++ i) {
      ifs >> pt5[i].x >> pt5[i].y;
    }

    /* Extract feature: ExtractFeatureWithCrop */
    start = clock();
    face_recognizer.ExtractFeatureWithCrop(src_img_data, pt5,
      feat_sdk + img_num * feat_size);
    count += clock() - start;

    /* Caculate similarity*/
    float* feat1 = feat_caffe + img_num * feat_size;
    float* feat2 = feat_sdk + img_num * feat_size;
    float sim = face_recognizer.CalcSimilarity(feat1, feat2);
    average_sim += sim;
    img_num ++ ;
  }
  ifs.close();
  average_sim /= img_num;
  if (1.0 - average_sim >  0.02) {
    std::cout<< "average similarity: " << average_sim << std::endl;
  }
  else {
    std::cout << "Test successful!\nAverage extract feature time: "
      << 1000.0 * count / CLOCKS_PER_SEC / img_num << "ms" << std::endl;
  }
  delete []feat_caffe;
  delete []feat_sdk;
}
#endif

float Detool::startRecog(){
    //TEST(FaceRecognizerTest, CropFace);
    //TEST(FaceRecognizerTest, ExtractFeature);
    //TEST(FaceRecognizerTest, ExtractFeatureWithCrop);
    //MessageBox(GetForegroundWindow(),"4444","sim value",1);
    return 0.0;
}