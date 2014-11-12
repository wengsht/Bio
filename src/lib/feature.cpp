#include "feature.h"

using namespace bio;

int Feature::descriptor_length = DEFAULT_DESCR_LEN;

Feature Feature::copyFeature(Feature& feature, Feature& newFeature){
    newFeature.originLoc = bio::point<int>(feature.originLoc.x,feature.originLoc.y);
    
    newFeature.scale = feature.scale;
    newFeature.orient = feature.orient;
    newFeature.meta = newFeature.meta;
    
    for(int i=0; i<DEFAULT_DESCR_LEN; i++)
        newFeature.descriptor[i] = feature[i];
}
