#include <errno.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "hashtable.h"
static const char INPUT_FILE[] = "input.txt";

void log_fatal(char *message, ...) {
  va_list args;
  va_start(args, message);

  fprintf(stderr, "[FATAL] ");
  vfprintf(stderr, message, args);
  fprintf(stderr, "\n");
  va_end(args);

  exit(1);
}

long long_pow(long base, int exp) {
  long result = 1L;
  for (int i = 0; i < exp; i++) {
    result *= base;
  }

  return result;
}

int num_digits(long x) {
  int nd = 0;

  while (0==0) {
    x /= 10L;
    nd++;

    if (x == 0L) {
      break;
    }
  }

  return nd;
}

void repeat_int(char *string, long value, int n) {
  int nDigits = num_digits(value);
  for (int i = 0; i < n * nDigits; i+=nDigits) {
    sprintf(string + i, "%ld", value);
  }
}


void p1() {
  FILE *file = fopen(INPUT_FILE, "r");
  if (file == NULL) {
    log_fatal("Failed to open '%s': %s", INPUT_FILE, strerror(errno));
  }

  // get file length in bytes
  int code = fseek(file, 0L, SEEK_END);
  if (code != 0) {
    log_fatal("Failed to read the filesize of '%s': %s", INPUT_FILE, strerror(errno));
  }
  long fsize = ftell(file);
  fseek(file, 0L, SEEK_SET);

  // get the entire input
  char buffer[fsize];
  if (!fread(buffer, fsize, 1, file)) {
    log_fatal("Failed to read '%s': %s", INPUT_FILE, strerror(errno));
  }

  // count number of elements
  int numElements = 1;
  for (int i = 0; i < fsize; i++) {
    if (buffer[i] == ',') {
      numElements++;
    }
  }

  char *outerPtr, *innerPtr, *strValue, *endPtr;
  long input[numElements][2], max;
  int row = 0;

  char *rangeStr = strtok_r(buffer, ",", &outerPtr);
  while (rangeStr != NULL) {

    for (int i = 0; i < 2; i++) {
      strValue = strtok_r(rangeStr, "-", &innerPtr);
      input[row][i] = strtol(strValue, &endPtr, 10);
      if (input[row][i] == 0 && strValue == endPtr) {
        log_fatal("Invalid string %s. Cannot parse out ints.", strValue);
      }
      if (max < input[row][i]) {
        max = input[row][i];
      }
      rangeStr = NULL;
    }

    rangeStr = strtok_r(NULL, ",", &outerPtr);
    row++;
  }


  long repeatNum = 0, total = 0;
  // loop through all numbers and get the get the 
  for (long i = 0; i < long_pow(10L, num_digits(max) / 2); i++) {
    char repeatNumStr[2 * num_digits(i)];
    repeat_int(repeatNumStr, i, 2);

    repeatNum = strtol(repeatNumStr, &endPtr, 10);

    for (int j = 0; j < numElements; j++) {
      if (input[j][0] <= repeatNum && repeatNum <= input[j][1]) {
        total += repeatNum; 
        break;
      }
    }
  }

  printf("Part one: %ld\n", total);

}

void p2() {
  FILE *file = fopen(INPUT_FILE, "r");
  if (file == NULL) {
    log_fatal("Failed to open '%s': %s", INPUT_FILE, strerror(errno));
  }

  // get file length in bytes
  int code = fseek(file, 0L, SEEK_END);
  if (code != 0) {
    log_fatal("Failed to read the filesize of '%s': %s", INPUT_FILE, strerror(errno));
  }
  long fsize = ftell(file);
  fseek(file, 0L, SEEK_SET);

  // get the entire input
  char buffer[fsize];
  if (!fread(buffer, fsize, 1, file)) {
    log_fatal("Failed to read '%s': %s", INPUT_FILE, strerror(errno));
  }

  // count number of elements
  int numElements = 1;
  for (int i = 0; i < fsize; i++) {
    if (buffer[i] == ',') {
      numElements++;
    }
  }

  char *outerPtr, *innerPtr, *strValue, *endPtr;
  long input[numElements][2], max;
  int row = 0;

  char *rangeStr = strtok_r(buffer, ",", &outerPtr);
  while (rangeStr != NULL) {

    for (int i = 0; i < 2; i++) {
      strValue = strtok_r(rangeStr, "-", &innerPtr);
      input[row][i] = strtol(strValue, &endPtr, 10);
      if (input[row][i] == 0 && strValue == endPtr) {
        log_fatal("Invalid string %s. Cannot parse out ints.", strValue);
      }
      if (max < input[row][i]) {
        max = input[row][i];
      }
      rangeStr = NULL;
    }

    rangeStr = strtok_r(NULL, ",", &outerPtr);
    row++;
  }


  long repeatNum = 0, total = 0;
  int nDigits = num_digits(max);
  HashTable *seen = new_hash_table(long_pow(10L, nDigits / 2) / 50);
  // loop through all numbers and get the get the 
  for (int numRepeat = 2; numRepeat <= nDigits; numRepeat++) {
    for (long i = 0; i < long_pow(10L, nDigits / 2); i++) {
      char repeatNumStr[numRepeat * num_digits(i)];
      repeat_int(repeatNumStr, i, numRepeat);

      repeatNum = strtol(repeatNumStr, &endPtr, 10);

      if (repeatNum > max) {
        break;
      }

      for (int j = 0; j < numElements; j++) {
        if (contains(seen, repeatNum) == 0 && input[j][0] <= repeatNum && repeatNum <= input[j][1]) {
          total += repeatNum; 
          add(seen, repeatNum);
          break;
        }
      }
    }
  }

  free_hash_table(seen);

  printf("Part two: %ld\n", total);
}

struct temp {
  int x;
};

int main(int argc, char *argv[]) {
  p1();
  p2();

  return 0;
}
