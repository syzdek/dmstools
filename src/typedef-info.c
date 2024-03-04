/*
 *  DMS Tools and Utilities
 *  Copyright (C) 2009 David M. Syzdek <david@syzdek.net>.
 *  All rights reserved.
 *
 *  Redistribution and use in source and binary forms, with or without
 *  modification, are permitted provided that the following conditions are
 *  met:
 *
 *     * Redistributions of source code must retain the above copyright
 *       notice, this list of conditions and the following disclaimer.
 *     * Redistributions in binary form must reproduce the above copyright
 *       notice, this list of conditions and the following disclaimer in the
 *       documentation and/or other materials provided with the distribution.
 *     * Neither the name of David M. Syzdek nor the
 *       names of its contributors may be used to endorse or promote products
 *       derived from this software without specific prior written permission.
 *
 *  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS
 *  IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
 *  THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 *  PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL DAVID M. SYZDEK BE LIABLE FOR
 *  ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 *  DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
 *  SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
 *  CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 *  LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 *  OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 *  SUCH DAMAGE.
 */
/**
 *  @file src/typedef-sizes.c displays the size of differnt data types
 */
/*
 *  Simple Build:
 *     gcc -W -Wall -O2 -c typedef-sizes.c
 *     gcc -W -Wall -O2 -o typedef-sizes   typedef-sizes.o
 *
 *  GNU Libtool Build:
 *     libtool --mode=compile gcc -W -Wall -g -O2 -c typedef-sizes.c
 *     libtool --mode=link    gcc -W -Wall -g -O2 -o typedef-sizes typedef-sizes.lo
 *
 *  GNU Libtool Install:
 *     libtool --mode=install install -c typedef-sizes /usr/local/bin/typedef-sizes
 *
 *  GNU Libtool Clean:
 *     libtool --mode=clean rm -f typedef-sizes.lo typedef-sizes
 */
#define _DMSTOOLS_SRC_TYPEDEF_SIZES_C 1

///////////////
//           //
//  Headers  //
//           //
///////////////
// MARK: - Headers

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <getopt.h>
#include <inttypes.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <regex.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <pthread.h>
#include <dirent.h>
#include <search.h>
#include <ftw.h>
#include <sys/stat.h>
#include <termios.h>
#include <sched.h>
#include <unistd.h>
#include <glob.h>
#include <iconv.h>
#include <setjmp.h>
#include <wchar.h>
#include <stddef.h>
#include <signal.h>
#include <aio.h>
#include <fcntl.h>
#include <fstab.h>
#include <grp.h>
#include <pwd.h>
#include <netdb.h>
#include <net/if.h>
#include <sys/uio.h>
#include <sys/time.h>
#include <locale.h>
#include <sys/socket.h>
#include <stdarg.h>
#include <utime.h>
#include <sys/utsname.h>
#include <sys/un.h>
#include <sys/resource.h>
#include <wctype.h>


///////////////////
//               //
//  Definitions  //
//               //
///////////////////
// MARK: - Definitions

#ifndef PROGRAM_NAME
#define PROGRAM_NAME "typedef-sizes"
#endif
#ifndef PACKAGE_BUGREPORT
#define PACKAGE_BUGREPORT "david@syzdek.net"
#endif
#ifndef PACKAGE_COPYRIGHT
#define PACKAGE_COPYRIGHT ""
#endif
#ifndef PACKAGE_NAME
#define PACKAGE_NAME ""
#endif
#ifndef PACKAGE_VERSION
#define PACKAGE_VERSION ""
#endif

#ifndef PARAMS
#define PARAMS(protos) protos
#endif


#define MY_NAME_WIDTH   25
#define MY_SIZE_WIDTH   8
#define MY_SIGN_WIDTH   8
#define MY_FILE_WIDTH   0

#define MY_SORT_REVERSE    0x1000
#define MY_SORT_NAME       0x00
#define MY_SORT_NAME_R     (MY_SORT_NAME|MY_SORT_REVERSE)
#define MY_SORT_WIDTH      0x02
#define MY_SORT_WIDTH_R    (MY_SORT_WIDTH|MY_SORT_REVERSE)
#define MY_SORT_INCLUDE    0x04
#define MY_SORT_INCLUDE_R  (MY_SORT_INCLUDE|MY_SORT_REVERSE)


