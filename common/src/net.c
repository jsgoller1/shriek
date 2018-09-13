#include <netdb.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

#include "net.h"
#include "serialization.h"
#include "shriek_types.h"

// connect_to_server() - establish socket to server, or quit;
// parts of this have been borrowed from Beej's guide
// to network programming (see README.md)
ssize_t connect_to_server(net_config* const config) {
  int rv = 0;
  int sockfd = 0;
  struct addrinfo hints, *servinfo, *p;
  memset(&hints, 0, sizeof hints);

  hints.ai_family = AF_UNSPEC;
  hints.ai_socktype = SOCK_STREAM;

  if ((rv = getaddrinfo(config->address, config->port, &hints, &servinfo)) !=
      0) {
    fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(rv));
    return 1;
  }

  // loop through all the results and connect to the first we can;
  // break if the host/service combination can't be used for
  // a socket, or the if the socket connection fails
  for (p = servinfo; p != NULL; p = p->ai_next) {
    if ((sockfd = socket(p->ai_family, p->ai_socktype, p->ai_protocol)) == -1) {
      perror("client: socket");
      continue;
    }

    if (connect(sockfd, p->ai_addr, p->ai_addrlen) == -1) {
      close(sockfd);
      perror("client: connect");
      continue;
    }

    break;
  }

  if (p == NULL) {
    fprintf(stderr, "client: failed to connect\n");
    freeaddrinfo(servinfo);
    return -1;
  }

  config->socket = sockfd;
  freeaddrinfo(servinfo);
  return 0;
}

char* send_data(const net_config* const config,
                const serialized_message* const s_message) {
  char* reply = NULL;

  if (s_message == NULL) {
    return NULL;
  }
  printf("Sending to %s:%s\n", config->address, config->port);

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
