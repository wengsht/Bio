// =====================================================================================
// 
//       Filename:  match2db.cpp
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
#include "configure.h"

#include "tool.h"


#include "ImgFileName.h"
using namespace std;

using namespace bio;

using namespace cv;

char templateDir[MAX_FILE_NAME_LEN] = "./img/att/train/";
char inputFile[MAX_FILE_NAME_LEN] = "./img/att/test/s9_8.pgm";

double matchRatio = DEFAULT_MATCH_RATIO;

int kdBackTrackRatio = KD_BACKTRACK_RATIO;

bool dealOpts(int argc, char **argv);

void combine(Mat &combineMat, Mat &inputImg, Mat &tempImg, int dist);
void copy2Comb(Mat & combineMat, Mat & inputImg, int startRow, int startCol);
void linkCombine(Mat & combineMat, int secStartRow, int secStartCol, Feature & input, Feature & temp);

void drawName(Mat & showImg, std::string name);

int main(int argc, char **argv) {
    if(!dealOpts(argc, argv))
        return -1;

    SiftExtractor extractor;

    Mat inputImg = imread(inputFile, 0);

    Mat showImg = inputImg;

    vector<Feature> inputFeats;

    extractor.sift(&inputImg, inputFeats);

    SiftMatcher matcher;

    matcher.loadDir(templateDir);
    matcher.setup();
    matcher.setMatchRatio(matchRatio);
    matcher.setKdBackTrackRatio(kdBackTrackRatio);

    unsigned long matchTag =  matcher.match(inputFeats);

    std::string resName = ImgFileName::descriptor(matchTag);

    std::cout << GREEN << "Input file name: " << inputFile << std::endl;

    std::cout << RED << "Match Result: " << resName << NONE << std::endl;

    drawName(showImg, resName);

    imshow("Match Result", showImg);

    waitKey(1000000);
    
    return 0;
} 

void drawName(Mat & showImg, std::string name) {
    putText(showImg, name, Point(showImg.rows / 3, showImg.cols / 3), FONT_HERSHEY_SIMPLEX, 1, Scalar(0,0,255));
}

bool dealOpts(int argc, char **argv) {
    int c;
    while((c = getopt(argc, argv, "hi:t:b:k:")) != -1) {
        switch(c) {
            case 'h':
                printf("usage: \n \
                        -i input file name\n \
                        -t template template Dir \n \
                        -k kd bracktrack ratio, lower value means higher accuracy and lower speed\n\
                        -b match ratio(0.8 is reasonable) \n");

                return false;
                break;
        case 't':
                strcpy(templateDir, optarg);
                break;
        case 'i':
                strcpy(inputFile, optarg);
                break;
        case 'b':
                sscanf(optarg, "%lf", &matchRatio);
                break;
        case 'k':
                kdBackTrackRatio = atoi(optarg);
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
