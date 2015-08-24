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
#include <string>
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

char trainDir[MAX_FILE_NAME_LEN] = "./img/att/train";
char testDir[MAX_FILE_NAME_LEN] = "./img/att/test";

double matchRatio = DEFAULT_MATCH_RATIO;
int kdBackTrackRatio = KD_BACKTRACK_RATIO;

bool dealOpts(int argc, char **argv);

int main(int argc, char **argv) {
    if(!dealOpts(argc, argv))
        return -1;

    SiftExtractor extractor;

    SiftMatcher trainMatcher;

    trainMatcher.loadDir(trainDir);
    trainMatcher.setup();
   
    vector<Feature> testFeatures;

    vector<char* > testFileNames;
    ImgFileName::generateImgNames(testDir, testFileNames);
   
    trainMatcher.setMatchRatio(matchRatio);
    trainMatcher.setKdBackTrackRatio(kdBackTrackRatio);
    int correctCnt = 0;
    Feature * f1, * f2;
    double b1, b2;
    for(int fIdx=0; fIdx<testFileNames.size(); fIdx++){
        ImageSet testImageSet;
        testFeatures.clear();
        testImageSet.loadTemplate(testFileNames[fIdx]);
        
        unsigned long matchTag = trainMatcher.match(testImageSet.getFeatures());
        
        if(testImageSet[0].getHashTag() == matchTag)
            correctCnt++;
    
        std::cout<<"Test accuracy: "<< correctCnt << " : " << fIdx+1 << " : "<<testFileNames.size()<<endl;
    }
    
    return 0;
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
                strcpy(trainDir, optarg);
                break;
        case 'i':
                strcpy(testDir, optarg);
                break;
        case 'b':
                sscanf(optarg, "%lf", & matchRatio);
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

