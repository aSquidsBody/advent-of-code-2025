#include <assert.h>
#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "hashtable.h"

static const long multiplier = 104729L;

Entry* new_entry(long value) {
  Entry *ePtr = (Entry*)malloc(sizeof(Entry));
  (*ePtr).value = value;

  return ePtr;
}

void free_entries(Entry *ePtr) {
  Entry *ptr = ePtr;
  Entry *nextPtr;
  do {
    nextPtr = (*ptr).next;
    free(ptr);
    ptr = nextPtr;

  } while(nextPtr != NULL);
}

void free_bucket_entries(long capacity, Entry **bucketArr) {
  for (long idx = 0; idx < capacity; idx++) {
    Entry *ePtr = bucketArr[idx];
    if (ePtr != NULL) {
      free_entries(ePtr);
    }
  }
}

void free_hash_table(HashTable *tPtr) {
  Entry **bucketArr = (*tPtr).buckets;
  if (bucketArr != NULL) {
    free_bucket_entries((*tPtr).capacity, bucketArr);
  }

  free(bucketArr);

  free(tPtr);
}



long hash_key(long value, long numBuckets) {
  return (value * multiplier) % numBuckets;
}

// return 0 if not added (already there), return 1 if added
int add(HashTable *tPtr, long value) {
  long key = hash_key(value, (*tPtr).capacity);

  Entry **buckets = (*tPtr).buckets;
  if (buckets[key] == NULL) {
    buckets[key] = new_entry(value);
    return 1;
  } else {

    // traverse to end of chain
    Entry *ePtr = buckets[key];
    while(1==1) {
      // two break conditions
      if ((*ePtr).value == value) {
        break;
      } else if ((*ePtr).next == NULL) {
        (*ePtr).next = new_entry(value);
        return 1;
      }

      ePtr = (*ePtr).next;
    } 
  }
  
  return 0;
}

int delete(HashTable *tPtr, long value) {
  long key = hash_key(value, (*tPtr).capacity);

  if ((*tPtr).buckets[key] == NULL) {
    return 0;
  }

  Entry *ePtr = (*tPtr).buckets[key];
  // if first element is the hit, then delete it
  if ((*ePtr).value == value) {
    (*tPtr).buckets[key] = (*ePtr).next;
    free(ePtr);

    return 1;
  }

  while ((*ePtr).next != NULL) {
    if ((*(*ePtr).next).value == value) {
      Entry *nextPtr = (*ePtr).next;
      (*ePtr).next = (*nextPtr).next;
      free(nextPtr);
      return 1;
    }

    ePtr = (*ePtr).next;
  }

  return 0;
}


int contains(HashTable *tPtr, long value) {
  long key = hash_key(value, (*tPtr).capacity);

  if ((*tPtr).buckets[key] == NULL) {
    return 0;
  }

  Entry *ePtr = (*tPtr).buckets[key];
  while (ePtr != NULL) {
    if ((*ePtr).value == value) {
      return 1;
    }
    ePtr = (*ePtr).next;
  }

  return 0;
}


HashTable *new_hash_table(long capacity) {
  HashTable *tPtr = (HashTable*)malloc(sizeof(HashTable));
  (*tPtr).capacity = capacity;
  (*tPtr).buckets = (Entry**)calloc(capacity, sizeof(Entry*));

  return tPtr;
}


void test_new_hash_table() {
  long capacity = 100L;
  HashTable *tPtr = new_hash_table(capacity);

  assert((*tPtr).buckets != NULL);

  free_hash_table(tPtr);
  tPtr = NULL;
}

void test_hash_table() {
  long capacity = 100L;
  HashTable *tPtr = new_hash_table(capacity);

  assert((*tPtr).buckets != NULL);

  assert(add(tPtr, 50L) == 1);
  assert(add(tPtr, 51L) == 1);
  assert(add(tPtr, 50L + capacity) == 1);
  assert(add(tPtr, 50L) == 0);
  for (long l = 0; l < capacity; l++) {
    if (l == 50L || l == 51L) {
      assert(contains(tPtr, l) == 1);
    } else {
      assert(contains(tPtr, l) == 0);
    }
  }

  long key50 = hash_key(50L, capacity);
  long key51 = hash_key(51L, capacity);
  Entry **bucketPtr = (*tPtr).buckets;
  assert((*bucketPtr[key50]).value = 50L);
  assert((*(*bucketPtr[key50]).next).value == 50L + capacity);
  assert((*(*bucketPtr[key50]).next).next == NULL);
  assert((*bucketPtr[key51]).value == 51L);
  assert((*bucketPtr[key51]).next == NULL);


  assert(delete(tPtr, 50L) == 1);
  assert((*bucketPtr[key50]).value == 50L + capacity);
  assert((*bucketPtr[key50]).next == NULL);
  assert(delete(tPtr, 50L) == 0);
  assert((*bucketPtr[key50]).value == 50L + capacity);
  assert((*bucketPtr[key50]).next == NULL);
  assert(delete(tPtr, 50L + capacity));
  assert(bucketPtr[50] == NULL);

  free_hash_table(tPtr);
  tPtr = NULL;
}
