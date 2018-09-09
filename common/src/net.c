#include <stdio.h>

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

ssize_t disconnect(const net_config* const config) {
  // TODO: Networking stuff here
  (void)config;
  return 0;
}
