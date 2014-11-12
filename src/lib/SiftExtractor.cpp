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

void SiftExtractor::extremaDetect(Octave & octave, int octIdx, vector<Feature> & outFeatures) {
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

                        addFeature( octave, octIdx, i, x, y, outFeatures );
                    }
                }
            }
        }
    }

    printf("%d\n", outFeatures.size());

    delete []maxFlags;
    delete []minFlags;
}

void SiftExtractor::addFeature(Octave &octave, int octIdx, int layer, int x, int y, vector<Feature> &outFeatures) {
    Feature newFea;
    Meta meta;

    meta.img = &(octave[layer]);
    meta.location = bio::point<int>(x, y);

    bufferMetas.push_back(meta);

    newFea.meta = & (bufferMetas[bufferMetas.size() - 1]);

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
        extremaDetect(octaves[i], i, outFeatures);
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
        //calculate the orientation histogram
        calcOriHist(features[feaIdx], hist);
       
        //smooth the orientation histogram
        for(int smoIdx=0; smoIdx < configures.smoothTimes; smoIdx++)
            smoothOriHist(hist);

   } 
}


void SiftExtractor::calcOriHist(Feature& feature, vector< double >& hist){
    double mag,ori,weight;
    int bin;
   
    Mat* img = feature.meta->img;
    
    //feature point position
    int p_x = feature.meta->location.x;
    int p_y = feature.meta->location.y;
    
    //sigma is to multiply a factor and get the radius
    double sigma = feature.scale * configures.oriSigmaTimes;
    int radius = cvRound(sigma * configures.oriWinRadius);
    
    //prepare for calculating the guassian weight,"gaussian denominator"
    double gauss_denom = -2.0*sigma*sigma;

    //size of histogram is according to the size of bins
    hist.resize(configures.histBins);

    //calculate the orientation within the radius
    for(int i=-1*radius; i<=radius; i++){
        for(int j=-1*radius; j<=radius; j++){
            if(calcMagOri(img,p_x+i,p_y+j,mag,ori)){
                //calculate the weight by gaussian distribution
                weight = exp((i*i+j*j)*gauss_denom);
                
                //check which bin the orientation related to
                bin = cvRound(configures.histBins*(0.5-ori/(2.0*CV_PI)));
                bin = bin < configures.histBins?bin:0;
                
                //value is recalculated by multiplying weight
                hist[bin] += mag * weight;
            }
        }
    }

}


bool SiftExtractor::calcMagOri(Mat* img, int x, int y, double& mag, double& ori){
   if(x>0 && x<img->cols-1 && y>0 && y<img->rows-1){
        double dx = getMatValue(img,x,y+1) - getMatValue(img, x, y-1);
        double dy = getMatValue(img,x-1,y) - getMatValue(img, x+1, y);
        mag = sqrt(dx*dx + dy*dy);
        ori = atan2(dy,dx);
        return true;
   }
   else
       return false;
}


double SiftExtractor::getMatValue(Mat* img, int x, int y){
    return ((double*) img->data)[x*img->cols+y]; 
}


void SiftExtractor::smoothOriHist(vector< double >& hist){
    double pre = hist[configures.histBins-1];

    for(int i=0; i<configures.histBins; i++){
        double temp = hist[i];
        double post = (i+1 == configures.histBins)?hist[0]:hist[i+1];
        hist[i] = 0.25*pre + 0.5*hist[i] + 0.25*post;
        pre = temp;
    }
    
}

