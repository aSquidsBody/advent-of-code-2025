#include <ctype.h>
#include <errno.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static const char *INPUT_FILENAME = "input.txt";

void log_fatal(char *string, ...) {
  va_list args;
  va_start(args, string);
  fprintf(stderr, "[FATAL] ");
  fprintf(stderr, string, args);
  va_end(args);
  exit(1);
}

void trim_end(char *str) {
  for (int i = strlen(str) - 1; i >= 0 && isspace(str[i]); i--) {
    str[i] = '\0';
  }
}

void parse_id_range(char *str, long range[2]) {
  char *lPtr;
  for (int i=0; i<2; i++) {
    char *strN = strtok_r(NULL, "-", &str);
    long n = strtol(strN, &lPtr, 10);
    if (*lPtr != '\0') {
      log_fatal("Failed to read long from '%s'\n", str);
    }
    range[i] = n;
  }
}

void parse_id(char *str, long *id) {
  char *lPtr;
  long n = strtol(str, &lPtr, 10);
  if (*lPtr != '\0') {
    log_fatal("Failed to read long from '%s'\n", str);
  }

  (*id) = n;
}

void p1() {
  FILE *fptr = fopen(INPUT_FILENAME, "r");
  if (fptr == NULL) {
    log_fatal("Problem opening file %s to read. %s\n", INPUT_FILENAME, strerror(errno));
  }

  char line[256];
  long idRanges[2048][2] = {0};
  long ids[2048] = {0};
  long rIdx = 0, idx = 0;
  while (NULL != fgets(line, 256, fptr)) {
    trim_end(line);
    if ( strchr(line, '-') != NULL ) {
      parse_id_range(line, idRanges[rIdx]);
      rIdx++;
    } else if (strlen(line) != 0) {
      parse_id(line, &ids[idx]);
      idx++;
    }
  }
  if (ferror(fptr)) {
    log_fatal("Error while reading file %s. %s\n", INPUT_FILENAME, strerror(errno));
  }
  fclose(fptr);

  long sum = 0;
  for (int i = 0; i < idx; i++) {
    for (int j = 0; j < rIdx; j++) {
      if (ids[i] >= idRanges[j][0] && ids[i] <= idRanges[j][1]) {
        sum++;
        break;
      }
    }
  }

  printf("Part one: %ld\n", sum);
}

int compare(void *thunk, const void *x, const void *y) {
  const long *ptr = (const long*) x;
  const long *ptr2 = (const long*) y;

  return ptr[0] > ptr2[0] ? 1 : -1; 
}

long max(long x, long y) {
  if (x > y) {
    return x;
  }

  return y;
}

void p2() {
  FILE *fptr = fopen(INPUT_FILENAME, "r");
  if (fptr == NULL) {
    log_fatal("Problem opening file %s to read. %s\n", INPUT_FILENAME, strerror(errno));
  }

  char line[256];
  long rangeIds[256][2] = {0};
  long rIdx = 0;
  while (NULL != fgets(line, 256, fptr)) {
    trim_end(line);
    if (strchr(line, '-') != NULL) {
      parse_id_range(line, rangeIds[rIdx]);
      rIdx++;
    }
  }
  if (ferror(fptr)) {
    log_fatal("Error while reading file %s. %s\n", INPUT_FILENAME, strerror(errno));
  }
  fclose(fptr);

  long *thunk;
  qsort_r(rangeIds, rIdx, sizeof(long)*2, thunk, compare);
  // for (int i =0; i< rIdx; i++) {
  //   printf("%ld\n", rangeIds[i][0]);
  // }

  int idx = 0; 
  long sum = 0;
  long combinedRange[2] = {0};
  while (idx < rIdx) {
    combinedRange[0] = rangeIds[idx][0]; 
    combinedRange[1] = rangeIds[idx][1];
    idx++;

    while (idx < rIdx && combinedRange[1] >= rangeIds[idx][0]) {
      combinedRange[1] = max(rangeIds[idx][1], combinedRange[1]);
      idx++;
    }
    sum += 1 + combinedRange[1] - combinedRange[0];
  }


  printf("Part two: %ld\n", sum);
}

int main(int argc, char **argv) {
  p1();
  p2();

  printf("%d %d\n", 1 > 0, 1 < 0);

  // long *thunk;
  // long arr[6][2] = {
  //   {559603756857369 , 1}, {4, 12}, {50, 0}, {123, 4}, {123, 1}, {10, -10}
  // };
  // qsort_r(arr, 6, sizeof(long[2]), thunk, compare);
  // for (int i = 0; i < 6; i++) {
  //   printf("%ld\n", arr[i][0]);
  // }

  return 0;
}
