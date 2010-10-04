/*
 *
 *  DMS Tools and Utilities
 *  Copyright (C) 2008 David M. Syzdek <david@syzdek.net>.
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
 *  @file src/recurse.c  simple recursive directory listing utility
 */
/*
 *  Simple Build:
 *     gcc -W -Wall -O2 -c recurse.c
 *     gcc -W -Wall -O2 -o recurse   recurse.o
 *
 *  GNU Libtool Build:
 *     libtool --mode=compile gcc -W -Wall -g -O2 -c recurse.c
 *     libtool --mode=link    gcc -W -Wall -g -O2 -o recurse recurse.lo
 *
 *  GNU Libtool Install:
 *     libtool --mode=install install -c recurse /usr/local/bin/recurse
 *
 *  GNU Libtool Clean:
 *     libtool --mode=clean rm -f recurse.lo recurse
 */
#define _DMSTOOLS_SRC_RECURSE_C 1

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
#include <getopt.h>
#include <string.h>
#include <errno.h>
#include <sys/stat.h>
#include <dirent.h>

///////////////////
//               //
//  Definitions  //
//               //
///////////////////

#ifndef PACKAGE_BUGREPORT
#define PACKAGE_BUGREPORT "david@syzdek.net"
#endif
#ifndef PACKAGE_NAME
#define PACKAGE_NAME "Directory Recusring Tool"
#endif
#ifndef PACKAGE_VERSION
#define PACKAGE_VERSION "0.0.1"
#endif
#ifndef PROGRAM_NAME
#define PROGRAM_NAME "recurse"
#endif

#define MY_OPT_RECURSE     0x01
#define MY_OPT_HIDDEN      0x02
#define MY_OPT_VERBOSE     0x04
#define MY_OPT_CONTINUE    0x08
#define MY_OPT_FORCE       0x10
#define MY_OPT_LINKS       0x20
#define MY_OPT_QUIET       0x40


//////////////////
//              //
//  Prototypes  //
//              //
//////////////////

int main(int argc, char * argv[]);

void my_version(void);

void my_usage(void);

int my_work(const char * file, void * data);

int recurse_directory(char * origin, int opts, void * data,
   int (*func)(const char * file, int opts, void * data));

int recurse_file(const char * file, int opts, void * data,
   int (*func)(const char * file, int opts, void * data), unsigned * countp,
   char *** queuep, unsigned * sizep);


/////////////////
//             //
//  Functions  //
//             //
/////////////////

int main(int argc, char * argv[])
{
   int        c;
   int        i;
   int        opts;
   int        option_index;
   char    ** list;

   static char   short_options[] = "acfhLqrvV";
   static struct option long_options[] =
   {
      {"help",          no_argument, 0, 'h'},
      {"quiet",         no_argument, 0, 'q'},
      {"silent",        no_argument, 0, 'q'},
      {"verbose",       no_argument, 0, 'v'},
      {"version",       no_argument, 0, 'V'},
      {NULL,            0,           0, 0  }
   };

   opts         = 0;
   option_index = 0;
   list         = NULL;

   while((c = getopt_long(argc, argv, short_options, long_options, &option_index)) != -1)
   {
      switch(c)
      {
         case -1:       /* no more arguments */
         case 0:        /* long option toggles */
            break;
         case 'a':
            opts |= MY_OPT_HIDDEN;
            break;
         case 'c':
            opts |= MY_OPT_CONTINUE;
            break;
         case 'f':
            opts |= MY_OPT_FORCE;
            break;
         case 'h':
            my_usage();
            return(0);
         case 'L':
            opts |= MY_OPT_LINKS;
            break;
         case 'q':
            opts |= MY_OPT_QUIET;
            break;
         case 'r':
            opts |= MY_OPT_RECURSE;
            break;
         case 'v':
            opts |= MY_OPT_VERBOSE;
            break;
         case 'V':
            my_version();
            return(0);
         case '?':
            fprintf(stderr, "Try `%s --help' for more information.\n", PROGRAM_NAME);
            return(1);
         default:
            fprintf(stderr, "%s: unrecognized option `--%c'\n", PROGRAM_NAME, c);
            fprintf(stderr, "Try `%s --help' for more information.\n", PROGRAM_NAME);
            return(1);
      };
   };

   if (argc < (optind+1))
   {
      fprintf(stderr, "%s: missing required arguments\n", PROGRAM_NAME);
      fprintf(stderr, "Try `%s --help' for more information.\n", PROGRAM_NAME);
      return(1);
   };

   for(i = optind; i < argc; i++)
      switch (recurse_directory(argv[i], opts, NULL, NULL))
      {
         case -1: return(1);
         case 0:  break;
         default:
            if (!(opts & MY_OPT_CONTINUE))
               return(1);
            break;
      };
   return(0);
}


// displays usage
void my_usage(void)
{
   printf("Usage: %s [OPTIONS] file1 file2 ... fileN\n", PROGRAM_NAME);
   printf("  -a                        include entries whose names begin with a dot (.).\n");
   printf("  -c                        continue on error\n");
   printf("  -f                        force writes\n");
   printf("  -L                        follow symbolic links\n");
   printf("  -h, --help                print this help and exit\n");
   printf("  -v, --verbose             print version number and exit\n");
   printf("  -V, --version             print version number and exit\n");
   printf("  -q, --quiet, --silent     do not print messages\n");
   printf("  -r                        recursively follow directories\n");
#ifdef PACKAGE_BUGREPORT
   printf("\n");
   printf("Report bugs to <%s>.\n", PACKAGE_BUGREPORT);
#endif
   return;
}


