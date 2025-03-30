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
#include "rng.h"

#include <stdlib.h>
#include <stdbool.h>
#include <unistd.h>
#include <time.h>


int main() {
    int invalid_cnt = 0;
    seed(1389ULL, 74135ULL);
    for (int i = 0; i < 10; i++) printf("%lf\n", randd());

    Vector* queue = malloc(sizeof(Vector)); 
    vector_init(queue);

    printf("~~ WELCOME TO PolyU ~~\n");

    while (true) {
        Request req = fetch_input();
        
        switch (req.type) {
            case BATCH: {
                bool is_termination = process_batch(queue, &req, &invalid_cnt);
                if (!is_termination) {
                    break;
                }
                __attribute__((fallthrough));
            }
            case TERMINATE:
                printf("Bye!");
                return EXIT_SUCCESS;
            case PRINT: {
                const pid_t pid = fork();
                if (pid < 0) {
                    perror("fork");
                    exit(EXIT_FAILURE);
                } else if (pid == 0) {
                    printf("DEBUG: A fork() has been called.\n");
                    printf("DEBUG: Here is the child process to run the schedulers and print all bookings. pid = %d.\n\n", getpid());

                    schedule_and_print_bookings(req.algo, queue, invalid_cnt);
                    exit(0);
                } else {
                    wait(NULL);
                }
                break;
            }
            case REQUEST:
                process_request(queue, &req);
                // Scheduling algorithms will be called in case PRINT. [Revision Mar 25]
                break;
            case INVALID:
                printf("The request is invalid. Please check and try again!\n");
                invalid_cnt++;
                break;
            default:
                break;
        }
    }

    return EXIT_SUCCESS;
}
