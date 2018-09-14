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
#include <stdlib.h>

#include "connection_pool.h"
#include "sockets.h"

static bool pool_listener = false;
static size_t pool_size = 0;
static struct pollfd* connection_pool = NULL;

/*
 * alloc_connection_pool(): set up connection pool
 */
ssize_t initialize_connection_pool(const size_t size) {
  // always create at least one socket for listening
  connection_pool = calloc(pool_size + 1, sizeof(struct pollfd));
  if (connection_pool == NULL) {
    fprintf(stderr, "initialize_connection_pool() | Memory error\n");
    free(connection_pool);
    return -1;
  }

  // initialize all socket fds to -1 to prevent poll()
  // from recieving stdin
  for (size_t i = 0; i < size; i++) {
    connection_pool[i].fd = -1;
  }

  return 0;
}

/*
 * free_connection_pool(): tear down connection pool
 */
void cleanup_connection_pool() {
  for (size_t i = 0; i < pool_size; i++) {
    if (connection_pool[i].fd) {
      cleanup_socket(connection_pool[i].fd);
    }
  }
  free(connection_pool);
}

/*
 * pool_add(): private function for adding new sockets to the pool
 */
ssize_t pool_add(const int socket_fd, bool listening) {
  size_t i;

  // listening socket goes in pool[0]; tear down existing listening
  // socket and insert new one
  if (listening) {
    if (connection_pool[0].fd != -1) {
      pool_remove(connection_pool[0].fd);
    }
    i = 0;
    pool_listener = true;
  } else {
    // scan for open slot in the pool, return an error if none available
    for (i = 1; i < pool_size; i++) {
      if (connection_pool[i].fd == -1) {
        break;
      }
    }
    // couldn't find an open slot
    if (i == pool_size) {
      return -1;
    }
  }

  // install socket in open slot
  connection_pool[i].fd = socket_fd;
  connection_pool[i].events = (POLLIN);
  connection_pool[i].revents = (short)0;
  return 0;
}

/*
 * pool_remove(): private function for removing sockets from the pool
 */
void pool_remove(const int socket_fd) {
  for (size_t i = 1; i < pool_size; i++) {
    if (connection_pool[i].fd == socket_fd) {
      cleanup_socket(connection_pool[i].fd);
      connection_pool[i].fd = -1;
      connection_pool[i].events = 0;
      connection_pool[i].revents = 0;
    }
  }
}

/*
 * pool_listen(): listen via poll() against
 * multiple sockets; if a listener socket is present and
 * ready, initiate new connections and add them to the pool.
 * For other sockets, read data from them into a serialized
 * message string. Returns a linked list of serialized messages that
 * must be deserialized.
 */
serialized_message* pool_listen(void) { return NULL; }

/*
 * pool_send(): send data to one of the connections in the pool.
 * connection_id is the offset in the connection_pool at which
 * the correct pollfd containing the associated socket exists,
 * but this is opaque to connection_pool users.
 */
ssize_t pool_send(size_t connection_id, serialized_message* s_message) {
  (void)connection_id;
  (void)s_message;
  return 0;
}
