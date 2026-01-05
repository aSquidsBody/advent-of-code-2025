#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include "util.h"

typedef struct slice slice;

typedef struct slice {
  size_t len, capacity, elemSize;
  void *array;
  slice *parent;
} slice;

size_t max(size_t x, size_t y) {
  if (x < y) {
    return y;
  }

  return x;
}

size_t slice_capacity(slice *s) {
  while (s->parent != NULL) {
    s = s->parent;
  }
  return s->capacity;
}

size_t slice_length(slice *s) {
  return s->len;
}

size_t slice_elem_size(slice *s) {
  return s->elemSize;
}

slice *slice_new_empty(size_t elemSize, size_t capacity) {
  slice *s = malloc(sizeof(slice));
  s->len = 0;
  s->elemSize = elemSize;
  s->capacity = capacity;
  s->array = calloc(capacity, elemSize);

  return s;
}

slice *slice_new(void *array, size_t elemSize, size_t arrLen, size_t startIdx, size_t endIdx) {
  size_t nElems = endIdx - startIdx;
  assert(nElems <= arrLen);
  assert(elemSize > 0);
  assert(nElems > 0);
  assert(array != NULL || nElems <= 0);
  slice *s = malloc(sizeof(slice));
  s->len = nElems;
  s->capacity = arrLen - startIdx;
  s->elemSize = elemSize;
  s->array = (array + (elemSize*startIdx));

  return s;
}

slice *slice_subslice(slice *s, size_t startIdx, size_t endIdx) {
  assert(endIdx < s->len);
  assert(startIdx >= 0);
  assert(startIdx < endIdx);

  slice *sub = malloc(sizeof(slice));
  sub->len = endIdx - startIdx;
  sub->capacity = s->capacity - startIdx;
  sub->elemSize = s->elemSize;
  sub->array = (s->array + (s->elemSize * startIdx));

  return sub;
}

void *slice_elem_ptr(size_t idx, slice *s) {
  assert(idx >= 0 && idx < s->capacity);
  return (s->array + idx*(s->elemSize));
}

void *slice_get(size_t idx, slice *s) {
  return slice_elem_ptr(idx, s);
}

void slice_put(void *value, size_t idx, slice *s) {
  assert(idx < s->len);

  memcpy(slice_elem_ptr(idx, s), value, s->elemSize);
}

void slice_append(void *value, slice *s) {
  if (s->len >= s->capacity) {
    void *ptr = realloc(s->array, 2 * s->capacity * s->elemSize);
    if (ptr == NULL) {
      free(ptr);
      log_fatal("Failed to reallocate slice\n");
    }
    s->array = ptr;
    s->capacity = 2*s->capacity;
  }
  
  memcpy(slice_elem_ptr(s->len, s), value, s->elemSize);
  s->len++;
}

int main() {
  int *arr = malloc(sizeof(int) * 10);
  for (int i = 0; i < 10; i++) {
    arr[i] = i;
  }

  slice *s = slice_new(arr, sizeof(int), 10, 2, 8);
  printf("%p\n", s);
  printf("Length %ld\n", slice_length(s));
  printf("Capacity %ld\n", slice_capacity(s));
  
  int new = 110;
  slice_append(&new, s);
  slice_append(&new, s);
  slice_append(&new, s);

  size_t len = slice_length(s);
  printf("[");
  for (int i = 0; i < len; i++) {
    int val = *(int*)slice_get(i, s);
    printf(" %d", val);
  }
  printf(" ]\n");

  slice *sub = slice_subslice(s, 2, 8);
  printf("subslice [");
  for (int i = 0; i < slice_length(sub); i++) {
    int val = *(int*)slice_get(i, sub);
    printf(" %d", val);
  }
  printf(" ]\n");
  
}
