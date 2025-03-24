//
// COMP2432 Operating Systems Group Project
// SPMS - Smart Parking Management System
// Project Pica Magpie [https://en.wikipedia.org/wiki/Magpie]
//
// WANG Yuqi
// YANG Xikun
// LIU Siyuan
// JIN Yixiao
//

#include "input.h"
#include "output.h"
#include "scheduler.h"
#include "state.h"

#include <stdlib.h>
#include <stdbool.h>

int main() {
    int invalid_cnt = 0;
    Statistics* stats[3];   // fcfs, prio, opti
    Vector* queues[3];      // fcfs, prio, opti
    Tracker* trackers[3];   // fcfs, prio, opti

    for (int i = 0; i < 3; i++) {
        stats[i] = malloc(sizeof(Statistics));
        queues[i] = malloc(sizeof(Vector));
        trackers[i] = malloc(sizeof(Tracker));
        init_statistics(stats[i]);
        vector_init(queues[i]);
        init_tracker(trackers[i]);
    }

    printf("~~ WELCOME TO PolyU ~~\n");

    while (true) {
        Request req = fetch_input();
        
        switch (req.type) {
            case BATCH: {
                bool is_termination = process_batch(queues, &req, stats, trackers, &invalid_cnt);
                if (is_termination) {
                    req.type = TERMINATE;
                }
            }
            case TERMINATE:
                printf("Bye!");
                return EXIT_SUCCESS;
            case PRINT:
                print_bookings(req.algo, queues, stats, trackers, &invalid_cnt);
                break;
            case REQUEST:
                process_request(queues, &req);
                break;
            case INVALID:
                invalid_cnt++;
                break;
            default:
                break;
        }
    }

    return EXIT_SUCCESS;
}
