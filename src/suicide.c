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
#include <stdlib.h>
#include <sys/types.h>
#include <signal.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <getopt.h>


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

/// table of signal data
typedef struct suicide_signal_data SuicideData;
struct suicide_signal_data
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

// compares two signal rows by signal int value
int suicide_cmp_val PARAMS((const SuicideData * p1,
   const SuicideData * p2));

// finds a signal from the table and returns action
int suicide_find_data PARAMS((SuicideData ** data, const char * str, int val));

// empty signal handler
void suicide_signal_handler PARAMS((int sig));

// displays SUSv3 signals
void suicide_susv3_sigs PARAMS((void));

// display C code to ignore signals
void suicide_susv3_sigs_ignore PARAMS((void));

// displays usage
void suicide_usage PARAMS((void));

// displays version
void suicide_version PARAMS((void));


/////////////////
//             //
//  Variables  //
//             //
/////////////////

SuicideData susv3_signals[] =
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
#ifdef SIGEMT
   { ' ', SIGEMT,      "SIGEMT",      "EMT instruction" },
#endif
#ifdef SIGIO
   { ' ', SIGIO,       "SIGIO",       "input/output possible signal" },
#endif
#ifdef SIGINFO
   { ' ', SIGINFO,     "SIGINFO",     "information request" },
#endif
#ifdef SIGWINCH
   { ' ', SIGWINCH,    "SIGWINCH",    "window size changes" },
#endif
#ifdef SIGIOT
   { ' ', SIGIOT,      "SIGIOT",      "IOT trap (4.2 BSD)." },
#endif
#ifdef SIGSTKFLT
   { ' ', SIGSTKFLT,   "SIGSTKFLT",   "Stack fault." },
#endif
#ifdef SIGPWR
   { ' ', SIGPWR,      "SIGPWR",      "Power failure restart (System V)." },
#endif
   { 'N', 0,           "NONE",        "N/A" },
   { -1, -1, NULL, NULL }
};

volatile sig_atomic_t my_toggle_verbose = 0;


/////////////////
//             //
//  Functions  //
//             //
/////////////////

/// compares two signal rows by signal int value
/// @param[in]  p1    pointer to first signal
/// @param[in]  p2    pointer to second signal
int suicide_cmp_val(const SuicideData * p1, const SuicideData * p2)
{
   if (p1->number < p2->number)
      return(-1);
   if (p1->number > p2->number)
      return(1);
   return(0);
}


/// main statement
/// @param[in]  argc  number of arguments passed to program
/// @param[in]  argv  array of arguments passed to program
int main(int argc, char * argv[])
{
   int              c;
   int              val;
   int              verbose;
   int              attempt_catch;
   int              opt_index;
   int              sig;
   char             buff[16];
   pid_t            p;   // process ID
   pid_t            pp;  // parent process ID
   SuicideData    * data;

   static char   short_opt[] = "chin:s:SvV";
   static struct option long_opt[] =
   {
      {"help",          no_argument, 0, 'h'},
      {"signals",       no_argument, 0, 's'},
      {"verbose",       no_argument, 0, 'v'},
      {"version",       no_argument, 0, 'V'},
      {NULL,            0,           0, 0  }
   };

   p           = getpid();
   pp          = getppid();
   sig         = 0;
   verbose     = 0;
   opt_index   = 0;
   attempt_catch = 0;

   while((c = getopt_long(argc, argv, short_opt, long_opt, &opt_index)) != -1)
   {
      switch(c)
      {
         case -1:	/* no more arguments */
         case 0:	/* long options toggles */
         break;

         case 'c':
         attempt_catch = 1;
         break;

         case 'h':
         suicide_usage();
         return(0);

         case 'i':
         suicide_susv3_sigs_ignore();
         return(0);

         case 'n':
         val = (int)strtol(optarg, NULL, 0);
         if ((sig = suicide_find_data(&data, NULL, val)) == -1)
            return(1);
         break;

         case 's':
         if ((sig = suicide_find_data(&data, optarg, -1)) == -1)
            return(1);
         break;

         case 'S':
         suicide_susv3_sigs();
         return(0);

         case 'V':
         suicide_version();
         return(0);

         case 'v':
         verbose = 1;
         break;

         case '?':
         fprintf(stderr, "Try `%s --help' for more information.\n", PROGRAM_NAME);
         return(1);

         default:
         fprintf(stderr, ("%s: unrecognized option `--%c'\n"
               "Try `%s --help' for more information.\n"
            ),  PROGRAM_NAME, c, PROGRAM_NAME
         );
         return(1);
      };
   };

   if (verbose)
   {
      if ((data))
         printf(">> %-9s == %i\n", data->name, sig);
      printf(">> getppid() == %i\n", pp);
      printf(">> getpid()  == %i\n", p);
   };

   if ((data))
      snprintf(buff, 16L, "%s", data->name);
   else
      snprintf(buff, 16L, "%i", sig);
   if (attempt_catch)
   {
      if (verbose)
         printf(">> signal(%s)\n", buff);
      signal(sig, suicide_signal_handler);
   };
   printf(">> kill(%i, %s)\n", p, buff);
   kill(p, sig);
   if (verbose)
      printf(">> usleep(100)\n");
   usleep(100);

   if (verbose)
      printf(">> return(0)\n");

   return(0);
}


