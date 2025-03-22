#ifndef VECTOR_H
#define VECTOR_H

#include <stdbool.h>
#include <stdlib.h>

#include "input.h"

typedef struct Vector {
    Request *data;
    int size;
    int capacity;
    int next;
} Vector;

void vector_init(Vector *vec);
void vector_add(Vector *vec, Request req);
void vector_free(Vector *vec);

/**
 * Sort the [l, r] segment of the vector using quick sort algorithm. l, r are both inclusive
 * @param vec The vector to be sorted
 * @param l The left bound of the segment (inclusive)
 * @param r The right bound of the segment (inclusive)
 * @param cmp The comparison function
 */
void vector_qsort(Vector *vec, int l, int r, int (*cmp)(const void*, const void*));

int cmp_request(const void *a, const void *b);
int cmp_priority(const void *a, const void *b);
int cmp_duration(const void *a, const void *b);

#endif