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

connection_pool* pool = NULL;

/*
 * alloc_connection_pool(): set up connection pool
 */
ssize_t initialize_connection_pool(const size_t size) {
  pool = calloc(1, sizeof(connection_pool));
  if (pool == NULL) {
    fprinf(stderr, "initialize_connection_pool() | Memory error\n");
    return NULL;
  }

  // always create at least one socket for listening
  pool->connections = calloc(size + 1, sizeof(struct pollfd));
  if (pool->connections == NULL) {
    fprinf(stderr, "initialize_connection_pool() | Memory error\n");
    free(pool);
    return NULL;
  }

  // initialize all socket fds to -1 to prevent poll()
  // from recieving stdin
  for (i = 0; i < size; i++) {
    pool->connections[i].fd = -1;
  }

  return 0;
}

/*
 * free_connection_pool(): tear down connection pool
 */
void cleanup_connection_pool() {
  for (size_t i = 0; i < pool->size; i++) {
    if (pool->connections[i].fd) {
      cleanup_socket(pool->connections[i].fd);
    }
  }
  free(pool->connections);
  free(pool);
}

/*
 * pool_add(): private function for adding new sockets to the pool
 */
static ssize_t pool_add(const int socket_fd, bool listening) {
  size_t i;

  // listening socket goes in pool[0]; tear down existing listening
  // socket and insert new one
  if (listening) {
    if (pool->connections[0].fd != -1) {
      pool_remove(pool->connections[0].fd);
    }
    i = 0;
    pool->has_listener = true;
  } else {
    // scan for open slot in the pool, return an error if none available
    for (i = 1; i < pool->size; i++) {
      if (pool->connections[i].fd == -1) {
        break;
      }
    }
    // couldn't find an open slot
    if (i == pool->size) {
      return -1;
    }
  }

  // install socket in open slot
  pool->connections[i].fd = socket_fd;
  pool->connections[i].events = (POLLIN);
  pool->connections[i].revents = (short)0;
  return 0;
}

/*
 * pool_remove(): private function for removing sockets from the pool
 */
static void pool_remove(const int socket_fd) {
  for (size_t i = 1; i < pool->size; i++) {
    if (pool->connections[i].fd == socket_fd) {
      cleanup_socket(pool->connections[i].fd);
      pool->connections[i].fd = -1;
      pool->connections[i].events = 0;
      pool->connections[i].revents = 0;
    }
  }
}

/*
 * initialize_socket(): create a socket for use in either connecting
 * or listening.
 */
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

/*
 * cleanup_socket() - tear down existing socket
 */
static void cleanup_socket(int sockfd) { close(sockfd); }

/*
 * node_listen() - begin listening for incoming connections by
 * creating a socket and adding it to the connection pool
 */
ssize_t node_listen(const char* const addr, const char* const port) {
  struct addrinfo* servinfo;
  int sockfd = initialize_socket(servinfo);

  if (listen(sockfd, MAX_CONNECTION_BACKLOG) == -1) {
    close(sockfd);
    fprintf(stderr, "node_listen() | failed to listen\n");
    freeaddrinfo(servinfo);
    return -1;
  }

  pool_add(sockfd, true);
  freeaddrinfo(servinfo);
  return 0;
}

/*
 * node_connect() - establish a connection to another node by creating
 * a socket and adding it to the connection pool
 */
ssize_t node_connect(const char* const addr, const char* const port) {
  struct addrinfo* servinfo;
  int sockfd = initialize_socket(servinfo);

  if (connect(sockfd, servinfo->ai_addr, servinfo->ai_addrlen) == -1) {
    close(sockfd);
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
