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
static void get_rating(Vector* occupy[], unsigned rating[], const unsigned K);
static unsigned calc_rating(Vector* reqs);
static void find_overlap (Vector* queue_og, SegTree* st, int l, int r, Vector* overlaps[], const unsigned K);

void process_request(Vector* queue, Request* req) {
    req->order = queue->size + 1; // order start from 1
    vector_add(queue, *req);
}

bool process_batch(Vector* queue, Request* req, int* invalid_cnt) {
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
                bool is_termination = process_batch(queue, &rq, invalid_cnt);
                if (is_termination) {
                    rq.type = TERMINATE;
                }
                __attribute__((fallthrough));
            }
            case TERMINATE:
                fclose(fp);
                return false;
            case REQUEST:
                process_request(queue, &rq);
                break;
            case PRINT:
                if (fork() == 0) {
                    printf("A fork() has been called.\n");
                    printf("Here is the child process to run the schedulers and print all bookings. pid = %d.\n\n", getpid());
                    schedule_and_print_bookings(req->algo, queue, *invalid_cnt);
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

/* Not needed for now
void run_all(Vector* queues[], Statistics* stats[], Tracker* trackers[]) {
    run_fcfs(queues[0], stats[0], trackers[0]);
    run_prio(queues[1], stats[1], trackers[1]);
    run_opti(queues[2], stats[2], trackers[2]);
    printf("\n");
}
*/

void run_fcfs(Vector* queue, Statistics* stats, Tracker* tracker) {
    for (int i = 0; i < queue->size; i++) {
        Request req = queue->data[i];
        int start = req.start;
        int end = start + req.duration - 1;
        if (try_put(i, start, end, req.parking, req.essential, tracker)) { 
            vector_add(&stats->accepted, req);
            printf("The FCFS scheduler has [accepted] the request.\n");
        } else {
            vector_add(&stats->rejected, req);
            printf("The FCFS scheduler has [rejected] the request.\n");
        }
    }
    printf("The FCFS scheduler is updated.\n");
}

void run_prio(Vector* queue, Statistics* stats, Tracker* tracker) {
    vector_qsort(queue, 0, queue->size-1, cmp_priority);
    for (int i = 0; i < queue->size; i++) {
        Request req = queue->data[i];
        int start = req.start;
        int end = start + req.duration - 1;
        if (try_put(i, start, end, req.parking, req.essential, tracker)) { 
            vector_add(&stats->accepted, req);
            printf("The PRIO scheduler has [accepted] the request.\n");
        } else {
            vector_add(&stats->rejected, req);
            printf("The PRIO scheduler has [rejected] the request.\n");
        }
    }
    printf("The PRIO scheduler is updated.\n");
}

// Greedy algorithm that maximize utilization.

void run_opti(Vector* queue, Statistics* stats, Tracker* tracker) {

    printf("DEBUG: The OPTI scheduler is running.\n");

    // initialization
    bool is_accepted[queue->size];
    Vector* queue_cpy = vector_copy(queue);
    reset_statistics(stats); 
    reset_tracker(tracker);
    memset(is_accepted, 0, sizeof(is_accepted));

    vector_qsort(queue_cpy, 0, queue_cpy->size - 1, cmp_start);

    // scheduling starts here

    for (int idx = 1; idx <= queue_cpy->size; idx++) {
        printf("DEBUG: OPTI scheduler is processing request %d.\n", idx);
        Request req = queue_cpy->data[idx - 1];
        int start = req.start;
        int end = start + req.duration - 1;
        bool parking = req.parking;
        char essential = req.essential;

        /* TRY TO SCHEDULE REQUEST */

        if (try_put(idx, start, end, parking, essential, tracker)) {
            printf("The OPTI scheduler has [direct accepted] the request.\n");
            is_accepted[idx - 1] = true;
            continue;
        }

        /* SCHEDULE FAILED, TRY REPLACE */

        Vector* prk_util[10];
        Vector* ess_util[3][3];
        for (int i = 0; i < 10; i++) {
            prk_util[i] = malloc(sizeof(Vector));
            vector_init(prk_util[i]);
        }
        for (int i = 0; i < 3; i++) for (int j = 0; j < 3; j++) {
            ess_util[i][j] = malloc(sizeof(Vector));
            vector_init(ess_util[i][j]);
        }

        unsigned prk_rating[10], ess_rating[3][3];
        memset(prk_rating, 0, sizeof(prk_rating));
        memset(ess_rating, 0, sizeof(ess_rating));

        if (parking) {
            find_overlap(queue, tracker->park, start, end, prk_util, (unsigned)10);
            get_rating(prk_util, prk_rating, (unsigned)10);
        }

        SegTree* st_dict[3] = {tracker->lu, tracker->bc, tracker->vi};
        if (essential) {
            for (int i = 2; i >= 0; i--) {
                if (essential & 0b1) {
                    essential >>= 1;
                    find_overlap(queue, st_dict[i], start, end, ess_util[i], (unsigned)3);
                    get_rating(ess_util[i], ess_rating[i], (unsigned)3);
                }
            }
        }

        // Enumerate all combination of removals, see which is best
        unsigned lowest_rating = 0xFFFFFFFF;
        unsigned best_prk = 0, best_ess = 0, best_slot = 0;
        for (unsigned prk = 0; prk < 10; prk++) { // each parking slot
            for (unsigned ess = 0; ess < 3; ess++) { // each type of essential pairs
                for (unsigned slot = 0; slot < 3; slot++) { // each essential slot
                    assert(prk_util[prk]->size > 0 || ess_util[ess][slot]->size > 0);

                    unsigned cur_rating = 0; 
                    if (parking) cur_rating += prk_rating[prk];
                    if (essential) cur_rating += ess_rating[ess][slot];
                    assert(cur_rating > 0);

                    if (lowest_rating > cur_rating) {
                        lowest_rating = cur_rating;
                        best_prk = prk;
                        best_ess = ess;
                        best_slot = slot;
                    }
                }
            }
        }

        Vector* tmp = malloc(sizeof(Vector));
        vector_init(tmp);
        vector_add(tmp, req);
        unsigned this_rating = calc_rating(tmp);
        vector_free(tmp);
        free(tmp);

        if (lowest_rating < this_rating) { // can replace
            // remove parkign slot
            for (int i = 0; i < prk_util[best_prk]->size; i++) {
                Request* tmp_req = &(prk_util[best_prk]->data[i]);
                int segl = tmp_req->start;
                int segr = segl + tmp_req->duration - 1;
                segtree_range_set(tracker->park, (unsigned)best_prk, segl, segr, 0);
            }
            // remove essential slot
            for (int i = 0; i < ess_util[best_ess][best_slot]->size; i++) {
                Request* tmp_req = &ess_util[best_ess][best_slot]->data[i];
                int segl = tmp_req->start;
                int segr = segl + tmp_req->duration - 1;
                segtree_range_set(st_dict[best_ess], (unsigned)best_slot, segl, segr, 0);
            }

            assert(try_put(idx, start, end, parking, essential, tracker));
            printf("The OPTI scheduler has [accepted] the request.\n");
            is_accepted[idx - 1] = true;
        } else {
            printf("The OPTI scheduler has [rejected] the request.\n");
        }


        /* FREE MEMORY */
        for (int i = 0; i < 10; i++) {
            vector_free(prk_util[i]);
            free(prk_util[i]);
        }
        
        for (int i = 0; i < 3; i++) {
            for (int j = 0; j < 3; j++) {
                vector_free(ess_util[i][j]);
                free(ess_util[i][j]);
            }
        }
    }

    /* PUT RESULT INTO STATS */
    for (int i = 0; i < queue_cpy->size; i++) {
        Request tmp_req = queue_cpy->data[i];
        if (is_accepted[i]) vector_add(&stats->accepted, tmp_req);
        else vector_add(&stats->rejected, tmp_req);
    }

    vector_free(queue_cpy);
    free(queue_cpy);

    printf("DEBUG: The OPTI scheduler is finished.\n");
}

/**
 * The method to rate the requests occupying the parking slots.
 * We want to keep the requests with higher rating, and replace the requests with lower rating.
 * 
 * @param queue The original queue
 * @param occupy Index of the requests occupying the parking slots
 * @param rating Where we store the calculated rating of each request
 * @param K How many resources we have
 * @attention occupy[k][0] and occupy[k][1] are the index of the requests to the original queue, that is occupying the k-th resource.
 * @attention rating[k] is the combined rating of the requests occupying the k-th resource (since we will replace both of them together, might as well combine the rating)
 */
static void get_rating(Vector* occupy[], unsigned rating[], const unsigned K) {
    for (unsigned k = 0; k < K; k++) {
        if (occupy[k]->size) {
            rating[k] = calc_rating(occupy[k]);
        }
    }
}

// rating function 1: uses utilization as measure
static unsigned calc_rating(Vector* reqs) {
    unsigned rating = 0;

    for (int i = 0; i < reqs->size; i++) {
        Request* req = &reqs->data[i];
        unsigned occupy_cnt = req->parking ? 1 : 0;
        char essentials = req->essential;
        while (essentials) {
            occupy_cnt += (unsigned)(essentials & 0b1);
            essentials >>= 1;
        }
        rating += occupy_cnt * (unsigned)req->duration;
    }
    
    return rating;
}

// Find the requests occupying segment [l, r], at most two requests
// Store the index for each tree (remember to - 1) in `results`
// -1 means no segments occupying. other values are index of `queue`
static void find_overlap (Vector* queue_og, SegTree* st, int l, int r, Vector* overlaps[], const unsigned K) {

    bool done = false;

    // find all overlapping segments for each of the K resources
    while (!done) {
        done = true;
        int results[K];
        segtree_range_query(st, l, r, results);

        for (unsigned k = 0; k < K; k++) {
            if (results[k] != 0) {
                done = false;
                int idx = results[k] - 1;
                Request req = queue_og->data[idx];
                int segl = req.start;
                int segr = segl + req.duration - 1;
                vector_add(overlaps[k], req);

                // temporarily remove overlapping segment
                // so we can search for other overlapping segments
                segtree_range_set(st, k, segl, segr, 0); 
            }
        }
    }

    // put the removed segments back
    for (unsigned k = 0; k < K; k++) {
        for (int i = 0; i < overlaps[k]->size; i++) {
            Request* req = &overlaps[k]->data[i];
            int segl = req->start;
            int segr = segl + req->duration - 1;
            int idx = req->order;
            segtree_range_set(st, k, segl, segr, idx);
        }
    }
}

// Try to response a request.
// This function will process both parking request and essential request(s).
static bool try_put(int idx, int start, int end, bool parking, char essential, Tracker* tracker) {

    /* TRY PARKING */

    int prk_slot = -1, ess_slot[3];
    for (int i = 0; i < 3; i++) ess_slot[i] = -1;

    if (parking) { // needs parking
        int query_res[10];
        segtree_range_query(tracker->park, start, end, query_res);
        for (int i = 0; i < 10; i++) {
            if (query_res[i] == 0) {
                // the segment tree is set to the index of the queue. 
                // This allows us to know which request is occupying the parking slot during printBooking.
                printf("DEBUG: Parking slot %d is FREE!\n", i);
                prk_slot = i;
                break;
            }
            if (i == 9) return false;
        }
    }

    /* TRY ESSENTIALS */

    const SegTree* st_dict[3] = {
        tracker->bc,
        tracker->lu,
        tracker->vi
    }

    if (essential) {
        int query_res[3];
        for (int i = 0; i < 3; i++) {
            char mask = 1 << (2 - i);
            if (essential & mask) { // needs essential
                segtree_range_query(st_dict[i], start, end, query_res);
                for (int j = 0; j < 3; j++) {
                    if (query_res[j] == 0) {
                        printf("DEBUG: Essential %d slot %d is FREE!\n", i, j);
                        ess_slot[i] = j;
                        break;
                    }
                    if (j == 2) return false;
                }
            }
        }
    }
    
    if (prk_slot != -1) {
        segtree_range_set(st, prk_slot, start, end, idx);
    }

    for (int i = 0; i < 3; i++) {
        if (ess_slot[i] != -1) {
            segtree_range_set(st, ess_slot[i], start, end, idx);
        }
    }
    return true;
}
