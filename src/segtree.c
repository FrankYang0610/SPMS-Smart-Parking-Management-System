#include "segtree.h"
#include <stdlib.h>
#include <stdbool.h>
#include <assert.h>
#include <limits.h>  // Added for INT_MIN

struct SegTree {
    int start;
    int end;
    unsigned K;
    int** tree;
    int** lazy;
    bool** ifLazy;
    unsigned n;
    unsigned n4;
};

/* PRIVATE METHODS */

static void maintain(SegTree* st, unsigned k, int cl, int cr, int p) {
    if (cl == cr) return;

    if (st->ifLazy[k][p]) {
        int left = p * 2;
        int right = p * 2 + 1;

        st->lazy[k][left] = st->lazy[k][p];
        st->ifLazy[k][left] = true;
        st->tree[k][left] = st->lazy[k][p];  // Changed for max

        st->lazy[k][right] = st->lazy[k][p];
        st->ifLazy[k][right] = true;
        st->tree[k][right] = st->lazy[k][p];  // Changed for max

        st->lazy[k][p] = 0;
        st->ifLazy[k][p] = false;
    }
}

static int range_max(SegTree* st, unsigned k, int l, int r, int cl, int cr, int p) {
    if (l > cr || r < cl) return INT_MIN;  // Changed to return min for max query
    if (l <= cl && cr <= r) return st->tree[k][p];
    maintain(st, k, cl, cr, p);
    int cm = cl + (cr - cl) / 2;
    int left_max = range_max(st, k, l, r, cl, cm, p * 2);
    int right_max = range_max(st, k, l, r, cm + 1, cr, p * 2 + 1);
    return (left_max > right_max) ? left_max : right_max;  // Compute max of children
}

static void range_set(SegTree* st, unsigned k, int l, int r, int val, int cl, int cr, int p) {
    if (l > cr || r < cl) return;
    if (l <= cl && cr <= r) {
        st->lazy[k][p] = val;
        st->ifLazy[k][p] = true;
        st->tree[k][p] = val;  // Set to val (not multiplied by size)
        return;
    }
    maintain(st, k, cl, cr, p);
    int cm = cl + (cr - cl) / 2;
    range_set(st, k, l, r, val, cl, cm, p * 2);
    range_set(st, k, l, r, val, cm + 1, cr, p * 2 + 1);
    // Update current node's value to max of children
    st->tree[k][p] = (st->tree[k][p * 2] > st->tree[k][p * 2 + 1]) ? st->tree[k][p * 2] : st->tree[k][p * 2 + 1];
}

/* PUBLIC METHODS */

SegTree* segtree_build(int start, int end, unsigned K) {
    assert(end - start + 1 > 0);
    SegTree* st = (SegTree*)malloc(sizeof(SegTree));
    st->start = start;
    st->end = end;
    st->K = K;
    st->n = (unsigned)(end - start + 1);
    st->n4 = st->n * 4;
    st->tree = (int**)malloc(K * sizeof(int*));
    st->lazy = (int**)malloc(K * sizeof(int*));
    st->ifLazy = (bool**)malloc(K * sizeof(bool*));
    for (unsigned k = 0; k < K; k++) {
        st->tree[k] = (int*)calloc(st->n4, sizeof(int));
        st->lazy[k] = (int*)calloc(st->n4, sizeof(int));
        st->ifLazy[k] = (bool*)calloc(st->n4, sizeof(bool));
    }
    return st;
}

void segtree_destroy(SegTree* st) {
    if (st) {
        if (st->tree) {
            for (unsigned k = 0; k < st->K; k++) {
                free(st->tree[k]);
            }
            free(st->tree);
        }
        if (st->lazy) {
            for (unsigned k = 0; k < st->K; k++) {
                free(st->lazy[k]);
            }
            free(st->lazy);
        }
        if (st->ifLazy) {
            for (unsigned k = 0; k < st->K; k++) {
                free(st->ifLazy[k]);
            }
            free(st->ifLazy);
        }
        free(st);
    }
}

void segtree_range_set(SegTree* st, unsigned k, int l, int r, int val) {
    range_set(st, k, l, r, val, st->start, st->end, 1);
}

void segtree_range_query(SegTree* st, int l, int r, int* results) {
    for (unsigned k = 0; k < st->K; k++) {
        results[k] = range_max(st, k, l, r, st->start, st->end, 1);  // Directly store max value
    }
}
