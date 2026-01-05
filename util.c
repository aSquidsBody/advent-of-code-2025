#include <assert.h>
#include <stdarg.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
  int value;
  char *name;
} example;

void log_fatal(char *string, ...) {
  va_list args;
  va_start(args, string);
  fprintf(stderr, "[FATAL] ");
  fprintf(stderr, string, args);
  fprintf(stderr, "\n");
  va_end(args);

  exit(1);
}

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

void *heap_get_element(int idx, heap *h) {
  if (idx >= h->len) {
    return NULL;
  }

  return *(h->elems + idx);
}


// heap left child
void *hlc(int idx, heap *h) {
  return heap_get_element(2 * idx, h);
}

// heap right child
void *hrc(int idx, heap *h) {
  return heap_get_element(2 * idx + 1, h);
}

// heap parent
void *hp(int idx, heap *h) {
  return heap_get_element(idx / 2, h);
}

// heap swap
void heap_swap(int idx1, int idx2, heap *h) {
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
void heapify(int idx, heap *h) {
  int largest = idx;
  int l = 2 * idx;
  int r = 2 * idx + 1;
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

void reverse_heapify(int idx, heap *h) {
  int parent = idx / 2;
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


int int_compare(const void *int1, const void *int2) {
  if (*(int*)int1 > *(int*)int2) {
    return 1;
  }

  if (*(int*)int1 < *(int*)int2) {
    return -1;
  }

  return 0;
}


int example_compare(const void *e1, const void *e2) {
  example v1 = *(example*) e1;
  example v2 = *(example*) e2;
  if (v1.value > v2.value) {
    return 1;
  }

  if (v1.value < v2.value) {
    return -1;
  }

  return 0;
}

example *new_example(int value, char *name) {
  example *ptr = malloc(sizeof(example));
  ptr->value = value;
  ptr->name = name;

  return ptr;
}

void print_example(example *e) {
  printf("%s %d\n", e->name, e->value);
}

int test_heap() {
  example *arr[13] = {
    new_example(17, "e8"),
    new_example(18, "e9"),
    new_example(19, "e10"),
    new_example(20, "e11"),
    new_example(10, "e1"),
    new_example(11, "e2"),
    new_example(12, "e3"),
    new_example(13, "e4"),
    new_example(14, "e5"),
    new_example(15, "e6"),
    new_example(16, "e7"),
    new_example(21, "e12"),
    new_example(22, "e13")
  };

  heap *h = new_heap(NULL, 0, 100, example_compare);

  for (int i = 0; i<13; i++) {
    printf("inserting ");
    print_example(arr[i]);

    heap_insert((void*)arr[i], h);
    
    printf("result ");
    for (int j = 0; j < h->len; j++) {
      printf("[(%d) %s] ", ((example *)(h->elems[j]))->value, ((example *)(h->elems[j]))->name);
    }
    printf("\n");
  }

  for (int i = 0; i < 13; i++) {
    example *e = (example*)heap_pop(h);
    print_example(e);
  }
  return 0;
}

void test_append() {
  heap *h = new_heap(NULL, 0, 100, example_compare);

  example *elem = new_example(10, "name10");
  example *elem2 = new_example(11, "name11");
  heap_append(elem, h);
  heap_append(elem2, h);

  long l = h->len;
  for (int i =0; i<l; i++) {
    print_example(heap_get_element(i, h));
  }

  heap_swap(0, 1, h);
  for (int i =0; i<l; i++) {
    print_example(heap_get_element(i, h));
  }
}

// int main() {
//   test_heap();
//   test_append();
//   return 0;
// }

// void heap_init(void **data, size_t nel, size_t nElems, int (* compar)(const void *, const void * )) {
//   int x = compar(NULL, NULL);
// }

