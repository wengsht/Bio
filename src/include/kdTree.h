// =====================================================================================
// 
//       Filename:  kdTree.h
// 
//    Description:  
// 
//        Version:  0.01
//        Created:  2014/11/23 14时05分16秒
//       Revision:  none
//       Compiler:  clang 3.5
// 
//         Author:  wengsht (SYSU-CMU), wengsht.sysu@gmail.com
//        Company:  
// 
// =====================================================================================
#ifndef _AUTOGUARD_KDTree_H_
#define _AUTOGUARD_KDTree_H_

#include <cstring>
#include <vector>
#include "feature.h"
#include <queue>
#include <iostream>

#include "tool.h"

namespace bio {

#define NIL_HEAD -1

struct KDNode {
    int head;      ///< link list head, index link in KDTree

    double median; ///< meadian of this KD-tree node
    int k;         ///< based dimension of this KD-tree node

    KDNode *left;  ///< left child of KDNode
    KDNode *right; ///< right child of KDNode

    KDNode() : head(NIL_HEAD), \
               median(0), \
               k(NIL_HEAD), \
               left(NULL), \
               right(NULL) 
    {}

    bool empty() const {
        return head == NIL_HEAD;
    }
    bool leaf() const {
        return ((!left) && (!right));
    }
    void clear() {
        head = NIL_HEAD;
    }
};

typedef std::pair< double, KDNode *> KD_DfsNode;

/**  
 * \brief kd-tree implementation. Finding closest point in a set of points
 *
 *  */
class KDTree {
    public:
        KDTree();
        ~KDTree();

        /**
         * \brief build kd-tree for a set of feature points
         * \param[in] features feature points( template )
         *
         * */
        void buildTree(std::vector<Feature> & features);

        /**
         * \brief match a input point, return the nearest point in "database"
         * \param[in] features "datebase"
         * \return first: nearest point second: sec nearest point
         *
         * */
        std::pair<Feature *,Feature *> bbfNearest( Feature & input );

        /**
         * \brief You should call this function before this object was destroy!
         * */
        void close();

        /**
         * \brief dump the kd-tree into .dot format stream
         * \param[in] dotOut
         *
         * */
        void dumpDot( std::ostream &dotOut );
    private:
        void dump(KDNode * node, std::ostream &dotOut);

        /**
         * \brief add feature's idx(features[idx]) to KDNode
         * \param[in] idx index in vector<Feature> *features;
         *
         * */
        void addFeature2KDNode(KDNode *, int idx); 

        /**
         *
         * \brief clear KDTree's memory, it is a recursive function
         * \param[in] node This funtion clear node's and its children's memories
         *
         * */
        void clear(KDNode **node);

        /**
         *
         * \brief split kd-node into 2 nodes
         *        It will split based on the dimension with highest variance. 
         * \param[in] parent
         *
         * */
        void split( KDNode * parent );

        /**
         *
         * \brief generate root node of KD-tree
         * \param[in] pointSiz size of features
         *
         * */
        KDNode * generateRoot(int pointSiz);

        /**
         *
         * \brief Select a dimension with larest variance
         * \param[in] node
         * \return dimension with larest variance
         *
         * */
        int selectDimension( KDNode * node );

        /**
         *
         * \brief find median in k-dimension
         * \param[in] k dimension 
         * \return median
         *
         * */
        double findMedian( KDNode * node, int k );

        /**
         *
         * \brief Add candid search node into priority queue, this node will be search if it is in the circle of best dist
         * \param[in] node search node
         * \param[in] val  if val less than best euDist, this node can be search 
         *
         * */
        void addCandid(std::priority_queue<KD_DfsNode> &backTrack_heap, KDNode * node, double val);

        /**
         *
         * \brief get next candid search node from priority queue
         * \return next avaliable node
         *
         * return NULL if bestEuDist is smaller than any candid
         *
         * */
        KDNode * getNextCandid(std::priority_queue<KD_DfsNode> & backTrack_heap, double bestEuDist);
    private:
        /**
         *
         * \brief Try to update best and second best val
         *        best and second val should be from different objects!!
         *
         * */
        inline void tryUpdate(int newIdx, double newVal, int &bestIdx, double &bestVal, int &secBestIdx, double &secBestVal);

        /**
         *
         * \brief Try to update best 
         *
         * */
        inline void tryUpdate(int newIdx, double newVal, int &bestIdx, double &bestVal);

        bool sameObject(int idx1, int idx2) {
            if(idx1 < 0 || idx2 < 0) return false;

            return (*features)[idx1].sameHashTag((*features)[idx2]);
        }
    private:
        std::vector<Feature> * features;
        KDNode * root;
        int * link; /**<
                      * link all the features' idx
                      *
                      * Example: 
                      *
                      * suppose 6 points, 2 (leaf) kdNodes:
                      *
                      * node1->head(3) -> 1 -> 5 -> -1
                      *
                      * node2->(head(2) -> 4 -> 0 -> -1
                      *
                      */
};
}


#endif

