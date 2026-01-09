#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include "_logging.h"

size_t max(size_t x, size_t y) {
  if (x < y) {
    return y;
  }

  return x;
}

typedef struct slice slice;
typedef struct dynamicArray dynamicArray;
typedef struct slice {
  size_t startIdx, endIdx;
  dynamicArray *dArray;
} slice;

typedef struct dynamicArray {
  size_t arrLen, elemSize, numSlices;
  void *array;
} dynamicArray;

// copy the contents of array into a new dynamic array
dynamicArray *dynamic_array_new(void *array, size_t arrLen, size_t elemSize) {
  assert(elemSize > 0);
  assert(arrLen > 0);

  dynamicArray *d = malloc(sizeof(dynamicArray));
  d->arrLen = arrLen;
  d->elemSize = elemSize;
  d->numSlices = 0;
  d->array = calloc(arrLen, elemSize);
  if (array != NULL) {
    memcpy(d->array, array, arrLen * elemSize);
  }

  return d;
}

void *dynamic_array_elem_ptr(size_t idx, dynamicArray *d) {
  assert(idx >= 0);
  assert(idx < d->arrLen);
  
  return (d->array + d->elemSize*idx);
}

void free_dynamic_array(dynamicArray *d) {
  free(d->array);
  free(d);
}

size_t dynamic_array_get_length(dynamicArray *d) {
  assert(d != NULL);
  assert(d->arrLen > 0);
  return d->arrLen;
}

size_t dynamic_array_get_slice_count(dynamicArray *d) {
  assert(d != NULL);
  assert(d->numSlices >= 0);
  return d->numSlices;
}

void dynamic_array_increment_slice_count(dynamicArray *d) {
  assert(d != NULL);
  d->numSlices++;
  assert(d->numSlices > 0);
}

void dynamic_array_decrement_slice_count(dynamicArray *d) {
  assert(d != NULL);
  d->numSlices--;
  assert(d->numSlices >= 0);
}

void dynamic_array_reallocate(dynamicArray *d) {
  assert(d != NULL);
  assert(d->arrLen != 0);
  assert(d->elemSize != 0);
  assert(d->array != NULL);

  void *new = realloc(d->array, 2 * d->arrLen * d->elemSize);
  if (new == NULL) {
    log_fatal("Failed to reallocate dynamic array");
  }

  d->array = new;
  d->arrLen = 2 * d->arrLen;
}

size_t dynamic_array_length(dynamicArray *d) {
  assert(d != NULL);
  return d->arrLen;
}

size_t dynamic_array_elem_size(dynamicArray *d) {
  assert(d != NULL);
  return d->elemSize;
}

size_t slice_capacity(slice *s) {
  assert(s != NULL);
  assert(s->startIdx >= 0);
  assert(s->startIdx < dynamic_array_length(s->dArray));
  return dynamic_array_length(s->dArray) - s->startIdx;
}

size_t slice_length(slice *s) {
  assert(s != NULL);
  assert(s->startIdx <= s->endIdx);
  assert(s->startIdx >= 0);
  return s->endIdx - s->startIdx;
}

size_t slice_elem_size(slice *s) {
  assert(s != NULL);
  return dynamic_array_elem_size(s->dArray);
}

slice *slice_new_empty(size_t length, size_t capacity, size_t elemSize) {
  assert(length <= capacity);
  assert(capacity > 0);
  assert(elemSize > 0);

  slice *s = malloc(sizeof(slice));
  s->dArray = dynamic_array_new(NULL, capacity, elemSize);
  dynamic_array_increment_slice_count(s->dArray);
  s->startIdx = 0;
  s->endIdx = length;
  return s;
}

slice *slice_new_from_array(void *array, size_t arrLen, size_t elemSize, size_t startIdx, size_t endIdx) {
  assert(array != NULL);
  assert(arrLen > 0);
  assert(elemSize > 0);
  assert(startIdx >= 0);
  assert(endIdx > 0);
  assert(endIdx >= startIdx);
  assert(arrLen >= endIdx);

  slice *s = malloc(sizeof(slice));
  s->dArray = dynamic_array_new(array, arrLen, elemSize);
  s->startIdx = startIdx;
  s->endIdx = endIdx;

  return s;
}

