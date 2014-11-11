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

#include "feature.h"

#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/highgui/highgui.hpp"

using namespace std;
using namespace cv;

namespace bio {
class SiftExtractor {
    struct sift_configure {
        // 每次添加configure的参数的时候
        // 记得在configure.h 写默认值，
        // 并且写一个set函数方便调节
        
        double basicSigma;

        int innerLayerPerOct;

        double extremaThres;
    };

    class Octave {
    public:
        double sigma;

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

        void generateBlurLayers(int layers) {
            assert(images.size() >= 1);

            images.resize(1);

//            printf("%lf\n", sigma);
            for(int i = 1; i < layers; i++) {
                images.push_back( images[i-1].clone() );

                GaussianBlur(images[i-1], images[i], Size(0, 0), sigma, sigma);

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
                imshow("test", images[i] * 10);
                waitKey(1000);
                */
            }

//            printf("%d\n", images.size());
            images.erase(images.begin() + layerSiz-1);
//            printf("w%d\n", images.size());
        }
    };

    public:

        SiftExtractor();
        ~SiftExtractor();

        // extract 
        void sift(Mat* img, vector<Feature> & outFeatures);

    private:
        // Generate pyramid, output into octaves
        void generatePyramid(Mat * img, vector< Octave > &octaves);

        // Generate DOG Pyramid
        void generateDOGPyramid(vector< Octave > & octaves);

        // Generate Eigen points
        void extremaDetect(vector< Octave > & octaves, vector<Feature> & outFeatures);
        void extremaDetect(Octave & octave, vector<Feature> & outFeatures);


        // check if octave[layer][y][x] is an extrema
        // compare with 26 pixels
        // near pixels that can not be max/min will be flagged
        // Caller should make sure that [layer][y][x] is not a pixel on the margin or outside
        bool isExtrema(Octave & octave, int layer, int x, int y, bool *nxtMinFlags, bool* nxtMaxFlags, int rollIdx);
        sift_configure configures;
};
}

#endif

