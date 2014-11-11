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
//         Author:  wengsht (SYSU-CMU), wengsht.sysu@gmail.com
//         chenss(SYSU-CMU), 
//        Company:  
// 
// =====================================================================================

#include "SiftExtractor.h"
#include <vector>
#include <cstring>
#include <cstdlib>

using namespace bio;
using namespace std;

SiftExtractor::SiftExtractor() {
    configures.basicSigma = DEFAULT_BASIC_SIGMA;

    configures.innerLayerPerOct = DEFAULT_INNER_LAYER_OCT;

    configures.extremaThres = DEFAULT_EXTREMA_THRESHOLD;
}

SiftExtractor::~SiftExtractor() {}

void SiftExtractor::generatePyramid(Mat * img, vector< Octave > &octaves) {
    octaves.clear();

    Mat tmp;

    int rowSiz = img->rows, colSiz = img->cols;

    int octaveSiz = log( std::min(rowSiz, colSiz) ) / log(2.0) - 1;
    if(octaveSiz <= 0) octaveSiz = 1;

    int i;
    int S = configures.innerLayerPerOct;
    // Sub Sampling
    octaves.push_back( Octave() );
    octaves[0].addImg( (*img) );
    octaves[0].sigma = configures.basicSigma;
    octaves[0].generateBlurLayers(S + 3);

    double k = pow(2.0, 1.0 / S);

    for(i = 1;i < octaveSiz; i++) {
        octaves.push_back( Octave() );

        if(i == 1) 
            octaves[i].sigma = octaves[i-1].sigma * sqrt(k * k - 1);
        else 
            octaves[i].sigma = octaves[i-1].sigma * k;

        // 当前oct的第一层是由上一个oct的倒数第三张图片复制来的
        Mat & prevImg = octaves[i-1][S];

        octaves[i].addImg(prevImg);

        pyrDown( prevImg, octaves[i][0], Size(prevImg.cols/2, prevImg.rows/2));

//        printf("%d %d\n", octaves[i][0].cols, octaves[i][0].rows);

        octaves[i].generateBlurLayers(S + 3);

        /*   
        printf("%d %d\n", prevImg.cols, prevImg.rows);
        imshow("= =", prevImg);
        waitKey(1000);
        */
    }

    generateDOGPyramid(octaves);
}

void SiftExtractor::generateDOGPyramid(vector< Octave > & octaves) {
    int octSiz = octaves.size();

    for(int i = 0;i < octSiz; i++) 
        octaves[i].generateDogLayers();
}


inline 
bool SiftExtractor::isExtrema(Octave & octave, int layer, int x, int y, bool *nxtMinFlags, bool* nxtMaxFlags) {
    int colSiz = octave[0].cols;

    int nearOct, nearX, nearY;
    int nearFlagIdx;
    bool minEx = true, maxEx = true;
    for(int nl = -1; nl <= 1; nl ++) {
        nearOct = layer + nl;
        for(int nx = -1; nx <= 1; nx ++) {
            nearX = x + nx;
            for(int ny = -1; ny <= 1 && (minEx || maxEx); ny ++) {
                nearY = y + ny;

                nearFlagIdx = nearX * colSiz + nearY;

                if(octave[layer].at<double>(y, x) > octave[nearOct].at<double>(nearY, nearX)) {
                    minEx = false;
                    nxtMaxFlags[nearFlagIdx] = false;
                }

                if(octave[layer].at<double>(y, x) < octave[nearOct].at<double>(nearY, nearX)) {
                    maxEx = false;
                    nxtMinFlags[nearFlagIdx] = false;
                }
            }
        }
    }

    return (minEx || maxEx);
}

void SiftExtractor::extremaDetect(Octave & octave, vector<Feature> & outFeatures) {
    if(octave.size() <= 0) return ;
    int laySiz = octave.size();

    int colSiz = octave[0].cols, rowSiz = octave[0].rows;
    int matrixSiz = colSiz * rowSiz;

    bool * maxFlags = new bool[2 * matrixSiz];
    bool * minFlags = new bool[2 * matrixSiz];

    memset(maxFlags, true, 2 * matrixSiz);
    memset(minFlags, true, 2 * matrixSiz);

    // Detect ith image's extremas
    for(int i = 1, rollIdx = 1; i < laySiz - 1; i ++, rollIdx ^= 1) {
        int nxtRollIdx = rollIdx ^ 1;

        memset(minFlags + nxtRollIdx * matrixSiz, true, matrixSiz);
        memset(maxFlags + nxtRollIdx * matrixSiz, true, matrixSiz);

        for(int x = 1; x < rowSiz - 1; x ++) {
            for(int y = 1; y < colSiz - 1; y ++) {
                int flagIdx = rollIdx * matrixSiz + x * colSiz + y;

                if(!maxFlags[flagIdx] && !minFlags[flagIdx])
                    continue;

                if(isExtrema(octave, i, x, y, minFlags + nxtRollIdx * matrixSiz, maxFlags + nxtRollIdx * matrixSiz)) {
//                    printf("%d %d %d\n", )
                }
            }
        }
    }

    delete []maxFlags;
    delete []minFlags;
}

void SiftExtractor::extremaDetect(vector< Octave > & octaves, vector<Feature> & outFeatures) {
    int octSiz = octaves.size();
    for(int i = 0; i < octSiz; i++) {
        extremaDetect(octaves[i], outFeatures);
    }
}

void SiftExtractor::sift(Mat *img, vector<Feature> & outFeatures) {
    outFeatures.clear();

    // Building Pyramid
    vector< Octave > octaves;

    generatePyramid( img, octaves );

    extremaDetect(octaves, outFeatures);
}