/////////////////
//             //
//  Datatypes  //
//             //
/////////////////
// MARK: - Datatypes

struct my_data
{
   const char * name;
   int          size;
   int          is_signed;
   const char * include;
};


//////////////////
//              //
//  Prototypes  //
//              //
//////////////////
// MARK: - Prototypes

extern int
main(
         int                           argc,
         char *                        argv[] );


static int
my_cmp_by_define(
         void *                        ptr1,
         void *                        ptr2 );


static int
my_cmp_by_define_r(
         void *                        ptr1,
         void *                        ptr2 );


static int
my_cmp_by_name(
         void *                        ptr1,
         void *                        ptr2 );


static int
my_cmp_by_name_r(
         void *                        ptr1,
         void *                        ptr2 );


static int
my_cmp_by_width(
         void *                        ptr1,
         void *                        ptr2 );


static int
my_cmp_by_width_r(
         void *                        ptr1,
         void *                        ptr2 );


static void
my_print_data(
         struct my_data *              data );


static void
my_usage( void );


static void
my_version( void );


/////////////////
//             //
//  Functions  //
//             //
/////////////////
// MARK: - Functions

/// main statement
/// @param[in] argc   number of arguments
/// @param[in] argv   array of arguments
int
main(
         int                           argc,
         char *                        argv[] )
{
   int                     c;
   int                     x;
   int                     y;
   int                     err;
   int                     sort_order;
   int                     sort_reverse;
   int                     opt_index;
   int                    list_count;
   char                    msg[128];
   regex_t                 regex;
   regmatch_t              matches[2];
   struct my_data       ** ptr;
   struct my_data       ** list;
   static struct my_data   data[] =
   {
//      { "DIR",                      sizeof(DIR),                     -1,  "<dirent.h>" },
      { "FILE",                     sizeof(FILE),                    -1,  "<stdio.h>" },
      { "NULL",                     sizeof(NULL),                    -1,  "<stddef.h>" },
      { "VISIT",                    sizeof(VISIT),                   -1,  "<search.h>" },
//      { "__ftw64_func_t",           sizeof(__ftw64_func_t),          -1,  "<ftw.h>" },
//      { "__ftw_func_t",             sizeof(__ftw_func_t),            -1,  "<ftw.h>" },
//      { "__nftw64_func_t",          sizeof(__nftw64_func_t),         -1,  "<ftw.h>" },
//      { "__nftw_func_t",            sizeof(__nftw_func_t),           -1,  "<ftw.h>" },
//      { "blkcnt64_t",               sizeof(blkcnt64_t),              -1,  "<sys/stat.h>" },
      { "blkcnt_t",                 sizeof(blkcnt_t),                -1,  "<sys/stat.h>" },
      { "cc_t",                     sizeof(cc_t),                    -1,  "<termios.h>" },
      { "char *",                   sizeof(char *),                   0,  "libc" },
      { "char",                     sizeof(char),                     1,  "libc" },
      { "clock_t",                  sizeof(clock_t),                 -1,  "<time.h>" },
//      { "comparison_fn_t",          sizeof(comparison_fn_t),         -1,  "<stdlib.h>" },
//      { "cookie_close_function",    0,                               -1,  "unknown" },
//      { "cookie_io_functions_t",    0,                               -1,  "unknown" },
//      { "cookie_read_function",     0,                               -1,  "unknown" },
//      { "cookie_seek_function",     0,                               -1,  "unknown" },
//      { "cookie_write_function",    0,                               -1,  "unknown" },
//      { "cpu_set_t",                sizeof(cpu_set_t),               -1,  "<sched.h>" },
      { "dev_t",                    sizeof(dev_t),                   -1,  "<sys/stat.h>" },
      { "div_t",                    sizeof(div_t),                   -1,  "<stdlib.h>" },
      { "double",                   sizeof(double),                   1,  "libc" },
      { "enum mcheck_status",       0,                               -1,  "<mcheck.h>" },
      { "fd_set",                   sizeof(fd_set),                  -1,  "<sys/types.h>" },
      { "float",                    sizeof(float),                    1,  "libc" },
//      { "fpos64_t",                 sizeof(fpos64_t),                -1,  "<stdio.h>" },
      { "fpos_t",                   sizeof(fpos_t),                  -1,  "<stdio.h>" },
      { "gid_t",                    sizeof(gid_t),                   -1,  "<sys/types.h> <unistd.h>" },
//      { "glob64_t",                 sizeof(glob64_t),                -1,  "<glob.h>" },
      { "glob_t",                   sizeof(glob_t),                  -1,  "<glob.h>" },
      { "iconv_t",                  sizeof(iconv_t),                 -1,  "<iconv.h>" },
      { "imaxdiv_t",                sizeof(imaxdiv_t),               -1,  "<stdlib.h>" },
      { "in_addr_t",                sizeof(in_addr_t),                0,  "<arpa/inet.h>" },
      { "in_port_t",                sizeof(in_port_t),                0,  "<arpa/inet.h>" },
//      { "ino64_t",                  sizeof(ino64_t),                 -1,  "<sys/types.h>" },
      { "ino_t",                    sizeof(ino_t),                   -1,  "<sys/types.h>" },
      { "int",                      sizeof(int),                      1,  "libc" },
      { "int16_t",                  sizeof(int16_t),                  1,  "<inttypes.h>" },
      { "int32_t",                  sizeof(int32_t),                  1,  "<inttypes.h>" },
      { "int64_t",                  sizeof(int64_t),                  1,  "<inttypes.h>" },
      { "int8_t",                   sizeof(int8_t),                   1,  "<inttypes.h>" },
      { "intmax_t",                 sizeof(intmax_t),                 1,  "<inttypes.h>" },
      { "jmp_buf",                  sizeof(jmp_buf),                 -1,  "<setjmp.h>" },
      { "ldiv_t",                   sizeof(ldiv_t),                  -1,  "<stdlib.h>" },
      { "lldiv_t",                  sizeof(lldiv_t),                 -1,  "<stdlib.h>" },
      { "long double",              sizeof(long double),              1,  "libc" },
      { "long long",                sizeof(long long),                1,  "libc" },
      { "long",                     sizeof(long),                     1,  "libc" },
      { "mbstate_t",                sizeof(mbstate_t),               -1,  "<wchar.h>" },
      { "mode_t",                   sizeof(mode_t),                  -1,  "<sys/types.h>" },
      { "nlink_t",                  sizeof(nlink_t),                 -1,  "<sys/types.h>" },
//      { "off64_t",                  sizeof(off64_t),                  0,  "<unistd.h>" },
      { "off_t",                    sizeof(off_t),                    0,  "<unistd.h>" },
      { "pid_t",                    sizeof(pid_t),                    1,  "<unistd.h> <sys/types.h>" },
//      { "printf_arginfo_function",  0,                               -1,  "unknown" },
//      { "printf_function",          0,                               -1,  "unknown" },
      { "pthread_attr_t",           sizeof(pthread_attr_t),          -1,  "<pthread.h>" },
      { "pthread_cond_t",           sizeof(pthread_cond_t),          -1,  "<pthread.h>" },
      { "pthread_condattr_t",       sizeof(pthread_condattr_t),      -1,  "<pthread.h>" },
      { "pthread_key_t",            sizeof(pthread_key_t),           -1,  "<pthread.h>" },
      { "pthread_mutex_t",          sizeof(pthread_mutex_t),         -1,  "<pthread.h>" },
      { "pthread_mutexattr_t",      sizeof(pthread_mutexattr_t),     -1,  "<pthread.h>" },
      { "pthread_once_t",           sizeof(pthread_once_t),          -1,  "<pthread.h>" },
      { "pthread_rwlock_t",         sizeof(pthread_rwlock_t),        -1,  "<pthread.h>" },
      { "pthread_rwlockattr_t",     sizeof(pthread_rwlockattr_t),    -1,  "<pthread.h>" },
      { "pthread_t",                sizeof(pthread_t),               -1,  "<pthread.h>" },
      { "ptrdiff_t",                sizeof(ptrdiff_t),               -1,  "<stddef.h>" },
      { "regex_t",                  sizeof(regex_t),                 -1,  "<regex.h>" },
      { "regmatch_t",               sizeof(regmatch_t),              -1,  "<regex.h>" },
      { "regoff_t",                 sizeof(regoff_t),                -1,  "<regex.h>" },
      { "sa_family_t",              sizeof(sa_family_t),              0,  "<arpa/inet.h>" },
      { "sig_atomic_t",             sizeof(sig_atomic_t),            -1,  "<signal.h>" },
//      { "sighandler_t",             sizeof(sighandler_t),            -1,  "<signal.h>" },
      { "sigjmp_buf",               sizeof(sigjmp_buf),              -1,  "<setjmp.h>" },
      { "sigset_t",                 sizeof(sigset_t),                -1,  "<signal.h>" },
      { "size_t",                   sizeof(size_t),                   0,  "<sys/types.h>" },
      { "socklen_t",                sizeof(socklen_t),                0,  "<socket.h>" },
      { "speed_t",                  sizeof(speed_t),                 -1,  "<termios.h>" },
      { "ssize_t",                  sizeof(ssize_t),                  1,  "<unistd.h>" },
      { "stack_t",                  sizeof(stack_t),                 -1,  "<signal.h>" },
      { "struct ENTRY *",           sizeof(struct ENTRY *),          -1,  "<search.h>" },
//      { "struct FTW",               sizeof(struct FTW),              -1,  "<ftw.h>" },
//      { "struct __gconv_step",      sizeof(struct __gconv_step),     -1,  "<gconv.h>" },
//      { "struct __gconv_step_data", sizeof(struct __gconv_step_data), -1,  "<gconv.h>" },
      { "struct aiocb",             sizeof(struct aiocb),            -1,  "<aio.h>" },
//      { "struct aiocb64",           sizeof(struct aiocb64),          -1,  "<aio.h>" },
//      { "struct aioinit",           sizeof(struct aioinit),          -1,  "<aio.h>" },
//      { "struct argp",              sizeof(struct argp),             -1,  "<argp.h>" },
//      { "struct argp_child",        sizeof(struct argp_child),       -1,  "<argp.h>" },
//      { "struct argp_option",       sizeof(struct argp_option),      -1,  "<argp.h>" },
//      { "struct argp_state",        sizeof(struct argp_state),       -1,  "<argp.h>" },
      { "struct dirent",            sizeof(struct dirent),           -1,  "<dirent.h>" },
//      { "struct exit_status *",     sizeof(struct exit_status *),    -1,  "<utmp.h>" },
      { "struct flock",             sizeof(struct flock),            -1,  "<fcntl.h>" },
      { "struct fstab",             sizeof(struct fstab),            -1,  "<fstab.h>" },
      { "struct group",             sizeof(struct group),            -1,  "<grp.h>" },
      { "struct hostent",           sizeof(struct hostent),          -1,  "<netdb.h>" },
      { "struct if_nameindex",      sizeof(struct if_nameindex),     -1,  "<net/if.h>" },
      { "struct in6_addr",          sizeof(struct in6_addr),         -1,  "<netinet/in.h>" },
      { "struct in_addr",           sizeof(struct in_addr),          -1,  "<netinet/in.h>" },
      { "struct iovec",             sizeof(struct iovec),            -1,  "<sys/uio.h>" },
      { "struct itimerval",         sizeof(struct itimerval),        -1,  "<sys/time.h>" },
      { "struct lconv",             sizeof(struct lconv),            -1,  "<locale.h>" },
      { "struct linger",            sizeof(struct linger),           -1,  "<sys/socket.h>" },
//      { "struct mallinfo *",        sizeof(struct mallinfo *),       -1,  "<malloc.h>" },
//      { "struct mntent",            sizeof(struct mntent),           -1,  "<fstab.h>" },
      { "struct netent",            sizeof(struct netent),           -1,  "<netdb.h>" },
//      { "struct ntptimeval",        sizeof(struct ntptimeval),       -1,  "<sys/timex.h>" },
//      { "struct obstack",           sizeof(struct obstack),          -1,  "<obstack.h>" },
      { "struct option",            sizeof(struct option),           -1,  "<getopt.h>" },
      { "struct passwd",            sizeof(struct passwd),           -1,  "<pwd.h>" },
//      { "struct printf_info",       sizeof(struct printf_info),      -1,  "<printf.h>" },
      { "struct protoent",          sizeof(struct protoent),         -1,  "<netdb.h>" },
//      { "struct random_data",       sizeof(struct random_data),      -1,  "<stdlib.h>" },
      { "struct rlimit",            sizeof(struct rlimit),           -1,  "<sys/resource.h>" },
//      { "struct rlimit64",          sizeof(struct rlimit64),         -1,  "<sys/resource.h>" },
      { "struct rusage",            sizeof(struct rusage),           -1,  "<sys/resource.h>" },
      { "struct sched_param",       sizeof(struct sched_param),      -1,  "<sched.h>" },
      { "struct servent",           sizeof(struct servent),          -1,  "<netdb.h>" },
//      { "struct sgttyb",            sizeof(struct sgttyb),           -1,  "<sgtty.h>" },
      { "struct sigaction",         sizeof(struct sigaction),        -1,  "<signal.h>" },
      { "struct sigstack",          sizeof(struct sigstack),         -1,  "<signal.h>" },
      { "struct sigvec",            sizeof(struct sigvec),           -1,  "<signal.h>" },
      { "struct sockaddr",          sizeof(struct sockaddr),         -1,  "<sys/socket.h>" },
      { "struct sockaddr_in",       sizeof(struct sockaddr_in),      -1,  "<netinet/in.h>" },
      { "struct sockaddr_in6",      sizeof(struct sockaddr_in6),     -1,  "<netinet/in.h>" },
      { "struct sockaddr_un",       sizeof(struct sockaddr_un),      -1,  "<sys/un.h>" },
      { "struct stat",              sizeof(struct stat),             -1,  "<sys/stat.h>" },
//      { "struct stat64",            sizeof(struct stat64),           -1,  "<sys/stat.h>" },
      { "struct termios",           sizeof(struct termios),          -1,  "<termios.h>" },
      { "struct timespec",          sizeof(struct timespec),         -1,  "<sys/time.h>" },
      { "struct timeval",           sizeof(struct timeval),          -1,  "<sys/time.h>" },
//      { "struct timex",             sizeof(struct timex),            -1,  "<sys/timex.h>" },
      { "struct timezone",          sizeof(struct timezone),         -1,  "<sys/time.h>" },
      { "struct tm",                sizeof(struct tm),               -1,  "<time.h>" },
//      { "struct tms",               sizeof(struct tms),              -1,  "<sys/times.h>" },
      { "struct utimbuf",           sizeof(struct utimbuf),          -1,  "<utime.h>" },
//      { "struct utmp",              sizeof(struct utmp),             -1,  "<utmp.h>" },
//      { "struct utmpx *",           sizeof(struct utmpx *),          -1,  "<utmp.h>" },
      { "struct utsname",           sizeof(struct utsname),          -1,  "<sys/utsname.h>" },
//      { "struct vtimes",            sizeof(struct vtimes),           -1,  "<sys/resource.h>" },
      { "tcflag_t",                 sizeof(tcflag_t),                -1,  "<termios.h>" },
      { "time_t",                   sizeof(time_t),                   0,  "<time.h>" },
//      { "ucontext_t",               sizeof(ucontext_t),              -1,  "<ucontext.h>" },
      { "uid_t",                    sizeof(uid_t),                   -1,  "<sys/types.h> <unistd.h>" },
      { "uint16_t",                 sizeof(uint16_t),                 0,  "<inttypes.h>" },
      { "uint32_t",                 sizeof(uint32_t),                 0,  "<inttypes.h>" },
      { "uint64_t",                 sizeof(uint64_t),                 0,  "<inttypes.h>" },
      { "uint8_t",                  sizeof(uint8_t),                  0,  "<inttypes.h>" },
      { "uintmax_t",                sizeof(uintmax_t),                0,  "<inttypes.h>" },
      { "union wait",               0,                               -1,  "<sys/wait.h>" },
      { "unsigned",                 sizeof(unsigned),                 0,  "libc" },
      { "va_list",                  sizeof(va_list),                 -1,  "<stdarg.h>" },
      { "void *",                   sizeof(void *),                   0,  "<stdarg.h>" },
      { "wchar_t",                  sizeof(wchar_t),                 -1,  "<stddef.h>" },
      { "wchar_t",                  sizeof(wchar_t),                 -1,  "<wchar.h>" },
//      { "wctrans_t",                sizeof(wctrans_t),               -1,  "<wchar.h>" },
      { "wctype_t",                 sizeof(wctype_t),                -1,  "<wchar.h>" },
      { "wint_t",                   sizeof(wint_t),                  -1,  "<wchar.h>" },
//      { "wordexp_t",                sizeof(wordexp_t),               -1,  "<wordexp.h>" },
      { NULL, 0, 0, NULL}
   };

   // getopt options
   static char   short_opt[] = "dhnrVw";
   static struct option long_opt[] =
   {
      {"help",          no_argument, 0, 'h'},
      {"perl",          no_argument, 0, 'p'},
      {"version",       no_argument, 0, 'V'},
      {NULL,            0,           0, 0  }
   };

   list           = NULL;
   list_count     = 0;
   opt_index      = 0;
   sort_order     = MY_SORT_NAME;
   sort_reverse   = 0;

   while((c = getopt_long(argc, argv, short_opt, long_opt, &opt_index)) != -1)
   {
      switch(c)
      {
         case -1:	/* no more arguments */
         case 0:	/* long options toggles */
         break;

         case 'd':
         sort_order = MY_SORT_INCLUDE;
         break;

         case 'h':
         my_usage();
         return(0);

         case 'n':
         sort_order = MY_SORT_NAME;
         break;

         case 'r':
         sort_reverse = (sort_reverse ^ MY_SORT_REVERSE);
         break;

         case 'V':
         my_version();
         return(0);

         case 'w':
         sort_order = MY_SORT_WIDTH;
         break;

         case '?':
         fprintf(stderr, "Try `%s --help' for more information.\n", PROGRAM_NAME);
         return(1);

         default:
         fprintf(stderr, "%s: unrecognized option `--%c'\n", PROGRAM_NAME, c);
         fprintf(stderr, "Try `%s --help' for more information.\n", PROGRAM_NAME);
         return(1);
      };
   };

   sort_order |= sort_reverse;

   if (optind == argc)
   {
      optind--;
      argv[optind] = strdup(".*");
   };

   for(x = optind; x < argc; x++)
   {
      ptr = NULL;
      if ((err = regcomp(&regex, argv[x], REG_EXTENDED|REG_ICASE)))
      {
         regerror(err, &regex, msg, (size_t)127);
         fprintf(stderr, PROGRAM_NAME ": %s\n", msg);
         return(1);
      };
      for(y = 0; data[y].name; y++)
         if ( (!(regexec(&regex, data[y].name, (size_t)2, matches, 0))) ||
              (!(regexec(&regex, data[y].include, (size_t)2, matches, 0))) )
         {
            list_count++;
            if (!(ptr = realloc(list, sizeof(struct my_data)*list_count)))
            {
               fprintf(stderr, PROGRAM_NAME ": out of virtual memory\n");
               return(1);
            };
            list = ptr;
            list[list_count-1] = &data[y];
         };
      regfree(&regex);
      if (!(ptr))
      {
         fprintf(stderr, PROGRAM_NAME ": data type `%s' not found\n", argv[x]);
         if (list)
            free(list);
         return(1);
      };
   };

   switch(sort_order)
   {
      case MY_SORT_NAME:
      qsort(list, (size_t)list_count, sizeof(struct my_data *), (int (*)(const void *, const void *))my_cmp_by_name);
      break;

      case MY_SORT_NAME_R:
      qsort(list, (size_t)list_count, sizeof(struct my_data *), (int (*)(const void *, const void *))my_cmp_by_name_r);
      break;

      case MY_SORT_WIDTH:
      qsort(list, (size_t)list_count, sizeof(struct my_data *), (int (*)(const void *, const void *))my_cmp_by_width);
      break;

      case MY_SORT_WIDTH_R:
      qsort(list, (size_t)list_count, sizeof(struct my_data *), (int (*)(const void *, const void *))my_cmp_by_width_r);
      break;

      case MY_SORT_INCLUDE:
      qsort(list, (size_t)list_count, sizeof(struct my_data *), (int (*)(const void *, const void *))my_cmp_by_define);
      break;

      case MY_SORT_INCLUDE_R:
      qsort(list, (size_t)list_count, sizeof(struct my_data *), (int (*)(const void *, const void *))my_cmp_by_define_r);
      break;

      default:
      qsort(list, (size_t)list_count, sizeof(struct my_data *), (int (*)(const void *, const void *))my_cmp_by_name);
      break;
   };

   printf("%-*s   %*s   %*s   %*s\n",
      MY_NAME_WIDTH,  "name:",
      MY_SIZE_WIDTH,  "width:",
      MY_SIGN_WIDTH,  "signed:",
      MY_FILE_WIDTH,  "defined"
   );
   for(x = 0; x < list_count; x++)
      my_print_data(list[x]);
   printf("\n");

   free(list);

   return(0);
}


