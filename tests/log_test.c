#include <stdio.h>

#include "log.h"

int main() {
#ifdef __FILE__
  printf("__FILE__ defined: %s\n", __FILE__);
#endif
#ifdef __LINE__
  printf("__LINE__ defined: %d\n", __LINE__);
#endif
  log_error("Hello, error!\n");
  return 0;
}
