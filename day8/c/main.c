#include <assert.h>
#include <limits.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <stdlib.h>
#include "../../util.h"

#define INPUT_FILENAME "input.txt"

typedef struct point point;
typedef struct {
  size_t len;
  size_t capacity;
  size_t idx;
  point **points;
} circuit;

typedef struct circuit_list {
  size_t len;
  size_t capacity;
  circuit **circuits;
} circuit_list;

typedef struct point {
  long x;
  long y;
  long z;
  size_t idx;
  circuit *circ;
} point;

typedef struct point_pair {
  point *pt1, *pt2;
  long distance;
} point_pair;

void print_point(point *pt);
point *new_point(int x, int y, int z, int idx, circuit *circ) {
  point *pt = (point*)malloc(sizeof(point));
  pt->x = x;
  pt->y = y;
  pt->z = z;
  pt->idx = idx;
  pt->circ = circ;

  return pt;
}

void free_point(point *pt) {
  free(pt);
}


void print_point(point *pt) {
  printf("(%ld, %ld, %ld)", pt->x, pt->y, pt->z);
}

void print_circuit(circuit *c) {
  printf("Circuit %ld, len = %ld, cap = %ld\n", c->idx, c->len, c->capacity);
  printf("[");
  for (int i = 0; i < c->len; i++) {
    point *pt = *((c->points) + i);
    printf("%ld", pt->idx);

    if (i < c->len - 1) {
      printf(", ");
    }
  }
  printf("]\n");
}

circuit *new_circuit(int idx, int capacity) {
  circuit *c = malloc(sizeof(circuit));
  c->len = 0;
  c->capacity = capacity;
  c->points = malloc(sizeof(point*)*capacity);
  c->idx = idx;

  return c;
}

circuit_list *new_circuit_list(int capacity) {
  circuit_list *c = malloc(sizeof(circuit));
  c->len = 0;
  c->capacity = capacity;
  c->circuits = malloc(sizeof(circuit*) * capacity);

  return c;
}

void add_circuit(circuit *c, circuit_list *cl) {
  if (cl->len == cl->capacity) {
    log_fatal("Circuit list overflow");
  }
  c->idx = cl->len;
  *(cl->circuits + cl->len) = c;
  cl->len++;
}



void free_circuit(circuit *c) {
  free(c->points);
  free(c);
}

void add_pt(point *pt, circuit *c);


int contains(point *pt, circuit *c) {
  for (int i = 0; i < c->len; i++) {
    if (*(c->points + i) == pt) {
      return 1;
    }
  }

  return 0;
}



void add_pt(point *pt, circuit *c) {
  if (c->len >= c->capacity) {
    log_fatal("Invalid 'add' into circuit. Too many elements");
  }

  pt->circ = c;
  *(c->points + c->len) = pt;
  c->len++;
}



int index_pt(point *pt, circuit *c) {
  for (int i = 0; i < c->len; i++) {
    if ( *(c->points + i) == pt) {
      return i;
    }
  }

  return -1;
}



int index_circuit(circuit *c, circuit_list *cl) {
  for (int i = 0; i < cl->len; i++) {
    if ( *(cl->circuits + i) == c) {
      return i;
    }
  }

  return -1;
}



void point_swap(int idx1, int idx2, circuit *c) {
  point *temp = *(c->points + idx1);
  *(c->points + idx1) = *(c->points + idx2);
  *(c->points + idx2) = temp;
}



void circuit_swap(int idx1, int idx2, circuit_list *cl) {
  circuit *temp = *(cl->circuits + idx1);
  *(cl->circuits + idx1) = *(cl->circuits + idx2);
  *(cl->circuits + idx2) = temp;

  (*(cl->circuits + idx1))->idx = idx1;
  (*(cl->circuits + idx2))->idx = idx2;
} 



void remove_circuit(int idx, circuit_list *lc) {
  if (lc->len == 0) {
    return;
  }

  if (idx >= lc->len) {
    log_fatal("Invalid index for remove_circuit");
  }
  circuit_swap(idx, lc->len - 1, lc);
  *(lc->circuits + lc->len - 1) = NULL;
  lc->len--;
}