/// compares by define
/// @param[in]  ptr1   pointer to first string
/// @param[in]  ptr2   pointer to second string
int
my_cmp_by_define(
         void *                        ptr1,
         void *                        ptr2 )
{
   int cmp;
   const struct my_data * d1 = *((const struct my_data **)ptr1);
   const struct my_data * d2 = *((const struct my_data **)ptr2);

   if ((!(d1)) && (!(d2)))
      return(0);
   if ((!(d1)) || (!(d2)))
      return(d1 ? 1 : -1);

   if ((cmp = strcasecmp(d1->include, d2->include)))
      return(cmp);

   if ((cmp = strcasecmp(d1->name, d2->name)))
      return(cmp);

   if ((cmp = ((int)d1->size) - ((int)d2->size)))
      return(cmp);

   return(0);
}


/// compares by reverse define
/// @param[in]  ptr1   pointer to first string
/// @param[in]  ptr2   pointer to second string
int
my_cmp_by_define_r(
         void *                        ptr1,
         void *                        ptr2 )
{
   int cmp;
   const struct my_data * d1 = *((const struct my_data **)ptr1);
   const struct my_data * d2 = *((const struct my_data **)ptr2);

   if ((!(d1)) && (!(d2)))
      return(0);
   if ((!(d1)) || (!(d2)))
      return(d1 ? -1 : 1);

   if ((cmp = strcasecmp(d1->include, d2->include)))
      return(cmp * -1);

   if ((cmp = strcasecmp(d1->name, d2->name)))
      return(cmp * -1);

   if ((cmp = ((int)d1->size) - ((int)d2->size)))
      return(cmp * -1);

   return(0);
}


