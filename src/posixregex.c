/*
 *  DMS Tools and Utilities
 *  Copyright (c) 2008, 2009 David M. Syzdek <david@syzdek.net>.
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
 *  @file src/posixregex.c simple POSIX regex testing utility/example
 */
/*
 *  Simple Build:
 *     gcc -W -Wall -O2 -c posixregex.c
 *     gcc -W -Wall -O2 -o posixregex   posixregex.o
 *
 *  GNU Libtool Build:
 *     libtool --mode=compile gcc -W -Wall -g -O2 -c posixregex.c
 *     libtool --mode=link    gcc -W -Wall -g -O2 -o posixregex posixregex.lo
 *
 *  GNU Libtool Install:
 *     libtool --mode=install install -c posixregex /usr/local/bin/posixregex
 *
 *  GNU Libtool Clean:
 *     libtool --mode=clean rm -f posixregex.lo posixregex
 */
#define _SCT_SRC_POSIXREGEX_C 1

/////////////
//         //
//  Notes  //
//         //
/////////////
// MARK: - Notes

//
//  POSIX        ASCII           Description
//  [:alnum:]    [A-Za-z0-9]     Alphanumeric characters
//  [:alpha:]    [A-Za-z]        Alphabetic characters
//  [:blank:]    [ \t]           Space and tab
//  [:cntrl:]    [\x00-\x1F\x7F] Control characters
//  [:digit:]    [0-9]           Digits
//  [:graph:]    [\x21-\x7E]     Visible characters
//  [:lower:]    [a-z]           Lowercase letters
//  [:print:]    [\x20-\x7E]     Visible characters and spaces
//  [:space:]    [ \t\r\n\v\f]   Whitespace characters
//  [:upper:]    [A-Z]           Uppercase letters
//  [:xdigit:]   [A-Fa-f0-9]     Hexadecimal digits
//  [:punct:]    [-!"#$%&'()*+,./:;<=>?@[\\\]_`{|}~]   Punctuation characters
//
//   Example Uses:
//      posixregex -rv '^([[:alpha:]]+)://(([[:alnum:]]+)(:([[:alnum:]]+)){0,1}@){0,1}([-.a-z]+)(:([[:digit:]]+))*/([-/[:alnum:]]*)(\?(.*)){0,1}$'  http://jdoe:password@www.foo.org:123/path/to/file?query_string
//      posixregex -rv '\$([[:digit:]]+)\.([[:digit:]]{2,2})' 'Your change is $7.45.'


///////////////
//           //
//  Headers  //
//           //
///////////////
// MARK: - Headers

#ifdef HAVE_CONFIG_H
#include <config.h>
#include "common.h"
#endif

#include <stdio.h>
#include <getopt.h>
#include <string.h>
#include <stdlib.h>
#include <regex.h>


///////////////////
//               //
//  Definitions  //
//               //
///////////////////
// MARK: - Definitions

#ifndef PROGRAM_NAME
#define PROGRAM_NAME "posixregex"
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

#undef BUFFER_SIZE
#define BUFFER_SIZE 1024

#undef MAX_MATCHES
#define MAX_MATCHES 128

#ifndef PARAMS
#define PARAMS(protos) protos
#endif


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


static void
my_posixregex( void );


static void
my_usage( void );


