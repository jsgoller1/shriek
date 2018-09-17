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
#include "log.h"
#include "sockets.h"

static bool pool_listener = false;
static size_t pool_size = 0;
static struct pollfd* connection_pool = NULL;

/*
 * alloc_connection_pool(): set up connection pool
 */
ssize_t initialize_connection_pool(const size_t size) {
  // always create at least one socket for listening
  pool_size = size + 1;
  connection_pool = calloc(pool_size, sizeof(struct pollfd));
  if (connection_pool == NULL) {
    log_error("memory error, quitting.");
    free(connection_pool);
    return -1;
  }

  // initialize all socket fds to -1 to prevent poll()
  // from recieving stdin
  for (size_t i = 0; i < pool_size; i++) {
    connection_pool[i].fd = -1;
  }

  log_trace("Initialized connection pool with %d slots.", pool_size);
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
  size_t i = 1;

  // listening socket goes in pool[0]; tear down existing listening
  // socket, decrement counter so pool scan installs at connection_pool[0]
  if (listening) {
    i--;
    if (connection_pool[0].fd != -1) {
      pool_remove(connection_pool[0].fd);
    }
    pool_listener = true;
  }
  // scan for open slot in the pool, return an error if none available
  while (i < pool_size) {
    if (connection_pool[i].fd == -1) {
      connection_pool[i].fd = socket_fd;
      connection_pool[i].events = POLLIN;
      connection_pool[i].revents = (short)0;
      // log_trace("installed socket at connection_pool[%lu]\n", i);
      return 0;
    }
    i++;
  }

  log_trace("couldn't find open slot in connection pool.\n");
  return -1;
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
serialized_message* pool_listen(void) {
  log_trace("beginning pool listen.");
  while (poll(connection_pool, (nfds_t)pool_size, POLL_TIMEOUT_PERIOD) != -1) {
    // if we are listening for new connections, accept them and
    // add them to the pool
    if (pool_listener && (connection_pool[0].revents && POLLIN)) {
      if (socket_accept(connection_pool[0].fd) == -1) {
        log_warn("couldn't accept incoming connection.");
      } else {
        // log_trace("accepted incoming connection.");
      }
      connection_pool[0].revents = 0;
    }
    // Otherwise, get first ready socket; knock out any
    // closed sockets we find
    for (size_t i = 1; i < pool_size; i++) {
      if (connection_pool[i].revents & POLLIN) {
        serialized_message* s_message = recv_data(connection_pool[i].fd);
        s_message->connection_id = (ssize_t)i;
        connection_pool[i].revents = 0;
        return s_message;
      }
      if (connection_pool[i].revents & (POLLHUP | POLLNVAL | POLLERR)) {
        log_trace("found hung up socket at connection_pool[%lu], removing.", i);
        pool_remove(connection_pool[i].fd);
      }
    }
  }

  return NULL;
}

/*
 * pool_send(): send data to one of the connections in the pool.
 */
ssize_t pool_send(serialized_message* s_message) {
  log_trace("Sending message.");
  int socket_fd = connection_pool[s_message->connection_id].fd;
  return send_data(socket_fd, s_message);
}
