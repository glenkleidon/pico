#ifndef __C_TESTMETHODS__
 #define __C_TESTMETHODS__

#define ANSI_COLOR_RED "\x1b[31m"
#define ANSI_COLOR_GREEN "\x1b[32m"
#define ANSI_COLOR_RESET "\x1b[0m"
#define ANSI_COLOR_YELLOW  "\x1b[33m"
#define ANSI_COLOR_BLUE    "\x1b[34m"
#define ANSI_COLOR_MAGENTA "\x1b[35m"
#define ANSI_COLOR_CYAN    "\x1b[36m"

#define pass_message printf("%s%s%s",ANSI_COLOR_GREEN,"PASS: ",ANSI_COLOR_RESET);
#define fail_message printf("%s%s%s",ANSI_COLOR_RED,"FAIL: ",ANSI_COLOR_RESET);
#define skip_message printf("%s%s%s",ANSI_COLOR_MAGENTA,"SKIP: ",ANSI_COLOR_RESET);
#define DEFAULT_MESSAGE "\0"

void new_test(char *new_test_name);
void new_set(char * new_set_name);
void check_int(int expected, int actual);
void check_int_msg(int expected, int actual, char *message);
void check_str(char *expected, char *actual);
void check_str_msg(char *expected, char *actual, char *message);
void end_run();



#endif /*__C_TESTMETHODS__*/
