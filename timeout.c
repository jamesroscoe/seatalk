#ifdef TESTING
  #include <time.h>
#else
  #include <linux/time.h>
  #include <linux/module.h>
#endif
#include "timeout.h"

timeout get_time(void) {
  #ifdef TESTING
    time_t now;
    time(&now);
    return now;
  #else
    struct timespec now = current_kernel_time();
    return now.tv_sec;
  #endif
}

int timeout_still_valid(timeout timeout_at) {
  return get_time() < timeout_at;
}

void restart_timeout(timeout *timeout, int time_to_live) {
  *timeout = get_time() + time_to_live;
}

void invalidate_timeout(timeout *timeout_at) {
  *timeout_at = TIMEOUT_DEFAULT;
}
