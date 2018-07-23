#pragma once
#include <cstdint>
#include <fstream>
#include <iostream>
#include <string>
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include "face_detection.h"
#include "face_alignment.h"
#include "face_identification.h"
//#include "math_functions.h"
//#include <vector>
//#include <algorithm>
using namespace cv;
class Detool
{
public:
    Detool(void);
    ~Detool(void);
    cv::Mat startDetection(cv::Mat img);
    void startalignment(cv::Mat img);
    float startIdentification(cv::Mat img);
    float startRecog();
};

