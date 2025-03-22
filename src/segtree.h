#ifndef SEGTREE_H
#define SEGTREE_H

/**
 * Opaque handle representing K parallel segment trees.
 * Use public interface below for interactions.
 */
typedef struct SegTree SegTree;

/**
 * Creates K parallel segment trees covering [start, end]
 * 
 * @param start     Start index of all segment trees (inclusive)
 * @param end       End index of all segment trees (inclusive)
 * @param K         Number of parallel segment tree instances
 * @return          New segment tree handle. Must be destroyed with 
 *                  segtree_destroy(). NULL on allocation failure
 */
SegTree* segtree_build(int start, int end, unsigned K);

/**
 * Safely destroys segment trees and releases all resources
 * 
 * @param st        Handle from segtree_build(). No-op if NULL
 */
void segtree_destroy(SegTree* st);

/**
 * Updates a specific segment tree instance within the collection
 * 
 * @param st        Handle from segtree_build()
 * @param k         Index of target segment tree (0 ≤ k < K)
 * @param l         Left range boundary to update (inclusive)
 * @param r         Right range boundary to update (inclusive)
 * @param val       Value to set for all elements in [l, r]
 */
void segtree_range_set(SegTree* st, unsigned k, int l, int r, int val);

/**
 * Queries all K instances for zero-sum check in [l, r]
 *                  Stores 1 at [k] if sum==0 in k-th tree, else 0
 * 
 * @param st        Handle from segtree_build()
 * @param l         Left query boundary (inclusive)
 * @param r         Right query boundary (inclusive)
 * @param results   Pre-allocated output array (size ≥ K).
 */
void segtree_range_query(SegTree* st, int l, int r, int* results);

#endif // SEGTREE_H
