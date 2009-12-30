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
#include <regex.h>
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

#define MY_SORT_REVERSE    0x1000
#define MY_SORT_NAME       0x00
#define MY_SORT_NAME_R     (MY_SORT_NAME|MY_SORT_REVERSE)
#define MY_SORT_WIDTH      0x02
#define MY_SORT_WIDTH_R    (MY_SORT_WIDTH|MY_SORT_REVERSE)
#define MY_SORT_INCLUDE    0x04
#define MY_SORT_INCLUDE_R  (MY_SORT_INCLUDE|MY_SORT_REVERSE)


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


//////////////////
//              //
//  Prototypes  //
//              //
//////////////////

// main statement
int main(int argc, char * argv[]);

// compares by define
int my_cmp_by_define(void * ptr1, void * ptr2);

// compares by reverse define
int my_cmp_by_define_r(void * ptr1, void * ptr2);

// compares by name
int my_cmp_by_name(void * ptr1, void * ptr2);

// compares by reverse name
int my_cmp_by_name_r(void * ptr1, void * ptr2);

// compares by width
int my_cmp_by_width(void * ptr1, void * ptr2);

// compares by reverse width
int my_cmp_by_width_r(void * ptr1, void * ptr2);

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
      { "char",                     sizeof(char),                     1,  NULL },
      { "char *",                   sizeof(char *),                   0,  NULL },
      { "double",                   sizeof(double),                   1,  NULL },
      { "float",                    sizeof(float),                    1,  NULL },
      { "in_addr_t",                sizeof(in_addr_t),                0,  "<arpa/inet.h>" },
      { "int",                      sizeof(int),                      1,  NULL },
      { "int16_t",                  sizeof(int16_t),                  1,  "<inttypes.h>" },
      { "int32_t",                  sizeof(int32_t),                  1,  "<inttypes.h>" },
      { "int64_t",                  sizeof(int64_t),                  1,  "<inttypes.h>" },
      { "int8_t",                   sizeof(int8_t),                   1,  "<inttypes.h>" },
      { "intmax_t",                 sizeof(intmax_t),                 1,  "<inttypes.h>" },
      { "long",                     sizeof(long),                     1,  NULL },
      { "long double",              sizeof(long double),              1,  NULL },
      { "long long",                sizeof(long long),                1,  NULL },
      { "off_t",                    sizeof(off_t),                    0,  NULL },
      { "regex_t",                  sizeof(regex_t),                 -1,  NULL },
      { "regmatch_t",               sizeof(regmatch_t),              -1,  NULL },
      { "size_t",                   sizeof(size_t),                   0,  NULL },
      { "struct sockaddr_in",       sizeof(struct sockaddr_in),      -1,  "<netinet/in.h>" },
      { "struct sockaddr_in6",      sizeof(struct sockaddr_in6),     -1,  "<netinet/in.h>" },
      { "time_t",                   sizeof(time_t),                   0,  "<time.h>" },
      { "uint16_t",                 sizeof(uint16_t),                 0,  "<inttypes.h>" },
      { "uint32_t",                 sizeof(uint32_t),                 0,  "<inttypes.h>" },
      { "uint64_t",                 sizeof(uint64_t),                 0,  "<inttypes.h>" },
      { "uint8_t",                  sizeof(uint8_t),                  0,  "<inttypes.h>" },
      { "uintmax_t",                sizeof(uintmax_t),                0,  "<inttypes.h>" },
      { "unsigned",                 sizeof(unsigned),                 0,  NULL  },
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

   sort_order |= sort_reverse;

   if (optind < argc)
      optind++;
   else
   {
      optind--;
      argv[optind] = ".*";
   };

   for(x = optind; x < argc; x++)
   {
      ptr = NULL;
      if ((err = regcomp(&regex, argv[x], REG_EXTENDED|REG_ICASE)))
      {
         regerror(err, &regex, msg, 127);
         fprintf(stderr, PROGRAM_NAME ": %s\n", msg);
         return(1);
      };
      for(y = 0; data[y].name; y++)
         if ( (!(err = regexec(&regex, data[y].name, 2, matches, 0))) ||
              (!(err = regexec(&regex, data[y].include, 2, matches, 0))) )
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
         qsort(list, list_count, sizeof(struct my_data *), (int (*)(const void *, const void *))my_cmp_by_name);
         break;
      case MY_SORT_NAME_R:
         qsort(list, list_count, sizeof(struct my_data *), (int (*)(const void *, const void *))my_cmp_by_name_r);
         break;
      case MY_SORT_WIDTH:
         qsort(list, list_count, sizeof(struct my_data *), (int (*)(const void *, const void *))my_cmp_by_width);
         break;
      case MY_SORT_WIDTH_R:
         qsort(list, list_count, sizeof(struct my_data *), (int (*)(const void *, const void *))my_cmp_by_width_r);
         break;
      case MY_SORT_INCLUDE:
         qsort(list, list_count, sizeof(struct my_data *), (int (*)(const void *, const void *))my_cmp_by_define);
         break;
      case MY_SORT_INCLUDE_R:
         qsort(list, list_count, sizeof(struct my_data *), (int (*)(const void *, const void *))my_cmp_by_define_r);
         break;
      default:
         qsort(list, list_count, sizeof(struct my_data *), (int (*)(const void *, const void *))my_cmp_by_name);
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
int my_cmp_by_define(void * ptr1, void * ptr2)
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
int my_cmp_by_define_r(void * ptr1, void * ptr2)
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
int my_cmp_by_name(void * ptr1, void * ptr2)
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
int my_cmp_by_name_r(void * ptr1, void * ptr2)
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
int my_cmp_by_width(void * ptr1, void * ptr2)
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
int my_cmp_by_width_r(void * ptr1, void * ptr2)
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
void my_print_data(struct my_data * data)
{
   char buff[16];
   snprintf(buff, 16, "%i", data->size);
   printf("%-*s   %*s   %*s   %-*s\n",
      MY_NAME_WIDTH,  data->name,
      MY_SIZE_WIDTH,  data->size ? buff : "unknown",
      MY_SIGN_WIDTH,  data->is_signed == 1 ? "yes" : (data->is_signed == 0 ? "no" : "n/a"),
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
         "  -d                        sort by object definition\n"
         "  -h, --help                print this help and exit\n"
         "  -n                        sort by object name\n"
         "  -r                        reverse sort order\n"
         "  -t type                   data type to display\n"
         "  -V, --version             print verbose messages\n"
         "  -w                        sort by width of object\n"
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
