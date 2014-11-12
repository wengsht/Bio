#include "feature.h"

using namespace bio;

int Feature::descriptor_length = DEFAULT_DESCR_LEN;

void Feature::copyFeature(Feature& feature, Feature& newFeature){
    newFeature = feature;
}
