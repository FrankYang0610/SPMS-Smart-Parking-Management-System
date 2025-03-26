#include "state.h"
#include "utils.h"

void init_tracker(Tracker* tr) {
    int start = parse_time("2025-05-10", "00:00") - 5;
    int end = parse_time("2025-05-16", "23:59") + 5;
    tr->lu = segtree_build(start, end, 3);              // 3 locker + umbrella
    tr->bc = segtree_build(start, end, 3);              // 3 battery + cable
    tr->vi = segtree_build(start, end, 3);              // 3 valet parking + inflation services
    tr->park = segtree_build(start, end,  10);          // 10 parking slots
}

void init_statistics(Statistics* stats) {
    vector_init(&stats->accepted);
    vector_init(&stats->rejected);
}

void reset_statistics(Statistics* stats) {
    vector_free(&stats->accepted);
    vector_free(&stats->rejected);
    init_statistics(stats);
}

void reset_tracker(Tracker* tr) {
    segtree_destroy(tr->lu);
    segtree_destroy(tr->bc);
    segtree_destroy(tr->vi);
    segtree_destroy(tr->park);
    init_tracker(tr);
}