static void
my_usage_incompatible(
         char                          x,
         char                          y );


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
   // declare local vars
   int          c;
   int          x;
   unsigned     y;
   int          err;
   int          exitcode;
   ssize_t      str_len;
   int          opt_index;
   char         arg[BUFFER_SIZE];
   char         msg[BUFFER_SIZE];
   char         str[BUFFER_SIZE];
   regex_t      regex;
   regmatch_t   matches[MAX_MATCHES];

   // toggled options
   struct
   {
      const char * re_str;
      int          quiet;
      int          verbosity;
      int          substring;
      int          reg_cflags;
      size_t       nmatch;
   } opt;

   // getopt options
   static char   short_opt[] = "bhilnpqr:svV";
   static struct option long_opt[] =
   {
      {"help",          no_argument, 0, 'h'},
      {"posixregex",    no_argument, 0, 'p'},
      {"quiet",         no_argument, 0, 'q'},
      {"silent",        no_argument, 0, 'q'},
      {"verbose",       no_argument, 0, 'v'},
      {"version",       no_argument, 0, 'V'},
      {NULL,            0,           0, 0  }
   };

   // initialize variables
   memset(&opt, 0, sizeof(opt));
   opt.reg_cflags = REG_EXTENDED;
   exitcode       = 0;
   opt_index      = 0;

   // loops through arguments
   while((c = getopt_long(argc, argv, short_opt, long_opt, &opt_index)) != -1)
   {
      switch(c)
      {
         case -1:       // no more arguments
         case 0:        // long options toggles
         break;

         case 'b':
         opt.reg_cflags = opt.reg_cflags & (~REG_EXTENDED);
         break;

         case 'h':
         my_usage();
         return(0);

         case 'i':
         opt.reg_cflags = opt.reg_cflags | REG_ICASE;
         break;

         case 'l':
         opt.reg_cflags = opt.reg_cflags | REG_NEWLINE;
         break;

         case 'n':
         if ((opt.reg_cflags & REG_EXTENDED))
         {
            fprintf(stderr, "%s: option `-n' requires option `-b'\n", PROGRAM_NAME);
            fprintf(stderr, "Try `%s --help' for more information.\n", PROGRAM_NAME);
            return(1);
         };
#ifdef REG_NOSPEC
         opt.reg_cflags = opt.reg_cflags | REG_NOSPEC;
#endif
         break;

         case 'p':
         my_posixregex();
         return(0);

         case 'q':
         opt.quiet = 1;
         break;

         case 'r':
         opt.re_str = optarg;
         break;

         case 's':
         opt.substring++;
         break;

         case 'V':
         my_version();
         return(0);

         case 'v':
         opt.verbosity++;
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

   // check for incompatible options
   if ( ((opt.verbosity)) && ((opt.quiet)) )
   {
      my_usage_incompatible('v', 'q');
      return(1);
   };
   if ( ((opt.verbosity)) && ((opt.substring)) )
   {
      my_usage_incompatible('v', 's');
      return(1);
   };
   if ( ((opt.quiet)) && ((opt.substring)) )
   {
      my_usage_incompatible('q', 's');
      return(1);
   };

   // checking for regular expressions string
   if (!(opt.re_str))
      opt.re_str = getenv("POSIXREGEX");
   if (!(opt.re_str))
   {
      fprintf(stderr, "%s: missing required argument `-- r'\n", PROGRAM_NAME);
      fprintf(stderr, "Try `%s --help' for more information.\n", PROGRAM_NAME);
      return(1);
   };

   // compile regular expression for later use
   if (!(opt.quiet))
      printf("regex: %s\n", opt.re_str);
   if ((err = regcomp(&regex, opt.re_str, opt.reg_cflags)))
   {
      regerror(err, &regex, msg, (size_t)BUFFER_SIZE-1);
      printf("%s\n", msg);
      return(1);
   };

   // determines number of subexpressions
   opt.nmatch = (MAX_MATCHES > regex.re_nsub) ? regex.re_nsub : MAX_MATCHES;
   opt.nmatch++;

   // Loops through command line arguments and tests the compiled expressions
   // against each argument.
   for (x = optind; x < argc; x++)
   {
      // copies string into buffer and prints to screen
      strncpy(arg, argv[x], (size_t)BUFFER_SIZE-1);
      if ( (!(opt.quiet)) && (!(opt.substring)) )
         printf("%3i: %s  ==> ", x-optind+1, arg);

      // tests the buffer against the regular expression
      err = regexec(&regex, arg, opt.nmatch, matches, 0);

      // skips displaying substring if error was encountered
      if ( ((err)) && ((opt.substring)) )
      {
         continue;
      }
      // displays substring if not in summary mode
      else if ( (!(err)) && ((opt.substring)) )
      {
         // copies sub matches in buffer string
         if ((str_len = matches[0].rm_eo - matches[0].rm_so))
         {
            strncpy(str, &arg[matches[0].rm_so], (size_t)str_len);
            printf("%s\n", str);
         };
      }
      // displays result in summary mode
      else if ((err))
      {
         if (!(opt.quiet))
            printf("not found\n");
         if (opt.verbosity)
            printf("\n");
         exitcode = 1;
      }
      // displays result in summary mode
      else if (!(err))
      {
         if (!(opt.quiet))
            printf(" found\n");
         if (opt.verbosity)
         {
            // displays submatches
            for(y = 0; (y < opt.nmatch); y++)
            {
               // copies sub matches in buffer string
               memset(str, 0, (size_t)BUFFER_SIZE);
               if ((str_len = matches[y].rm_eo - matches[y].rm_so) > 0)
               {
                  strncpy(str, &arg[matches[y].rm_so], (size_t)str_len);
                  if (!(y))
                     printf("     full match: %s\n", str);
                  else
                     printf("     submatch %u: %s\n", y, str);
               } else {
                  printf("     submatch %u:\n", y);
               };
            };
            printf("\n");
         };
      };
   };

   // free regular expression
   regfree(&regex);

   // end function
   return(exitcode);
}


/// displays POSIX regular expressions
void
my_posixregex( void )
{
   printf("POSIX        ASCII           Description\n");
   printf("[:alnum:]    [A-Za-z0-9]     Alphanumeric characters\n");
   printf("[:alpha:]    [A-Za-z]        Alphabetic characters\n");
   printf("[:blank:]    [ \\t]           Space and tab\n");
   printf("[:cntrl:]    [\\x00-\\x1F\\x7F] Control characters\n");
   printf("[:digit:]    [0-9]           Digits\n");
   printf("[:graph:]    [\\x21-\\x7E]     Visible characters\n");
   printf("[:lower:]    [a-z]           Lowercase letters\n");
   printf("[:print:]    [\\x20-\\x7E]     Visible characters and spaces\n");
   printf("[:space:]    [ \\t\\r\\n\\v\\f]   Whitespace characters\n");
   printf("[:upper:]    [A-Z]           Uppercase letters\n");
   printf("[:xdigit:]   [A-Fa-f0-9]     Hexadecimal digits\n");
   printf("[:punct:]    [-!\"#$%%&'()*+,./:;<=>?@[\\]_`{|}~]   Punctuation characters\n");
   printf("\n");
   printf("Example Uses:\n");
   printf("    posixregex -r '^([[:alpha:]]+)://(([[:alnum:]]+)(:([[:alnum:]]+)){0,1}@){0,1}([-.a-z]+)(:([[:digit:]]+))*/([-/[:alnum:]]*)(\\?(.*)){0,1}$'  http://jdoe:password@www.foo.org:123/path/to/file?query_string\n");
   printf("    posixregex -r '\\$([[:digit:]]+)\\.([[:digit:]]{2,2})' 'Your change is $7.45.'\n");
   return;
}


/// displays usage
void
my_usage( void )
{
   printf("Usage: %s [OPTIONS] string1 string2 ... stringN\n", PROGRAM_NAME);
   printf("  -b                        use obsolete ``basic'' REs (REG_BASIC)\n");
   printf("  -h, --help                print this help and exit\n");
   printf("  -i                        ignore upper/lower case distinctions. (REG_ICASE)\n");
   printf("  -l                        enable newline-sensitive matching (REG_NEWLINE)\n");
   printf("  -n                        turn off recoginition of special characters (REG_NOSPEC)\n");
   printf("  -p, --posixregex          print regular expression patterns\n");
   printf("  -q, --quiet, --silent     do not print messages\n");
   printf("  -r regex                  regular expression to use for testing strings\n");
   printf("  -s                        display sub string which matches entire regular expression\n");
   printf("  -v, --verbose             print verbose messages\n");
   printf("  -V, --version             print version number and exit\n");
   printf("\n");
   printf("Variables:\n");
   printf("  POSIXREGEX                default regular expression to use for testing strings\n");
   printf("\n");
   printf("Report bugs to <%s>.\n", PACKAGE_BUGREPORT);
   return;
}


/// display incompatible usage
void
my_usage_incompatible(
         char                          x,
         char                          y )
{
   fprintf(stderr, "%s: incompatible options `-%c' and `-%c'\n", PROGRAM_NAME, x, y);
   fprintf(stderr, "Try `%s --help' for more information.\n", PROGRAM_NAME);
   return;
}


/// displays version
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

/* end of source file */
