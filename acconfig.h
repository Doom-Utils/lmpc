/* Name of package. */
#define PACKAGE "noname"

/* Version number of package. */
#define VERSION "0.0.0"

/* Define the build date. */
#define DATE "0/0/00"

/* Define a release note. */
#define COMMENT "test"

/* Define the host, where the program should run. */
#define HOST "0-unknown-0"

/* Define if LMP support should be compiled in. */
#define ENABLE_LMP 1

/* Define if DMO support should be compiled in. */
#define ENABLE_DMO 1

/* Define if DEM support should be compiled in. */
#define ENABLE_DEM 1

/* Define if QWD support should be compiled in. */
#define ENABLE_QWD 1

/* Define if DM2 support should be compiled in. */
#define ENABLE_DM2 1

/* Define if DM3 support should be compiled in. */
#undef ENABLE_DM3

/* Define if you have the memmem prototype. */
#undef HAVE_MEMMEM_PROTOTYPE

@BOTTOM@


/*
 *  The following functions and data types are defined but strict ANSI
 *  provides no prototypes for them.
 *  I really don't like this part. I may remove this -ansi flag in the future.
 */

#if defined (MAYBE_I_NEED_IT_LATER__STRICT_ANSI__)
#include <stdio.h>
struct timeval {
  time_t tv_sec;
  long tv_usec;
};
struct timezone {
  int tz_minuteswest;
  int tz_dsttime;
};
extern int fileno(FILE *stream);
extern int gethostname(char *name, size_t len);
extern char* strdup(const char*);
extern char *strsep(char **stringp, const char *delim);
extern int gettimeofday(struct timeval *tv, struct timezone *tz);
#endif /* defined (__STRICT_ANSI__) */


/*
 *  With the following definitions I can use always the non-reversed order.
 *  The check to get the SETVBUF_REVERSED value needs a real program run,
 *  which is impossible with cross-compiling. So you have to manually
 *  uncomment the next line, if you suspect something bad with setvbuf.
 */
/* #define SETVBUF_REVERSED 1 */
#if defined(SETVBUF_REVERSED)
#define SETVBUF(stream, buf, mode , size) setvbuf(stream, mode, buf, size)
#else
#define SETVBUF(stream, buf, mode , size) setvbuf(stream, buf, mode, size)
#endif

