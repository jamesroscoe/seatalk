#ifdef TESTING
  #include <stdio.h>
  #define LOG printf
#else
  #include <linux/kernel.h>
  #define LOG pr_info
#endif
