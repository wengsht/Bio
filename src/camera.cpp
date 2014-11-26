// =====================================================================================
// 
//       Filename:  camera.cpp
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
#include <fstream>
#include <cstdio>
#include <unistd.h>
#include "feature.h"
#include "SiftExtractor.h"

#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/highgui/highgui.hpp"

#include "SiftMatcher.h"
#include <cstdlib>

using namespace std;

using namespace bio;

using namespace cv;

bool dealOpts(int argc, char **argv);

int main(int argc, char **argv) {
    if(!dealOpts(argc, argv))
        return -1;

    VideoCapture cap(0);
    cap.set( CV_CAP_PROP_FRAME_WIDTH,320);
    cap.set( CV_CAP_PROP_FRAME_HEIGHT,240 );

    SiftExtractor extractor;
    vector<Feature> null;

    while (1)
    {
        Mat frame;
        cap>>frame;

        extractor.sift(& frame, null);

        imshow( "Capture",frame);
        if( waitKey(30)>=0 ) break;
    }

    return 0;
} 

bool dealOpts(int argc, char **argv) {
    int c;
    while((c = getopt(argc, argv, "h")) != -1) {
        switch(c) {
            case 'h':
                printf("usage: \n \
                        \n");

                return false;
                break;
            default:
                break;
        }
    }
    return true;
}
