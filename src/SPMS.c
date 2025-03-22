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
#include "utils.h"
#include "scheduler.h"
#include "state.h"
#include "analyzer.h"

#include <unistd.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <sys/select.h>
#include <signal.h>
#include <stdbool.h>

int main() {
    bool is_running = true; 
    
    int invalid_cnt = 0;
    Statistics* stats[3]; // fcfs, prio, opti
    Vector* queues[3]; // fcfs, prio, opti
    Tracker* trackers[3]; // fcfs, prio, opti
    Request req;

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
        req = fetch_input();
        
        switch (req.type) {
            case PRINT:
                print_bookings(queues, stats, trackers, &invalid_cnt);
                break;
            case BATCH:
                is_running = process_batch(queues, &req, stats, trackers, &invalid_cnt);
                break;
            case NORMAL:
                is_running = process_request(queues, &req);
                break;
            case INVALID:
                invalid_cnt++;
                break;
        }

        if (!is_running) break;
    }
    return 0;
}