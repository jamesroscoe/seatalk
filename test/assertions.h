int total_assertions(void);
int total_failures(void);
void assert(int true_or_false, char *explanation);
void refute(int true_or_false, char *explanation);
void assert_equal_int(int reference, int test, char *explanation);
void assert_equal_string(char *reference, char *test, char *explanation);
void assert_equal_float(float reference, float test, char *explanation);
