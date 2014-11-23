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

    int rowSiz = img->rows, colSiz = img->cols;

    int octaveSiz = log( std::min(rowSiz, colSiz) ) / log(2.0) - 1;
    if(octaveSiz <= 0) octaveSiz = 1;

    int i;
    int S = configures.innerLayerPerOct;
    // Sub Sampling
    octaves.push_back( Octave() );


    double k = pow(2.0, 1.0 / S);
    
    double sigmas[S + 3];
    sigmas[0] = configures.basicSigma;
    sigmas[1] = sigmas[0] * sqrt(k * k - 1);
    for(i = 2;i < S+3;i++) 
        sigmas[i] = sigmas[i-1] * k;

    Mat baseImg;
    double baseSigma;

    if(configures.baseOctIdx == 1) {
        pyrUp( *img, baseImg, Size((img->cols)*2, (img->rows)*2));

        baseSigma = sqrt(pow(sigmas[0], 2.0) - pow(configures.initSigma * 2.0, 2.0));
    }

    else {
        baseImg = img->clone();

        baseSigma = sqrt(pow(sigmas[0], 2.0) - pow(configures.initSigma, 2.0));
    }

    GaussianBlur(baseImg, baseImg, Size(0, 0), baseSigma, baseSigma);

    octaves[0].addImg( baseImg );
    octaves[0].generateBlurLayers(S + 3, sigmas);

    for(i = 1;i < octaveSiz; i++) {
        octaves.push_back( Octave() );

        // Sth image of prev octave 
        Mat &prevImg = octaves[i-1][S];
    
        octaves[i].addImg( prevImg.clone() );

        pyrDown( prevImg, octaves[i][0], Size((prevImg.cols+1)/2, (prevImg.rows+1)/2));

        octaves[i].generateBlurLayers(S + 3, sigmas);

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
                    int accurateLay = i, accurateX = x, accurateY = y;
                    double _X[3];
                    if(! shouldEliminate(octave, accurateLay, accurateX, accurateY, _X)) {
                        addFeature( octave, octIdx, accurateLay, accurateX, accurateY, _X, outFeatures);
                    }
                }
            }
        }
    }

    printf("%d\n", outFeatures.size());

    delete []maxFlags;
    delete []minFlags;
}

bio::point<double> SiftExtractor::pixelOriMap(int octIdx, int layer, int x, int y, double *_X) {
    bio::point<double> oriPixel;

    oriPixel.x = (x + _X[0]) * pow(2.0, octIdx - configures.baseOctIdx);
    oriPixel.y = (y + _X[1]) * pow(2.0, octIdx - configures.baseOctIdx);

    return oriPixel;
}

void SiftExtractor::addFeature(Octave &octave, int octIdx, int layer, int x, int y, double *_X, vector<Feature> &outFeatures) {
    Feature newFea;
    Meta *newMeta = new Meta;
    Meta &meta = *newMeta;

    meta.img = &(octave[layer]);
    meta.location = bio::point<int>(x, y);
    meta.octIdx = octIdx;
    meta.layerIdx = layer;

    meta.scale = configures.basicSigma * pow(2.0, (1.0 * layer + _X[2]) / configures.innerLayerPerOct);
    meta.globalScale = meta.scale * pow(2.0, octIdx);

//    meta.subLayer = _X[2];

    bufferMetas.push_back(newMeta);

    newFea.meta = newMeta;
    newFea.originLoc = pixelOriMap(octIdx, layer, x, y, _X);

    outFeatures.push_back(newFea);
}

