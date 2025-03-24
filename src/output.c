//
// output.c
// The Output Module (Part III)
//

#include "state.h"
#include "vector.h"
#include "output.h"

// Note in this project there are only five members
#define MEMBER_CNT  5

static inline void print_header() {
    printf("Date         Start    End      Type          Device                        \n");
    printf("===========================================================================\n");
}

static inline void print_end() {
    printf("   - End -\n\n");
    printf("===========================================================================\n");
}

// TODO: Why the function signature couldn't be 'inline void'?
void
print_booking(char* algo_name, Vector* queue, Statistics* stat, Tracker* tracker, int* invalid_cnt) {
    printf("*** Parking Booking - ACCEPTED / %s ***\n\n", algo_name);

    for (int i = 0; i < MEMBER_CNT; i++) {
        char member_name = 'A' + i;
        printf("Member_%c has the following bookings:\n\n", member_name);
        print_header();
        printf("... (Data unavailable)\n\n"); // TODO: placeholder
    }

    printf("*** Parking Booking - REJECTED / FCFS ***\n");
    print_end();
}


// TODO: Finish the Output Module
void
print_bookings(char *algo, Vector* queues[], Statistics* stats[], Tracker* trackers[], int* invalid_cnt) {

    bool is_fcfs = strcmp(algo, "fcfs") == 0 || strcmp(algo, "all") == 0;
    bool is_prio = strcmp(algo, "prio") == 0 || strcmp(algo, "all") == 0;
    bool is_opti = strcmp(algo, "opti") == 0 || strcmp(algo, "all") == 0;
    bool is_all = strcmp(algo, "all") == 0;

    if (!is_fcfs && !is_prio && !is_opti && !is_all) {
        printf("Unsupported scheduling algorithm: \"%s\".\n", algo);
    }

    if (is_fcfs) {
        print_booking("FCFS", queues[0], stats[0], trackers[0], invalid_cnt);
    }

    if (is_prio) {
        print_booking("PRIO", queues[1], stats[1], trackers[1], invalid_cnt);
    }

    if (is_opti) {
        print_booking("OPTI", queues[2], stats[2], trackers[2], invalid_cnt);
    }

    if (is_all) {
        // TODO: print the summary report
    }
}
