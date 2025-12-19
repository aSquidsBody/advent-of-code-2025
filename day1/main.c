
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
static const char INPUT_FILE[] = "input.txt";
static const int START_LOCATION = 50;
static const int DIAL_SIZE = 100;

int mod(int x, int y) {
  return ((x % y) + y) % y;
}


int abs(int x) {
  if (x < 0) {
    return -x;
  }
  return x;
}

int getZeroCrosses(int start, int delta) {
  if (delta == 0) {
    return 0;
  }
  
  int result = (start + delta) / DIAL_SIZE;
  if (delta > 0) {
    return result;
  }

  result = abs(result);
  if (start != 0 && start + delta <= 0) {
    result++;
  }
  return result;
}

int spinDial(int location, int value, int *zeroCrosses) {
  *zeroCrosses = *zeroCrosses + getZeroCrosses(location, value);
  return mod(location + value, DIAL_SIZE);
}

void p2() {
  FILE *fptr = fopen(INPUT_FILE, "r");
  if (fptr == NULL) {
    printf("Could not open %s\n", INPUT_FILE);
    exit(1);
  }

  char line[265];
  int counter = 0;
int location = START_LOCATION;
  while(fgets(line, 10, fptr)) {
    size_t len = strlen(line);
    char valuePart[len-1];
    char directionPart[2];

    strncpy(directionPart, line, sizeof(directionPart) - 1);
    directionPart[sizeof(directionPart) - 1] = '\0';
    memcpy(valuePart, &line[1], len-1);

    int value = atoi(valuePart);
    
    if ('L' == directionPart[0]) {
      value = -value;
    } 

    location = spinDial(location, value, &counter);

  }
  fclose(fptr);

  printf("Part two: %d\n", counter);

  return;
}

void p1() {
  FILE *fptr = fopen(INPUT_FILE, "r");
  if (fptr == NULL) {
    printf("Could not open %s\n", INPUT_FILE);
    exit(1);
  }

  char line[265];
  int counter = 0;
   int location = START_LOCATION;
  while(fgets(line, 10, fptr)) {
    size_t len = strlen(line);
    char valuePart[len-1];
    char directionPart[2];

    strncpy(directionPart, line, sizeof(directionPart) - 1);
    directionPart[sizeof(directionPart) - 1] = '\0';
    memcpy(valuePart, &line[1], len-1);

    int value = atoi(valuePart);
    
    if ('R' == directionPart[0]) {
      location = mod(location + value, DIAL_SIZE);
    } else {
      location = mod(location - value, DIAL_SIZE);
    }

    if (location == 0) {
      counter++;
    }

  }
  fclose(fptr);

  printf("Part one: %d\n", counter);

  return;
}

int main(int argc, char* args[]) {
  p1();
  p2();
  return 0;
}
