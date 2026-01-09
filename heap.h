#include <stddef.h>

// heap data structure
typedef void heap;

size_t heap_length(heap *h);
size_t heap_capacity(heap *h);
size_t heap_elem_size(heap *h);

void *heap_get_element(int idx, heap *h);
void heap_insert(void *value, heap *h);
void *heap_pop(heap *h);
heap *new_heap(void **ptr, size_t nel, size_t cap, int (* compare)(const void *v1, const void *v2));
void free_heap(heap *h);
