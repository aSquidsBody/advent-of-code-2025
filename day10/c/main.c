#include <assert.h>
#include <limits.h>
#include <errno.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../../logging.h"
#include "../../slice.h"

#define INPUT_FILENAME "input.txt"
#define LINE_SIZE 1024

typedef struct {
  uint64_t numLights;
  uint64_t lights;
  slice *buttons;
  slice *requirements;
} machine;

machine *new_machine() {
  machine *m = malloc(sizeof(machine));
  m->numLights = 0;
  m->lights = 0;
  m->buttons = slice_new_empty(0, 64, sizeof(uint64_t));
  m->requirements = NULL;

  return m;
}

void print_machine(machine *m) {
	printf("Machine:\n");
	printf("%lld, %lld\n", m->numLights, m->lights);
  printf("[");
  for (size_t i = 0; i < slice_length(m->buttons); i++) {
	printf("%lld ", *(uint64_t*)slice_get(i, m->buttons));
  }
  printf("]\n");
  printf("[");
  for (size_t i = 0; i < slice_length(m->requirements); i++) {
	  printf("%lld ", *(uint64_t*)slice_get(i, m->requirements));
  }
  printf("]\n");
}

void free_machine(machine *m) {
  slice_free(m->buttons);
  slice_free(m->requirements);
}

void populateLights(machine *m, char *word) {
  size_t len = strlen(word);
  if (word[len - 1] == '\n') {
    word[len-1] = '\0';
    len--;
  }

  m->numLights = (uint64_t) len - 2;
  m->lights = (uint64_t) 0;

  for (size_t i = 1; i < len - 1; i++) {
    if (word[i] == '#') {
      m->lights |= (1<<(i-1));
    }
  }
}

void addButton(machine *m, char *word) {
  size_t len = strlen(word);
  if (word[len - 1] == '\n') {
    word[len - 1] = '\0';
    word[len - 2] = '\0';
    len -= 2;
  } else {
    word[len - 1] = '\0';
    len -= 1;
  }
  word++;
  
  uint64_t btn = 0;
  char *lasts;
  for (char *digit = strtok_r(word, ",", &lasts); digit; digit = strtok_r(NULL, ",", &lasts)) {
    char *endPtr;
    uint64_t d = strtol(digit, &endPtr, 10);
    if (*endPtr != '\0') {
      log_fatal("Failed to convert digit %s\n", digit);
    }

    btn |= (1 << d);
  }

  slice_append((void *) &btn, m->buttons);
}

void populateRequirements(machine *m, char *word) {
  size_t len = strlen(word);
  if (word[len - 1] == '\n') {
    word[len - 1] = '\0';
    word[len - 2] = '\0';
    len -= 2;
  } else {
    word[len - 1] = '\0';
    len -= 1;
  }
  word++;

  m->requirements = slice_new_empty(0, m->numLights, sizeof(uint64_t));
  
  char *lasts;
  for (char *digit = strtok_r(word, ",", &lasts); digit; digit = strtok_r(NULL, ",", &lasts)) {
    char *endPtr;
    uint64_t d = (uint64_t) strtol(digit, &endPtr, 10);
    if (*endPtr != '\0') {
      log_fatal("Failed to convert req digit %s\n", digit);
    }

    slice_append((void *) &d, m->requirements);
  }
  assert(slice_length(m->requirements) == (size_t) m->numLights);
}

machine *readMachine(char *line) {
  machine *m = new_machine();

  char *lasts;
  for (char *word = strtok_r(line, " ", &lasts); word; word = strtok_r(NULL, " ", &lasts)) { 
    switch (word[0]) {
      case '[':
        populateLights(m, word);
        break;
      case '(':
        addButton(m, word);
        break;
      case '{':
        populateRequirements(m, word);
        break;
    }
  }

  return m;
}

long min(long x, long y) {
  return x * (x < y) + y * (y <= x);
}

uint64_t toggle(machine *m, uint64_t lights, size_t buttonIdx) {
  return lights ^ *(uint64_t*) slice_get(buttonIdx, m->buttons);
}

long minButtonPushesRecursive(machine *m, uint64_t current, size_t startIdx) {
  if (current == m->lights) {
    return 0; 
  }

  if (startIdx == slice_length(m->buttons)) {
    return LONG_MAX - 1;
  }

  return min(1 + minButtonPushesRecursive(m, toggle(m, current, startIdx), startIdx + 1), minButtonPushesRecursive(m, current, startIdx + 1));
}

long minButtonPushes(machine *m) {
  return minButtonPushesRecursive(m, 0, 0);
}


void p1() {
  FILE *f = fopen(INPUT_FILENAME, "r");
  if (f == NULL) {
    log_fatal("Could not open file. %s", strerror(errno));
  }

  long total = 0;
  char line[LINE_SIZE];
  while (fgets(line, LINE_SIZE, f) != NULL) {
    machine *m = readMachine(line);
    total += minButtonPushes(m);

  }
  if (ferror(f)) {
    log_fatal("Error reading lines from file. %s", strerror(errno));
  }
  fclose(f);

  printf("Part one: %ld\n", total);
}

int main() {
  p1();
}
