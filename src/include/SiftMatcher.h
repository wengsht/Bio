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
         * \brief setup KDTree for a set of features
         * \param[in] features "datebase"
         *
         * */
        void setup(std::vector<Feature> & features);

        /**
         * \brief match a input point, return the nearest point in "database"
         * \param[in] features "datebase"
         *
         * */
        Feature & match(Feature & input);

        void dumpDot(std::ostream &dotOut);

    private:

        KDTree kdTree;
};
}

#endif

