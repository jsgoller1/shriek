/*
 * net.c - low level networking library for handling
 * sockets and data transmission. Much of this has been
 * shamelessly borrowed from Beej's Guide to Networking
 * (see README.md)
 *
 * The connection_pool[] array exists to support socket
 * multiplexing via poll(2). By convention, connection_pool[0] is a listening
 * socket upon which accept(2) is called; connections are added to the socket
 * pool, or an error will be thrown and the connection will be closed.
 *
 * Shriek clients and servers both use this library; clients may establish
 * new connections manually and should leave connection_pool[0] set to NULL
 * so that it is skipped by connections_listen();
 */

#include <netdb.h>
#include <poll.h>
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

struct pollfd* connection_pool[MAX_CONNECTIONS] = {0};

/*
 * node_connect() - establish a connection to another node
 */
ssize_t node_connect(configuration* const config) {
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

  // TODO: add socket to connection pool here

  freeaddrinfo(servinfo);
  return 0;
}

/*
 * node_disconnect(): terminate a connection to another node.
 */
ssize_t node_disconnect(const configuration* const config) {
  // TODO: Networking stuff here
  (void)config;
  return 0;
}

/*
 * send_data(): given an established connection, send data to it
 */
char* send_data(const size_t connection_id,
                const serialized_message* const s_message) {
  char* reply = NULL;

  if (s_message == NULL) {
    return NULL;
  }
  printf("Sending to %lu\n", connection_id);

  // TODO: Socket stuff goes here; in the meantime,
  // dump the binary data to disk for examination.

  FILE* out = fopen("serialized_data.bin", "w");
  fwrite(s_message->data, s_message->len, 1, out);
  fclose(out);

  return reply;
}

/*
 * rev_data():
 */
serialized_message* recv_data() {
  // TODO: Socket stuff goes here; in the meantime,
  // read the binary data from disk.
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
