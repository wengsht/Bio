// =====================================================================================
// 
//       Filename:  ImageSet.cpp
// 
//    Description:  
// 
//        Version:  0.01
//        Created:  2014/11/27 21时20分35秒
//       Revision:  none
//       Compiler:  clang 3.5
// 
//         Author:  wengsht (SYSU-CMU), wengsht.sysu@gmail.com
//        Company:  
// 
// =====================================================================================

#include "ImageSet.h"
#include "ImgFileName.h"
#include "SiftExtractor.h"

#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/highgui/highgui.hpp"

#include <fstream>
#include <sstream>

#include "tool.h"

using namespace bio;

ImageSet::ImageSet() {
}

ImageSet::~ImageSet() {
}

void ImageSet::loadTemplates(const char *dirName) {
    ImgFileName::generateImgNames(dirName, fileNames);

    for(int idx = 0;idx < fileNames.size(); idx ++) {
        Log("Loading image [%s]", fileNames[idx]);
        loadTemplate( fileNames[idx] );
    }
}

void ImageSet::loadTemplate(const char *fileName) {
    static vector<Feature> tmpFeats;

    tmpFeats.clear();

    Mat img = imread(fileName, 0);

    char * siftFeatFileName = ImgFileName::generateSiftFileName(fileName);

    if(true == tryLoadFromSiftFile(tmpFeats, siftFeatFileName)) {
        SiftExtractor::assignContainer(tmpFeats, (void *)(const_cast<char *>(fileName)));
    }
    else {
        extractor.sift(& img, tmpFeats, (void *) fileName );

        storeSiftFile(tmpFeats, siftFeatFileName);
    }

    feat_database.insert(feat_database.end(), tmpFeats.begin(), tmpFeats.end());
}

void ImageSet::loadFeatures(std::vector<Feature> & inputFeat) {
    feat_database.insert(feat_database.end(), inputFeat.begin(), inputFeat.end());
}

bool ImageSet::tryLoadFromSiftFile(std::vector<Feature> & feats, const char * siftFileName) {
    std::ifstream fin;

    fin.open(siftFileName, std::ios::in);

    if(fin.fail() == true)
        return false;

    std::string tmp;

    while(getline(fin, tmp)) {
        Feature newFeat;
        std::stringstream sin( tmp );
        newFeat.load( sin );

        feats.push_back(newFeat);
    }

    return true;
}

// TODO 
void ImageSet::storeSiftFile(std::vector<Feature> & feats, const char * siftFileName) {
    std::ofstream fout(siftFileName);

    int idx = 0;
    for(idx = 0; idx < feats.size(); idx ++) {
        std::stringstream sout("");

        feats[idx].store(sout);

        fout << sout.str() << std::endl;
    }
}


void ImageSet::close() {
    int idx;
    for(idx = 0; idx < fileNames.size(); idx++) 
        delete [] (fileNames[idx]);

    fileNames.clear();
}
