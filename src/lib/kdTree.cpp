// =====================================================================================
// 
//       Filename:  kdTree.cpp
// 
//    Description:  
// 
//        Version:  0.01
//        Created:  2014/11/23 14时08分02秒
//       Revision:  none
//       Compiler:  clang 3.5
// 
//         Author:  wengsht (SYSU-CMU), wengsht.sysu@gmail.com
//        Company:  
// 
// =====================================================================================

#include "kdTree.h"
#include "tool.h"
#include <vector>
#include <algorithm>
#include "configure.h"

using namespace bio;

KDTree::KDTree() : link(NULL), root(NULL) {

}

KDTree::~KDTree() {

}

void KDTree::buildTree(std::vector<Feature> & features) {
    close(); 

    this->features = & features;

    int pointSiz = features.size();

//    (*(this->features))[0].dump(std::cout);

    root = generateRoot(pointSiz);

    split( root );
}

void KDTree::split( KDNode * parent ) {
    std::vector<Feature> & features = *(this->features);

    int k = parent->k = selectDimension( parent );

//    printf("%d\n", k);

    if(NIL_HEAD == k)
        return ;

    double median = parent->median = findMedian( parent, parent->k );
//    printf("%lf\n", median);
    
    KDNode *left  =  new KDNode;
    KDNode *right = new KDNode;

    int nodeIdx;
    int nxtIdx;
    for(nodeIdx = parent->head; nodeIdx != NIL_HEAD; nodeIdx = nxtIdx) {
//            printf("%d\n", nodeIdx);

        nxtIdx = link[nodeIdx];

        double val = features[nodeIdx][k];

        if(val <= median) 
            addFeature2KDNode(left, nodeIdx);
        else 
            addFeature2KDNode(right, nodeIdx);
    }

    if(left->empty() || right->empty()) {
        if(! left->empty()) parent->head = left->head;
        else                parent->head = right->head;

        delete left;
        delete right;

        return ;
    }

    parent->left = left;
    parent->right = right;
    parent->clear();

    split(left);
    split(right);
}

std::pair<Feature *, Feature *> KDTree::bbfNearest( Feature & input ) {

    backTrackTimes = KD_MAX_BACKTRACK;
    while(!backTrack_heap.empty())
        backTrack_heap.pop();

    std::pair<int, int> idxs = kd_dfs( root, input, INF, NIL_HEAD, INF, NIL_HEAD );

    return std::make_pair( &((*features)[idxs.first]),&((*features)[idxs.second]))  ;
}

KDNode * KDTree::getNextCandid(double bestEuDist) {
    if(backTrackTimes --  <= 0)
        return NULL;

    if(backTrack_heap.empty())
        return NULL;

    KD_DfsNode candid = backTrack_heap.top();

    backTrack_heap.pop();

    if(candid.first < bestEuDist) 
        return candid.second;

    return NULL;
}

std::pair<int, int> KDTree::kd_dfs(KDNode * node, Feature & input, double bestEuDist, int bestIdx, double secBestEuDist, int secBestIdx) {
    if(!node) 
        return std::make_pair(bestIdx, secBestIdx);

    std::vector<Feature> & features = *(this->features);

    if(node->leaf()) {
        int nodeIdx = node->head;

        double tmpDist = (features[nodeIdx] - input);

        if(tmpDist < secBestEuDist) {
            secBestEuDist = tmpDist;

            secBestIdx = nodeIdx;
        }
        if(secBestEuDist < bestEuDist) {
            std::swap(secBestEuDist, bestEuDist);
            std::swap(bestIdx, secBestIdx);
        }

        KDNode * candid = getNextCandid(bestEuDist);

        if(NULL == candid) 
            return std::make_pair(bestIdx, secBestIdx);

        return kd_dfs(candid, input, bestEuDist, bestIdx, secBestEuDist, secBestIdx);
    }

    const int &k = node->k;
    const double &median = node->median;

    double val = input[k];

    KDNode * nextNode, *candidNode;

    if(val <= median) {
        /// left 
        nextNode = node->left;
        candidNode = node->right;
    }
    else {
        /// right
        nextNode = node->right;
        candidNode = node->left;
    }

    addCandid( candidNode, fabs(val - median) );

    return kd_dfs(nextNode, input, bestEuDist, bestIdx, secBestEuDist, secBestIdx);
}

