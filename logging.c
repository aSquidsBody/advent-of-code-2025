#include <stdio.h>
#include <stdarg.h>
#include <stdlib.h>

void log_fatal(char *string, ...) {
  va_list args;
  va_start(args, string);
  fprintf(stderr, "[FATAL] ");
  fprintf(stderr, string, args);
  fprintf(stderr, "\n");
  va_end(args);

  exit(1);
}
