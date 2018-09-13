#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "client.h"
#include "messages.h"
#include "net.h"
#include "shriek_types.h"

int main(int argc, char** argv) {
  size_t line_size = 1000;
  char* linep = NULL;
  char* key = NULL;
  char* value = NULL;
  net_config* config = NULL;

  flag_settings* flags = parse_client_flags(argc, argv);
  if (initialize_client(flags, &config, &linep, &key, &value) == -1) {
    return -1;
  }

  printf("> ");
  while (getline(&linep, &line_size, stdin) != -1) {
    // TODO: sscanf can overflow the max key size / max val size; maybe
    // realloc() each based on size of linep?
    if (sscanf(linep, "get %s", key) == 1) {
      if (send_message(config, GET, key, NULL) == 0) {
        printf("OK\n");
      }
    } else if (sscanf(linep, "set %s %s", key, value) == 2) {
      if (send_message(config, SET, key, value) == 0) {
        printf("OK\n");
      }
    } else if (strcmp(linep, "exit\n") == 0) {
      printf("Quitting...\n");
      break;
    } else {
      printf("Invalid command.\n");
    }
    printf("> ");
  }

  cleanup(config, linep, key, value);
  return 0;
}
