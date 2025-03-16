//
// utils.h
// The Utilities
//

#ifndef UTILS_H
#define UTILS_H

#include <ctype.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdlib.h>
#include <string.h>

static const char* SEP_CHARS = " \t\n\v\f\r";

static bool is_separator(const char);

/*
 * Returns the stripped string of the original string
 * Strip means to remove the prefix and suffix spacers. For example, strip("   str  ") = "str".
 */
char* strip(const char*);

/*
 * Returns the split segments of a string. The split delimiters are " ", \t, \n, \v, \f and \r.
 * For example, split(" hello world lovely-magpie") = {"hello", "world", "lovely-magpie"}.
 * This function is for the parser, where the input string should be stripped.
 * Use strip() to strip the input string before call this function.
 */
char** split(const char*, size_t*);

#endif //UTILS_H
