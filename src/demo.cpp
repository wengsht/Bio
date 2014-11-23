// =====================================================================================
// 
//       Filename:  democ.cpp
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

char imgFile[125] = "5.pgm";
char inputFile[125] = "jobs.jpeg";

bool dealOpts(int argc, char **argv);

double randDouble() {
    static int N = 10000;
    return 1.0 * (rand() % N) * 100 / N;

}

void random(vector<Feature> & fs) {
    for(int i = 0;i < fs.size(); i++) {
        for(int j = 0;j < Feature::descriptor_length; j++) {
            fs[i][j] = randDouble();
        }
    }
}

int main(int argc, char **argv) {
    if(!dealOpts(argc, argv))
        return -1;

    cout << "Final Proj" << endl;

    SiftExtractor extractor;

    //Mat src = imread("./beaver.png", 0);
//    Mat src = imread("./jobs.jpeg", 0);
    Mat src = imread(inputFile, 0);

//    src.convertTo(src, CV_64FC1, 1.0/255);

//    Mat dst = src.clone();

//    imshow("demo", src);
//    waitKey(1500);

//    GaussianBlur( src, dst, Size(3, 3), 0, 0);

//    imshow("blur", dst);
//    waitKey(150000);

    vector<Feature> null;
   // TODO
    extractor.sift(&src, null);

    Mat src2 = imread(inputFile, 0);
    vector<Feature> null2;
    extractor.sift(&src2, null2);

    SiftMatcher matcher;

    srand(-1);
    random(null);

    random(null2);

    matcher.setup( null );

    ofstream dotOut("1.dot");
    matcher.dumpDot(dotOut);
    dotOut.close();


    Feature & feature = matcher.match( null2[0] );

    double testVal = 10000000000;
    for(int i  =0;i < null.size() ;i++)
        testVal = fmin(testVal, null2[0] - null[i]);
    feature.dump(cout);
    null2[0].dump(cout);
    printf("kdTree: %lf\n", feature - null2[0]);
    printf("暴力: %lf\n", testVal);

    return 0;
} 

bool dealOpts(int argc, char **argv) {
    int c;
    while((c = getopt(argc, argv, "hi:")) != -1) {
        switch(c) {
            case 'h':
                printf("usage: \n \
                        -i input file name\n");

                return false;
                break;
        case 'i':
                strcpy(imgFile, optarg);
                break;
            default:
                break;
        }
    }
    return true;
}
