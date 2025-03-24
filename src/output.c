//
// output.c
// The Output Module (Part III)
//

#include "state.h"
#include "vector.h"
#include "output.h"

// In this project, we assume there are only five members.
// See the instruction document, Page 6.
#define MEMBERS_CNT  5

#define DATE_LEN    12
#define START_LEN   8
#define END_LEN     8
#define TYPE_LEN    13
#define DEVICE_LEN  29

// Adjust the string into a fixed size of `fixed_len`.
// call free() when the adjusted string is out of use!
char* adjust_string(const char* str, size_t fixed_len) {
    char *result = malloc(fixed_len + 1);
    if (result == NULL) {
        perror("failed to allocate memory");
        return NULL;
    }

    size_t len = strlen(str);

    if (len < fixed_len) { // copy and padding
        strncpy(result, str, len);
        for (size_t i = len; i < fixed_len; i++) {
            result[i] = ' ';
        }
    } else { // copy and cut
        strncpy(result, str, fixed_len);
    }

    result[fixed_len] = '\0';
    return result;
}


static void print_header() {
    printf("Date         Start    End      Type          Device                        \n");
    printf("===========================================================================\n");
}

static void print_divider() {
    printf("\n... ... ... ... ... ... ... ... ... ... ... ... ... ... ... ... ... ... ...\n\n");
}

static void print_new_device_line() {
    printf("                                             ");
}

static void print_no_record() {
    printf("   No record for this member.\n");
}

static void print_end() {
    printf("   - End -\n\n");
    printf("===========================================================================\n\n");
}


// quite weird here.
char* get_request_typename(const Request* req) {
    switch (req->priority) {
        case 0:
            return "Event";
        case 1:
            return "Reservation";
        case 2:
            return "Parking";
        case 3:
            return "*";
        default:
            return "(Error)";
    }
}

// quite weird here.
// For simplicity, we chose not to apply all OOP principles in this project.
char* get_date_string(const Request* req) {
    const int total_days = req->start / (24 * 60);
    const int day = 10 + total_days;

    char* date_string = malloc(11);
    sprintf(date_string, "2025-05-%02d", day);
    return date_string;
}

// quite weird here.
// For simplicity, we chose not to apply all OOP principles in this project.
char* get_start_string(const Request* req) {
    const int remaining_minutes = req->start % (24 * 60);
    const int hour = remaining_minutes / 60;
    const int minute = remaining_minutes % 60;

    char* start_string = malloc(6);
    sprintf(start_string, "%02d:%02d", hour, minute);
    return start_string;
}

char* get_end_string(const Request* req) {
    const int remaining_minutes = (req->start + req->duration) % (24 * 60);
    const int hour = remaining_minutes / 60;
    const int minute = remaining_minutes % 60;

    char* start_string = malloc(6);
    sprintf(start_string, "%02d:%02d", hour, minute);
    return start_string;
}


/**
 * This function prints the requests of each member.
 * @param member_name The name of the member (not the full name, only 'A', 'B', 'C', 'D', or 'E').
 * @param stat_vector The state vector. This can be only stat->accepted or stat->rejected.
 * @param records_cnt The records made by the member.
 * @param print Print the schedule tables or calculate records_cnt only.
 */
