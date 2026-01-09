#include <stddef.h>

typedef void slice;
typedef void dynamicArray;

size_t slice_capacity(slice *s);

size_t slice_length(slice *s); 

size_t slice_elem_size(slice *s);

// create a slice with an empty backing array.
slice *slice_new_empty(size_t length, size_t capacity, size_t elemSize);

// create a slice out of an array. The array contents are copied to a new dynamic array. startIdx is inclusive, endIdx is exclusive
slice *slice_new_from_array(void *array, size_t arrLen, size_t elemSize, size_t startIdx, size_t endIdx); 

// create a subslice out of an existing slice. startIdx is inclusive, endIdx is exclusive
slice *slice_new_subslice(slice *s, size_t startIdx, size_t endIdx);

void free_slice(slice *s);

void *slice_get(size_t idx, slice *s); 

void *slice_upper_bound(slice *s); 

void slice_put(void *value, size_t idx, slice *s); 

void slice_append(void *value, slice *s); 
