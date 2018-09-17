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
#include "log.h"
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
    log_trace("getaddrinfo failure: %s\n", gai_strerror(rv));
    return -1;
  }

  for (p = *servinfo; p != NULL; p = p->ai_next) {
    socket_fd = socket(p->ai_family, p->ai_socktype, p->ai_protocol);
    if (socket_fd != -1) {
      return socket_fd;
    }
  }

  log_trace("failed to create socket to %s:%s", address, port);
  return -1;
}

/*
 * cleanup_socket() - tear down existing socket
 */
void cleanup_socket(int sockfd) { close(sockfd); }

/*
 * get_connected_host(): given a file descriptor,
 * get its associated host IP and port number.
 */
char* get_connected_host(const int socket_fd) {
  struct sockaddr_storage addr;
  struct sockaddr* saddr = (struct sockaddr*)(&addr);
  struct sockaddr_in* saddr_in = (struct sockaddr_in*)(&addr);
  socklen_t saddr_len = sizeof(saddr);
  char ip_str[INET6_ADDRSTRLEN] = {0};
  char* address_str = NULL;
  int port = 0;

  memset(&addr, 0, sizeof(addr));
  getpeername(socket_fd, saddr, &saddr_len);

  port = ntohs(saddr_in->sin_port);
  inet_ntop(AF_INET, &(saddr_in->sin_addr), ip_str, sizeof(ip_str));

  // port length is never more than 6 chars; allocate extra for ":"
  address_str = calloc(1, strlen(ip_str) + 7);
  if (address_str == NULL) {
    return NULL;
  }
  sprintf(address_str, "%s:%d", ip_str, port);
  return address_str;
}

/*
 * socket_listen() - begin listening for incoming connections by
 * creating a socket and adding it to the connection pool
 */
ssize_t socket_listen(const char* const address, const char* const port) {
  struct addrinfo servinfo;
  memset(&servinfo, 0, sizeof(servinfo));
  struct addrinfo* p_servinfo = &servinfo;
  p_servinfo->ai_flags = AI_PASSIVE;
  int yes = 1;

  int socket_fd = initialize_socket(address, port, &p_servinfo);
  if (socket_fd == -1) {
    freeaddrinfo(p_servinfo);
    return -1;
  }

  // Allow socket address reuse so bind doesn't fail while kernel cleans up
  // previous runs. "yes" is the setting for the option.
  if (setsockopt(socket_fd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(yes)) ==
      -1) {
    log_trace("failed to set socket options.");
    cleanup_socket(socket_fd);

    freeaddrinfo(p_servinfo);
    return -1;
  }

  if (bind(socket_fd, p_servinfo->ai_addr, p_servinfo->ai_addrlen) == -1) {
    log_trace("failed to bind()");
    cleanup_socket(socket_fd);
    freeaddrinfo(p_servinfo);
    return -1;
  }

  if (listen(socket_fd, MAX_CONNECTION_BACKLOG) == -1) {
    log_trace("failed to listen()");
    cleanup_socket(socket_fd);
    freeaddrinfo(p_servinfo);
    return -1;
  }

  if (pool_add(socket_fd, LISTENING) == -1) {
    cleanup_socket(socket_fd);
    freeaddrinfo(p_servinfo);
    return -1;
  }

  freeaddrinfo(p_servinfo);
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

  if (connect(socket_fd, servinfo->ai_addr, servinfo->ai_addrlen) != 0) {
    close(socket_fd);
    log_trace("failed to connect to %s:%s\n", address, port);
    freeaddrinfo(servinfo);
    return -1;
  }

  if (pool_add(socket_fd, NON_LISTENING) == -1) {
    cleanup_socket(socket_fd);
    freeaddrinfo(servinfo);
    return -1;
  }

  freeaddrinfo(servinfo);
  return 0;
}

/*
 * socket_accept(): calls accept() on listening
 * socket and adds result to connection pool if appropriate.
 * https://stackoverflow.com/questions/2064636/getting-the-source-address-of-an-incoming-socket-connection
 */
ssize_t socket_accept(const int listener_socket_fd) {
  struct sockaddr_storage addr;
  socklen_t len = sizeof(addr);
  char* address_string;

  memset(&addr, 0, sizeof(struct sockaddr_storage));
  int new_socket_fd = accept(listener_socket_fd, (struct sockaddr*)&addr, &len);
  address_string = get_connected_host(new_socket_fd);

  if (new_socket_fd == -1) {
    log_error("couldn't accept incoming connection from %s", address_string);
    free(address_string);
    return -1;
  }

  if (pool_add(new_socket_fd, NON_LISTENING) == -1) {
    log_error("too many connections; rejecting incoming connection from %s ",
              address_string);
    cleanup_socket(new_socket_fd);
    free(address_string);

    return -1;
  }

  log_info("Incoming connection from %s", address_string);
  free(address_string);
  return 0;
}

/*
 * send_data(): send a serialized message to a socket
 */
ssize_t send_data(const int socket_fd,
                  const serialized_message* const s_message) {
  if (s_message == NULL) {
    return -1;
  }

  // TODO: this may result in partial sends
  // TODO: convert to network byte order
  ssize_t len = send(socket_fd, s_message->data, s_message->len, 0);
  if (len == -1) {
    perror("send()");
  }
  return len;
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

  len = recv(socket_fd, data, KB(300), 0);
  if (len == -1) {
    log_error("recv error.");
    free(data);
    return NULL;
  }

  if (len == 0) {
    char* address_string = get_connected_host(socket_fd);
    log_info("%s disconnected.", address_string);
    free(address_string);
    free(data);
    return NULL;
  }

  serialized_message* s_message =
      alloc_serialized_message(-1, data, (size_t)len);
  if (s_message == NULL) {
    free(data);
    return NULL;
  }

  return s_message;
}
