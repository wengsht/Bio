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

    Mat src = imread("./beaver.png", 1);
    imshow("demo", src);

    vector<Feature> null;
     // TODO
    extractor.sift(&src, null);

    return 0;
} 
