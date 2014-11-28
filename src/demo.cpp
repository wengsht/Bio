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

    SiftMatcher matcher;
    SiftExtractor extractor;

    Mat input = imread(inputFile, 0);
    vector<Feature> inputFeats;
    extractor.sift(&input , inputFeats);

    matcher.loadFile( imgFile );
    matcher.setup();

    /*  
    ofstream dotOut("1.dot");
    matcher.dumpDot(dotOut);
    dotOut.close();
    */


    pair<Feature *, Feature *> res = (matcher.match( inputFeats[0] ));
    Feature * feature = (res.first);
    Feature * secFeature = (res.second);

    vector<Feature> &templateFeats = matcher.getFeatures();

    double testVal = 10000000000;
    for(int i  =0;i < templateFeats.size() ;i++) {
        testVal = fmin(testVal, inputFeats[0] - templateFeats[i]);
    }

    feature->dump(cout);
    inputFeats[0].dump(cout);
    printf(RED "best one kdTree: %lf from [%d][%s]\n", *feature - inputFeats[0], feature->getHashTag(), (char *)(feature->getContainer()));

    if(secFeature)
        printf(RED "second best one kdTree: %lf from [%d][%s]\n", *secFeature - inputFeats[0], secFeature->getHashTag(), (char *)(secFeature->getContainer()));
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