void
process_member(const char member_name, const Vector* stat_vector, int* records_cnt, const bool print) {
    for (int j = 0; j < stat_vector->size; j++) {
        const Request* curreq = &stat_vector->data[j];

        if (curreq->member == member_name) {

            if (!print) {
                (*records_cnt)++;
                continue;
            }

            if ((*records_cnt) == 0) {
                print_header();
            }

            (*records_cnt)++;

            char* date_string       = get_date_string(curreq);
            char* start_string      = get_start_string(curreq);
            char* end_string        = get_end_string(curreq);

            char* date_adjusted     = adjust_string(date_string, DATE_LEN);
            char* start_adjusted    = adjust_string(get_start_string(curreq), START_LEN);
            char* end_adjusted      = adjust_string(get_end_string(curreq), END_LEN);
            char* type_adjusted     = adjust_string(get_request_typename(curreq), TYPE_LEN);

            printf("%s ", date_adjusted);
            printf("%s ", start_adjusted);
            printf("%s ", end_adjusted);
            printf("%s ", type_adjusted);

            free(date_adjusted);
            free(start_adjusted);
            free(end_adjusted);
            free(type_adjusted);

            free(date_string);
            free(start_string);
            free(end_string);


            // Print the devices

            bool is_battery_cable                       = curreq->essential & 0b100;
            bool is_locker_umbrella                     = curreq->essential & 0b010;
            bool is_inflationservice_valetparking       = curreq->essential & 0b001;

            int essential_cnt = 0;
            char *essential_names[6];

            if (is_battery_cable) {
                essential_names[essential_cnt++] = "Battery";
                essential_names[essential_cnt++] = "Cable";
            }

            if (is_locker_umbrella) {
                essential_names[essential_cnt++] = "Locker";
                essential_names[essential_cnt++] = "Umbrella";
            }

            if (is_inflationservice_valetparking) {
                essential_names[essential_cnt++] = "Inflation Service";
                essential_names[essential_cnt++] = "Valet Parking";
            }

            if (!essential_cnt) {
                char* asterisk_adjusted = adjust_string("*", DEVICE_LEN);
                printf("%s \n", asterisk_adjusted);
                free(asterisk_adjusted);
            } else {
                char* essential_name_adjusted = adjust_string(essential_names[0], DEVICE_LEN);
                printf("%s \n", essential_name_adjusted);
                free(essential_name_adjusted);

                for (int k = 1; k < essential_cnt; k++) {
                    print_new_device_line();
                    essential_name_adjusted = adjust_string(essential_names[k], DEVICE_LEN);
                    printf("%s \n", essential_name_adjusted);
                    free(essential_name_adjusted);
                }
            }
        }
    }
}


/**
 *
 * @param algo_name The name of the scheduling algorithm. 'fcfs', 'prio' and 'opti' are available now.
 * @param queue The processing queue. TODO: need queue here?
 * @param stat The statistics data.
 * @param tracker The trackers. TODO: need tracker here?
 * @param invalid_cnt Invalid requests. TODO: need invalid_cnt here?
 */
void
print_booking(char* algo_name, Statistics* stat) {

    printf("*** Parking Booking - ACCEPTED / %s ***\n\n", algo_name);

    for (int i = 0; i < MEMBERS_CNT; i++) {
        const char member_name = 'A' + i;
        printf("Member_%c has the following bookings:\n", member_name);

        int records_cnt = 0;
        process_member(member_name, &stat->accepted, &records_cnt, true);

        if (!records_cnt) {
            print_no_record();
        }

        if (i < MEMBERS_CNT - 1) {
            print_divider();
        }
    }


    printf("\n*** Parking Booking - REJECTED / FCFS ***\n\n");

    for (int i = 0; i < MEMBERS_CNT; i++) {
        const char member_name = 'A' + i;

        int records_cnt = 0;
        process_member(member_name, &stat->rejected, &records_cnt, false);

        printf("Member_%c (there are %d bookings rejected):\n", member_name, records_cnt);
        records_cnt = 0;

        process_member(member_name, &stat->rejected, &records_cnt, true);

        if (!records_cnt) {
            print_no_record();
        }

        if (i < MEMBERS_CNT - 1) {
            print_divider();
        }
    }

    print_end();
}


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
        print_booking("FCFS", stats[0]);
    }

    if (is_prio) {
        print_booking("PRIO", stats[1]);
    }

    if (is_opti) {
        print_booking("OPTI", stats[2]);
    }

    if (is_all) {
        // TODO: print the summary report
    }

    // To suppress warnings
    (void)queues;
    (void)trackers;
    (void)invalid_cnt;
}
