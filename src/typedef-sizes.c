/*
 *  DMS Tools and Utilities
 *  Copyright (C) 2009 David M. Syzdek <david@syzdek.net>.
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

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <getopt.h>
#include <inttypes.h>
#include <stdio.h>
#include <string.h>
#include <arpa/inet.h>


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


#define MY_NAME_WIDTH   20
#define MY_SIZE_WIDTH   5
#define MY_SIGN_WIDTH   8
#define MY_FILE_WIDTH   0


/////////////////
//             //
//  Structs    //
//             //
/////////////////

struct my_data
{
   const char * name;
   int          size;
   int          is_signed;
   const char * include;
};


////////////////////////
//                    //
//  Global Variables  //
//                    //
////////////////////////

const char * typedef_sizes_perl[] =
{
   "if (@ARGV != 1)",
   "{",
   "   printf(\"Usage: typedef-sizes --perl | perl - <filename>\\n\");",
   "   exit(1);",
   "};",
   "open(FILE, $ARGV[0]) || die \"typedef-sizes-perl: unable to open $ARGV[0]\";",
   "while(<FILE>)",
   "{",
   "   my $line = $_;",
   "   chomp($line);",
   "   printf(\"{ \\\"%s\\\", sizeof(%s) },\\n\", $line, $line);",
   "};",
   "close(FILE);",
   NULL
};

//////////////////
//              //
//  Prototypes  //
//              //
//////////////////

// main statement
int main(int argc, char * argv[]);

// prints data record
void my_print_data(struct my_data * data);

//displays usage information
void my_usage();

// displays version information
void my_version(void);


//////////////////
//              //
//  Prototypes  //
//              //
//////////////////

/// main statement
/// @param[in] argc   number of arguments
/// @param[in] argv   array of arguments
int main(int argc, char * argv[])
{
   int                     c;
   int                     x;
   int                     y;
   int                     opt_index;
   struct my_data        * ptr;
   static struct my_data   data[] =
   {
      { "char",        sizeof(char),        1,  NULL },
      { "char *",      sizeof(char *),      0,  NULL },
      { "double",      sizeof(double),      0,  NULL },
      { "float",       sizeof(float),       0,  NULL },
      { "in_addr_t",   sizeof(in_addr_t),   0,  "<arpa/inet.h>" },
      { "int",         sizeof(int),         1,  NULL },
      { "int16_t",     sizeof(int16_t),     1,  "<inttypes.h>" },
      { "int32_t",     sizeof(int32_t),     1,  "<inttypes.h>" },
      { "int64_t",     sizeof(int64_t),     1,  "<inttypes.h>" },
      { "int8_t",      sizeof(int8_t),      1,  "<inttypes.h>" },
      { "intmax_t",    sizeof(intmax_t),    1,  "<inttypes.h>" },
      { "long",        sizeof(long),        1,  NULL },
      { "long double", sizeof(long double), 1,  NULL },
      { "long long",   sizeof(long long),   1,  NULL },
      { "off_t",       sizeof(off_t),       0,  NULL },
      { "size_t",      sizeof(size_t),      0,  NULL },
      { "struct sockaddr_in6",      sizeof(struct sockaddr_in6),      0,  "<netinet/in.h>" },
      { "time_t",      sizeof(time_t),      0,  "<time.h>" },
      { "uint16_t",    sizeof(uint16_t),    0,  "<inttypes.h>" },
      { "uint32_t",    sizeof(uint32_t),    0,  "<inttypes.h>" },
      { "uint64_t",    sizeof(uint64_t),    0,  "<inttypes.h>" },
      { "uint8_t",     sizeof(uint8_t),     0,  "<inttypes.h>" },
      { "uintmax_t",   sizeof(uintmax_t),   0,  "<inttypes.h>" },
      { "unsigned",    sizeof(unsigned),    0,  NULL  },
      { NULL, 0, 0, NULL}
   };

   // getopt options
   static char   short_opt[] = "htV";
   static struct option long_opt[] =
   {
      {"help",          no_argument, 0, 'h'},
      {"perl",          no_argument, 0, 'p'},
      {"version",       no_argument, 0, 'V'},
      {NULL,            0,           0, 0  }
   };

   while((c = getopt_long(argc, argv, short_opt, long_opt, &opt_index)) != -1)
   {
      switch(c)
      {
         case -1:	/* no more arguments */
         case 0:	/* long options toggles */
            break;
         case 'h':
            my_usage();
            return(0);
         case 'p':
            for(x = 0; typedef_sizes_perl[x]; x++)
               printf("%s\n", typedef_sizes_perl[x]);
            return(0);
         case 'V':
            my_version();
            return(0);
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

   if ((opt_index+1) < argc)
   {
      printf("%-*s   %*s   %*s   %*s\n",
         MY_NAME_WIDTH,  "name:",
         MY_SIZE_WIDTH,  "size:",
         MY_SIGN_WIDTH,  "signed:",
         MY_FILE_WIDTH,  "#include"
      );
      for(x = opt_index + 1; x < argc; x++)
      {
         ptr = NULL;
         for(y = 0; data[y].name; y++)
            if (!(strcasecmp(data[y].name, argv[x])))
               ptr = &data[y];
         if (!(ptr))
         {
            fprintf(stderr, PROGRAM_NAME ": data type `%s' not found\n", argv[x]);
            return(1);
         };
         my_print_data(ptr);
      };
      printf("\n");
      return(0);
   };

   printf("%-*s   %*s   %*s   %*s\n",
      MY_NAME_WIDTH,  "name:",
      MY_SIZE_WIDTH,  "size:",
      MY_SIGN_WIDTH,  "signed:",
      MY_FILE_WIDTH,  "#include"
   );
   for(c = 0; data[c].name; c++)
      my_print_data(&data[c]);
   printf("\n");

   return(0);
}


/// prints data record
/// @param[in]  data    data to print
void my_print_data(struct my_data * data)
{
   printf("%-*s   %-*i    %-*s  %-*s\n",
      MY_NAME_WIDTH,  data->name,
      MY_SIZE_WIDTH,  data->size,
      MY_SIGN_WIDTH,  data->is_signed == 1 ? "yes" : (data->is_signed == 0 ? "no" : ""),
      MY_FILE_WIDTH,  data->include ? data->include : ""
   );
   return;
}


/// displays usage information
void my_usage()
{
   // TRANSLATORS: The following strings provide usage for command. These
   // strings are displayed if the program is passed `--help' on the command
   // line. The two strings referenced are: PROGRAM_NAME, and
   // PACKAGE_BUGREPORT
   printf(_("Usage: %s [options] [typedef]\n"
         "  -h, --help                print this help and exit\n"
         "  -t type                   data type to display\n"
         "  -V, --version             print verbose messages\n"
         "\n"
         "Report bugs to <%s>.\n"
      ), PROGRAM_NAME, PACKAGE_BUGREPORT
   );
   return;
}


/// displays version information
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

// end of source file
