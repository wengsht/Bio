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

using namespace std;

using namespace bio;

using namespace cv;

char templateDir[MAX_FILE_NAME_LEN] = "./img";
char inputFile[MAX_FILE_NAME_LEN] = "jobs.jpeg";

double matchThres = DEFAULT_MATCH_THRESHOLD;

bool dealOpts(int argc, char **argv);

void combine(Mat &combineMat, Mat &inputImg, Mat &tempImg, int dist);
void copy2Comb(Mat & combineMat, Mat & inputImg, int startRow, int startCol);
void linkCombine(Mat & combineMat, int secStartRow, int secStartCol, Feature & input, Feature & temp);

int main(int argc, char **argv) {
    if(!dealOpts(argc, argv))
        return -1;

    SiftExtractor extractor;

    Mat inputImg = imread(inputFile, 0);

    vector<Feature> inputFeats;

    extractor.sift(&inputImg, inputFeats);

    SiftMatcher matcher;

    matcher.loadDir(templateDir);
    matcher.setup();

    int idx;
    map<string, int> cnt;
    for(idx = 0; idx < inputFeats.size(); idx ++) {
        pair<Feature *, Feature *> matchs = (matcher.match( inputFeats[idx] ));

        Feature * f1 = matchs.first, * f2 = matchs.second;

        double b1 = *f1 - inputFeats[idx];
        double b2 = *f2 - inputFeats[idx];

        printf("%lf %lf %lf\n", b1, b2, b1/b2);

        if(b1 / b2 < 0.6) {
            if(cnt.count( string((char *)(f1->getContainer()))) == false) 
                cnt[string((char *)(f1->getContainer()))] =0;
            cnt[string((char *)(f1->getContainer()))] ++;
        }

        if(f1) {
            Log("best From [%lu][%s]", f1->getHashTag(), (char *)(f1->getContainer()));
        }
        if(f2) {
            Log("second best From [%lu][%s]", f2->getHashTag(), (char *)(f2->getContainer()));
        }
        puts("++++++++++++");
    }
    map<string, int>::iterator Itr;
    for(Itr = cnt.begin(); Itr != cnt.end(); Itr++) {
        std::cout << Itr->first << " " << Itr->second << std::endl;
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
                strcpy(templateDir, optarg);
                break;
        case 'i':
                strcpy(inputFile, optarg);
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
