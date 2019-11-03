typedef int timeout;
#define TIMEOUT_DEFAULT 0

int timeout_still_valid(timeout timeout_at);
void restart_timeout(timeout *timeout, int time_to_live);
void invalidate_timeout(timeout *timeout);
