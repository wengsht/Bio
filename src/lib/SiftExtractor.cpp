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
//         chenss(SYSU-CMU),shushanc@andrew.cmu.edu 
//        Company:  
// 
// =====================================================================================

#include "SiftExtractor.h"
#include <vector>
#include <cstring>
#include <cstdlib>
#include <assert.h>
#include <stdio.h>

using namespace bio;
using namespace std;

SiftExtractor::SiftExtractor() {
#define SIFT_CONFIGURE(type, name, value) \
        configures.name = value;
#include "siftConfigure.def"
#undef SIFT_CONFIGURE
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

    octaves[0].addImg( img->clone() );
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
        Mat &prevImg = octaves[i-1][S];

    
        octaves[i].addImg( prevImg.clone() );

        pyrDown( prevImg, octaves[i][0], Size(prevImg.cols/2, prevImg.rows/2));

        octaves[i].generateBlurLayers(S + 3);

        /*   
        imshow("= =", prevImg);
        waitKey(1000);
        */
    }

    generateDOGPyramid(octaves);

//    imshow("ww", *img);
//    waitKey(150000);
}

void SiftExtractor::generateDOGPyramid(vector< Octave > & octaves) {
    int octSiz = octaves.size();

    for(int i = 0;i < octSiz; i++) 
        octaves[i].generateDogLayers();

//    imwrite("haha.jpeg", octaves[1][2]);
}


bool SiftExtractor::isExtrema(Octave & octave, int layer, int x, int y, EXTREMA_FLAG_TYPE *nxtMinFlags, EXTREMA_FLAG_TYPE* nxtMaxFlags, int rollIdx) {
    int colSiz = octave[0].cols;
    int rowSiz = octave[0].rows;
    int matrixSiz = colSiz * rowSiz;

    int nearOct, nearX, nearY;
    int nearFlagIdx;
    bool minEx = true, maxEx = true;


    for(int nl = -1; nl <= 1; nl ++) {
        nearOct = layer + nl;

        for(int nx = -1; nx <= 1; nx ++) {
            nearX = x + nx;
            for(int ny = -1; ny <= 1 && (minEx || maxEx); ny ++) {
                // don't compare with itself
                if(!nl && !nx && !ny) continue;

                nearY = y + ny;

                nearFlagIdx = (rollIdx ^ nl) * matrixSiz + nearX * rowSiz + nearY;
                if(nl != -1)
                    assert(nearFlagIdx < 2*matrixSiz);

                if(octave[layer].at<double>(y, x) >= octave[nearOct].at<double>(nearY, nearX)) {
                    minEx = false;

                    if(nl !=  -1) {
                        nxtMaxFlags[nearFlagIdx] = nearOct;
                    }
                }

                if(octave[layer].at<double>(y, x) <= octave[nearOct].at<double>(nearY, nearX)) {
                    maxEx = false;

                    if(nl != -1) {
                        nxtMinFlags[nearFlagIdx] = nearOct;
                    }
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

    int margin = configures.imgMargin;

    EXTREMA_FLAG_TYPE * maxFlags = new EXTREMA_FLAG_TYPE[2 * matrixSiz];
    EXTREMA_FLAG_TYPE * minFlags = new EXTREMA_FLAG_TYPE[2 * matrixSiz];

    memset(maxFlags, -1, 2 * matrixSiz);
    memset(minFlags, -1, 2 * matrixSiz);

    // Detect ith image's extremas
    for(int i = 1, rollIdx = 0; i < laySiz - 1; i ++, rollIdx ^= 1) {
        int nxtRollIdx = rollIdx ^ 1;

        for(int x = margin; x < colSiz - margin; x ++) {
            for(int y = margin; y < rowSiz - margin; y ++) {
                int flagIdx = rollIdx * matrixSiz + x * rowSiz + y;

                if(i == maxFlags[flagIdx] && i == minFlags[flagIdx])
                    continue;

                if(isExtrema(octave, i, x, y, minFlags, maxFlags, rollIdx)) {
                    if(! shouldEliminate(octave, i, x, y)) {

                        addFeature( octave, i, x, y, outFeatures );
                    }
                }
            }
        }
    }

    printf("%d\n", outFeatures.size());

    delete []maxFlags;
    delete []minFlags;
}

void SiftExtractor::addFeature(Octave &octave, int layer, int x, int y, vector<Feature> &outFeatures) {
    Feature newFea;
//    newFea.img = &(octave[layer]);
//    newFea.location = bio::point(x, y);

    outFeatures.push_back(newFea);
}

bool SiftExtractor::edgePointEliminate(Mat &img, int x, int y) {
    static double threshold = (configures.r + 1) * (configures.r + 1) / configures.r;

    if(! configures.doEliminateEdgeResponse) 
        return false;

    double Dxx, Dyy, Dxy, Tr, Det;

    //Hessian
    Dxx = img.at<double>(y, x + 1) - img.at<double>(y, x) * 2 + img.at<double>(y, x-1);
    Dyy = img.at<double>(y+1, x) - img.at<double>(y, x) * 2 + img.at<double>(y-1, x);
    Dxy = img.at<double>(y+1, x+1) - img.at<double>(y+1, x-1) - img.at<double>(y-1, x+1) + img.at<double>(y-1, x-1);
    Dxy /= 4.0;

    Tr = Dxx + Dyy;
    Det = Dxx * Dyy - Dxy * Dxy;

    if(0.0 == Det) return true;

    double val = Tr * Tr / Det;

    return val > threshold;
}

bool SiftExtractor::poorContrast(Octave & octave, int &layer, int &x, int &y) {

}

bool SiftExtractor::shouldEliminate(Octave &octave, int &layer, int &x, int &y) {
    if(poorContrast(octave, layer, x, y)) 
        return true;

    if(edgePointEliminate(octave[layer], x, y)) 
        return true;
    return false;
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

void SiftExtractor::calcFeatureOri(vector< Feature >& features, vector< Octave >& octaves){
    vector< double > hist;
    for( int feaIdx=0; feaIdx < features.size(); feaIdx++){
        calcOriHist(features[feaIdx], hist);
        
        for(int smoIdx=0; smoIdx < smoothTimes; smoIdx++)
            smoothOriHist(hist);

   } 
}

void SiftExtractor::calcOriHist(Feature& feature, vector< double >& hist){
    double mag,ori,weight;
   
    Mat* img = feature.meta->img;
    int p_x = feature.meta->location.x;
    int p_y = feature.meta->location.y;
    int radius = feature.scale * oriWinRadius;
    double sigma = feature.scale * oriSigmaTimes;

    hist.resize(histBins);
    

}