void remove_pt(point *pt, circuit *c) {
  if (c->len == 0) {
    printf("WARNING: tried to remove point %d from empty circuit %d\n", pt->idx, c->idx);
    return;
  }
  size_t idx = index_pt(pt, c);
  point_swap(idx, c->len - 1, c);
  *(c->points + c->len-1) = NULL;
  c->len--;
}



point *parse_line(char *line) {
  long coords[3];
  char *lasts, *endPtr;
  int idx = 0;
  
  for (char *word = strtok_r(line, ",", &lasts); word; word = strtok_r(NULL, ",", &lasts)) {
    if (idx > 2) {
      log_fatal("Read too many longs from %s\n", line);
    }

    long result = strtol(word, &endPtr, 10);
    coords[idx++] = result;
    if (*endPtr != '\0' && *endPtr != '\n') {
      log_fatal("Failed to convert %s to a long.\n", *word);
    }
  }
  
  return new_point(coords[0], coords[1], coords[2], 0, NULL);
}

int dist_compare(const void *p1, const void *p2) {
  point_pair *pair1 = (point_pair*)p1; 
  point_pair *pair2 = (point_pair*)p2; 

  if (pair1->distance > pair2->distance) {
    return -1; // inverse for min heap
  }

  if (pair1->distance < pair2->distance) {
    return 1; // inverse for min heap
  }

  return 0;
}

long sq(long x) {
  return x * x;
}

long dist(point *pt1, point *pt2) {
  return sq(pt1->x - pt2->x) + sq(pt1->y - pt2->y) + sq(pt1->z - pt2->z);
}

heap *init_dist_heap(point **points, int n) {
  heap *h = new_heap(NULL, 0, n*(n-1) / 2, dist_compare);

  for (size_t i = 0; i < n; i++) {
    for (size_t j = i+1; j < n; j++) {
      point_pair *pair = malloc(sizeof(point_pair));
      pair->pt1 = *(points + i);
      pair->pt2 = *(points + j);
      pair->distance = dist(pair->pt1, pair->pt2);
      heap_insert(pair, h);
    }
  }

  return h;
}

void merge_circuits(circuit *c1, circuit *c2, circuit_list *cl) {
  int idx1 = index_circuit(c1, cl);
  int idx2 = index_circuit(c2, cl);

  if (idx1 > idx2) {
    merge_circuits(c2, c1, cl);
    return;
  }

  if (idx1 < 0 || idx2 < 0) {
    log_fatal("Encountered a circuit which is not on the list");
  }

  for (intptr_t i = c2->len - 1; i >= 0; i--) {
    point *pt = *(c2->points + i);
    add_pt(pt, c1);
    remove_pt(pt, c2);
  }

  remove_circuit(idx2, cl);
  free_circuit(c2);
}



void handle_same_circuit(point *pt1, point *pt2, circuit_list *cl) {
  if (pt1->circ != NULL) {
    // do nothing since the two points are already part of the same circuit
    return;
  }

  // two points are not part of any circuits, 
  // so need to create a new one
  circuit *c = new_circuit(cl->len, 1024);
  add_pt(pt1, c);
  add_pt(pt2, c);
  add_circuit(c, cl);
}

void handle_sep_circuits(point *pt1, point *pt2, circuit_list *cl) {
  if (pt2->circ == NULL) {
    handle_sep_circuits(pt2, pt1, cl); // reverse order so p2 is not-NULL
    return;
  }

  if (pt1->circ == NULL) {
    // pt1 is not part of any circuits, so it must be added to pt2's
    add_pt(pt1, pt2->circ);
    return;
  }

  // pt1 and pt2 are part of different not-null circuits, so they
  // must be merged
  merge_circuits(pt1->circ, pt2->circ, cl);
}

long largest_circuit_product(circuit_list *cl) {
  long arr[3] = {0};
  for (int i = 0; i< cl->len; i++) {
    long len = cl->circuits[i]->len;
		if (len > arr[0]) {
			arr[2] = arr[1];
			arr[1] = arr[0];
			arr[0] = len;
		} else if (len > arr[1]) {
			arr[2] = arr[1];
			arr[1] = len;
		} else if (len > arr[2]) {
			arr[2] = len;
		}
  }

  return arr[0] * arr[1] * arr[2];
}

