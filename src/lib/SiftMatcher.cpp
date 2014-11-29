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
        matchRatio(DEFAULT_MATCH_RATIO), \
        matchThres(DEFAULT_MATCH_THRESHOLD)
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
    std::pair<Feature *, Feature *> matchPair = kdTree.bbfNearest(input);

    if(isGoodMatch(matchPair, input)) {
        return matchPair;
    }
    else 
        return std::make_pair((Feature *)NULL, (Feature *) NULL);
//    return kdTree.bbfNearest( input );
}

void SiftMatcher::dumpDot(std::ostream &dotOut) {
    kdTree.dumpDot(dotOut);
}

void SiftMatcher::loadDir(const char *dirName) {
    images.loadTemplates( dirName );
}

bool SiftMatcher::isGoodMatch(std::pair<Feature *, Feature *> matchs, Feature &inputFeat) {
    if(matchs.first == NULL) 
        return false;
    Feature & best = *(matchs.first);

    double bestVal = best - inputFeat;

    if(matchs.second == NULL) 
        return bestVal < matchThres;

    Feature & second = *(matchs.second);

    double secBestVal = second - inputFeat;

    return (bestVal / secBestVal < matchRatio);
}

unsigned long SiftMatcher::match(vector<Feature> &inputFeats) {
    int tagCnt = ImgFileName::getTagCnt();
    int *matchCnt = new int[tagCnt];
    memset(matchCnt, 0, sizeof(int) * tagCnt);

    int featIdx, tagIdx;
    std::pair<Feature *, Feature *> matchPair;

    for(featIdx = 0; featIdx < inputFeats.size(); featIdx ++) {
        matchPair = match( inputFeats[featIdx] );

        if(matchPair.first == NULL) continue;
//        if(isGoodMatch(matchPair, inputFeats[featIdx])) {
        int matchTag = (matchPair.first)->getHashTag();

        if(matchTag >= 0 && matchTag < tagCnt)
            matchCnt[matchTag] ++;
//        }
    }

    unsigned long resTag = -1;
    int maxCnt = 0;

    for(tagIdx = 0; tagIdx < tagCnt; tagIdx ++) {
        if(maxCnt < matchCnt[tagIdx]) {
            maxCnt = matchCnt[tagIdx];
            resTag = tagIdx;
        }
    }
    delete matchCnt;

    Log("%s %d\n", (ImgFileName::descriptor(resTag)).c_str(), maxCnt);
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
void SiftMatcher::setMatchRatio(double ratio) {
    matchRatio = ratio;
}

void SiftMatcher::setMatchThres(double thres) {
    matchThres = thres;
}
