#pragma once

int initialize_socket(void);
void cleanup_socket(int sockfd);
ssize_t node_listen(const char* const addr, const char* const port);
ssize_t node_connect(const char* const addr, const char* const port);
