#include <stdio.h>
#include <string.h>

static int get_handler(char* const line) {}

static int set_handler(char* const line) {}

static int exit_handler(char* const line) {}

int main(int argc, char** argv) {
  size_t rsize = 0;
  size_t line_size = 1000;
  char* linep = malloc(sizeof(char) * line_size);
  if (linep == NULL) {
    printf("main() | cannot read commands. Quitting...");
  }

  printf("> ");
  while (rsize = getline(&linep, &line_size, stdin != -1)) {
    /*
    // Prepare string for command parsing
    int i;
    for (i = 0; linep[i] != ' ' && i < line_size; i++) {
    }
    if (i != line_size) {
      linep[i] = '\0';
    }
    */

    if (sscanf("get %s %s", linep)) {
    } else if ((sscanf("set %s %s", linep)) {
    } else if (strcmp(linep, "exit") {
    } else

    printf("> ");
  }
  return 0;
}
