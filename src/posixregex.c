/*
 *  DMS Tools and Utilities
 *  Copyright (c) 2008, 2009 David M. Syzdek <david@syzdek.net>.
 *
 *  @SYZDEK_LICENSE_HEADER_START@
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License Version 2 as
 *  published by the Free Software Foundation.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA
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

/*
 *  POSIX        ASCII           Description
 *  [:alnum:]    [A-Za-z0-9]     Alphanumeric characters
 *  [:alpha:]    [A-Za-z]        Alphabetic characters
 *  [:blank:]    [ \t]           Space and tab
 *  [:cntrl:]    [\x00-\x1F\x7F] Control characters
 *  [:digit:]    [0-9]           Digits
 *  [:graph:]    [\x21-\x7E]     Visible characters
 *  [:lower:]    [a-z]           Lowercase letters
 *  [:print:]    [\x20-\x7E]     Visible characters and spaces
 *  [:space:]    [ \t\r\n\v\f]   Whitespace characters
 *  [:upper:]    [A-Z]           Uppercase letters
 *  [:xdigit:]   [A-Fa-f0-9]     Hexadecimal digits
 *  [:punct:]    [-!"#$%&'()*+,./:;<=>?@[\\\]_`{|}~]   Punctuation characters
 */

//   Example Uses:
//      posixregex '^([[:alpha:]]+)://(([[:alnum:]]+)(:([[:alnum:]]+)){0,1}@){0,1}([-.a-z]+)(:([[:digit:]]+))*/([-/[:alnum:]]*)(\?(.*)){0,1}$'  http://jdoe:password@www.foo.org:123/path/to/file?query_string
//      posixregex '\$([[:digit:]]+)\.([[:digit:]]{2,2})' 'Your change is $7.45.'


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

/* main statement */
int main PARAMS((int, char **));

/* displays POSIX regular expressions */
void my_posixregex PARAMS((void));

/* displays usage */
void my_usage PARAMS((void));

/* displays usage */
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
   /* declare local vars */
   int          c;
   int          x;
   int          y;
   int          err;
   int          code;
   int          str_len;
   int          quiet;
   int          verbosity;
   int          opt_index;
   char         arg[BUFFER_SIZE];
   char         msg[BUFFER_SIZE];
   char         str[BUFFER_SIZE];
   regex_t      regex;
   regmatch_t   matches[MAX_MATCHES];
   const char * restr;

   /* getopt options */
   static char   short_opt[] = "hpqr:vV";
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

   /* initialize variables */
   code      = 0;
   restr     = NULL;
   quiet     = 0;
   verbosity = 0;
   opt_index = 0;

   /* loops through arguments */
   while((c = getopt_long(argc, argv, short_opt, long_opt, &opt_index)) != -1)
   {
      switch(c)
      {
         case -1:       /* no more arguments */
         case 0:        /* long options toggles */
            break;
         case 'h':
            my_usage();
            return(0);
         case 'p':
            my_posixregex();
            return(0);
         case 'q':
            quiet = 1;
            verbosity = 0;
            break;
         case 'r':
           restr = optarg;
           break;
         case 'V':
            my_version();
            return(0);
         case 'v':
            quiet = 0;
            verbosity++;
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

   if (!(restr))
      restr = getenv("POSIXREGEX");
   if (!(restr))
   {
      fprintf(stderr, _("%s: missing required argument `-- r'\n"
            "Try `%s --help' for more information.\n"
         ), PROGRAM_NAME, PROGRAM_NAME
      );
      return(1);
   };

   /* compile regular expression for later use */
   if (!(quiet))
      printf("regex: %s\n",restr);
   if ((err = regcomp(&regex, restr, REG_EXTENDED|REG_ICASE)))
   {
      regerror(err, &regex, msg, (size_t)BUFFER_SIZE-1);
      printf("%s\n", msg);
      return(1);
   };

   /*
    *  Loops through command line arguments and tests the compiled expressions
    *  against each argument.
    */
   for (x = optind; x < argc; x++)
   {
      /* copies string into buffer and prints to screen */
      strncpy(arg, argv[x], (size_t)BUFFER_SIZE-1);
      if (!(quiet))
         printf("%3i: %s  ==> ", x-optind+1, arg);

      /* tests the buffer against the regular expression */
      if ((regexec(&regex, arg, (size_t)MAX_MATCHES, matches, 0)))
      {
         if (!(quiet))
            printf("not found\n");
         code = 1;
      } else {
         if (!(quiet))
            printf(" found\n");

         /* copies sub matches in buffer string */
         if (verbosity)
         {
            for(y = 0; ((y < MAX_MATCHES) && (matches[y].rm_eo > -1)); y++)
            {
               memset(str, 0, (size_t)BUFFER_SIZE);
               if ((str_len = matches[y].rm_eo - matches[y].rm_so))
               {
                  strncpy(str, &arg[matches[y].rm_so], (size_t)str_len);
                  printf("     submatch %i: %s\n", y, str);
               } else if (matches[y].rm_eo > 0) {
                  printf("   submatch %i:\n", y);
               };
            };
            printf("\n");
         };
      };
   };

   /* free regular expression */
   regfree(&regex);

   /* end function */
   return(code);
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
         "  -h, --help                print this help and exit\n"
         "  -p, --posixregex          print regular expression patterns\n"
         "  -q, --quiet, --silent     do not print messages\n"
         "  -r regex                  regular expression to use for testing strings\n"
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
