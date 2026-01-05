#include <errno.h>
#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../../util.h"

static const char *INPUT_FILENAME = "input.txt";

void deleteAtIdx(int idx, char *line) {
  int size = strlen(line);

  if (idx >= size) {
    return;
  }

  char *lPtr = line;
  char *nextPtr = line;
  for (int i = idx; i < size - 1; i++) {
    lPtr = line + i*sizeof(char);
    nextPtr = line + (i+1)*sizeof(char);
    *lPtr = *nextPtr;
  }
  line[size - 1] = '\0';
}

long concatMaxN(char *line, int requiredLength) {
  char currentStr[256] = {0};
  strncpy(currentStr, line, requiredLength);

  char *endPtr;
  long currentValue = strtol(currentStr, &endPtr, 10); 
  if (*endPtr != '\0') {
    log_fatal("Failed to read string %s to a long. Line=%s", currentStr, line);
  }

  long bestCandidateValue = currentValue;
  long candidateValue;
  char bestCandidateStr[256] = {0}, candidateStr[256] = {0};

  // printf("(%c) Line %s\n", line[0], line);
  for (int nextIdx = requiredLength; nextIdx < strlen(line); nextIdx++) {
    bestCandidateValue = 0, candidateValue = 0;
    if (line[nextIdx] == '\n') break;

    // printf(" char = %c\n", line[nextIdx]);
    for (int deleteIdx = 0; deleteIdx < requiredLength; deleteIdx++) {
      strncpy(candidateStr, currentStr, requiredLength);
      deleteAtIdx(deleteIdx, candidateStr);
      candidateStr[requiredLength - 1] = line[nextIdx];
      // printf("  current = %s, candidate = %s, best = %s\n", currentStr, candidateStr, bestCandidateStr);

      candidateValue = strtol(candidateStr, &endPtr, 10);
      if (*endPtr != '\0') {
        log_fatal("Failed to strtol on %s. Final value=%c", candidateStr, endPtr);
      }

      if (candidateValue >= bestCandidateValue) {
        bestCandidateValue = candidateValue;
        strncpy(bestCandidateStr, candidateStr, requiredLength);
      }
      // printf("   current = %s\n", currentStr);
    }

    // printf("best,current = %s,%s\n", bestCandidateStr, currentStr);
    if (bestCandidateValue >= currentValue) {
      currentValue = bestCandidateValue;
      strncpy(currentStr, bestCandidateStr, requiredLength);
    }
  }

  return currentValue;
}

void p1() {
  FILE *file = fopen(INPUT_FILENAME, "r");
  if (file == NULL) {
    log_fatal("Failed to open file %s. %s", INPUT_FILENAME, strerror(errno));
  }

  int totalJoltage = 0;
  char line[256];
  while (NULL != fgets(line, 256, file)) {
    totalJoltage += concatMaxN(line, 2);
  }
  int err = errno;
  if (ferror(file)) {
    log_fatal("Failed to read all data from the file %s. %s", INPUT_FILENAME, strerror(errno));
  }

  printf("Part one: %d\n", totalJoltage);

  fclose(file);
}

void p2() {
  FILE *file = fopen(INPUT_FILENAME, "r");
  if (file == NULL) {
    log_fatal("Failed to open file %s. %s", INPUT_FILENAME, strerror(errno));
  }

  long totalJoltage = 0;
  char line[256];
  while (NULL != fgets(line, 256, file)) {
    totalJoltage += concatMaxN(line, 12);
  }
  int err = errno;
  if (ferror(file)) {
    log_fatal("Failed to read all data from the file %s. %s", INPUT_FILENAME, strerror(errno));
  }

  printf("Part two: %ld\n", totalJoltage);

  fclose(file);
}


int main(int argc, char **argv) {
  p1();
  p2();
  return 0;
}