void p1() {
  circuit_list *circuits = new_circuit_list(256); // fixed number of circuits
  point *points[1024]; // fixed array of points
  heap *dHeap;

  FILE *f = fopen(INPUT_FILENAME, "r");
  if (f == NULL) {
    log_fatal("Failed to open file. %s", strerror(errno));
  }

  int nPts = 0;
  char line[256];
  while (fgets(line, 256, f) != NULL) {
    size_t l = strlen(line);
    if (line[l - 1] == '\n') {
      line[l - 1] = '\0';
    }
    points[nPts] = parse_line(line);
    points[nPts]->idx = nPts;
    nPts++;
  }

  dHeap = init_dist_heap(points, nPts);

  for (int i = 0; i < 1000; i++) {
    point_pair *shortest  = (point_pair*) heap_pop(dHeap);
    point *pt1 = shortest->pt1;
    point *pt2 = shortest->pt2;

    if (pt1->circ == pt2->circ) {
      handle_same_circuit(pt1, pt2, circuits);
    } else {
      handle_sep_circuits(pt1, pt2, circuits);
    }

  }


  long result = largest_circuit_product(circuits); 
  printf("Part one: %ld\n", result);
}

void p2() {
}

void test();
int main(int argc, char **argv) {
  // test();
  p1();

  return 0;
}

void test_add_circuit() {
  circuit *expected[10];
  circuit_list *cl = new_circuit_list(10);
  for (int i = 0; i < 10; i++) {
    circuit *c = new_circuit(i, 10);
    add_circuit(c, cl);
    expected[i] = c;
  }

  assert(cl->len == 10);
  assert(cl->circuits != NULL);

  for (int i = 0; i < cl->len; i++) {
    assert(*(cl->circuits + i) == expected[i]);
  }

}

void test_free_circuit() {
  circuit *c = new_circuit(0, 10);
  point *p0 = new_point(0, 0, 0, 0, c);
  point *p1 = new_point(1, 1, 1, 1, c);
  add_pt(p0, c);
  add_pt(p1, c);
  free_circuit(c);
}

void test_contains() {
  circuit *c = new_circuit(0, 10);
  point *p0 = new_point(0, 0, 0, 0, c);
  point *p1 = new_point(1, 1, 1, 1, c);
  point *p2 = new_point(2, 2, 2, 2, c);
  add_pt(p0, c);
  add_pt(p2, c);

  assert(contains(p0, c));
  assert(contains(p2, c));
  assert(!contains(p1, c));
  free_point(p0);
  free_point(p1);
  free_point(p2);
  free_circuit(c);
}

void test_add_pt() {
  circuit *c = new_circuit(0, 10);
  point *p0 = new_point(0, 0, 0, 0, c);
  point *p1 = new_point(1, 1, 1, 1, c);
  point *p2 = new_point(2, 2, 2, 2, c);
  point *expected[3];
  add_pt(p0, c);
  add_pt(p2, c);
  add_pt(p1, c);
  expected[0] = p0;
  expected[1] = p2;
  expected[2] = p1;

  assert(c->len == 3);

  for (int i = 0; i < 3; i++) {
    assert(*(c->points + i) == expected[i]);
  }

  free_point(p0);
  free_point(p1);
  free_point(p2);
  free_circuit(c);
}

void test_index_pt() {
  circuit *c = new_circuit(0, 10);
  point *p0 = new_point(0, 0, 0, 0, c);
  point *p1 = new_point(1, 1, 1, 1, c);
  point *p2 = new_point(2, 2, 2, 2, c);
  point *expected[3];
  add_pt(p0, c);
  add_pt(p2, c);
  add_pt(p1, c);
  expected[0] = p0;
  expected[1] = p2;
  expected[2] = p1;

  assert(c->len == 3);

  for (int i = 0; i < 3; i++) {
    assert(index_pt(expected[i], c) == i);
  }

  free_point(p0);
  free_point(p1);
  free_point(p2);
  free_circuit(c);
}

void test_index_circuit() {
  circuit_list *cl = new_circuit_list(10);
  circuit *c0 = new_circuit(0, 10);
  circuit *c1 = new_circuit(1, 10);
  circuit *c2 = new_circuit(2, 10);
  circuit *expected[3];
  add_circuit(c0, cl);
  add_circuit(c2, cl);
  add_circuit(c1, cl);
  expected[0] = c0;
  expected[1] = c2;
  expected[2] = c1;

  assert(cl->len == 3);

  for (int i = 0; i < 3; i++) {
    assert(index_circuit(expected[i], cl) == i);
  }

  free_circuit(c0);
  free_circuit(c1);
  free_circuit(c2);
  free(cl->circuits);
  free(cl);
}

