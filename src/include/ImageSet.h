// =====================================================================================
// 
//       Filename:  ImageSet.h
// 
//    Description:  
// 
//        Version:  0.01
//        Created:  2014/11/27 21时19分19秒
//       Revision:  none
//       Compiler:  clang 3.5
// 
//         Author:  wengsht (SYSU-CMU), wengsht.sysu@gmail.com
//        Company:  
// 
// =====================================================================================
#ifndef _AUTOGUARD_ImageSet_H_
#define _AUTOGUARD_ImageSet_H_

#include <vector>
#include "feature.h"
#include "SiftExtractor.h"

/**
 *
 * \brief This class is to store template images
 *
 */
namespace bio {
class ImageSet {
    public:
        friend class SiftMatcher;

        ImageSet();
        ~ImageSet();

        /**
         *
         * \brief load template images in the dirName
         * \param[in] dirName template images' directory 
         *
         * */
        void loadTemplates(const char *dirName);
        /**
         *
         * \brief load template image from a file
         * \param[in] fileName template image' file name
         *
         * */
        void loadTemplate(const char *fileName);
        
        void loadFeatures(std::vector<Feature> & inputFeat);
        
        void close();

        vector<Feature> & getFeatures() ;
        
        Feature & operator [](int idx);
    private:
        /**
         *
         * \brief try load sift feature from sift file(*.sift)
         *
         * */
        bool tryLoadFromSiftFile(std::vector<Feature> & feats, const char * siftFileName);
        /**
         *
         * \brief store sift feature to sift file(*.sift)
         *
         * */
        void storeSiftFile(std::vector<Feature> & feats, const char * siftFileName);

        SiftExtractor extractor;

        std::vector<Feature> feat_database;

        std::vector< char * > fileNames;
};
}

#endif
