// =====================================================================================
// 
//       Filename:  SiftExtractor.cpp
// 
//    Description:  
// 
//        Version:  0.01
//        Created:  2014/11/10 16时49分55秒
//       Revision:  none
//       Compiler:  clang 3.5
// 
//         Author:  
//         chenss(SYSU-CMU), 
//         wengsht (SYSU-CMU), wengsht.sysu@gmail.com
//        Company:  
// 
// =====================================================================================

#include <iostream>
#include <cstdio>
#include "feature.h"
#include "SiftExtractor.h"

#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/highgui/highgui.hpp"
using namespace std;

using namespace bio;

using namespace cv;
int main() {
    cout << "Final Proj" << endl;

    SiftExtractor extractor;

//    Mat src = imread("./beaver.png", 0);
    Mat src = imread("./jobs.jpeg", 0);

    src.convertTo(src, CV_64FC1, 1.0/255);

//    Mat dst = src.clone();

//    imshow("demo", src);
//    waitKey(1500);

//    GaussianBlur( src, dst, Size(3, 3), 0, 0);

//    imshow("blur", dst);
//    waitKey(150000);

    vector<Feature> null;
   // TODO
    extractor.sift(&src, null);

    return 0;
} 
