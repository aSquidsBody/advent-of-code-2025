#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <strings.h>
#include "../../util.h"

#define INPUT_FILENAME "input.txt"

typedef struct long_matrix {
  long size, nCols, nRows;
  long *data;
} LongMatrix;

typedef struct matrix {
  long size, nCols, nRows;
  char *data;
} Matrix;

char get(int r, int c, Matrix *m) {
  int dataIdx = c + r*(m->nCols);
  
  return (*m).data[dataIdx];
}

long getL(int r, int c, LongMatrix *m) {
  int dataIdx = c + r*(m->nCols);
  
  return (*m).data[dataIdx];
}

char *get_row(int r, Matrix *m) {
  int dataIdx = r*(m->nCols);
  
  return &(*m).data[dataIdx];
}

long *get_rowL(int r, LongMatrix *lm) {
  int dataIdx = r*(lm->nCols);

  return &(*lm).data[dataIdx];
}

void reset_row(int r, Matrix *m) {
  char *row = get_row(r, m);
  for (int cIdx = 0; cIdx < m->nCols; cIdx++) {
    if (row[cIdx] == '|') { row[cIdx] = '.'; }
  }
}

long filesize(FILE *f) {
  if (fseek(f, 0L, SEEK_END) != 0) {
    log_fatal("fseek err: %s\n", strerror(errno));
  }

  long offset = ftell(f);
  if (offset < 0) {
    log_fatal("ftell err: %s\n", strerror(errno));
  }

  if (fseek(f, 0L, SEEK_SET) != 0) {
    log_fatal("fseek err: %s\n", strerror(errno));
  }

  return offset;
}
long start(Matrix *m) {
  char *row = get_row(0, m);
  for (int cIdx = 0; cIdx < m->nCols; cIdx++) {
    if (row[cIdx] == 'S') {
      row[cIdx] = '|';
    }
  }

  return 0;
}


long start2(Matrix *m, LongMatrix *lm) {
  char *row = get_row(0, m);
  long *rowl = get_rowL(0, lm);
  for (int cIdx = 0; cIdx < m->nCols; cIdx++) {
    if (row[cIdx] == 'S') {
      row[cIdx] = '|';
      rowl[cIdx] = 1L;
    }
  }

  return 0;
}

void simulate2(int rIdx, Matrix *m, LongMatrix *lm) {
  char *row = get_row(rIdx, m);
  char *prev_row = get_row(rIdx - 1, m);

  long *rowl = get_rowL(rIdx, lm);
  long *prev_rowl = get_rowL(rIdx - 1, lm);

  for (int cIdx = 1; cIdx < m->nCols - 1; cIdx++) {
    if (prev_row[cIdx] == '|') {
      if (row[cIdx] == '^') {
        row[cIdx - 1] = '|';
        rowl[cIdx - 1] += prev_rowl[cIdx];
        row[cIdx + 1] = '|';
        rowl[cIdx + 1] += prev_rowl[cIdx];
      } else {
        row[cIdx] = '|';
        rowl[cIdx] += prev_rowl[cIdx];
      }
    }
  }

  // boundary conditions
  if (prev_row[0] == '|') {
    if (row[0] == '^') {
      row[1] = '|';
      rowl[1] += prev_rowl[0];
    } else {
      row[0] = '|';
      rowl[0] += prev_rowl[0];
    }
  }

  int last = m->nCols - 1;
  if (prev_row[last] == '|') {
    if (row[last] == '^') {
      row[last-1] = '|';
      rowl[last-1] += prev_rowl[last];
    } else {
      row[last] = '|';
      rowl[last] += prev_rowl[last];
    }
  }
}

long simulate(int rIdx, Matrix *m) {
  char *row = get_row(rIdx, m);
  char *prev_row = get_row(rIdx - 1, m);
  long total = 0;

  for (int cIdx = 1; cIdx < m->nCols - 1; cIdx++) {
    if (prev_row[cIdx] == '|') {
      if (row[cIdx] == '^') {
        total += prev_row[cIdx];
        row[cIdx - 1] = '|';
        row[cIdx + 1] = '|';
      } else {
        row[cIdx] = '|';
      }
    }
  }

  // boundary conditions
  if (prev_row[0] == '|') {
    if (row[0] == '^') {
      total += 1;
      row[1] = '|';
    } else {
      row[0] = '|';
    }
  }

  int last = m->nCols - 1;
  if (prev_row[last] == '|') {
    if (row[last] == '^') {
      total += 1;
      row[last-1] = '|';
    } else {
      row[last] = '|';
    }
  }

  return total;
}

void p1() {
  FILE *f = fopen(INPUT_FILENAME, "r");
  if (f == NULL) {
    log_fatal("Error opening file %s", strerror(errno));
  }

  long fSize = filesize(f);
  Matrix m;
  m.size = fSize;
  m.data = (char *)malloc(fSize * sizeof(char));
  size_t read = fread(m.data, sizeof(char), fSize, f);
  m.nCols = (long) (strchr(m.data, '\n') - m.data) + 1; // diff between beginning and '\n' + 1
  m.nRows = m.size / m.nCols;
  fclose(f);

  long total = start(&m);
  long numHits = 0;

  for (int rIdx = 1; rIdx < m.nRows; rIdx++) {
    total += simulate(rIdx, &m);
  }
  printf("Part one: %ld\n", total);

  free(m.data);
}

void p2() {

  FILE *f = fopen(INPUT_FILENAME, "r");
  if (f == NULL) {
    log_fatal("Error opening file %s", strerror(errno));
  }

  long fSize = filesize(f);
  Matrix m;
  m.size = fSize;
  m.data = (char *)malloc(fSize * sizeof(char));
  size_t read = fread(m.data, sizeof(char), fSize, f);
  m.nCols = (long) (strchr(m.data, '\n') - m.data) + 1; // diff between beginning and '\n' + 1
  m.nRows = m.size / m.nCols;
  fclose(f);

  LongMatrix lm;
  lm.size = fSize;
  lm.data = (long *)calloc(fSize, sizeof(long));
  lm.nCols = m.nCols;
  lm.nRows = m.nRows;

  long total = start2(&m, &lm);

  for (int rIdx = 1; rIdx < m.nRows; rIdx++) {
    simulate2(rIdx, &m, &lm);
  }

  long *row = get_rowL(lm.nRows - 1, &lm);
  for (int cIdx = 0; cIdx < lm.nCols - 1; cIdx++) {
    total += row[cIdx];
  }
  printf("Part two: %ld\n", total);

  free(lm.data);
  free(m.data);
}


int main() {
  p1();
  p2();
  return 0;
}
