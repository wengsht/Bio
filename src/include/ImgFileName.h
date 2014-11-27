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

    private:
        static bool isImgFile(const char * fileName);
};
}

#endif

