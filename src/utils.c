//
// utils.c
// The Utilities
//

#include "utils.h"

/* GENERIC STRING MANIPULATION */

// using isspace() to cover all delimiters (' ', \f, \t, \n, \r, \v, etc.)
void strip_no_semicolon(char* str) {
    if (str == NULL || *str == '\0') return;

    char* start = str;
    while (isspace(*start)) {
        start++;
    }

    char* end = str + strlen(str) - 1;
    while (end > start && isspace(*end) && *end != ';') {
        end--;
    }
    if (*end == ';') end--;

    size_t len = end - start + 1;
    memmove(start, start, len);
    str[len] = '\0';
}

char** split(const char* str) {
    char** res = (char**)malloc(sizeof(char*) * 8);
    for (int i = 0; i < 8; i++) {
        res[i] = (char*)malloc(sizeof(char) * 100);
        memset(res[i], 0, sizeof(char) * 100);
    }
    int param_i = 0, j = 0;
    for (int i = 0; str[i]; i++) {
        if (str[i] == ' ') {
            if (param_i >= 8) break;
            else param_i++, j = 0;
            continue;
        }
        res[param_i][j++] = str[i];
    }
    return res;
}

[[deprecated("Use strip_no_semicolon() instead")]]
void strip(char* str) {
    int n = (int)strlen(str);
    printf("stripping \"%s\"\n", str);
    int i = 0, pre = 0;
    while (str[pre] == ' ') pre++;
    for (i = pre; i < n && str[i] != ';'; i++) {
        str[i - pre] = str[i];
    }
    str[i - pre] = '\0';
}

bool compare(const char* str1, const char* str2) {
    for (int i = 0; str1[i] == str2[i]; i++) {
        if (str1[i] == 0) return true;
    }
    return false;
}


/* INPUT PARSING */

int parse_time(const char* date, const char* time) {
    // date format: YYYY-MM-DD
    // time format: hh:mm
    // return the time in minutes
    // error handling: return -1 if 
    //      1. the date         const char ccc[] = tokens[6];
    //      2. wrong format, 
    //      3. or not within range of 2025-05-10 ~ 2025-05-16

    int year, month, day, hour, minute;
    
    if (sscanf(date, "%d-%d-%d", &year, &month, &day) != 3 ||
        sscanf(time, "%d:%d", &hour, &minute) != 2)
        return -1;
    
    if (year != 2025 || month != 5 || day < 10 || day > 16 ||
        hour < 0 || hour > 23 || minute < 0 || minute > 59)
        return -1;
    
    return ((day - 10) * 24 + hour) * 60 + minute;
}

int parse_duration(const char* duration) {
    // duration format: n.n
    // return the duration in minutes
    // error handling: return -1 if
    //      1. the duration is invalid
    //      2. wrong format

    float hours;
    if (sscanf(duration, "%f", &hours) != 1) return -1;
    if (hours <= 0) return -1;
    return (int)(hours * 60);
}

char parse_member(const char* member) {
    // member format: '-member_A', '-member_B', '-member_C', '-member_D', '-member_E'
    // return the member 'A', 'B', 'C' ... in char
    // error handling: return 0 if
    //      1. member other than A, B, C, D, E.
    //      2. wrong format (Expected format: "memberX" where X is A-E)

    if (strlen(member) != 9 || strncmp(member, "-member_", 8) != 0) return 0;
    char member_char = member[8];
    printf("DEBUG: member_char: %c\n", member_char);
    if (member_char < 'A' || member_char > 'E' || member[9] != '\0') return 0;
    return member_char;

}

bool is_valid_essentials_pair(const char* bbb, const char* ccc) {
    // bbb, ccc are essential items
    // return true if they are valid paired, false otherwise
    // valid pairs are: 
    // [battery]+[cables] or [locker]+[umbrella] or [InflationService] + [valetPark]

    const char* valid_pair = get_valid_pair(bbb);
    if (valid_pair == NULL) return false;
    return compare(valid_pair, ccc);
}

const char* get_valid_pair(const char* essential) {
    // essential is one of the essential items
    // return the paired essential item
    // return NULL if the essential item is invalid

    if (compare(essential, "battery")) return "cables";
    if (compare(essential, "cables")) return "battery";
    if (compare(essential, "locker")) return "umbrella";
    if (compare(essential, "umbrella")) return "locker";
    if (compare(essential, "InflationService")) return "valetPark";
    if (compare(essential, "valetPark")) return "InflationService";
    return NULL;
}

void add_essential_value(char* original_code, const char* essential) {
    // struct Request object uses binary to represent the requested essential items
    // It uses 3 bit binary, each bit represetend [battery + cable], [locker + umbrella], [inflation service + valet parking] respectively
    // E.g., 0b011 = (battery + cable) + (inflation + valet);
    // this function updates the binary code based on the given

    if (compare(essential, "battery") || compare(essential, "cables")) *original_code |= 0b100;
    if (compare(essential, "locker") || compare(essential, "umbrella")) *original_code |= 0b010;
    if (compare(essential, "InflationService") || compare(essential, "valetPark")) *original_code |= 0b001;
}

int get_priority(const char* type) {
    // priority: Event > Reservation > Parking > Essentials
    // use convention: priority value smaller is higher priority
    // return the priority value based on the given type

    if (compare(type, "addEvent")) return 0;
    if (compare(type, "addReservation")) return 1;
    if (compare(type, "addParking")) return 2;
    if (compare(type, "bookEssentials")) return 3;
    return 4;
}


/*
char** split1(const char* input, size_t* argc) {
    if (input == NULL || *input == '\0') {
        return NULL;
    }

    *argc = 0;
    char* _input = strdup(input);
    char* _input_orig = _input;  // save a copy of the strdup-ed string. In the end of this func, free _input_orig.

    if (_input == NULL) {
        return NULL;
    }

    size_t len = strlen(_input);

    bool in_arg = false;
    for (int i = 0; i < len; i++) {
        if (is_separator(_input[i])) {
            in_arg = false;
        } else if (!in_arg) { // the beginning of a new argument
            in_arg = true;
            (*argc)++;
        }
    }

    char** result = (char**)malloc(sizeof(char*) * (*argc));

    if (result == NULL) {
        return NULL;
    }

    int pargc = 0; // processed argument count
    char* delimiters = strtok(_input, SEP_CHARS);

    while (delimiters != NULL) {
        result[pargc++] = strdup(delimiters);
        delimiters = strtok(NULL, SEP_CHARS);
    }

    free(_input_orig);
    return result;
}

char* strip(const char* str) {
    if (str == NULL || *str == '\0') {
        return NULL;
    }

    char* _str = strdup(str);
    size_t len = strlen(_str);

    // strip the head and add a new head
    int start = 0;
    while (start < len && is_separator(_str[start])) {
        start++;
    }

    if (start == len) {
        free(_str);
        return NULL;
    }

    // strip the end and add a new end
    int end = (int)len - 1;
    while (end >= start && is_separator(_str[end])) {
        end--;
    }

    _str[end + 1] = '\0';
    return _str;
}

*/

/*
static bool is_separator(const char c) {
    for (int i = 0; SEP_CHARS[i] != '\0'; i++) {
        if (c == SEP_CHARS[i]) {
            return true;
        }
    }
    return false;
}
*/
