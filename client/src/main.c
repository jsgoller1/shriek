#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "client.h"

static int get_handler(const char* const key) {
  printf("Getting %s\n", key);
  return 0;
}

static int set_handler(const char* const key, const char* const value) {
  printf("Setting %s = %s\n", key, value);
  return 0;
}

int main() {
  ssize_t rsize = 0;
  size_t line_size = 1000;
  char* linep = malloc(sizeof(char) * line_size);
  if (linep == NULL) {
    printf("main() | cannot read commands. Quitting...");
    return -1;
  }

  char* key = malloc(sizeof(char) * MAX_KEY_SIZE);
  if (key == NULL) {
    printf("main() | cannot read commands. Quitting...");
    free(linep);
    return -1;
  }

  char* value = malloc(sizeof(char) * MAX_VAL_SIZE);
  if (value == NULL) {
    printf("main() | cannot read commands. Quitting...");
    free(linep);
    free(key);
    return -1;
  }

  printf("> ");
  while ((rsize = getline(&linep, &line_size, stdin)) != -1) {
    // TODO: the below can overflow the max key size / max val size
    if (sscanf(linep, "get %s", key, value) == 1) {
      get_handler(key);
    } else if (sscanf(linep, "set %s %s", key, value) == 2) {
      set_handler(key, value);
    } else if (strcmp(linep, "exit") == 0) {
      printf("Quitting...");
      break;
    } else {
      printf("Invalid command.\n");
    }
    printf("> ");
  }

  free(linep);
  free(key);
  free(value);
  return 0;
}
