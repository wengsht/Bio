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
//         Author:  
//         chenss(SYSU-CMU), 
//         wengsht (SYSU-CMU), wengsht.sysu@gmail.com
//        Company:  
// 
// =====================================================================================
#ifndef _AUTOGUARD_SiftExtractor_H_
#define _AUTOGUARD_SiftExtractor_H_

#include <vector> 
#include <assert.h>
#include <map>
#include <cmath>

#include "feature.h"
#include "define.h"

#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/highgui/highgui.hpp"

using namespace std;
using namespace cv;

namespace bio {
class SiftExtractor {
    struct sift_configure {
        // siftConfigure.def
        
#define SIFT_CONFIGURE(type, name, value) \
        type name;
#include "siftConfigure.def"
#undef SIFT_CONFIGURE

    };

    class Octave {
    public:
        vector< Mat > images;

        int size() const {
            return images.size();
        }
        Mat & operator[] (int idx) {
            return images[idx];
        }
        void addImg(const Mat & img) {
            images.push_back(img);
        }

        void generateBlurLayers(int layers, double *sigmas) {
            assert(images.size() >= 1);

            images.resize(1);

//            printf("%lf\n", sigma);
            for(int i = 1; i < layers; i++) {
                images.push_back( images[i-1].clone() );

                GaussianBlur(images[i-1], images[i], Size(0, 0), sigmas[i], sigmas[i]);

                /*  
                imshow("test", images[i]);
                waitKey(1000);
                */
            }
        }

        void generateDogLayers() {
            int layerSiz = images.size();
            
            for(int i = 0;i < layerSiz - 1; i++) {
                assert(images[i].cols == images[0].cols && images[i].rows == images[0].rows);

                images[i] = images[i+1] - images[i];

                /*  
                imshow("test", 10.0 * images[i]);
                waitKey(1000);

                */
            }

//            printf("%d\n", images.size());
            images.erase(images.begin() + layerSiz-1);
//            printf("w%d\n", images.size());
        }

        void show() {
            for(int i = 0;i < images.size();i++) {
                imshow("debug", images[i]);
                waitKey(1000);
            }
        }
    };

    public:

        SiftExtractor();
        ~SiftExtractor();

        sift_configure & getConfigureFile() {
            return configures;
        }
        // extract 
        void sift(Mat* img, vector<Feature> & outFeatures);

    private:
        // Generate pyramid, output into octaves
        void generatePyramid(Mat * img, vector< Octave > &octaves);

        // Generate DOG Pyramid
        void generateDOGPyramid(vector< Octave > & octaves);

        // Generate Eigen points
        void extremaDetect(vector< Octave > & octaves, vector<Feature> & outFeatures);
        void extremaDetect(Octave & octave, int octIdx, vector<Feature> & outFeatures);


        // check if octave[layer][y][x] is an extrema
        // compare with 26 pixels
        // near pixels that can not be max/min will be flagged
        // Caller should make sure that [layer][y][x] is not a pixel on the margin or outside
        bool isExtrema(Octave & octave, int layer, int x, int y, EXTREMA_FLAG_TYPE *nxtMinFlags, EXTREMA_FLAG_TYPE* nxtMaxFlags, int rollIdx);
        bool shouldEliminate(Octave &octave, int &layer, int &x, int &y, double *_X);
        bool poorContrast(Octave & octave, int &layer, int &x, int &y, double *_X);
        bool edgePointEliminate(Mat &img, int x, int y);

        void addFeature(Octave &octave, int octIdx, int layer, int x, int y, double *_X, vector<Feature> &outFeatures);

        void calcJacobian(Octave &octave, int layer, int x, int y, double Jacobian[3]);
        void calcHessian(Octave &octave, int layer, int x, int y, double Hessian[3][3]);

        // _X = -Hessian^(-1) X Jacobian
        void calcOffset(double Hessian[3][3], double Jacobian[3], double _X[3]);
        // check if octave[layer][x][y] is an inner point( bound imgMargin)
        inline bool innerPixel(Octave &octave, int layer, int x, int y);

        bio::point<double> pixelOriMap(int octIdx, int layer, int x, int y, double *_X);

        // D + 0.5 * Jacobian(D,X) * _X
        // X = {x, y, layerIdx}
        // _X = {_x, _y, _layerIdx}
        double calcDxValue(Octave &octave, int X[3], double _X[3]);

        // clear buffers used during feature extracting
        void clearBuffers() {
            for(int i = 0;i < bufferMetas.size(); i++)
                delete bufferMetas[i];
            bufferMetas.clear();
        }

        static void show(Mat *img, vector<Feature> & features) {
            Mat tmp = Mat(img->size(), CV_8UC3);
            
            img->convertTo(tmp, CV_8UC1, 255);
            cvtColor(tmp, tmp, CV_GRAY2RGB);

            for(int i = 0;i < features.size(); i++) {
                double scale = 6 * features[i].meta->scale;
                double orient = features[i].orient;

                Point p1 = Point(features[i].originLoc.x, features[i].originLoc.y);
                Point p2 = Point(p1.x + scale * cos(orient),\
                        p1.y + scale * sin(orient));


                circle(tmp, p1, 2, Scalar(0,0,255));

                line(tmp, p1, p2, Scalar(255,0,0), 1, 8);
            }

            imshow("debug", tmp);
            waitKey(10000000);
        }
        Mat &siftFormatImg(Mat *img);

    private:
        sift_configure configures;

        void calcFeatureOri(vector< Feature >& features, vector< Octave > & octaves);
        void calcOriHist(Feature& feature, vector< double >& hist);
        bool calcMagOri(Mat* img, int x, int y, double& mag, double& ori);
        double getMatValue(Mat* img, int x, int y);
        void smoothOriHist(vector< double >& hist );
        void addOriFeatures(vector<Feature>& features, Feature& feat, vector< double >& hist);
        
        vector<Meta *> bufferMetas;
};
}

#endif

