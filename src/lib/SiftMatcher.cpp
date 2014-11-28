// =====================================================================================
// 
//       Filename:  SiftMatcher.cpp
// 
//    Description:  
// 
//        Version:  0.01
//        Created:  2014/11/23 15时28分42秒
//       Revision:  none
//       Compiler:  clang 3.5
// 
//         Author:  wengsht (SYSU-CMU), wengsht.sysu@gmail.com
//        Company:  
// 
// =====================================================================================

#include "SiftMatcher.h"
#include <fstream>
#include "ImgFileName.h"

using namespace bio;

SiftMatcher::SiftMatcher() : \
        matchRatio(DEFAULT_MATCH_RATIO)
{

}

SiftMatcher::~SiftMatcher() {
    kdTree.close();
    images.close();
}

void SiftMatcher::setup() {
    kdTree.buildTree( images.feat_database );
}


std::pair<Feature *, Feature *> SiftMatcher::match(Feature & input) {
    return kdTree.bbfNearest( input );
}

void SiftMatcher::dumpDot(std::ostream &dotOut) {
    kdTree.dumpDot(dotOut);
}

void SiftMatcher::loadDir(const char *dirName) {
    images.loadTemplates( dirName );
}

bool SiftMatcher::isGoodMatch(std::pair<Feature *, Feature *> matchs, Feature &inputFeat) {
    if(matchs.second == NULL) 
        return false;

    Feature & best = *(matchs.first);
    Feature & second = *(matchs.second);

    double bestVal = best - inputFeat;
    double secBestVal = second - inputFeat;

    return (bestVal / secBestVal < matchRatio);
}

unsigned long SiftMatcher::match(vector<Feature> &inputFeats) {
    int *matchCnt = new int[ImgFileName::getTagCnt()];

    int featIdx, tagIdx;
    std::pair<Feature *, Feature *> matchPair;

    for(featIdx = 0; featIdx < inputFeats.size(); featIdx ++) {
        matchPair = match( inputFeats[featIdx] );

        if(isGoodMatch(matchPair, inputFeats[featIdx])) {
            int matchTag = (matchPair.first)->getHashTag();
            if(matchTag >= 0 && matchTag < ImgFileName::getTagCnt())
                matchCnt[matchTag] ++;
        }
    }

    unsigned long resTag = -1;
    int maxCnt = 0;

    for(tagIdx = 0; tagIdx < ImgFileName::getTagCnt(); tagIdx ++) {
        if(maxCnt < matchCnt[tagIdx]) {
            maxCnt = matchCnt[tagIdx];
            resTag = tagIdx;
        }
    }
    delete matchCnt;

    return resTag;
}

void SiftMatcher::loadFile(const char *fileName) {
    images.loadTemplate( fileName );
}

void SiftMatcher::loadFeatures(std::vector<Feature> & inputFeat) {
    images.loadFeatures(inputFeat);
}

std::vector< Feature > & SiftMatcher::getFeatures() {
    return images.feat_database;
}
