/*
 *  DMS Tools and Utilities
 *  Copyright (c) 2008, 2009 David M. Syzdek <david@syzdek.net>.
 *
 *  @SYZDEK_LICENSE_HEADER_START@
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
 *
 *  @SYZDEK_LICENSE_HEADER_END@
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

// main statement
int main PARAMS((int, char **));

// displays POSIX regular expressions
void my_posixregex PARAMS((void));

// displays usage
void my_usage PARAMS((void));

// display incompatible usage
void my_usage_incompatible PARAMS((char x, char y));

// displays usage
void my_version PARAMS((void));


/////////////////
//             //
//  Functions  //
//             //
/////////////////

/// main statement
/// @param[in] argc   number of arguments
/// @param[in] argv   array of arguments
int main(int argc, char * argv[])
{
   // declare local vars
   int          c;
   int          x;
   unsigned     y;
   int          err;
   int          exitcode;
   int          str_len;
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
      unsigned     nmatch;
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
               fprintf(stderr, _("%s: option `-n' requires option `-b'\n"
                     "Try `%s --help' for more information.\n"
                  ),  PROGRAM_NAME, PROGRAM_NAME
               );
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
            fprintf(stderr, _("Try `%s --help' for more information.\n"), PROGRAM_NAME);
            return(1);
         default:
            fprintf(stderr, _("%s: unrecognized option `--%c'\n"
                  "Try `%s --help' for more information.\n"
               ),  PROGRAM_NAME, c, PROGRAM_NAME
            );
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
      fprintf(stderr, _("%s: missing required argument `-- r'\n"
            "Try `%s --help' for more information.\n"
         ), PROGRAM_NAME, PROGRAM_NAME
      );
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
void my_posixregex(void)
{
   // TRANSLATORS: The following strings provide a table of POSIX regular
   // expersion key words, key word values, and a description of the key
   // word.  The table is displayed if the program is passed --posixregex
   // on the command line.
   printf(_("POSIX        ASCII           Description\n"
          "[:alnum:]    [A-Za-z0-9]     Alphanumeric characters\n"
          "[:alpha:]    [A-Za-z]        Alphabetic characters\n"
          "[:blank:]    [ \\t]           Space and tab\n"
          "[:cntrl:]    [\\x00-\\x1F\\x7F] Control characters\n"
          "[:digit:]    [0-9]           Digits\n"
          "[:graph:]    [\\x21-\\x7E]     Visible characters\n"
          "[:lower:]    [a-z]           Lowercase letters\n"
          "[:print:]    [\\x20-\\x7E]     Visible characters and spaces\n"
          "[:space:]    [ \\t\\r\\n\\v\\f]   Whitespace characters\n"
          "[:upper:]    [A-Z]           Uppercase letters\n"
          "[:xdigit:]   [A-Fa-f0-9]     Hexadecimal digits\n"
          "[:punct:]    [-!\"#$%%&'()*+,./:;<=>?@[\\]_`{|}~]   Punctuation characters\n"
          "\n"
          "Example Uses:\n"
          "    posixregex -r '^([[:alpha:]]+)://(([[:alnum:]]+)(:([[:alnum:]]+)){0,1}@){0,1}([-.a-z]+)(:([[:digit:]]+))*/([-/[:alnum:]]*)(\\?(.*)){0,1}$'  http://jdoe:password@www.foo.org:123/path/to/file?query_string\n"
          "    posixregex -r '\\$([[:digit:]]+)\\.([[:digit:]]{2,2})' 'Your change is $7.45.'\n"
      )
   );
   return;
}


/// displays usage
void my_usage(void)
{
// TRANSLATORS: The following strings provide usage for command. These
   // strings are displayed if the program is passed `--help' on the command
   // line. The two strings referenced are: PROGRAM_NAME, and
   // PACKAGE_BUGREPORT
   printf(_("Usage: %s [OPTIONS] string1 string2 ... stringN\n"
         "  -b                        use obsolete ``basic'' REs (REG_BASIC)\n"
         "  -h, --help                print this help and exit\n"
         "  -i                        ignore upper/lower case distinctions. (REG_ICASE)\n"
         "  -l                        enable newline-sensitive matching (REG_NEWLINE)\n"
         "  -n                        turn off recoginition of special characters (REG_NOSPEC)\n"
         "  -p, --posixregex          print regular expression patterns\n"
         "  -q, --quiet, --silent     do not print messages\n"
         "  -r regex                  regular expression to use for testing strings\n"
         "  -s                        display sub string which matches entire regular expression\n"
         "  -v, --verbose             print verbose messages\n"
         "  -V, --version             print version number and exit\n"
         "\n"
         "Variables:\n"
         "  POSIXREGEX                default regular expression to use for testing strings\n"
         "\n"
         "Report bugs to <%s>.\n"
      ), PROGRAM_NAME, PACKAGE_BUGREPORT
   );
   return;
}


/// display incompatible usage
void my_usage_incompatible(char x, char y)
{
   fprintf(stderr,
      _(
         "%s: incompatible options `-%c' and `-%c'\n"
         "Try `%s --help' for more information.\n"
      ),  PROGRAM_NAME, x, y, PROGRAM_NAME
   );
   return;
}


/// displays version
void my_version(void)
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
