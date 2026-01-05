#include <assert.h>
#include <ctype.h>
#include <stdio.h>
#include "../../util.h"
#include <errno.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>

#define LINE_SIZE 20480
#define INPUT_FILENAME "input.txt"
#define WORD_SIZE 128

int array_idx(int r, int c, int nCols) {
  return r * nCols + c;
}

int num_cols(FILE *f) {
  int nCols = 0;
  char fLine[LINE_SIZE], *c;
  char *end = fLine;

  if (fseek(f, 0L, SEEK_SET) != 0) {
    log_fatal("num_cols: Failed to seek the start of the file. %s", strerror(errno));
  }

  fgets(fLine, sizeof(fLine), f);
  if (ferror(f)) {
    log_fatal("Failed to read line from file. %s", strerror(errno));
  }
  while ( (c = strtok_r(NULL, " ", &end)) != NULL){
    nCols++;
  }

  if (fseek(f, 0L, SEEK_SET) != 0) {
    log_fatal("num_cols: Failed to re-seek the start of the file. %s", strerror(errno));
  }

  return nCols;
}

int num_rows(FILE *f) {
  if (fseek(f, 0L, SEEK_SET) != 0) {
    log_fatal("num_rows: Failed to seek the start of the file. %s", strerror(errno));
  }

  char fLine[LINE_SIZE];
  int count = 0;
  while (fgets(fLine, sizeof(fLine), f) != NULL) {
    count++;
  }
  if (ferror(f)) { 
    log_fatal("Failed to fgets. %s", strerror(errno));
  }

  if (fseek(f, 0L, SEEK_SET) != 0) {
    log_fatal("num_rows: Failed to re-seek the start of the file. %s", strerror(errno));
  }
  return count;
}

long product_col(int nRows, int nCols, int cIdx, char *input) {
  long total = 1;
  char *endPtr;
  for (int r = 0; r < nRows - 1; r++ ) {
    int elemIdx = array_idx(r, cIdx, nCols) * WORD_SIZE;
    long n = strtol(input + elemIdx, &endPtr, 10);
    if (*endPtr != '\0') {
      log_fatal("Failed to convert '%s' to a long.", input + elemIdx);
    }

    total *= n;
  }

  return total;
}

long sum_col(int nRows, int nCols, int cIdx, char *input) {
  long total = 0;
  char *endPtr;
  for (int r = 0; r < nRows - 1; r++ ) {
    int elemIdx = array_idx(r, cIdx, nCols) * WORD_SIZE;
    long n = strtol(input + elemIdx, &endPtr, 10);
    if (*endPtr != '\0') {
      log_fatal("Failed to convert '%s' to a long.", input + elemIdx);
    }

    total += n;
  }

  return total;
}


long apply_operation(int nRows, int nCols, int cIdx, char *input) {
  int rIdx = (nRows - 1);
  int opIdx = array_idx(rIdx, cIdx, nCols) * WORD_SIZE;
  switch (*(input + opIdx)) {
    case '*':
      return product_col(nRows, nCols, cIdx, input);
      break;
    case '+':
      return sum_col(nRows, nCols, cIdx, input);
      break;
  }

  return 0;
}

void p1() {

  FILE *f = fopen(INPUT_FILENAME, "r");
  if (f == NULL) {
    log_fatal("Failed to open file %s. %s", INPUT_FILENAME, strerror(errno));
  }

  int nRows = num_rows(f);
  int nCols = num_cols(f);

  char input[nRows][nCols][WORD_SIZE];
  char fLine[LINE_SIZE], *lasts;

  int rIdx = 0, cIdx = 0;
  while (fgets(fLine, LINE_SIZE, f) != NULL) {
    long l = strlen(fLine);
    if (fLine[l-1] == '\n') {
      fLine[l-1] = '\0';
    }
    cIdx = 0;
    for (char *word = strtok_r(fLine, " ", &lasts); word; word = strtok_r(NULL, " ", &lasts)) {
      strncpy(input[rIdx][cIdx], word, strlen(word) + 1 );
      cIdx++;
    }
    rIdx++;
  }
  if (ferror(f)) {
    log_fatal("p1: failed to read input. %s.", strerror(errno));
  }

  long total = 0;
  for (int cIdx = 0; cIdx < nCols; cIdx++) {
    total += apply_operation(nRows, nCols, cIdx, (char*) input);
  }

  printf("Part one: %ld\n", total);

}

