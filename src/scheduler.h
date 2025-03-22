//
// scheduler.h
// The Scheduler
//

#ifndef SCHEDULER_H
#define SCHEDULER_H

#include "analyzer.h"
#include "segtree.h"
#include "utils.h"
#include "state.h"

bool process_request(Vector* queues[], Request* req);
bool process_batch(Vector* queues[], Request* req, Statistics* stats[], Tracker* trackers[], int* invalid_cnt);

void run_all(Vector* queue[], Statistics* stats[], Tracker* trackers []);
void run_fcfs(Vector* queue, Statistics* stats, Tracker* tracker);
void run_prio(Vector* queue, Statistics* stats, Tracker* tracker);
void run_opti(Vector* queue, Statistics* stats, Tracker* tracker);


#endif //SCHEDULER_H
