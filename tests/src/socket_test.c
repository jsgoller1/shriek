#include <stdio.h>

#include "connection_pool.h"
#include "sockets.h"

int main() {
  initialize_connection_pool(1);
  printf("result: %ld\n", node_listen("127.0.0.1", "49234"));
  cleanup_connection_pool();
  return 0;
}
