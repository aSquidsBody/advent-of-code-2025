#include <errno.h>
#include <math.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "../../util.h"

static const char *INPUT_FILENAME = "input.txt";

unsigned long filesize(FILE *f) {
  if(fseek(f, 0L, SEEK_END) == -1) {
    log_fatal("Failed to seek file. %s", strerror(errno));
  }

  long offset = ftell(f);
  if (offset == -1) {
    log_fatal("Failed to get size of file. %s", strerror(errno));
  }

  if (fseek(f, 0L, SEEK_SET) == -1) {
    log_fatal("Failed to reset file seek. %s", strerror(errno));
  }

  return (unsigned long) offset;
}

long buffer_size(unsigned long filesize) {
  int sigBit = sizeof(unsigned long)*8 - __builtin_clzl(filesize);
  
  return (int) pow(2.0, sigBit);
}

int matrix_coord(int r, int c, int rowSize) {
  return c + r*rowSize; 
}

int is_spool(int r, int c, int rowSize, int nRows, char *matrix) {
  if (r < 0 || r > nRows - 1) {
    return 0;
  }

  if (c < 0 || c > rowSize - 1) {
    return 0;
  }

  return matrix[matrix_coord(r, c, rowSize)] == '@';
}

int has_four_neighbors(int r, int c, int rowSize, int nRows, char *matrix) {

  int nNeighbors = 0;
  nNeighbors += is_spool(r + 1, c + 1, rowSize, nRows, matrix);
  nNeighbors += is_spool(r + 1, c, rowSize, nRows, matrix);
  nNeighbors += is_spool(r + 1, c - 1, rowSize, nRows, matrix);
  nNeighbors += is_spool(r , c + 1, rowSize, nRows, matrix);
  nNeighbors += is_spool(r , c - 1, rowSize, nRows, matrix);
  nNeighbors += is_spool(r - 1, c + 1, rowSize, nRows, matrix);
  nNeighbors += is_spool(r - 1, c , rowSize, nRows, matrix);
  nNeighbors += is_spool(r - 1, c - 1, rowSize, nRows, matrix);

  return nNeighbors > 3;
}

int clear_rolls(int rowSize, int nRows, char *matrix) {
  char *removeIndices[rowSize * nRows];
  int sum = 0;
  for (int r = 0; r < nRows; r++) {
    for (int c = 0; c < rowSize; c++) {
      int coord = matrix_coord(r, c, rowSize);
      if (matrix[coord] == '@' && !has_four_neighbors(r, c, rowSize, nRows, matrix)) {
        removeIndices[sum++] = &matrix[coord];
      }
    }
  }

  for (int i = 0; i < sum; i++) {
    *removeIndices[i] = '.';
  }

  return sum;

}

void p1() {
  FILE *f = fopen(INPUT_FILENAME, "r"); 
  if (f == NULL) {
    log_fatal("Could not open %s. %s", INPUT_FILENAME, strerror(errno));
  }

  long fsize = filesize(f);
  long bsize = buffer_size(fsize);
  char matrix[bsize];

  fread(matrix, sizeof(char), bsize, f);
  if (ferror(f)) {
    log_fatal("Failed to read from file %s. %s", INPUT_FILENAME, strerror(errno));
  }
  fclose(f);

  int rowSize = (int) (strchr(matrix, '\n') - matrix + 1); // first '\n' - matrix is the _index_ of the newline char, so plus one to get count.
  int nRows = fsize / (sizeof(char) * rowSize);

  long sum = 0;
  for (int r = 0; r < nRows; r++) {
    for (int c = 0; c < rowSize; c++) {
      if (matrix[matrix_coord(r, c, rowSize)] == '@' && !has_four_neighbors(r, c, rowSize, nRows, matrix)) {
        sum++;
      }
    }
  }

  printf("Part one: %ld\n", sum);
}

void p2() {
  FILE *f = fopen(INPUT_FILENAME, "r"); 
  if (f == NULL) {
    log_fatal("Could not open %s. %s", INPUT_FILENAME, strerror(errno));
  }

  long fsize = filesize(f);
  long bsize = buffer_size(fsize);
  char matrix[bsize];

  fread(matrix, sizeof(char), bsize, f);
  if (ferror(f)) {
    log_fatal("Failed to read from file %s. %s", INPUT_FILENAME, strerror(errno));
  }
  fclose(f);

  int rowSize = (int) (strchr(matrix, '\n') - matrix + 1); // first '\n' - matrix is the _index_ of the newline char, so plus one to get count.
  int nRows = fsize / (sizeof(char) * rowSize);

  long sum = 0;
  long numCleared = 1;
  while (numCleared != 0) {
    numCleared = clear_rolls(rowSize, nRows, matrix);
    sum += numCleared;
  }

  printf("Part two: %ld\n", sum);
}

int main() {
  p1();
  p2();
  
  return 0;
}
