#include "vector.h"
#include <assert.h>
#include <stdlib.h>

#define INIT_CAPACITY 20

void vector_init(Vector *vec) {
    vec->data = malloc(INIT_CAPACITY * sizeof(Request));
    vec->capacity = INIT_CAPACITY;
    vec->size = 0;
    vec->next = 0;
}

void vector_add(Vector *vec, Request req) {
    if (vec->size >= vec->capacity) {
        int new_capacity = vec->capacity == 0 ? 1 : vec->capacity * 2;
        Request *new_data = realloc(vec->data, (size_t)new_capacity * sizeof(Request));
        if (!new_data) exit(1);
        vec->data = new_data;
        vec->capacity = new_capacity;
    }
    vec->data[vec->size++] = req;
}

void vector_free(Vector *vec) {
    free(vec->data);
    vec->data = NULL;
    vec->size = vec->capacity = 0;
}

int cmp_request(const void *a, const void *b) {
    const Request *ra = (const Request*)a;
    const Request *rb = (const Request*)b;
    return ra->start - rb->start;
}

int cmp_priority(const void *a, const void *b) {
    const Request *ra = (const Request*)a;
    const Request *rb = (const Request*)b;
    return ra->priority - rb->priority;
}

int cmp_duration(const void *a, const void *b) {
    const Request *ra = (const Request*)a;
    const Request *rb = (const Request*)b;
    return ra->duration - rb->duration;
}

void vector_qsort(Vector *vec, int l, int r, int (*cmp)(const void*, const void*)) {
    assert (r - l + 1 >= 0);
    qsort(vec->data + l, (size_t)(r - l + 1), sizeof(Request), cmp);
}
