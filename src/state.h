#ifndef STATE_H
#define STATE_H

#include "vector.h"
#include "segtree.h"

// The Statistics struct keeps track of accepted and rejected requests.
// Each scheduling algorithm should have exactly one Statistics instance.
// Run try_put() before modify this struct.
// If try_put() returns true, then add the corresponding request into the accepted Vector, else,
// add the request into the rejected Vector.
typedef struct Statistics {
    Vector accepted;
    Vector rejected;
} Statistics;


// The Tracker struct keeps track of the occupied time ranges for each resource.
// Each scheduling algorithm should have exactly one Tracker instance.
// This struct shall be modified in try_put() and try_essential().
typedef struct Tracker {
    SegTree* lu;    // locker + umbrella
    SegTree* bc;    // battery + cable
    SegTree* vi;    // valet parking + inflation services
    SegTree* park;  // parking slots
} Tracker;


void init_tracker(Tracker* tr);
void init_statistics(Statistics* stats);

#endif //STATE_H
