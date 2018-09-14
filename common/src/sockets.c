/*
 * sockets.c - low level networking library for handling
 * sockets and data transmission. Much of this has been
 * shamelessly borrowed from Beej's Guide to Networking
 * (see README.md)
 */

#include <netdb.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

#include "connection_pool.h"
#include "serialization.h"
#include "shriek_types.h"

/*
 * initialize_socket(): create a socket for use in either connecting
 * or listening.
 */
int initialize_socket(const char* const address, const char* const port,
                      struct addrinfo* servinfo) {
  int rv = 0;
  int socket_fd = 0;
  struct addrinfo hints, *p;
  memset(&hints, 0, sizeof(hints));

  hints.ai_family = AF_UNSPEC;
  hints.ai_socktype = SOCK_STREAM;

  if ((rv = getaddrinfo(address, port, &hints, &servinfo)) != 0) {
    fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(rv));
    return 1;
  }

  for (p = servinfo; p != NULL; p = p->ai_next) {
    socket_fd = socket(p->ai_family, p->ai_socktype, p->ai_protocol);
    if (socket_fd != -1) {
      return socket_fd;
    }
  }

  fprintf(stderr, "initialize_socket() | failed to create socket to %s:%s\n",
          address, port);
  return -1;
}

/*
 * cleanup_socket() - tear down existing socket
 */
void cleanup_socket(int sockfd) { close(sockfd); }

/*
 * node_listen() - begin listening for incoming connections by
 * creating a socket and adding it to the connection pool
 */
ssize_t node_listen(const char* const address, const char* const port) {
  struct addrinfo* servinfo;
  int socket_fd = initialize_socket(address, port, servinfo);
  if (socket_fd == -1) {
    freeaddrinfo(servinfo);
    return -1;
  }

  if (listen(socket_fd, MAX_CONNECTION_BACKLOG) == -1) {
    close(socket_fd);
    fprintf(stderr, "node_listen() | failed to listen\n");
    freeaddrinfo(servinfo);
    return -1;
  }

  pool_add(socket_fd, true);
  freeaddrinfo(servinfo);
  return 0;
}

/*
 * node_connect() - establish a connection to another node by creating
 * a socket and adding it to the connection pool
 */
ssize_t node_connect(const char* const address, const char* const port) {
  struct addrinfo* servinfo;
  int socket_fd = initialize_socket(address, port, servinfo);
  if (socket_fd == -1) {
    freeaddrinfo(servinfo);
    return -1;
  }

  if (connect(socket_fd, servinfo->ai_addr, servinfo->ai_addrlen) == -1) {
    close(socket_fd);
    fprintf(stderr, "node_connect() | failed to connect\n");
    freeaddrinfo(servinfo);
    return -1;
  }

  pool_add(sockfd, false);
  freeaddrinfo(servinfo);
  return 0;
}

/*
static int initialize_socket() {
  int rv = 0;
  int sockfd = 0;
  struct addrinfo hints, *servinfo, *p;
  memset(&hints, 0, sizeof hints);

  hints.ai_family = AF_UNSPEC;
  hints.ai_socktype = SOCK_STREAM;

  if ((rv = getaddrinfo(address, port, &hints, &servinfo)) != 0) {
    fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(rv));
    return 1;
  }

  // loop through all the results and connect to the first we can;
  // break if the host/service combination can't be used for
  // a socket, or the if the socket connection fails
  for (p = servinfo; p != NULL; p = p->ai_next) {
    (sockfd = socket(p->ai_family, p->ai_socktype, p->ai_protocol));
    if (sockfd == -1) {
      perror("client: socket");
    }
  }

  if (p == NULL) {
    fprintf(stderr, "client: failed to connect\n");
    freeaddrinfo(servinfo);
    return -1;
  }

  pool_add(fd);
  freeaddrinfo(servinfo);
  return 0;
}
*/

/*
 * node_disconnect(): terminate a connection to another node.

ssize_t node_disconnect(const configuration* const config,
                        const size_t connection_id) {
  // TODO: Networking stuff here
  (void)config;
  return 0;
}
 */

/*
 * send_data(): given an established connection, send data to it
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
 */

/*
 * rev_data():

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
*/