bool SiftExtractor::edgePointEliminate(Mat &img, int x, int y) {
    static double threshold = (configures.r + 1.0) * (configures.r + 1) / configures.r;

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

void SiftExtractor::calcJacobian(Octave &octave, int layer, int x, int y, double Jacobian[3]) {
    double & Dx = Jacobian[0], \
           & Dy = Jacobian[1], \
           & Dz = Jacobian[2];

    Dx = octave[layer].at<double> (y, x+1) - octave[layer].at<double> (y, x-1); 
    Dy = octave[layer].at<double> (y+1, x) - octave[layer].at<double> (y-1, x); 
    Dz = octave[layer+1].at<double> (y, x) - octave[layer-1].at<double> (y, x); 

    Dx /= 2.0; Dy /= 2.0; Dz /= 2.0;
}

void SiftExtractor::calcHessian(Octave &octave, int layer, int x, int y, double Hessian[3][3]) {
    double &Dxx = Hessian[0][0], \
           &Dyy = Hessian[1][1], \
           &Dzz = Hessian[2][2], \
           &Dxy = Hessian[0][1], \
           &Dxz = Hessian[0][2], \
           &Dyx = Hessian[1][0], \
           &Dyz = Hessian[1][2], \
           &Dzx = Hessian[2][0], \
           &Dzy = Hessian[2][1];

    Dxx = octave[layer].at<double> (y, x+1) + \
          octave[layer].at<double> (y, x-1) - \
          2 * octave[layer].at<double> (y, x);
    Dyy = octave[layer].at<double> (y+1, x) + \
          octave[layer].at<double> (y-1, x) - \
          2 * octave[layer].at<double> (y, x);
    Dzz = octave[layer+1].at<double> (y, x) + \
          octave[layer-1].at<double> (y, x) - \
          2 * octave[layer].at<double> (y, x);

    Dxy = ( octave[layer].at<double>(y+1, x+1) - \
            octave[layer].at<double>(y+1, x-1) - \
            octave[layer].at<double>(y-1, x+1) + \
            octave[layer].at<double>(y-1, x-1)) / 4.0;

    Dxz = ( octave[layer+1].at<double>(y, x+1) - \
            octave[layer+1].at<double>(y, x-1) - \
            octave[layer-1].at<double>(y, x+1) + \
            octave[layer-1].at<double>(y, x-1)) / 4.0;

    Dyz = ( octave[layer+1].at<double>(y+1, x) - \
            octave[layer-1].at<double>(y+1, x) - \
            octave[layer+1].at<double>(y-1, x) + \
            octave[layer-1].at<double>(y-1, x)) / 4.0;

    Dyx = Dxy; Dzx = Dxz; Dzy = Dyz;
}

void SiftExtractor::calcOffset(double Hessian[3][3], double Jacobian[3], double _X[3]) {
    Mat HeMat = Mat(3, 3, CV_64FC1, Hessian);
    Mat JaMat = Mat(3, 1, CV_64FC1, Jacobian);

    Mat _XMat = - (HeMat.inv() * JaMat);

    _X[0] = _XMat.at<double> (0, 0);
    _X[1] = _XMat.at<double> (1, 0);
    _X[2] = _XMat.at<double> (2, 0);
}

inline 
bool SiftExtractor::innerPixel(Octave &octave, int layer, int x, int y) {
    int margin = configures.imgMargin;

    if(layer < 1 || layer >= octave.size() - 1) 
        return false;
    if(x < margin || x >= octave[layer].cols - margin)
        return false;
    if(y < margin || y >= octave[layer].rows - margin)
        return false;

    return true;
}

bool SiftExtractor::poorContrast(Octave & octave, int &layer, int &x, int &y, double *_X) {
    int laySiz = octave.size();

    double &offX = _X[0], \
           &offY = _X[1], 
           &offZ = _X[2];

    double Hessian[3][3], Jacobian[3];

    // Default: 0.5
    double thres = configures.offsetThres;

    int step = configures.extreOffsetStep;
    while(step --) {
        calcJacobian(octave, layer, x, y, Jacobian);
        calcHessian(octave, layer, x, y, Hessian);

        // _X = -Hessian^(-1) * Jacobian
        calcOffset(Hessian, Jacobian, _X);

        if(fabs(offX) <= thres && fabs(offY) <= thres && fabs(offZ) <= thres)
            break;

        x += cvRound(offX);
        y += cvRound(offY);
        layer += cvRound(offZ);

        if(! innerPixel(octave, layer, x, y)) 
            return true;
    }

    if(fabs(offX) > thres || fabs(offY) > thres || fabs(offZ) > thres)
        return true;

    int X[3] = {x, y, layer};

    double DxValue = calcDxValue(octave, X, _X);

    if(fabs(DxValue) < configures.DxValueThres / configures.innerLayerPerOct) 
        return true;

    return false;
}

double SiftExtractor::calcDxValue(Octave &octave, int X[3], double _X[3]) {
    double Jacobian[3];

    calcJacobian(octave, X[2], X[0], X[1], Jacobian);

    Mat JaMat = Mat(1, 3, CV_64FC1, Jacobian);
    Mat _XMat = Mat(3, 1, CV_64FC1, _X);

    Mat tmp = JaMat * _XMat;
    return octave[X[2]].at<double>(X[1], X[0]) + 0.5 * tmp.at<double>(0, 0);
}

bool SiftExtractor::shouldEliminate(Octave &octave, int &layer, int &x, int &y, double *_X) {
    if(poorContrast(octave, layer, x, y, _X))  {
        return true;
    }

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

Mat &SiftExtractor::siftFormatImg(Mat *img) {
    Mat temp;
    Mat result = Mat(img->rows, img->cols, CV_64FC1);
    if(img->channels() == 1)
        temp = *img;
    else {
        temp = Mat(img->rows, img->cols, CV_8UC1);
        cvtColor( *img, temp, CV_BGR2GRAY );
    }

    temp.convertTo(*img, CV_64FC1, 1.0/255);
    return *img;
}

void SiftExtractor::sift(Mat *img, vector<Feature> & outFeatures) {
    outFeatures.clear();

    *img = siftFormatImg(img);

    // Building Pyramid
    vector< Octave > octaves;

    generatePyramid( img, octaves );

    extremaDetect(octaves, outFeatures);
    
    calcFeatureOri(outFeatures, octaves);

//    printf("%d\n", outFeatures[0].meta->location.x);

    show(img, outFeatures);
    // endding
    clearBuffers();
}


void SiftExtractor::calcFeatureOri(vector< Feature >& features, vector< Octave >& octaves){
    vector< double > hist;
    int featPointSize = features.size();

    for( int feaIdx=0; feaIdx < featPointSize; feaIdx++){
        //calculate the orientation histogram
        calcOriHist(features[feaIdx], hist);
       
        //smooth the orientation histogram
        for(int smoIdx=0; smoIdx < configures.smoothTimes; smoIdx++)
            smoothOriHist(hist);
        
        addOriFeatures(features, features[feaIdx],hist);
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
    double sigma = feature.meta->scale * configures.oriSigmaTimes;
    int radius = cvRound(sigma * configures.oriWinRadius);

    //prepare for calculating the guassian weight,"gaussian denominator"
    double gauss_denom = -1.0/(2.0*sigma*sigma);

    //size of histogram is according to the size of bins
    hist.resize(configures.histBins, 0);

    //calculate the orientation within the radius
    for(int i=-1*radius; i<=radius; i++){
        for(int j=-1*radius; j<=radius; j++){
            if(calcMagOri(img,p_x+i,p_y+j,mag,ori)){
//                printf("%lf\n", ori);
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
   if(x>0 && (x < (img->cols)-1) && y>0 && (y < (img->rows)-1)){
        double dx = getMatValue(img,x,y+1) - getMatValue(img, x, y-1);
        double dy = getMatValue(img,x-1,y) - getMatValue(img, x+1, y);
//        printf("%lf %lf\n", dx, dy);
        mag = sqrt(dx*dx + dy*dy);
        ori = atan2(dy,dx);
        return true;
   }
   else
       return false;
}


double SiftExtractor::getMatValue(Mat* img, int x, int y){
    return img->at<double>(y, x);
//    return ((double*) img->data)[x*(img->cols)+y]; 
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


void SiftExtractor::addOriFeatures(vector<Feature>& features, Feature& feat, vector< double >& hist){
    double oriDenThres;
    int pre,post,count;
    
    count = 0;

    //get the dominant orientation
    double maxOriDen = hist[0];
    for(int i=1; i < configures.histBins; i++){
        if(hist[i] > maxOriDen)
            maxOriDen = hist[i];
        
    }

    oriDenThres = maxOriDen * configures.oriDenThresRatio;
//    printf("%lf %lf\n",maxOriDen, oriDenThres);

    for(int i=0; i<configures.histBins; i++){
        if(i==0)
            pre = configures.histBins-1;
        else
            pre = i-1;

        post = (i+1)%configures.histBins;

        if(hist[i] > oriDenThres && hist[i]>hist[pre] && hist[i]>hist[post]){
            count++;

            double binOffset = (0.5* (hist[pre]-hist[post])) / (hist[pre] - 2.0*hist[i] +hist[post]);
            double newBin = i + binOffset;
//            printf("%lf\n", newBin);

            if(newBin < 0)
                newBin = configures.histBins + newBin;
            else if(newBin >= configures.histBins)
                newBin = newBin - configures.histBins;

            if(count > 1){
               // continue;
                Feature newFeature;
                feat.copyFeature( feat, newFeature );
                newFeature.orient =( (CV_PI * 2.0 * newBin)/configures.histBins ) - CV_PI;
                features.push_back(newFeature);
            }
            else{
                //This feature is already in the vector Features,we do not need to push_back it
                feat.orient = ( (CV_PI * 2.0 * newBin)/configures.histBins ) - CV_PI;
            }
        }
    }
}


void SiftExtractor::calcDescriptor(vector<Feature>& features){
   vector< vector< vector<double> > > hist;
   
   for(int i=0; i<features.size(); i++){
       calcDescHist(features[i], hist);
   }
}


void SiftExtractor::calcDescHist(Feature& feature, vector< vector< vector<double> > >& hist){
   double orient = feature.orient;
   double octave = feature.meta->scale;
   double cosOri = cos(orient);
   double sinOri = sin(orient);
   double curSigma = 0.5 * (configures.descWinWidth);
   double subWidth = feature.meta->scale * configures.descScaleAdjust; 

   //The radius of required image field to calculate
   int radius = (subWidth*sqrt(2.0)*(configures.descWinWidth+1))/2.0 + 0.5;

    //initialize the histogram
    hist.erase(hist.begin(),hist.end());
    for(int i=0; i<configures.descWinWidth; i++){
        hist.resize(configures.descWinWidth,0.0);
        for(int j=0; j<configures.descWinWidth; j++){
            hist[i][j].resize(configures.descHistBins,0.0);
        }
    }
    
    double subOri,subMag;
    for(int i=-1*radius; i<=radius; i++){
        for(int j=-1*radius; j<=radius; j++){
            double xRotate = (cosOri*j - sinOri*i)/subWidth;
            double yRotate = (sinOri*j + cosOri*i)/subWidth;
            
            double xIdx = xRotate + configures.descWinWidth/2 -0.5;
            double yIdx = yRotate + configures.descWinWidth/2 -0.5;
           
            if(xIdx>-1.0 && xIdx<configures.descWinWidth && yIdx>-1.0 && yIdx<configures.descWinWidth){
                int posX = feature.meta->location.x+j;
                int posY = feature.meta->location.y+i;
                if(calcMagOri(feature.meta->img, posX, posY, subMag, subOri)){
                   subOri = CV_PI-subOri-orient;
                   
                   //TO TEST
                  while(subOri < 0.0)
                      subOri += 2*CV_PI;
                  while(subOri >= 2*CV_PI)
                      subOri -= 2*CV_PI;
                   
                   double resultIdx = subOri * (configures.descHistBins/(2*CV_PI));
                   double weight = exp((-1.0/(2*curSigma*curSigma))*(xRotate*xRotate + yRotate*yRotate));
                   
                   //TODO cha zhi yun suan
                   //---------------------------
                }
            }
        }
    }

}
