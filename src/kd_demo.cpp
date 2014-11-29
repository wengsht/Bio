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

#include "tool.h"

using namespace std;

using namespace bio;

using namespace cv;

char inputFile[125] = "./img/demo/jobs.png";
char templateFile[125] = "./img/demo/job_inbook_50.png";

double matchThres = DEFAULT_MATCH_THRESHOLD;

bool dealOpts(int argc, char **argv);

int main(int argc, char **argv) {
    if(!dealOpts(argc, argv))
        return -1;

    SiftExtractor extractor;
    SiftMatcher matcher;
    vector<Feature> inputFeats;

    Mat input = imread(inputFile, 0);
    extractor.sift(&input , inputFeats);

    matcher.loadFile( templateFile );
    matcher.setMatchThres(matchThres);
    matcher.setup();

    pair<Feature *, Feature *> res = (matcher.match( inputFeats[0] ));

    Feature * feature = (res.first);
    Feature * secFeature = (res.second);

    vector<Feature> &templateFeats = matcher.getFeatures();

    double testVal = INF;

    for(int i  =0;i < templateFeats.size() ;i++) {
        testVal = fmin(testVal, inputFeats[0] - templateFeats[i]);
    }

    if(feature) {
        printf(RED "best one kdTree: %lf from [%lu][%s]\n", *feature - inputFeats[0], feature->getHashTag(), (char *)(feature->getContainer()));
    }

    printf("Naive n^2 search: %lf\n", testVal);

    return 0;
} 

bool dealOpts(int argc, char **argv) {
    int c;
    while((c = getopt(argc, argv, "hi:t:b:")) != -1) {
        switch(c) {
            case 'h':
                printf("usage: \n \
                        -t template file name\n \
                        -b match threshold\n \
                        -i input file name\n");

                return false;
                break;
        case 'i':
                strcpy(inputFile, optarg);
                break;
        case 'b':
                matchThres = atoi(optarg);
                break;
        case 't':
                strcpy(templateFile, optarg);
                break;
            default:
                break;
        }
    }
    return true;
}
