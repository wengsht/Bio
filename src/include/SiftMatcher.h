// =====================================================================================
// 
//       Filename:  SiftMatcher.h
// 
//    Description:  
// 
//        Version:  0.01
//        Created:  2014/11/23 15时27分36秒
//       Revision:  none
//       Compiler:  clang 3.5
// 
//         Author:  wengsht (SYSU-CMU), wengsht.sysu@gmail.com
//        Company:  
// 
// =====================================================================================
#ifndef _AUTOGUARD_SiftMatcher_H_
#define _AUTOGUARD_SiftMatcher_H_

#include "kdTree.h"
#include "feature.h"
#include <vector>
#include "ImageSet.h"


namespace bio {

/**  
 * \brief This is a class to match a feature point into a set of features ! (typically find the nearest feature point)
 *
 *  */
class SiftMatcher {
    public:
        SiftMatcher();
        ~SiftMatcher();

        /**
         * \brief setup KDTree
         *
         * */
        void setup();
        void loadDir(const char *dirName);
        void loadFile(const char *fileName);
        void loadFeatures(std::vector<Feature> & inputFeat);

        unsigned long match(vector<Feature> &inputFeats);

        /**
         * \brief match a input point, return the nearest point in "database"
         * \param[in] features "datebase"
         *
         * */
        std::pair<Feature *, Feature *> match(Feature & input);

        void dumpDot(std::ostream &dotOut);

        /**
         * \brief 
         *
         * */
        std::vector< Feature > & getFeatures();

        bool isGoodMatch(std::pair<Feature *, Feature *> matchs, Feature &inputFeat);

        void setMatchRatio(double ratio);

    private:
        double matchRatio;
        ImageSet images;

        KDTree kdTree;
};
}

#endif

