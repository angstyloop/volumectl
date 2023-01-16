/* Copy .h file to a temporary .c file, compile an executable, and delete the
 * temporary .c file 
 *
a=msleep; cp $a.h $a.c && gcc -o $a -Dtest_main $a.c && rm $a.c
 *
 * USE (sleep 500 milliseconds)
 *
msleep 500
 *
 * GET USAGE
 *
msleep
 *
 */

#ifndef msleep_h
#define msleep_h

#include "stdlib_h.h"
#include "stdio_h.h"
#include "time_h.h"
#include "sys_types_h.h"
#include "errno_h.h"

int msleep(long msec) {

    /*********************************************************************/
    /* The timespec object, initialized to all zeroes with the universal */
    /* initializer {0}.                                                  */
    /*********************************************************************/
    struct timespec ts={0};

    /**********************************/
    /* A variable to hold the result. */
    /**********************************/
    int res=0;

    /*****************************************************/
    /* Negative time is how <time.h> communicates errors */
    /*****************************************************/
    if (msec < 0) {
        errno = EINVAL;
        return -1;
    }

    /**************************/
    /* Set the SECONDS field. */
    /**************************/
    ts.tv_sec = msec / 1000;

    /******************************/
    /* Set the NANOSECONDS field. */
    /******************************/
    ts.tv_nsec = (msec % 1000) * 1000000;

    /****************************************************************/
    /* Sleep for @msec MILLISECONDS                                 */
    /*                                                              */
    /* Type "man nanosleep" at the command line to read the Linux   */
    /* Programmer's Manual description of the "nanosleep" function. */
    /****************************************************************/
    do { res = nanosleep(&ts, &ts); } while (res && errno==EINTR);

    return res;
}

// DRIVER CODE
// Produce a script for testing
// Run ./msleep to see usage.
#ifdef test_main
int main(int argc, char* argv[argc]) {
    if (argc != 2) {
        printf("USAGE\n"
                   "\t%s <MILLISECONDS>\n"
               "\n"
               "DESCRIPTION\n"
                   "\tPause execution (i.e. \"sleep\") for MILLISECONDS.)\n",
               argv[0]);
        exit(EXIT_FAILURE);
    }
    msleep(atol(argv[1]));
    exit(EXIT_SUCCESS);
}
#endif // test_main

#endif // msleep_h