/// compares by name
/// @param[in]  ptr1   pointer to first string
/// @param[in]  ptr2   pointer to second string
int
my_cmp_by_name(
         void *                        ptr1,
         void *                        ptr2 )
{
   int cmp;
   const struct my_data * d1 = *((const struct my_data **)ptr1);
   const struct my_data * d2 = *((const struct my_data **)ptr2);

   if ((!(d1)) && (!(d2)))
      return(0);
   if ((!(d1)) || (!(d2)))
      return(d1 ? 1 : -1);

   if ((cmp = strcasecmp(d1->name, d2->name)))
      return(cmp);

   if ((cmp = strcasecmp(d1->include, d2->include)))
      return(cmp);

   if ((cmp = ((int)d1->size) - ((int)d2->size)))
      return(cmp);

   return(0);
}


/// compares by reverse name
/// @param[in]  ptr1   pointer to first string
/// @param[in]  ptr2   pointer to second string
int
my_cmp_by_name_r(
         void *                        ptr1,
         void *                        ptr2 )
{
   int cmp;
   const struct my_data * d1 = *((const struct my_data **)ptr1);
   const struct my_data * d2 = *((const struct my_data **)ptr2);

   if ((!(d1)) && (!(d2)))
      return(0);
   if ((!(d1)) || (!(d2)))
      return(d1 ? -1 : 1);

   if ((cmp = strcasecmp(d1->name, d2->name)))
      return(cmp * -1);

   if ((cmp = strcasecmp(d1->include, d2->include)))
      return(cmp * -1);

   if ((cmp = ((int)d1->size) - ((int)d2->size)))
      return(cmp * -1);

   return(0);
}


