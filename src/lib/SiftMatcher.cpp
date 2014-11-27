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

using namespace bio;

SiftMatcher::SiftMatcher() {

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

void SiftMatcher::loadFile(const char *fileName) {
    images.loadTemplate( fileName );
}

void SiftMatcher::loadFeatures(std::vector<Feature> & inputFeat) {
    images.loadFeatures(inputFeat);
}

std::vector< Feature > & SiftMatcher::getFeatures() {
    return images.feat_database;
}
