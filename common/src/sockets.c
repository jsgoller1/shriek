/*
 * sockets.c - a low level networking library for handling
 * C standard library sockets. Much of this has been
 * shamelessly borrowed from Beej's Guide to Networking
 * (see README.md)
 */

#include <arpa/inet.h>
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
#include "sockets.h"

/*
 * initialize_socket(): create a socket for use in either connecting
 * or listening.
 */
int initialize_socket(const char* const address, const char* const port,
                      struct addrinfo** servinfo) {
  int rv = 0;
  int socket_fd = 0;
  struct addrinfo hints, *p;
  memset(&hints, 0, sizeof(hints));

  hints.ai_family = AF_UNSPEC;
  hints.ai_socktype = SOCK_STREAM;

  if ((rv = getaddrinfo(address, port, &hints, servinfo)) != 0) {
    fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(rv));
    return -1;
  }

  for (p = *servinfo; p != NULL; p = p->ai_next) {
    socket_fd = socket(p->ai_family, p->ai_socktype, p->ai_protocol);
    if (socket_fd != -1) {
      bind(socket_fd, p->ai_addr, p->ai_addrlen);
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
 * socket_listen() - begin listening for incoming connections by
 * creating a socket and adding it to the connection pool
 */
ssize_t socket_listen(const char* const address, const char* const port) {
  struct addrinfo* servinfo = {0};

  int socket_fd = initialize_socket(address, port, &servinfo);
  if (socket_fd == -1) {
    freeaddrinfo(servinfo);
    return -1;
  }

  if (listen(socket_fd, MAX_CONNECTION_BACKLOG) == -1) {
    close(socket_fd);
    fprintf(stderr, "socket_listen() | failed to listen\n");
    freeaddrinfo(servinfo);
    return -1;
  }

  pool_add(socket_fd, true);
  freeaddrinfo(servinfo);
  return 0;
}

/*
 * socket_connect() - establish a connection to another node by creating
 * a socket and adding it to the connection pool
 */
ssize_t socket_connect(const char* const address, const char* const port) {
  struct addrinfo* servinfo = {0};

  int socket_fd = initialize_socket(address, port, &servinfo);
  if (socket_fd == -1) {
    freeaddrinfo(servinfo);
    return -1;
  }

  if (connect(socket_fd, servinfo->ai_addr, servinfo->ai_addrlen) == -1) {
    close(socket_fd);
    fprintf(stderr, "socket_connect() | failed to connect\n");
    freeaddrinfo(servinfo);
    return -1;
  }

  pool_add(socket_fd, false);
  freeaddrinfo(servinfo);
  return 0;
}

/*
 * socket_accept(): calls accept() on listening
 * socket and adds result to connection pool if appropriate.
 * https://stackoverflow.com/questions/2064636/getting-the-source-address-of-an-incoming-socket-connection
 */
ssize_t socket_accept(const int listener_socket_fd) {
  struct sockaddr_storage addr = {0};
  socklen_t len = sizeof(addr);
  char ipstr[INET6_ADDRSTRLEN] = {0};
  int port = 0;

  int new_socket_fd = accept(listener_socket_fd, (struct sockaddr*)&addr, &len);

  if (new_socket_fd == -1) {
    fprintf(stderr, "Couldn't accept incoming connection.\n");
    return -1;
  }

  if (pool_add(new_socket_fd, false) == -1) {
    fprintf(stderr, "Couldn't add incoming connection to pool.\n");
    cleanup_socket(new_socket_fd);
  }

  struct sockaddr_in* s_in = (struct sockaddr_in*)&addr;
  port = ntohs(s_in->sin_port);
  inet_ntop(AF_INET, &(s_in->sin_addr), ipstr, sizeof(ipstr));
  printf("Incoming connection from %s:%d\n", ipstr, port);

  return 0;
}

/*
 * send_data(): send a serialized message to a socket
 */
ssize_t send_data(const serialized_message* const s_message) {
  if (s_message == NULL) {
    return -1;
  }

  // TODO: this may result in partial sends
  // TODO: convert to network byte order
  ssize_t reply =
      send((int)s_message->connection_id, s_message->data, s_message->len, 0);

  return reply;
}

/*
 * rev_data(): read data from a socket and return a serialized message
 */
serialized_message* recv_data(const int socket_fd) {
  ssize_t len = 0;
  // TODO: Recieve arbitrarily long messages, not just up to 300KB
  char* data = calloc(1, sizeof(char) * KB(300));
  if (data == NULL) {
    return NULL;
  }

  // TODO: convert from network byte order
  len = recv(socket_fd, data, KB(300), 0);
  if (len == -1) {
    free(data);
    return NULL;
  }

  serialized_message* s_message =
      alloc_serialized_message(socket_fd, data, (size_t)len);
  if (s_message == NULL) {
    return NULL;
  }

  printf("recv_data() | got: ");
  for (size_t i = 0; i < s_message->len; i++) {
    printf("%c", s_message->data[i]);
  }
  printf("\n");

  return s_message;
}