/// compares by width
/// @param[in]  ptr1   pointer to first string
/// @param[in]  ptr2   pointer to second string
int
my_cmp_by_width(
         void *                        ptr1,
         void *                        ptr2 )
{
   int cmp;
   const struct my_data * d1 = *((const struct my_data **)ptr1);
   const struct my_data * d2 = *((const struct my_data **)ptr2);

   if ((!(d1)) && (!(d2)))
      return(0);
   if ((!(d1)) || (!(d2)))
      return(d1 ? 1 : -1);

   if ((cmp = ((int)d1->size) - ((int)d2->size)))
      return(cmp);

   if ((cmp = strcasecmp(d1->name, d2->name)))
      return(cmp);

   if ((cmp = strcasecmp(d1->include, d2->include)))
      return(cmp);

   return(0);
}


/// compares by reverse width
/// @param[in]  ptr1   pointer to first string
/// @param[in]  ptr2   pointer to second string
int
my_cmp_by_width_r(
         void *                        ptr1,
         void *                        ptr2 )
{
   int cmp;
   const struct my_data * d1 = *((const struct my_data **)ptr1);
   const struct my_data * d2 = *((const struct my_data **)ptr2);

   if ((!(d1)) && (!(d2)))
      return(0);
   if ((!(d1)) || (!(d2)))
      return(d1 ? -1 : 1);

   if ((cmp = ((int)d1->size) - ((int)d2->size)))
      return(cmp * -1);

   if ((cmp = strcasecmp(d1->name, d2->name)))
      return(cmp * -1);

   if ((cmp = strcasecmp(d1->include, d2->include)))
      return(cmp * -1);

   return(0);
}