/// displays version
void my_version(void)
{
   printf("%s (%s) %s\n", PROGRAM_NAME, PACKAGE_NAME, PACKAGE_VERSION);
   printf("Written by David M. Syzdek.\n");
   printf("\n");
#ifdef PACKAGE_COPYRIGHT
   printf("%s\n", PACKAGE_COPYRIGHT);
#endif
   printf("This is free software; see the source for copying conditions.  There is NO\n");
   printf("warranty; not even for MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.\n");
   return;
}


int recurse_directory(char * origin, int opts, void * data,
   int (*func)(const char * file, int opts, void * data))
{
   int             err;
   DIR           * d;
   char          * file;
   char         ** queue;
   void          * ptr;
   size_t          len_dir;
   size_t          len_file;
   unsigned        size;
   unsigned        count;
   struct dirent * dp;

   size  = 0;
   count = 0;
   ptr   = NULL;
   queue = NULL;

   // seeds queue with first file/directory
   if ((err = recurse_file(origin, opts, data, func, &count, &queue, &size)))
      return(err);

   if (!(opts & MY_OPT_RECURSE))
   {
      if (queue)
      {
         free(queue[0]);
         free(queue);
      };
      return(0);
   };

   while(count)
   {
      if (opts & MY_OPT_VERBOSE)
         printf("%s\n", queue[count-1]);

      // opens directory for processing
      if (!(d = opendir(queue[count-1])))
      {
         if ( ((!(opts & MY_OPT_QUIET))&&(opts & MY_OPT_CONTINUE)) || (!(opts & MY_OPT_CONTINUE)) )
            fprintf(stderr, "%s: %s: %s\n", PROGRAM_NAME, queue[count-1], strerror(errno));
         if (!(opts & MY_OPT_CONTINUE))
            return(1);
      };

      len_dir = strlen(queue[count-1]);

      for(dp = readdir(d); dp; dp = readdir(d))
      {
         len_file = strlen(dp->d_name);
         if (!(file = malloc(sizeof(char) * (len_dir + len_file + 2))))
         {
            fprintf(stderr, "%s: out of virtual memory\n", PROGRAM_NAME);
            return(-1);
         };
         sprintf(file, "%s/%s", queue[count-1], dp->d_name);

         if ( (dp->d_name[0] == '.') && (opts & MY_OPT_HIDDEN) )
         {
            if ( (dp->d_name[1] != '/') &&
                 (dp->d_name[1] != '\0') &&
                 (dp->d_name[1] != '.') )
            {
               switch (recurse_file(file, opts, data, func, &count, &queue, &size))
               {
                  case -1:
                     free(file);
                     closedir(d);
                     return(1);
                  case 0:
                     free(file);
                     break;
                  default:
                     free(file);
                     if (!(opts & MY_OPT_CONTINUE))
                     {
                        closedir(d);
                        return(1);
                     };
                     break;
               };
            };
         }
         else if (dp->d_name[0] != '.')
         {
            switch (recurse_file(file, opts, data, func, &count, &queue, &size))
            {
               case -1:
                  free(file);
                  closedir(d);
                  return(1);
               case 0:
                  free(file);
                  break;
               default:
                  free(file);
                  if (!(opts & MY_OPT_CONTINUE))
                  {
                     closedir(d);
                     return(1);
                  };
                  break;
            };
         };
      };

      // closes directory and removes from queue
      closedir(d);
      count--;
      free(queue[count]);
   };

   free(queue);

   return(0);
}


int recurse_file(const char * file, int opts, void * data,
   int (*func)(const char * file, int opts, void * data), unsigned * countp,
   char *** queuep, unsigned * sizep)
{
   int             err;
   void          * ptr;
   unsigned        u;
   struct stat     sb;

   if (opts & MY_OPT_LINKS)
      err = stat(file, &sb);
   else
      err = lstat(file, &sb);
   if (err == -1)
   {
      if ( ((!(opts & MY_OPT_QUIET))&&(opts & MY_OPT_CONTINUE)) || (!(opts & MY_OPT_CONTINUE)) )
         fprintf(stderr, "%s: %s: %s\n", PROGRAM_NAME, file, strerror(errno));
      return(1);
   };
   switch(sb.st_mode & (S_IFDIR|S_IFREG|S_IFLNK))
   {
      case S_IFDIR:
         // adjust size of array
         if (*countp >= *sizep)
         {
            *sizep += 20;
            if (!(ptr = realloc(*queuep, sizeof(char *) * (*sizep))))
            {
               fprintf(stderr, "%s: out of virtual memory\n", PROGRAM_NAME);
               return(-1);
            };
            *queuep = ptr;
         };

         // copy path name
         if (!(ptr = strdup(file)))
         {
            fprintf(stderr, "%s: out of virtual memory\n", PROGRAM_NAME);
            return(-1);
         };

         // insert path name at head of array
         for(u = (*countp); u > 0; u--)
            (*queuep)[u] = (*queuep)[u-1];
         (*queuep)[0] = ptr;
         (*countp)++;
         break;

      case S_IFLNK:  // ignore symbolic links
         break;

      case S_IFREG:
         if (opts & MY_OPT_VERBOSE)
            printf("%s\n", file);
         if (func)
            return(func(file, opts, data));
         break;

      default:
         if ( ((!(opts & MY_OPT_QUIET))&&(opts & MY_OPT_CONTINUE)) || (!(opts & MY_OPT_CONTINUE)) )
            fprintf(stderr, "%s: %s: unknown file type\n", PROGRAM_NAME, file);
         return(1);
         break;
   };

   return(0);
}

// end of source code
