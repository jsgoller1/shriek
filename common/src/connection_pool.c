/*
 * connection_pool.c - the connection_pool array exists to support socket
 * multiplexing via poll(2). By convention, connection_pool[0] is a listening
 * socket upon which accept(2) is called; connections are added to the socket
 * pool, or an error will be thrown and the connection will be closed.
 *
 * Shriek clients and servers both use this library; clients may establish
 * new connections manually and leave connection_pool[0] unset
 * so that it is skipped by connections_listen();
 */

#include <poll.h>
#include <stdio.h>

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
ssize_t pool_add(const int socket_fd, bool listening) {
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
void pool_remove(const int socket_fd) {
  for (size_t i = 1; i < pool->size; i++) {
    if (pool->connections[i].fd == socket_fd) {
      cleanup_socket(pool->connections[i].fd);
      pool->connections[i].fd = -1;
      pool->connections[i].events = 0;
      pool->connections[i].revents = 0;
    }
  }
}