void KDTree::addCandid(KDNode * node, double val) {
    backTrack_heap.push(std::make_pair(val, node));
}

void KDTree::addFeature2KDNode(KDNode *node, int idx) {
    link[idx] = node->head;
    node->head = idx;
}

void KDTree::close() {
    if(link) {
        delete [] link;
        link = NULL;
    }

    clear(&root);
}

void KDTree::clear(KDNode **node) {
    if(NULL == (*node)) 
        return ;

    clear( &((*node)->left ) );
    clear( &((*node)->right) );

    delete (*node);
    *node = NULL;
}

KDNode * KDTree::generateRoot(int pointSiz) {
    link = new int[pointSiz];

    KDNode * root = new KDNode;

    int idx;
    for(idx = 0;idx < pointSiz; idx++)
        addFeature2KDNode(root, idx);

    return root;
}

int KDTree::selectDimension( KDNode * node ) {
    std::vector<Feature> & features = *(this->features);

    if(features.size() == 0)
        return NIL_HEAD;

    int dimension = features[0].size();
    int pointSiz  = 0;

    int k = NIL_HEAD;

    int d_idx, nodeIdx;
    double mean, sqrMean;
    double maxVar = 0.0, var;

    for(d_idx = 0;d_idx < dimension; d_idx++) {
        pointSiz = 0;
        mean = sqrMean = 0.0;
        for(nodeIdx = node->head; \
                nodeIdx != NIL_HEAD; \
                nodeIdx = link[nodeIdx]) {

            pointSiz ++;

            double val = features[nodeIdx][d_idx];

            mean += val;
            sqrMean += (val * val);
        }

        ///
        /// Don't split cluster with [<=1] point
        ///
        if(pointSiz <= 1) 
            return NIL_HEAD;

        mean /= pointSiz;
        sqrMean /= pointSiz;

        var = sqrMean - mean * mean;

        if(var > maxVar) {
            maxVar = var;

            k = d_idx;
        }
    }

    return k;
}

double KDTree::findMedian( KDNode * node, int k ) {
    static std::vector<double> tmpVec;
    std::vector<Feature> & features = *(this->features);

    int pointSiz = 0;
    int nodeIdx;
    for(nodeIdx = node->head; \
            nodeIdx != NIL_HEAD; \
            nodeIdx = link[nodeIdx]) {


        double val = features[nodeIdx][k];

        if(tmpVec.size() <= pointSiz)
            tmpVec.resize( pointSiz + 10 );

        tmpVec[pointSiz ++] = val;
    }

//    Log("Clac median %d points", pointSiz);

    int medianIdx = (pointSiz - 1) / 2;
    std::nth_element(tmpVec.begin(), tmpVec.begin() + medianIdx, tmpVec.begin() + pointSiz);

    return tmpVec[medianIdx];
}

void KDTree::dump(KDNode * node, std::ostream &dotOut) {
    static char buf[1024];
    if(!node) return ;
    if(node->left) {
        sprintf(buf, "%llu -> %llu[label=\"<%d,%lf>\"]", (unsigned long long)node, (unsigned long long)(node->left), node->k, node->median);
        dotOut << buf << std::endl;

        dump(node->left, dotOut);
    }
    if(node->right) {
        sprintf(buf,"%llu -> %llu[label=\"<%d,%lf>\"]", (unsigned long long)node, (unsigned long long)(node->right), node->k, node->median);
        dotOut << buf << std::endl;

        dump(node->right, dotOut);
    }
    if(node->leaf()) {
        int nodeIdx;
        for(nodeIdx = node->head; \
                nodeIdx != NIL_HEAD; \
                nodeIdx = link[nodeIdx]) {
            sprintf(buf, "%llu -> %d[label=\"<leaf>\"]", (unsigned long long)node, nodeIdx);
            dotOut << buf << std::endl;
        }
    }
}

void KDTree::dumpDot( std::ostream &dotOut ) {
    dotOut << "digraph {\n";
    dump(root, dotOut);
    dotOut << "}\n";
}
