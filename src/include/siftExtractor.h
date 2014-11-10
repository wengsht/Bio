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

#include "feature.h"
#include "opencv/cxcore.h"

using namespace std;

namespace bio {
class SiftExtractor {
    struct sift_configure {
        // 每次添加configure的参数的时候
        // 记得在configure.h 写默认值，
        // 并且写一个set函数方便调节
    };

    public:
        SiftExtractor();
        ~SiftExtractor();

        // extract 
        void sift(IplImage * img, vector<Feature> & outFeatures);

    private:
        sift_configure configures;
};
}

#endif

