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

#include "feature.h"

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
                imshow("test", 10.0 * images[i]);
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
        bool shouldEliminate(Octave &octave, int &layer, int &x, int &y);
        bool poorContrast(Octave & octave, int &layer, int &x, int &y);
        bool edgePointEliminate(Mat &img, int x, int y);

        void addFeature(Octave &octave, int octIdx, int layer, int x, int y, vector<Feature> &outFeatures);

    private:
        sift_configure configures;

        void calcFeatureOri(vector< Feature >& features, vector< Octave > & octaves);
        void calcOriHist(Feature& feature, vector< double >& hist);
        bool calcMagOri(Mat* img, int x, int y, double& mag, double& ori);
        double getMatValue(Mat* img, int x, int y);
        void smoothOriHist(vector< double >& hist );
        void addOriFeatures(vector< Feature >& features, vector< double >& hist);
        
        vector<Meta> bufferMetas;
};
}

#endif

