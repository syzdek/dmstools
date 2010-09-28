/*
 *  DMS Tools and Utilities
 *  Copyright (C) 2010 David M. Syzdek <david@syzdek.net>.
 *
 *  Self Signalling Program
 *  Copyright (c) 2008 Alaska Communications Systems
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 */
/*
 *  @file src/suicide.c  Utility for testing default signal behavior
 */
/*
 *  Simple Build:
 *     gcc -W -Wall -O2 -c suicide.c
 *     gcc -W -Wall -O2 -o suicide   suicide.o
 *
 *  GNU Libtool Build:
 *     libtool --mode=compile gcc -W -Wall -g -O2 -c suicide.c
 *     libtool --mode=link    gcc -W -Wall -g -O2 -o suicide suicide.lo
 *
 *  GNU Libtool Install:
 *     libtool --mode=install install -c suicide /usr/local/bin/suicide
 *
 *  GNU Libtool Uninstall:
 *     libtool --mode=uninstall rm -f /usr/local/bin/suicide
 *
 *  GNU Libtool Clean:
 *     libtool --mode=clean rm -f suicide.lo suicide
 */
#define _DMSTOOLS_SRC_SUICIDE_C 1

///////////////
//           //
//  Headers  //
//           //
///////////////

#ifdef HAVE_COMMON_H
#include "common.h"
#endif

#include <stdio.h>
#include <sys/types.h>
#include <signal.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>


///////////////////
//               //
//  i18l Support //
//               //
///////////////////

#ifdef HAVE_GETTEXT
#   include <gettext.h>
#   include <libintl.h>
#   define _(String) gettext (String)
#   define gettext_noop(String) String
#   define N_(String) gettext_noop (String)
#else
#   define _(String) (String)
#   define N_(String) String
#   define textdomain(Domain)
#   define bindtextdomain(Package, Directory)
#endif


///////////////////
//               //
//  Definitions  //
//               //
///////////////////

#ifndef PROGRAM_NAME
#define PROGRAM_NAME "suicide"
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

#define MY_LIST_LEN 32

#define MY_ACTION_SIGNAL	0x01
#define MY_ACTION_HANDLE	0x02
#define MY_ACTION_IGNORE	0x03
#define MY_ACTION_BLOCK		0x04
#define MY_ACTION_UNBLOCK	0x05

#ifndef PARAMS
#define PARAMS(protos) protos
#endif

/////////////////
//             //
//  Datatypes  //
//             //
/////////////////

/// signal action data
typedef struct my_signal_action MyAction;
struct my_signal_action
{
   int number;
   int action;
};


/// Utility configuration
typedef struct my_runtime_config MyConf;
struct my_runtime_config
{
   int count;
   MyAction pair[MY_LIST_LEN];
};


/// table of signal data
struct my_signal_data
{
   int action;
   int number;
   const char * name;
   const char * desc;
};


//////////////////
//              //
//  Prototypes  //
//              //
//////////////////

// processes signals
int main PARAMS((int argc, char * argv[]));

// processes custom command line options
int suicide_getopt PARAMS((int argc, char * argv[], MyConf * cnf));

// empty signal handler
void suicide_signal_handler PARAMS((int sig));

// verifies signal number
int suicide_signum PARAMS((const char * str));

// interprets signal name
int suicide_sigspec PARAMS((const char * str));

// displays SUSv3 signals
int suicide_susv3_sigs PARAMS((void));

// displays usage
void suicide_usage PARAMS((void));

// displays version
void suicide_version PARAMS((void));


/////////////////
//             //
//  Variables  //
//             //
/////////////////