// --- PART 2 ---
unsigned long filesize(FILE *f) {
  if (fseek(f, 0L, SEEK_END) != 0) {
    log_fatal("Failed to get filesize - fseek issue: %s", strerror(errno));
  }

  long offset = ftell(f);
  if (offset == -1L) {
    log_fatal("Failed to get filesize - ftell issue: %s", strerror(errno));
  }

  if (fseek(f, 0L, SEEK_SET) != 0) {
    log_fatal("Failed to get filesize - fseek issue: %s", strerror(errno));
  }

  return (unsigned long) offset;
}

long line_size(FILE *f) {
  if (fseek(f, 0L, SEEK_SET) != 0) {
    log_fatal("Failed to get line_size - fseek issue: %s", strerror(errno));
  }

  char fLine[LINE_SIZE];
  if (fgets(fLine, LINE_SIZE, f) == NULL && ferror(f)) {
    log_fatal("Failed to get line_size - fgets issue: %s", strerror(errno));
  }

  if (fseek(f, 0L, SEEK_SET) != 0) {
    log_fatal("Failed to get line_size - fseek issue: %s", strerror(errno));
  }

  return strlen(fLine);
}

long apply_product(long *operands) {
  long idx = 0, total = 1, l;
  while ( (l = operands[idx++]) ) { total *= l; }

  return total;
}

long apply_sum(long *operands) {
  long idx = 0, total = 0, l;
  while ( (l = operands[idx++]) ) { total += l; }

  return total;
}

long apply_operation_p2(char op, long *operands) {
  if (op == '*') {
    return apply_product(operands);
  }
  return apply_sum(operands);
}

int is_all_whitespace(const char *s) {
  while (*s != '\0') {
    if (!isspace((unsigned char)*s)) {
      return 0;        
    }
    s++;
  }
  return 1; 
}

void p2() {
  FILE *f = fopen(INPUT_FILENAME, "r");
  if (f == NULL) {
    log_fatal("Could not open file. %s", strerror(errno));
  }

  long nCols = line_size(f); // including the "\n" but not the "\0"
  long nRows = filesize(f) / nCols;

  char input[nRows][nCols]; // including the "\0" at the nCols'th elem
  char fLine[LINE_SIZE] = {0};
  int rIdx = 0;
  while (fgets(fLine, nCols + 1, f) != NULL) {
    if (fLine[nCols - 1] == '\n') {
      fLine[nCols - 1] = '\0';
    }
    strncpy(input[rIdx], fLine, nCols);
    rIdx++;
  }
  if (ferror(f)) {
    log_fatal("Failed to read input. %s", strerror(errno));
  }
  fclose(f);

  long total = 0;
  long operands[256] = {0};
  int opIdx = 0;
  char op = ' ';
  for (int cIdx = 0; cIdx < nCols - 1; cIdx++) {
    char numStr[256] = {0};
    int strIdx = 0;

    if (input[nRows - 1][cIdx] != ' ') {
      op = input[nRows - 1][cIdx]; 
    }

    for (int rIdx = 0; rIdx < nRows - 1; rIdx++) {
      if (!isspace(input[rIdx][cIdx])) {
        numStr[strIdx++] = input[rIdx][cIdx];
      }
    }

    if (strlen(numStr) == 0) {
      total += apply_operation_p2(op, operands);
      for (int o = opIdx - 1; o>=0; o--) {
        operands[o] = 0;
      }
      opIdx = 0;
      continue;
    }

    char *end;
    long n = strtol(numStr, &end, 10);
    if (*end != '\0') {
      log_fatal("Invalid long '%s' %ld", numStr);
    }

    operands[opIdx++] = n;

  }
  // do a final calculation
  total += apply_operation_p2(op, operands);

  printf("Part two: %ld\n", total);
}

int main(int argc, char **argv) {
  p1();
  p2();

  return 0;
}
