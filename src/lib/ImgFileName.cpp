// =====================================================================================
// 
//       Filename:  ImgFileName.cpp
// 
//    Description:  
// 
//        Version:  0.01
//        Created:  2014/11/27 21时55分48秒
//       Revision:  none
//       Compiler:  clang 3.5
// 
//         Author:  wengsht (SYSU-CMU), wengsht.sysu@gmail.com
//        Company:  
// 
// =====================================================================================

#include "ImgFileName.h"
#include "configure.h"

#include "unistd.h"
#include <sys/types.h>
#include <sys/stat.h>
#include <dirent.h>

#include <cstdio>
#include <cstdlib>

#include <cstring>
#include <cstdio>
#include <cstdlib>

#include "tool.h"

using namespace bio;

ImgFileName::ImgFileName() {
}
ImgFileName::~ImgFileName() {
}

void ImgFileName::generateImgNames(const char * dirName, std::vector<char *> & imgFileNames) {
    DIR *dirp;
    struct dirent *file;
    struct stat fileStat;

    dirp = opendir(dirName);

    int dirLen =  strlen(dirName);
    while(NULL != (file = readdir(dirp))) {
        if(isImgFile(file->d_name)) {
            char *newFileName = new char[strlen(dirName) + strlen(file->d_name) + 2];
            sprintf(newFileName, "%s%c%s", dirName, DIR_SEPARATOR, file->d_name);

            imgFileNames.push_back( newFileName );
        }
    }
    closedir(dirp);
}

char * ImgFileName::generateSiftFileName(const char * fileName) {
    static char siftFileName[MAX_FILE_NAME_LEN];
    strcpy(siftFileName, fileName);

    char *suffix = getSuffix(siftFileName);

    strcpy(suffix, SIFT_SUFFIX);

    return siftFileName;
}

char * ImgFileName::getSuffix(char * fileName) {
    int idx = strlen(fileName) - 1;

    while(idx >= 0 && fileName[idx] != '.') 
        idx --;

    idx ++;

    return fileName + idx;
}

bool ImgFileName::isImgFile(const char * fileName) {
    char * imgSuffix = getSuffix(const_cast<char *>(fileName));
#define IMG_SUFFIX(suffix) \
    if(0 == strcmp(suffix, imgSuffix)) \
        return true;
#include "imgSuffix.def"

    return false;
}

unsigned long ImgFileName::parseHashTag( const char * fileName ) {
    static unsigned long count = 0;
    static char prefix[MAX_FILE_NAME_LEN];

    parseObjectPrefix(fileName, prefix);

    if(! hashTable.count(std::string(prefix)) )
        hashTable[std::string(prefix)] = count ++;

    return hashTable[std::string(prefix)];
}

std::map< std::string, unsigned long > ImgFileName::hashTable;

void ImgFileName::parseObjectPrefix(const char *fileName, char *prefix) {
    int idx = strlen(fileName) - 1;
    while(idx >= 0 && fileName[idx] != DIR_SEPARATOR) 
        idx --;

    idx ++;
    strcpy(prefix, fileName + idx);

    char *separator = strstr(prefix, PREFFIX_SEPARATOR);

    if(!separator)
        separator = strstr(prefix, DOT_SEPARATOR);

    if(separator)
        separator[0] = '\0';

    Log("Filename:[%s], Object Prefix: [%s]", fileName, prefix);
}
