#pragma once

#include <netdb.h>
#include <sys/socket.h>
#include <sys/types.h>

int initialize_socket(const char* const address, const char* const port,
                      struct addrinfo** servinfo);
void cleanup_socket(int sockfd);

char* get_connected_host(const int socket_fd);

ssize_t socket_listen(const char* const addr, const char* const port);
ssize_t socket_connect(const char* const addr, const char* const port);
ssize_t socket_accept(const int listener_socket_fd);

serialized_message* recv_data(const int socket_fd);
ssize_t send_data(const int socket_fd,
                  const serialized_message* const s_message);
