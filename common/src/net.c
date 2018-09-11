#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>

#include "common.h"

ssize_t connect_to_server(net_config* const config) {
  // TODO: Networking stuff here
  config->socket = 0;
  return 0;
}

char* send_data(const net_config* const config,
                const serialized_message* const s_message) {
  char* reply = NULL;

  if (s_message == NULL) {
    return NULL;
  }
  printf("Sending to %s:%lu\n", config->address, config->port);

  // TODO: Socket stuff goes here; in the meantime,
  // dump the binary data to disk for examination.

  FILE* out = fopen("serialized_data.bin", "w");
  fwrite(s_message->data, s_message->len, 1, out);
  fclose(out);

  return reply;
}

serialized_message* recv_data(const net_config* const config) {
  // TODO: Socket stuff goes here; in the meantime,
  // read the binary data from disk.
  (void)config;
  struct stat buf;
  if (stat("serialized_data.bin", &buf) == -1) {
    return NULL;
  }

  FILE* in = fopen("serialized_data.bin", "r");
  fseek(in, 0L, SEEK_END);
  ssize_t sz = ftell(in);
  rewind(in);
  if (sz == -1) {
    return NULL;
  }

  serialized_message* s_message = malloc(sizeof(serialized_message));
  if (s_message == NULL) {
    fprintf(stderr, "recv_data() | Memory error.\n");
    return NULL;
  }
  s_message->data = malloc(sizeof(char) * (size_t)sz);
  s_message->len = (size_t)sz;
  if (s_message->data == NULL) {
    fprintf(stderr, "recv_data() | Memory error.\n");
    free(s_message);
    return NULL;
  }
  fread(s_message->data, (size_t)sz, 1, in);
  fclose(in);
  remove("serialized_data.bin");

  return s_message;
}

ssize_t disconnect(const net_config* const config) {
  // TODO: Networking stuff here
  (void)config;
  return 0;
}
