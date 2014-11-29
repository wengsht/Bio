// =====================================================================================
// 
//       Filename:  accuracyTest.cpp
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
#include "ImageSet.h"
#include "ImgFileName.h"
#include <cstdlib>
#include "configure.h"

#include "tool.h"

using namespace std;

using namespace bio;

using namespace cv;

char trainDir[MAX_FILE_NAME_LEN] = "./img/att_train";
char testDir[MAX_FILE_NAME_LEN] = "./img/att_test";

double matchThres = DEFAULT_MATCH_THRESHOLD;

bool dealOpts(int argc, char **argv);

void combine(Mat &combineMat, Mat &inputImg, Mat &tempImg, int dist);
void copy2Comb(Mat & combineMat, Mat & inputImg, int startRow, int startCol);
void linkCombine(Mat & combineMat, int secStartRow, int secStartCol, Feature & input, Feature & temp);

int main(int argc, char **argv) {
    if(!dealOpts(argc, argv))
        return -1;

    SiftExtractor extractor;

    SiftMatcher trainMatcher;

    trainMatcher.loadDir(trainDir);
    trainMatcher.setup();
   
    ImageSet testImageSet;
    vector<Feature> testFeatures;

    vector<char* > testFileNames;
    ImgFileName::generateImgNames(testDir, testFileNames);
   
    trainMatcher.setMatchRatio(0.8);
    int correctCnt = 0;
    Feature * f1, * f2;
    double b1, b2;
    for(int fIdx=0; fIdx<testFileNames.size(); fIdx++){
        testFeatures.clear();
        testImageSet.loadTemplate(testFileNames[fIdx], testFeatures);
        
        unsigned long matchTag = trainMatcher.match(testFeatures);
        if(testFeatures[fIdx].getHashTag() == matchTag)
            correctCnt++;
    
        std::cout<<"Test accuracy: "<< correctCnt << " : " << fIdx+1 << " : "<<testFileNames.size()<<endl;
    }
    
    return 0;
} 

bool dealOpts(int argc, char **argv) {
    int c;
    while((c = getopt(argc, argv, "hi:t:b:")) != -1) {
        switch(c) {
            case 'h':
                printf("usage: \n \
                        -i input file name\n \
                        -t template template Dir \n \
                        -b match threshold(KD TREE) \n");

                return false;
                break;
        case 't':
                strcpy(trainDir, optarg);
                break;
        case 'i':
                strcpy(testDir, optarg);
                break;
        case 'b':
                matchThres = atoi(optarg);
                break;
            default:
                break;
        }
    }
    return true;
}

void combine(Mat &combineMat, Mat &inputImg, Mat &tempImg, int dist) {
    Mat tmp = Mat(max(inputImg.rows, tempImg.rows), inputImg.cols+tempImg.cols+dist, inputImg.type());

    copy2Comb(tmp, inputImg, 0, 0);
    copy2Comb(tmp, tempImg, 0, inputImg.cols + dist);

    tmp.convertTo(combineMat, CV_8UC1, 255);
    cvtColor(combineMat, combineMat, CV_GRAY2RGB);

}
void copy2Comb(Mat & combineMat, Mat & inputImg, int startRow, int startCol) {
    for(int i = 0;i < inputImg.rows; i++) {
        for(int j = 0;j < inputImg.cols;j++) {
            combineMat.at<double>(i + startRow, j + startCol) = inputImg.at<double>(i, j);
        }
    }
}

void linkCombine(Mat & combineMat, int secStartRow, int secStartCol, Feature & input, Feature & temp) {
    Point p1 = Point(input.originLoc.x, input.originLoc.y);
    Point p2 = Point(secStartCol + temp.originLoc.x, secStartRow + temp.originLoc.y);

    line(combineMat, p1, p2, Scalar(255,0,0), 1, 8);
}
