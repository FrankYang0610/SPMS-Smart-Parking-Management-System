//
// scheduler.c
// The Scheduling Module (Part II)
//

#include <assert.h>
#include <unistd.h>
#include <stdbool.h>
#include <sys/wait.h>

#include "scheduler.h"
#include "input.h"
#include "vector.h"
#include "state.h"

static bool try_put(int idx, int start, int end, bool parking, char essential, Tracker* tracker);
static bool try_essentials(SegTree *st, int start, int end, int idx);

void process_request(Vector* queues[], Request* req) {
    for (int i = 0; i < 3; i++) vector_add(queues[i], *req);
}

bool process_batch(Vector* queues[], Request* req, Statistics* stats[], Tracker* trackers[], int* invalid_cnt) {
    // read the batch file
    // parse the batch file
    // add the requests to the queue

    const char* file = req->file;
    FILE* fp = fopen(file, "r");

    if (fp == NULL) {
        printf("Error: Cannot open the batch file %s\n", file);
        return true;
    }
    
    while (!feof(fp)) {
        Request rq = file_input(fp);

        switch (rq.type) {
            case BATCH: {
                bool is_termination = process_batch(queues, &rq, stats, trackers, invalid_cnt);
                if (is_termination) {
                    rq.type = TERMINATE;
                }
                __attribute__((fallthrough));
            }
            case TERMINATE:
                fclose(fp);
                return false;
            case REQUEST:
                process_request(queues, &rq);
                break;
            case PRINT:
                if (fork() == 0) {
                    printf("A fork() has been called.\n");
                    printf("Here is the child process to run the schedulers and print all bookings. pid = %d.\n\n", getpid());
                    schedule_and_print_bookings(req->algo, queues, stats, trackers, *invalid_cnt);
                    exit(0);
                } else {
                    wait(NULL);
                }
                break;
            case INVALID:
                (*invalid_cnt)++;
                break;
            default:
                break;
        }
    }

    return true;
}


void run_all(Vector* queues[], Statistics* stats[], Tracker* trackers[]) {
    run_fcfs(queues[0], stats[0], trackers[0]);
    run_prio(queues[1], stats[1], trackers[1]);
    run_opti(queues[2], stats[2], trackers[2]);
    printf("\n");
}

void run_fcfs(Vector* queue, Statistics* stats, Tracker* tracker) {
    if (queue->size == queue->next) return;
    for (int i = queue->next; i < queue->size; i++) {
        Request req = queue->data[i];
        int start = req.start;
        int end = start + req.duration;
        if (try_put(i, start, end, req.parking, req.essential, tracker)) { 
            stats->accepted.data[stats->accepted.size++] = req;
            printf("The FCFS scheduler has [accepted] the request.\n");
        } else {
            stats->rejected.data[stats->rejected.size++] = req;
            printf("The FCFS scheduler has [rejected] the request.\n");
        }
    }
    queue->next = (int)(queue->size);
    printf("The FCFS scheduler is updated.\n");
}

void run_prio(Vector* queue, Statistics* stats, Tracker* tracker) {
    if (queue->size == queue->next) return;
    int l = queue->next;
    int r = queue->size - 1;
    vector_qsort(queue, l, r, cmp_priority);
    for (int i = queue->next; i < queue->size; i++) {
        Request req = queue->data[i];
        int start = req.start;
        int end = start + req.duration;
        if (try_put(i, start, end, req.parking, req.essential, tracker)) { 
            stats->accepted.data[stats->accepted.size++] = req;
            printf("The PRIO scheduler has [accepted] the request.\n");
        } else {
            stats->rejected.data[stats->rejected.size++] = req;
            printf("The PRIO scheduler has [rejected] the request.\n");
        }
    }
    queue->next = queue->size;
    printf("The PRIO scheduler is updated.\n");
}

// TODO: Implement Optimized Scheduling Algorithm
void run_opti(Vector* queue, Statistics* stats, Tracker* tracker) {
    (void)queue;
    (void)stats;
    (void)tracker;
    printf("The OPTI scheduler is currently unavailable.\n");
}


// Try to response a request.
// This function will process both parking request and essential request(s).
static bool try_put(int idx, int start, int end, bool parking, char essential, Tracker* tracker) {

    idx += 1; // TODO: requires further discussion.

    /* Try Parking */

    if (parking) { // needs parking
        int query_res[10];
        segtree_range_query(tracker->park, start, end, query_res);
        for (int i = 0; i < 10; i++) {
            if (query_res[i]) {
                // the segment tree is set to the index of the queue. 
                // This allows us to know which request is occupying the parking slot during printBooking.
                segtree_range_set(tracker->park, (unsigned)i, start, end, idx);
                break;
            }
            if (i == 9) return false;
        }
    }


    /* Try Essentials */

    // battery + cable
    if ((essential & 0b100) && !try_essentials(tracker->bc, start, end, idx))
        return false;

    // locker + umbrella
    if ((essential & 0b010) && !try_essentials(tracker->lu, start, end, idx))
        return false;

    // valet parking + inflation services
    if ((essential & 0b001) && !try_essentials(tracker->vi, start, end, idx))
        return false;

    return true;
}

// This function will process a kind of essential.
// The argument SegTree *st is the corresponding segment tree of the essential.
static bool try_essentials(SegTree *st, int start, int end, int idx) {
    int query_res[3];
    segtree_range_query(st, start, end, query_res);

    for (int i = 0; i < 3; i++) {
        if (query_res[i]) {
            segtree_range_set(st, (unsigned)i, start, end, idx);
            break;
        }
        if (i == 2) return false;
    }
    return true;
}