void test_point_swap() {
  circuit *c = new_circuit(0, 10);
  point *p0 = new_point(0, 0, 0, 0, c);
  point *p1 = new_point(1, 1, 1, 1, c);
  point *p2 = new_point(2, 2, 2, 2, c);
  point *expected[3];
  add_pt(p0, c);
  add_pt(p1, c);
  add_pt(p2, c);
  expected[0] = p2;
  expected[1] = p1;
  expected[2] = p0;

  point_swap(0, 2, c);
  assert(c->len == 3);
  
  for (int i = 0; i < 3; i++) {
    assert( *(c->points + i) == expected[i]);
  }

  free_point(p0);
  free_point(p1);
  free_point(p2);
  free_circuit(c);
}

void test_circuit_swap() {
  circuit_list *cl = new_circuit_list(10);
  circuit *c0 = new_circuit(0, 10);
  circuit *c1 = new_circuit(1, 10);
  circuit *c2 = new_circuit(2, 10);
  circuit *expected[3];
  add_circuit(c0, cl);
  add_circuit(c2, cl);
  add_circuit(c1, cl);
  expected[0] = c1;
  expected[1] = c2;
  expected[2] = c0;

  circuit_swap(0, 2, cl);
  assert(cl->len == 3);
  
  for (int i = 0; i < 3; i++) {
    assert( *(cl->circuits + i) == expected[i]);
    assert( (*(cl->circuits + i))->idx == i);
  }

  free_circuit(c0);
  free_circuit(c1);
  free_circuit(c2);
  free(cl->circuits);
  free(cl);
}

void test_remove_circuit() {
  circuit_list *cl = new_circuit_list(10);
  circuit *c0 = new_circuit(0, 10);
  circuit *c1 = new_circuit(1, 10);
  circuit *c2 = new_circuit(2, 10);
  circuit *expected[2];
  expected[0] = c0;
  expected[1] = c2;
  add_circuit(c0, cl);
  add_circuit(c1, cl);
  add_circuit(c2, cl);

  assert(cl->len == 3);
  remove_circuit(1, cl);
  assert(expected[0] == *(cl->circuits + 0));
  assert(expected[1] == *(cl->circuits + 1));
  assert(cl->len == 2);

  free_circuit(c0);
  free_circuit(c1);
  free_circuit(c2);
  free(cl->circuits);
  free(cl);
}

void test_remove_point() {
  circuit *c = new_circuit(0, 10);
  point *p0 = new_point(0, 0, 0, 0, c);
  point *p1 = new_point(1, 1, 1, 1, c);
  point *p2 = new_point(2, 2, 2, 2, c);
  point *expected[2];
  add_pt(p0, c);
  add_pt(p2, c);
  add_pt(p1, c);
  expected[0] = p0;
  expected[1] = p1;

  assert(c->len == 3);
  remove_pt(p2, c);
  assert(c->len == 2);
  
  for (int i = 0; i < 2; i++) {
    assert( *(c->points + i) == expected[i]);
  }

  free_point(p0);
  free_point(p1);
  free_point(p2);
  free_circuit(c);
}

void test_merge_circuits(){
  circuit_list *cl = new_circuit_list(3);
  circuit *c1 = new_circuit(0, 10);
  circuit *c2 = new_circuit(1, 10);
  
  point *expected[10];
  for (int i = 0; i< 5; i++) {
    point *pt = new_point(i, i, i, i, NULL);
    expected[i] = pt;
    add_pt(pt, c1);
  }

  for (int i = 5; i< 10; i++) {
    point *pt = new_point(i, i, i, i, NULL);
    expected[i] = pt;
    add_pt(pt, c2);
  }

  add_circuit(c1, cl);
  add_circuit(c2, cl);

  merge_circuits(c1, c2, cl);
}

void test() {
  test_add_circuit();
  test_free_circuit();
  test_contains();
  test_add_pt();
  test_index_pt();
  test_index_circuit();
  test_point_swap();
  test_circuit_swap();
  test_remove_circuit();
  test_remove_point();
  test_merge_circuits();
}
