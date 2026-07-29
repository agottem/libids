AC_PREREQ([2.71])
AC_INIT([libids], [0.1], [andrew-pub@agottem.com])
AC_PROG_CC

AC_MSG_CHECKING([for working C11 threads])
AC_LINK_IFELSE(
    [AC_LANG_PROGRAM(
        [[#include <threads.h>
          static int ThreadMain (void* context) { return context != 0; }]],
        [[thrd_t thread;
          int result;
          if(thrd_create(&thread, ThreadMain, 0) != thrd_success)
              return 1;
          return thrd_join(thread, &result);]])],
    [AC_MSG_RESULT([yes])],
    [AC_MSG_RESULT([no])
     AC_MSG_ERROR([working C11 threads are required to run the test suite])])

AM_INIT_AUTOMAKE([foreign subdir-objects])

LT_INIT([disable-shared])

AC_CONFIG_FILES([Makefile])
AC_OUTPUT
