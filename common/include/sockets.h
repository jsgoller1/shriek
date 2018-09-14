#pragma once

#include <netdb.h>
#include <sys/socket.h>
#include <sys/types.h>

int initialize_socket(const char* const address, const char* const port,
                      struct addrinfo** servinfo);
void cleanup_socket(int sockfd);
ssize_t node_listen(const char* const addr, const char* const port);
ssize_t node_connect(const char* const addr, const char* const port);
