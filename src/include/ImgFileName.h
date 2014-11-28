// =====================================================================================
// 
//       Filename:  ImgFileName.h
// 
//    Description:  
// 
//        Version:  0.01
//        Created:  2014/11/27 21时54分46秒
//       Revision:  none
//       Compiler:  clang 3.5
// 
//         Author:  wengsht (SYSU-CMU), wengsht.sysu@gmail.com
//        Company:  
// 
// =====================================================================================
#ifndef _AUTOGUARD_ImgFileName_H_
#define _AUTOGUARD_ImgFileName_H_

#include <vector>
#include <map>
#include <string>
/**
 * \brief this class process the file name of images
 *
 * */ 
namespace bio {
class ImgFileName {
    public:
        ImgFileName();
        ~ImgFileName();

        static void generateImgNames(const char * dirName, std::vector<char *> &imgFileNames);

        static char * generateSiftFileName(const char * fileName);

        static char * getSuffix(char * fileName);

        /**
         * \brief refine object prefix
         * \param[in] fileName e.g.   xx/xx/xx/john_yy.suffix
         * \param[out] prefix e.g. john
         *
         * */
        static void parseObjectPrefix(const char *fileName, char *prefix);

        /**
         * \brief calculate hash value for file name, file from the same object should have the same hash value
         *
         *
         * */
        static unsigned long parseHashTag( const char * fileName );

        static std::string descriptor(unsigned long hashTag) ;

        static unsigned long getTagCnt();

    private:
        static unsigned long tagCnt;
        static std::map< std::string, unsigned long > hashTable;
        static std::vector< std::string > descriptors;
        static bool isImgFile(const char * fileName);
};
}

#endif