slice *slice_new_subslice(slice *s, size_t startIdx, size_t endIdx) { // endIdx is not inclusive
  assert(startIdx >= 0);
  assert(endIdx <= slice_length(s));
  assert(startIdx <= endIdx);
  assert(s->dArray != NULL);
  
  slice *sub = malloc(sizeof(slice));
  sub->dArray = s->dArray;
  sub->startIdx = s->startIdx + startIdx;
  sub->endIdx = s->startIdx + endIdx;
  dynamic_array_increment_slice_count(s->dArray);

  return sub;
}

void free_slice(slice *s) {
  assert(s != NULL);
  if (dynamic_array_get_slice_count(s->dArray) == 0) {
    free_dynamic_array(s->dArray); 
  } else {
    dynamic_array_decrement_slice_count(s->dArray);
  }

  free(s);
}

void *slice_get(size_t idx, slice *s) {
  assert(idx >= 0);
  assert(idx < slice_length(s));
  assert(s->dArray != NULL);
  return dynamic_array_elem_ptr(s->startIdx + idx, s->dArray);
}

void *slice_upper_bound(slice *s) {
  assert(s != NULL);
  assert(s->dArray != NULL);
  return slice_get(slice_length(s) - 1, s) + dynamic_array_elem_size(s->dArray);
}

void slice_put(void *value, size_t idx, slice *s) {
  assert(value != NULL);
  assert(idx >= 0);
  assert(idx < s->endIdx);
  assert(s != NULL);
  assert(s->dArray != NULL);

  memcpy(
    slice_get(idx, s),
    value,
    dynamic_array_elem_size(s->dArray)
  );
}

void slice_append(void *value, slice *s) {
  assert(value != NULL);
  assert(s != NULL);

  if (slice_length(s) == slice_capacity(s)) {
    dynamic_array_reallocate(s->dArray);
  }

  memcpy(
    slice_upper_bound(s),
    value,
    dynamic_array_elem_size(s->dArray)
  );
  s->endIdx++;
}

// int main() {
//   int arr[10];
//   printf("print(arr)\n >>> [ ");
//   for (int i = 0; i < 10; i++) {
//     arr[i] = i;
//     printf(" %d", i);
//   }
//
//   printf(" ]\n\n");
//
//
//   slice *s = slice_new_from_array(arr, 10, sizeof(int), 2, 8);
//   printf("Slice s = arr[2:8]\n");
//   printf("print(s)\n");
//   printf(" >>> [");
//   for (int i = 0; i < slice_length(s); i++) {
//     int val = *(int*)slice_get(i, s);
//     printf(" %d", val);
//   }
//   printf(" ]\n\n");
//
//   printf("s.append(110)\n");
//   printf("s.append(110)\n");
//   printf("s.append(110)\n");
//   int new = 110;
//   slice_append(&new, s);
//   slice_append(&new, s);
//   slice_append(&new, s);
//
//
//   printf("print(s[0:])\n");
//   printf(" >>> [");
//   for (int i = 0; i < slice_length(s); i++) {
//     int val = *(int*)slice_get(i, s);
//     printf(" %d", val);
//   }
//   printf(" ]\n\n");
//
//   printf("sub = s[2:8]\n");
//   slice *sub = slice_new_subslice(s, 2, 8);
//   printf("print(sub)\n >>> [");
//   for (int i = 0; i < slice_length(sub); i++) {
//     int val = *(int*)slice_get(i, sub);
//     printf(" %d", val);
//   }
//   printf(" ]\n\n");
//
//
//   printf("sub[2] = 35\n");
//   int newVal = 35;
//   slice_put(&newVal, 2, sub);
//   printf("print(s)\n >>> [");
//   for (int i = 0; i < slice_length(s); i++) {
//     int val = *(int*)slice_get(i, s);
//     printf(" %d", val);
//   }
//   printf(" ]\n\n");
// }