static const struct my_signal_data susv3_signals[] =
{
#ifdef SIGABRT
   { 'A', SIGABRT,     "SIGABRT",     "Process abort signal." },
#endif
#ifdef SIGALRM
   { 'T', SIGALRM,     "SIGALRM",     "Alarm clock." },
#endif
#ifdef SIGBUS
   { 'A', SIGBUS,      "SIGBUS",      "Access to an undefined portion of a memory object." },
#endif
#ifdef SIGCHLD
   { 'I', SIGCHLD,     "SIGCHLD",     "Child process terminated, stopped, or continued." },
#endif
#ifdef SIGCONT
   { 'C', SIGCONT,     "SIGCONT",     "Continue executing, if stopped." },
#endif
#ifdef SIGFPE
   { 'A', SIGFPE,      "SIGFPE",      "Erroneous arithmetic operation." },
#endif
#ifdef SIGHUP
   { 'T', SIGHUP,      "SIGHUP",      "Hangup." },
#endif
#ifdef SIGILL
   { 'A', SIGILL,      "SIGILL",      "Illegal instruction." },
#endif
#ifdef SIGINT
   { 'T', SIGINT,      "SIGINT",      "Terminal interrupt signal." },
#endif
#ifdef SIGKILL
   { 'T', SIGKILL,     "SIGKILL",     "Kill (cannot be caught or ignored)." },
#endif
#ifdef SIGPIPE
   { 'T', SIGPIPE,     "SIGPIPE",     "Write on a pipe with no one to read it." },
#endif
#ifdef SIGQUIT
   { 'A', SIGQUIT,     "SIGQUIT",     "Terminal quit signal." },
#endif
#ifdef SIGSEGV
   { 'A', SIGSEGV,     "SIGSEGV",     "Invalid memory reference." },
#endif
#ifdef SIGSTOP
   { 'S', SIGSTOP,     "SIGSTOP",     "Stop executing (cannot be caught or ignored)." },
#endif
#ifdef SIGTERM
   { 'T', SIGTERM,     "SIGTERM",     "Termination signal." },
#endif
#ifdef SIGTSTP
   { 'S', SIGTSTP,     "SIGTSTP",     "Terminal stop signal." },
#endif
#ifdef SIGTTIN
   { 'S', SIGTTIN,     "SIGTTIN",     "Background process attempting read." },
#endif
#ifdef SIGTTOU
   { 'S', SIGTTOU,     "SIGTTOU",     "Background process attempting write." },
#endif
#ifdef SIGUSR1
   { 'T', SIGUSR1,     "SIGUSR1",     "User-defined signal 1." },
#endif
#ifdef SIGUSR2
   { 'T', SIGUSR2,     "SIGUSR2",     "User-defined signal 2." },
#endif
#ifdef SIGPOLL
   { 'T', SIGPOLL,     "SIGPOLL",     "Pollable event." },
#endif
#ifdef SIGPROF
   { 'T', SIGPROF,     "SIGPROF",     "Profiling timer expired." },
#endif
#ifdef SIGSYS
   { 'A', SIGSYS,      "SIGSYS",      "Bad system call." },
#endif
#ifdef SIGTRAP
   { 'A', SIGTRAP,     "SIGTRAP",     "Trace/breakpoint trap." },
#endif
#ifdef SIGURG
   { 'I', SIGURG,      "SIGURG",      "High bandwidth data is available at a socket." },
#endif
#ifdef SIGVTALRM
   { 'T', SIGVTALRM,   "SIGVTALRM",   "Virtual timer expired." },
#endif
#ifdef SIGXCPU
   { 'A', SIGXCPU,     "SIGXCPU",     "CPU time limit exceeded." },
#endif
#ifdef SIGXFSZ
   { 'A', SIGXFSZ,     "SIGXFSZ",     "File size limit exceeded." },
#endif
   { 'N', 0,           "NONE",        "N/A" },
   { 0, -1, NULL, NULL }
};

volatile sig_atomic_t my_toggle_verbose = 0;


/////////////////
//             //
//  Functions  //
//             //
/////////////////

