#include <assert.h>
#include <stdarg.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "logging.h"

typedef struct heap {
  size_t len, capacity, elemSize;
  void **elems; 
  
  // v1 < v2 -> -1, 
  // v1 = v2 -> 0
  // v1 > v2 -> 1
  int (* compare)(const void *, const void *);
} heap;

size_t heap_length(heap *h) {
  return h->len;
}

size_t heap_capacity(heap *h) {
  return h->capacity;
}

size_t heap_elem_size(heap *h) {
  return h->elemSize;
}

void *heap_get_element(size_t idx, heap *h) {
  if (idx >= h->len) {
    return NULL;
  }

  return *(h->elems + idx);
}


// heap left child
void *hlc(size_t idx, heap *h) {
  return heap_get_element(2 * idx, h);
}

// heap right child
void *hrc(size_t idx, heap *h) {
  return heap_get_element(2 * idx + 1, h);
}

// heap parent
void *hp(size_t idx, heap *h) {
  return heap_get_element(idx / 2, h);
}

// heap swap
void heap_swap(size_t idx1, size_t idx2, heap *h) {
  void *temp = *((h->elems) + idx1);
  *((h->elems) + idx1) = *((h->elems) + idx2);
  *((h->elems) + idx2) = temp;
}

// append an element to the end of a heap
void heap_append(void *elem, heap *h) {
  if (h->len == h->capacity) {
    log_fatal("Heap capacity reached");
  }
  h->elems[h->len] = elem;
  h->len++;
}

// max heap
void heapify(size_t idx, heap *h) {
  size_t largest = idx;
  size_t l = 2 * idx;
  size_t r = 2 * idx + 1;
  if (l < h->len && h->compare(heap_get_element(l,h), heap_get_element(largest,h)) > 0) {
    largest = l;
  } 

  if (r < h->len && h->compare(heap_get_element(r, h), heap_get_element(largest, h)) > 0) {
    largest = r; 
  }

  if (largest != idx) {
    heap_swap(idx, largest, h);
    heapify(largest, h);
  }
}

void reverse_heapify(size_t idx, heap *h) {
  size_t parent = idx / 2;
  if (idx > 0 && h->compare(heap_get_element(idx, h), heap_get_element(parent, h)) > 0) {
    heap_swap(idx, parent, h); 
    reverse_heapify(parent, h);
  }
}

void heap_insert(void *value, heap *h) {
  heap_append(value, h);
  reverse_heapify(h->len-1, h);
}

void *heap_pop(heap *h) {
  if (h->len == 0) {
    log_fatal("Heap underflow error.");
  }

  void *max = *(h->elems);
  heap_swap(0, h->len - 1, h);
  h->len--;
  heapify(0, h);
  return max;
}

void hinit(heap *h) {
  if (h->len <= 0) {
    return;
  }

  for (long i = (h->len - 1) / 2; i >= 0; i--){
    heapify(i, h);
  }
}


heap *new_heap(void **ptr, size_t nel, size_t cap, int (* compare)(const void *v1, const void *v2)) {
  heap *h = malloc(sizeof(heap)); 
  h->elems = calloc(cap, sizeof(void *));
  h->compare = compare;
  h->len = nel;
  h->capacity = cap;

  if (ptr != NULL) {
    memcpy(h->elems, ptr, cap * sizeof(void *));
  }

  hinit(h);

  return h;
}

void free_heap(heap *h) {
  free(h->elems); 
  free(h);
}
