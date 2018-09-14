#include <stdio.h>

#include "sockets.c"

int main() {
  int socket_fd = node_listen("127.0.0.1", "49234");
  printf("socket: %d\n", socket_fd);
  return 0;
}