/// prints data record
/// @param[in]  data    data to print
void
my_print_data(
         struct my_data *              data )
{
   char buff[16];
   snprintf(buff, (size_t)16, "%i", data->size);
   printf("%-*s   %*s   %*s   %-*s\n",
      MY_NAME_WIDTH,  data->name,
      MY_SIZE_WIDTH,  data->size ? buff : " ",
      MY_SIGN_WIDTH,  data->is_signed == 1 ? "yes" : (data->is_signed == 0 ? "no" : " "),
      MY_FILE_WIDTH,  data->include ? data->include : ""
   );
   return;
}


/// displays usage information
void
my_usage( void )
{
   printf("Usage: %s [options] [typedef]\n", PROGRAM_NAME);
   printf("  -d                        sort by object definition\n");
   printf("  -h, --help                print this help and exit\n");
   printf("  -n                        sort by object name\n");
   printf("  -r                        reverse sort order\n");
   printf("  -t type                   data type to display\n");
   printf("  -V, --version             print verbose messages\n");
   printf("  -w                        sort by width of object\n");
   printf("\n");
   printf("Report bugs to <%s>.\n", PACKAGE_BUGREPORT);
   return;
}


/// displays version information
void
my_version( void )
{
   printf("%s (%s) %s\n", PROGRAM_NAME, PACKAGE_NAME, PACKAGE_VERSION);
   printf("Written by David M. Syzdek.\n");
   printf("\n");
   printf("%s\n", PACKAGE_COPYRIGHT);
   printf("This is free software; see the source for copying conditions.  There is NO\n");
   printf("warranty; not even for MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.\n");
   return;
}

// end of source file