/// main statement
/// @param[in]  argc  number of arguments passed to program
/// @param[in]  argv  array of arguments passed to program
int main(int argc, char * argv[])
{
   int   i;
   int   e;
   int   s;	// signal number
   pid_t p;	// process ID
   MyConf cnf;

   if ((s = suicide_getopt(argc, argv, &cnf)) == -1)
      return(1);

signal(SIGHUP, suicide_signal_handler);

   p = getpid();
   if (my_toggle_verbose)
   {
      printf("getppid() == %i\n", getppid());
      printf("getpid() == %i\n", p);
   };

   for(i = 0; i < cnf.count; i++)
   {
      switch(cnf.pair[cnf.count].action)
      {
         case MY_ACTION_SIGNAL:
            if (my_toggle_verbose)
               printf("kill(%i, %i) == ", p, s);
            fflush(stdout);
            e = kill(p, s);
            if (my_toggle_verbose)
               printf("%i\n", e);
            break;
         case MY_ACTION_HANDLE:
            break;
         default:
            printf("unknown action\n");
            break;
      };
   };

   printf("return(0) == 0\n");
   return(0);
}


/// processes custom command line options
/// @param[in]  argc  number of arguments passed to program
/// @param[in]  argv  array of arguments passed to program
/// @param[out] cnf   configuration memory
int suicide_getopt(int argc, char * argv[], MyConf * cnf)
{
   int i;
   int sig;

   my_toggle_verbose = 0;
   sig = -1;
   memset(cnf, 0, sizeof(MyConf));

   // Due to the wierd command options such as
   // -SIG and -NUM, it is not practical to use
   // getopt() or getopt_long().  As a result
   // the following custom code is used.

   for(i = 1; i < argc; i++)
   {
      if ((!(strcasecmp(argv[i], "--help"))) || (!(strcmp(argv[i], "-h"))))
      {
         suicide_usage();
         return(-1);
      }

      else if ( (!(strcasecmp(argv[i], "--version"))) || (!(strcmp(argv[i], "-V"))))
      {
         suicide_version();
         return(-1);
      }

      else if (!(strcasecmp(argv[i], "--signals")))
         return(suicide_susv3_sigs());
      else if (!(strcmp(argv[i], "-S")))
         return(suicide_susv3_sigs());

      else if (!(strcasecmp(argv[i], "--verbose")))
         my_toggle_verbose = 1;
      else if (!(strcmp(argv[i], "-v")))
         my_toggle_verbose = 1;

      else if (!(strcmp(argv[i], "-s")))
      {
         i++;
         if (i == argc)
         {
            fprintf(stderr, "%s: option requires an argument -- s\n", PROGRAM_NAME);
            fprintf(stderr, "Try `%s --help' for more information.\n", PROGRAM_NAME);
            return(-1);
         };
         if ((sig = suicide_sigspec(argv[i])) == -1)
         {
            fprintf(stderr, "%s: invalid signal name -- %s\n", PROGRAM_NAME, argv[i]);
            fprintf(stderr, "Try `%s --signals' for more information.\n", PROGRAM_NAME);
            return(-1);
         };
         cnf->pair[cnf->count].number = sig;
         cnf->pair[cnf->count].action = MY_ACTION_SIGNAL;
         cnf->count++;
      }

      else if (!(strcmp(argv[i], "-n")))
      {
         i++;
         if (i == argc)
         {
            fprintf(stderr, "%s: option requires an argument -- n\n", PROGRAM_NAME);
            fprintf(stderr, "Try `%s --help' for more information.\n", PROGRAM_NAME);
            return(-1);
         };
         if ((sig = suicide_signum(argv[i])) == -1)
         {
            fprintf(stderr, "%s: invalid signal number -- %s\n", PROGRAM_NAME, argv[i]);
            fprintf(stderr, "Try `%s --signals' for more information.\n", PROGRAM_NAME);
            return(-1);
         };
         cnf->pair[cnf->count].number = sig;
         cnf->pair[cnf->count].action = MY_ACTION_SIGNAL;
         cnf->count++;
      }

      else
      {
         if ((sig = suicide_signum(&argv[i][1])) == -1)
         {
            if ((sig = suicide_sigspec(&argv[i][1])) == -1)
            {
               fprintf(stderr, "%s: invalid option -- %s\n", PROGRAM_NAME, argv[i]);
               fprintf(stderr, "Try `%s --help' for more information.\n", PROGRAM_NAME);
               return(-1);
            };
         };
         cnf->pair[cnf->count].number = sig;
         cnf->pair[cnf->count].action = MY_ACTION_SIGNAL;
         cnf->count++;
      };

      if ((cnf->count - 1) >= MY_LIST_LEN)
      {
         fprintf(stderr, "%s: too many signal actions specified\n", PROGRAM_NAME);
         return(-1);
      };
   };

   
   if (!(cnf->count))
   {
      fprintf(stderr, "%s: missing required arguments\n", PROGRAM_NAME);
      fprintf(stderr, "Try `%s --help' for more information.\n", PROGRAM_NAME);
      return(-1);
   };

   return(0);
}


