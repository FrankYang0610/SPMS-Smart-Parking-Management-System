#ifndef STATE_H
#define STATE_H

#include "vector.h"
#include "segtree.h"

typedef struct Statistics {
    Vector accepted;
    Vector rejected;
} Statistics;

typedef struct Tracker {
    SegTree* lu; // locker + umbrella
    SegTree* bc; // battery + cable
    SegTree* vi; // valet parking + inflation services
    SegTree* park; // parking slots
} Tracker;

void init_tracker(Tracker* tr);
void init_statistics(Statistics* stats);

#endif //STATE_H
