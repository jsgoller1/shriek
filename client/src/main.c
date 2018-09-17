#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "client.h"
#include "config.h"
#include "log.h"
#include "messages.h"
#include "shriek_types.h"

/*
 * main() - entrypoint for Shriek client
 */
int main(int argc, char** argv) {
  size_t line_size = 1000;
  char* linep = NULL;
  char* key = NULL;
  char* value = NULL;
  message* reply = NULL;
  configuration* config = parse_flags(argc, argv);

  if (initialize_client(config, &linep, &key, &value) == -1) {
    return -1;
  }

  printf("> ");
  while (getline(&linep, &line_size, stdin) != -1) {
    // TODO: sscanf can overflow the max key size / max val size; maybe
    // realloc() each based on size of linep?
    if (sscanf(linep, "get %s", key) == 1) {
      if (send_message(GET, SERVER_CONNECTION_ID, key, NULL) != -1) {
        reply = recv_message();
        printf("%s\n", reply->value);
        free_message(reply);
      }
    } else if (sscanf(linep, "set %s %s", key, value) == 2) {
      if (send_message(SET, SERVER_CONNECTION_ID, key, value) != -1) {
        reply = recv_message();
        if (reply->key_size == 1 && reply->key[0] == '0') {
          printf("OK\n");
        } else {
          printf("ERROR\n");
          free_message(reply);
        }
      }
    } else if (strcmp(linep, "exit\n") == 0) {
      printf("Quitting...\n");
      break;
    } else {
      printf("Invalid command.\n");
    }
    printf("> ");
  }

  cleanup_client(config, linep, key, value);
  return 0;
}