/// empty signal handler
void suicide_signal_handler(int sig)
{
   int i;
   signal(sig, SIG_IGN);
   if (my_toggle_verbose)
      for(i = 0; susv3_signals[i].name; i++)
         if (susv3_signals[i].number == sig)
            printf("<< caught %s >> ", susv3_signals[i].name);
   signal(sig, suicide_signal_handler);
   return;
}


/// verifies signal number
int suicide_signum(const char * str)
{
   unsigned u;
   int sig;
   for(u = 0; u < strlen(str); u++)
      if ((str[u] < '0') || (str[u] > '9'))
         return(-1);
   sig = atol(str);
   for(u = 0; susv3_signals[u].name; u++)
      if (susv3_signals[u].number == sig)
         return(sig);
   return(-1);
}


/// interprets signal name
int suicide_sigspec(const char * str)
{
   int i;
   for(i = 0; susv3_signals[i].name; i++)
      if (!(strcasecmp(str, susv3_signals[i].name)))
         return(susv3_signals[i].number);
   for(i = 0; susv3_signals[i].name; i++)
      if (!(strcasecmp(str, &susv3_signals[i].name[3])))
         return(susv3_signals[i].number);
   return(-1);
}


/// displays SUSv3 signals
int suicide_susv3_sigs(void)
{
   int i;
   printf("Available Single Unix Specification v3 Signals\n");
   printf("   Signal      Value  Action  Description\n");
   for(i = 0; susv3_signals[i].desc; i++)
      if (susv3_signals[i].number > 0)
         printf("   %-9s   %-5i  %c       %s\n", susv3_signals[i].name, susv3_signals[i].number, susv3_signals[i].action, susv3_signals[i].desc);
   return(-1);
}


/// displays usage
void suicide_usage(void)
{
   // TRANSLATORS: The following strings provide usage for command. These
   // strings are displayed if the program is passed `--help' on the command
   // line. The two strings referenced are: PROGRAM_NAME, and
   // PACKAGE_BUGREPORT
   printf(_("Usage: %s [OPTIONS]\n"
         "  -signum                   signal number to send\n"
         "  -sigspec                  signal name to send\n"
         "  -n signum                 signal number to send\n"
         "  -s sigspec                signal name to send\n"
         "  -S, --signals             print SUSv3 signal table\n"
         "  -h, --help                print this help and exit\n"
         "  -v, --verbose             print verbose messages\n"
         "  -V, --version             print version number and exit\n"
         "\n"
         "This utility is used to test the behavior of unhandled\n"
         "signals without needing to run a program and using the kill\n"
         "utility.  This utility will send itself the specified signal\n"
         "emulating the behavior of kill on itself.\n"
         "\n"
         "If kill is for sending signals to other processes on the\n"
         "system, then suicide is used for sending signals to itself.\n"
         "\n"
         "Report bugs to <%s>.\n"
      ), PROGRAM_NAME, PACKAGE_BUGREPORT
   );
   return;
}


/// displays version
void suicide_version(void)
{
   // TRANSLATORS: The following strings provide version and copyright
   // information if the program is passed --version on the command line.
   // The three strings referenced are: PROGRAM_NAME, PACKAGE_NAME,
   // PACKAGE_VERSION.
   printf(_("%s (%s) %s\n"
         "Written by David M. Syzdek.\n"
         "\n"
         "%s\n"
         "This is free software; see the source for copying conditions.  There is NO\n"
         "warranty; not even for MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.\n"
      ), PROGRAM_NAME, PACKAGE_NAME, PACKAGE_VERSION, PACKAGE_COPYRIGHT
   );
   return;
}

/* end of source file */
