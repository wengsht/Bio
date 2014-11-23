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
}

void SiftMatcher::setup(std::vector<Feature> & features) {
    kdTree.buildTree( features );
}

Feature & SiftMatcher::match(Feature & input) {
    return kdTree.bbfNearest( input );
}

void SiftMatcher::dumpDot(std::ostream &dotOut) {
    kdTree.dumpDot(dotOut);
}
