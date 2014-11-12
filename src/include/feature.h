#ifndef _AUTOGUARD_Feature_H_
#define _AUTOGUARD_Feature_H_

#include "define.h"
#include "configure.h"
#include "opencv2/imgproc/imgproc.hpp"

namespace bio {

template<class Type>
class point {
public:
    Type x, y;
    point() {}
    point(Type x, Type y) : x(x), y(y) {}
};

class Meta {
public:
    int octIdx;
    int layerIdx;

    double subLayer;

    cv::Mat * img;
    
    point<int> location;
};

class Feature {
public:
    static int descriptor_length;
    /* Location in the image */

    /* Location in original image */
    point<double> originLoc;

    /* scale */
    double scale;

    /*  orientation */
    double orient;

    double descriptor[DEFAULT_DESCR_LEN];

    Meta * meta;
};

}

#endif