/// empty signal handler
void suicide_signal_handler(int sig)
{
   int i;
   const char * name;
   signal(sig, SIG_IGN);
   name = NULL;
   for(i = 0; susv3_signals[i].name; i++)
      if (susv3_signals[i].number == sig)
         name =  susv3_signals[i].name;
   if (name)
      printf("   -- caught %s --\n", name);
   else
      printf("   -- caught signal %i --\n", sig);
   signal(sig, suicide_signal_handler);
   return;
}


/// finds a signal from the table and returns action
/// @param[in]  str
/// @param[in]  val
int suicide_find_data(SuicideData ** datap, const char * str,
   int val)
{
   int    i;
   size_t offset;

   if ((datap))
      *datap = NULL;

   if (str)
   {
      offset = (strncasecmp("sig", str, 3L)) ? 3 : 0;
      for(i = 0; susv3_signals[i].name; i++)
      {
         if (!(strcasecmp(&susv3_signals[i].name[offset], str)))
         {
            if ((datap))
               *datap = &susv3_signals[i];
            return(susv3_signals[i].number);
         };
      };
      fprintf(stderr, PROGRAM_NAME ": unknown signal\n");
      fprintf(stderr, "Try `%s --help' for more information.\n", PROGRAM_NAME);
      return(-1);
   };

   if ((datap))
      for(i = 0; susv3_signals[i].name; i++)
         if (susv3_signals[i].number == val)
            *datap = &susv3_signals[i];

   return(val);
}


/// displays SUSv3 signals
void suicide_susv3_sigs(void)
{
   size_t count;
   size_t i;
   printf("Available Single Unix Specification v3 Signals\n");
   printf("   Signal      Value  Action  Description\n");
   for(i = 0; susv3_signals[i].desc; i++);
   count = i;
   qsort(&susv3_signals, count, sizeof(SuicideData), (int (*)(const void *, const void *))suicide_cmp_val);
   for(i = 0; i < (count+1); i++)
      if (susv3_signals[i].name)
         printf("   %-9s   %-5i  %c       %s\n", susv3_signals[i].name, susv3_signals[i].number, susv3_signals[i].action, susv3_signals[i].desc);
   printf
   (
      "Actions:\n"
      "   T  Abnormal termination of the process. The process is terminated \n"
      "      with all the consequences of _exit() except that the status made\n"
      "      available to wait() and waitpid() indicates abnormal termination\n"
      "      by the specified signal.\n"
      "   A  Abnormal termination of the process. Additionally, implementation\n"
      "      defined abnormal termination actions, such as creation of a core file,\n"
      "      may occur.\n"
      "   I  Ignore the signal.\n"
      "   S  Stop the process.\n"
      "   C  Continue the process, if it is stopped; otherwise, ignore the signal.\n"
   );
   return;
}


/// displays SUSv3 signals
void suicide_susv3_sigs_ignore(void)
{
   size_t count;
   size_t i;
   for(i = 0; susv3_signals[i].desc; i++);
   count = i;
   qsort(&susv3_signals, count, sizeof(SuicideData), (int (*)(const void *, const void *))suicide_cmp_val);
   printf("// ignore SUSv3 signals\n");
   for(i = 0; i < (count+1); i++)
      if (susv3_signals[i].name)
         printf("singal(%s, SIG_IGN);\n", susv3_signals[i].name);
   return;
}


/// displays usage
void suicide_usage(void)
{
   printf(("Usage: %s [OPTIONS]\n"
         "  -c                        attempt to catch signals\n"
         "  -i                        print C code to ignore signals\n"
         "  -h, --help                print this help and exit\n"
         "  -n signum                 signal number to send\n"
         "  -s sigspec                signal name to send\n"
         "  -S, --signals             print SUSv3 signal table\n"
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
   printf(("%s (%s) %s\n"
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
