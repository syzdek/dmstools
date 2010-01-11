/*
 *  DMS Tools and Utilities
 *  Copyright (C) 2010 David M. Syzdek <david@syzdek.net>.
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
 *  @file src/bindump.c simple utility for quick bitwise operations
 */
/*
 *  Simple Build:
 *     gcc -W -Wall -O2 -c bindump.c
 *     gcc -W -Wall -O2 -o bindump   bindump.o
 *
 *  GNU Libtool Build:
 *     libtool --mode=compile gcc -W -Wall -g -O2 -c bindump.c
 *     libtool --mode=link    gcc -W -Wall -g -O2 -o bindump bindump.lo
 *
 *  GNU Libtool Install:
 *     libtool --mode=install install -c bindump /usr/local/bin/bindump
 *
 *  GNU Libtool Clean:
 *     libtool --mode=clean rm -f bindump.lo bindump
 */
#define _SCT_SRC_BINDUMP_C 1

///////////////
//           //
//  Headers  //
//           //
///////////////

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <stdio.h>
#include <inttypes.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <getopt.h>
#include <sys/types.h>
#include <sys/uio.h>
#include <fcntl.h>


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
#define PROGRAM_NAME "bindump"
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

#define MY_TERM_RESET      "\x1b[0m"
#define MY_TERM_BOLD       "\x1b[1m"
#define MY_TERM_DIFF       "\x1b[31m"

#define MY_OPT_ALL         0x01
#define MY_OPT_XTERM       0x02
#define MY_OPT_NOXTERM     0x04


/////////////////
//             //
//  Datatypes  //
//             //
/////////////////

/// contains the state information for a file.
typedef struct bindump_file BinDumpFile;


/////////////////
//             //
//  Structs    //
//             //
/////////////////

/// contains the state information for a file.
struct bindump_file
{
   int           fd;          ///< file handle of open file
   int           eof;         ///< toggledupon reaching the end-of-file
   size_t        pos;         ///< current position within the file
   size_t        read;        ///< number of bytes read
   ssize_t       code;        ///< return code of last file operation
   const char  * filename;    ///< name of the file
   char          data[8];     ///< buffer for storing data for processing
};


//////////////////
//              //
//  Prototypes  //
//              //
//////////////////

// main statement
int main PARAMS((int argc, char * argv[]));

// closes a file
int my_close PARAMS((BinDumpFile * file, uint32_t verbose));

// preforms lseek on file
int my_lseek PARAMS((BinDumpFile * file, size_t offset, uint32_t verbose));

// opens a file
int my_open PARAMS((BinDumpFile * file, uint32_t verbose));

// displays the diff between two files
size_t my_print_diff PARAMS((BinDumpFile * file1, BinDumpFile * file2,
   size_t offset, size_t len, uint32_t opts));

// displays one line 8 byte chunk of data
size_t my_print_dump PARAMS((BinDumpFile * file, size_t offset, size_t len,
   uint32_t opts));

// performs read upon file
int my_read PARAMS((BinDumpFile * file, size_t offset, size_t len,
   uint32_t verbose));

//displays usage information
void my_usage PARAMS((void));

// displays version information
void my_version PARAMS((void));

// displays data in binary notation
char * my_byte2str PARAMS((uint8_t data, char * buff));


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
   int         c;
   int         opts;
   int         opt_index;
   size_t      offset;
   size_t      offset_mod;
   size_t      offset_div;
   size_t      len;
   size_t      line;
   uint32_t    verbose;
   BinDumpFile  file1;
   BinDumpFile  file2;

   // getopt options
   static char   short_opt[] = "dhl:o:vVx";
   static struct option long_opt[] =
   {
      {"help",          no_argument, 0, 'h'},
      {"verbose",       no_argument, 0, 'v'},
      {"version",       no_argument, 0, 'V'},
      {NULL,            0,           0, 0  }
   };

   len          = 0;
   opts         = (MY_OPT_XTERM | MY_OPT_ALL);
   line         = 0;
   offset       = 0;
   offset_mod   = 0;
   offset_div   = 0;
   verbose      = 0;
   memset(&file1, 0, sizeof(BinDumpFile));
   memset(&file2, 0, sizeof(BinDumpFile));
   
   while((c = getopt_long(argc, argv, short_opt, long_opt, &opt_index)) != -1)
   {
      switch(c)
      {
         case -1:	/* no more arguments */
         case 0:	/* long options toggles */
            break;
         case 'd':
            opts = opts & (~MY_OPT_ALL);
            break;
         case 'h':
            my_usage();
            return(0);
         case 'l':
            len = strtoul(optarg, NULL, 0);
            break;
         case 'o':
            offset = strtoul(optarg, NULL, 0);
            offset_div = (offset / 8);
            offset_mod = (offset - (offset_div * 8));
            break;
         case 'V':
            my_version();
            return(0);
         case 'v':
            verbose++;
            break;
         case 'x':
            opts = opts & (~MY_OPT_XTERM);
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

   // determines file to process (or STDIN)
   switch(argc - optind)
   {
      case 2:
         file2.filename = argv[optind+1];
      case 1:
         file1.filename = argv[optind+0];
         break;
      case 0:
         file1.filename = "-";
         break;
      default:
         fprintf(stderr, _("%s: missing required argument\n"
               "Try `%s --help' for more information.\n"
            ),  PROGRAM_NAME, PROGRAM_NAME
         );
         return(1);
   };
   if ((file1.filename) && (file2.filename))
      if (!(strcmp(file1.filename, file2.filename)))
         file2.filename = NULL;

   // show them who we are
   if (verbose > 1)
      printf("%s (%s) %s\n", PROGRAM_NAME, PACKAGE_NAME, PACKAGE_VERSION);

   // open file for reading or set to STDIN
   if ((my_open(&file1, verbose) == -1))
      return(my_close(&file2, verbose));
   if ((my_open(&file2, verbose) == -1))
      return(my_close(&file1, verbose));

   // checks file handles for user errors
   if (file2.fd == STDIN_FILENO)
   {
      fprintf(stderr, PROGRAM_NAME ": unable to diff stdin\n");
      my_close(&file1, verbose);
      my_close(&file2, verbose);
      return(1);
   };
   if (file1.fd == STDIN_FILENO)
   {
      if (offset)
      {
         fprintf(stderr, PROGRAM_NAME ": unable to specify an offset with stdin\n");
         my_close(&file1, verbose);
         my_close(&file2, verbose);
         return(1);
      };
      if (file2.fd != -1)
      {
         fprintf(stderr, PROGRAM_NAME ": unable to diff stdin\n");
         my_close(&file1, verbose);
         my_close(&file2, verbose);
         return(1);
      };
      if (verbose > 2)
         printf("using stdin...\n");
   };

   // move to the specified offset
   if (offset)
   {
      if (verbose > 2)
         printf("offsetting by %zi bytes...\n", offset);
      if ((my_lseek(&file1, offset, verbose) == -1))
         return(my_close(&file2, verbose));
      if ((my_lseek(&file2, offset, verbose) == -1))
         return(my_close(&file1, verbose));
   };

   // fill in white space so the offset markings align with the position in the file
   if (verbose > 2)
      printf("reading data...\n");
   if (file2.filename)
      printf("  ");
   if (opts & MY_OPT_XTERM)
      printf("%s", MY_TERM_BOLD);
   printf("offset     00       01       02       03       04       05       06       07 \n");
   if (opts & MY_OPT_XTERM)
      printf("%s", MY_TERM_RESET);
   if (offset_mod)
   {
      if ((my_read(&file1, offset_mod, len, verbose) == -1))
         return(my_close(&file2, verbose));
      if ((my_read(&file2, offset_mod, len, verbose) == -1))
         return(my_close(&file1, verbose));
      if (file2.filename)
         line += my_print_diff(&file1, &file2, offset_mod, len, opts);
      else
         line += my_print_dump(&file1, offset_mod, len, opts);
   };

   // read data from file handle
   while ( (!(file1.eof)) || (!(file2.eof)) )
   {
      if ((my_read(&file1, 0, len, verbose) == -1))
         return(my_close(&file2, verbose));
      if ((my_read(&file2, 0, len, verbose) == -1))
         return(my_close(&file1, verbose));
      if (file2.filename)
         line += my_print_diff(&file1, &file2, 0, len, opts);
      else
         line += my_print_dump(&file1, 0, len, opts);
      if (!(line %  22))
      {
         if (file2.filename)
            printf("  ");
         if (opts & MY_OPT_XTERM)
            printf("%s", MY_TERM_BOLD);
         printf("offset     00       01       02       03       04       05       06       07 \n");
         if (opts & MY_OPT_XTERM)
            printf("%s", MY_TERM_RESET);
      };
   };

   // close file and finish up
   my_close(&file1, verbose);
   my_close(&file2, verbose);
   printf("\n");

   return(0);
}


/// closes a file
/// @param[in]  file       file to use for operations
/// @param[in]  verbose    verbose level of messages to display
int my_close(BinDumpFile * file, uint32_t verbose)
{
   if (file->fd == -1)
      return(1);

   if (verbose > 0)
      printf("read %zu bytes from %s\n", file->read, file->filename);

   if (file->fd == STDIN_FILENO)
      return(1);

   if (verbose > 2)
      printf("closing file %s...\n", file->filename);
   close(file->fd);
   return(1);
}


/// preforms lseek on file
/// @param[in]  file       file to use for operations
/// @param[in]  offset     offset
/// @param[in]  verbose    verbose level of messages to display
int my_lseek(BinDumpFile * file, size_t offset, uint32_t verbose)
{
   if ( (file->fd == -1) || (file->fd == STDIN_FILENO) )
      return(0);
   if ((lseek(file->fd, (off_t)offset, SEEK_SET) == -1))
   {
      perror(PROGRAM_NAME ": lseek()");
      my_close(file, verbose);
      return(-1);
   };
   file->pos += offset;
   return(0);
}


/// opens a file
/// @param[in]  file       file to use for operations
/// @param[in]  verbose    verbose level of messages to display
int my_open(BinDumpFile * file, uint32_t verbose)
{
   file->fd = -1;

   if (!(file->filename))
   {
      file->eof = 1;
      return(0);
   };
   if (!(strcmp(file->filename, "-")))
   {
      file->filename = "<stdin>";
      file->fd = STDIN_FILENO;
      return(0);
   };

   if (verbose > 2)
      printf("opening %s...\n", file->filename);

   if ((file->fd = open(file->filename, O_RDONLY)) == -1)
   {
      perror(PROGRAM_NAME ": open()");
      return(1);
   };

   return(0);
}


/// displays the diff between two files
/// @param[in]  file1      first file to use for operations
/// @param[in]  file2      second file to use for operations
/// @param[in]  offset     offset
/// @param[in]  len        len
/// @param[in]  verbose    verbose level of messages to display
/// @param[in]  opts       output options
size_t my_print_diff(BinDumpFile * file1, BinDumpFile * file2, size_t offset,
   size_t len, uint32_t opts)
{
   char    buff[9];
   size_t  s;
   size_t  max1;
   size_t  max2;
   size_t  len1;
   size_t  len2;
   size_t  line;
   uint8_t diff;
   uint8_t diff1[9];
   uint8_t diff2[9];

   line = 0;
   max1 = 0;
   max2 = 0;

   if (file1->code > 0)
   {
      len1 = len ? (len - (file1->pos-offset)) : 8;
      max1 = (len1 > ((size_t)file1->code)) ? (size_t)file1->code : len1;
   };
   if (file2->code > 0)
   {
      len2 = len ? (len - (file2->pos-offset)) : 8;
      max2 = (len2 > ((size_t)file2->code)) ? (size_t)file2->code : len2;
   };

   if ( (!(max1)) && (!(max2)) )
      return(0);

   diff = 0;
   memset(diff1, 0, 9);
   memset(diff2, 0, 9);
   for(s = 0; s < (8-offset); s++)
   {
      if ( (s < max1) && (s < max2) )
      {
         if (file1->data[s] != file2->data[s])
         {
            diff = 1;
            diff1[s] = 1;
            diff1[8] = 1;
            diff2[s] = 1;
            diff2[8] = 1;
         };
      } else if (s < max2) {
         diff = 1;
         diff2[s] = 1;
         diff2[8] = 1;
      } else {
         diff = 1;
         diff1[s] = 1;
         diff1[8] = 1;
      };
   };

   if (!(diff))
      return(0);

   // prints line offset
   if ((max1) && (diff1[8]))
   {
      if (opts & MY_OPT_XTERM)
         printf("%s", MY_TERM_BOLD);
      printf("< 0%04zo0:", (file1->pos/8));
      if (opts & MY_OPT_XTERM)
         printf("%s", MY_TERM_RESET);

      for(s = 0; s < offset; s++)
         printf("         ");

      for(s = 0; s < max1; s++)
      {
         if (diff1[s])
         {
            if (opts & MY_OPT_XTERM)
               printf("%s", MY_TERM_DIFF);
            printf(" %s", my_byte2str(file1->data[s], buff));
            if (opts & MY_OPT_XTERM)
               printf("%s", MY_TERM_RESET);
         } else {
            printf(" %s", (opts & MY_OPT_ALL) ? my_byte2str(file1->data[s], buff) : "        ");
         };
      };
      printf("\n");

      file1->pos += max1;

      line++;
   };

   if ((max2) && (diff2[8]))
   {
      if (opts & MY_OPT_XTERM)
         printf("%s", MY_TERM_BOLD);
      printf("> 0%04zo0:", (file2->pos/8));
      if (opts & MY_OPT_XTERM)
         printf("%s", MY_TERM_RESET);

      for(s = 0; s < offset; s++)
         printf("         ");

      for(s = 0; s < max2; s++)
      {
         if (diff2[s])
         {
            if (opts & MY_OPT_XTERM)
               printf("%s", MY_TERM_DIFF);
            printf(" %s", my_byte2str(file2->data[s], buff));
            if (opts & MY_OPT_XTERM)
               printf("%s", MY_TERM_RESET);
         } else {
            printf(" %s", (opts & MY_OPT_ALL) ? my_byte2str(file2->data[s], buff) : "        ");
         };
      };
      printf("\n");

      file2->pos += max2;

      line++;
   };

   return(line);
}


/// displays one line 8 byte chunk of data
/// @param[in]  file       file to use for operations
/// @param[in]  offset     offset
/// @param[in]  len        len
/// @param[in]  verbose    verbose level of messages to display
/// @param[in]  opts       output options
size_t my_print_dump(BinDumpFile * file, size_t offset, size_t len,
   uint32_t opts)
{
   char   buff[9];
   size_t s;
   size_t max;

   if (file->code < 1)
      return(0);

   // prints line offset
   if (opts & MY_OPT_XTERM)
      printf("%s", MY_TERM_BOLD);
   printf("< 0%04zo0:", (file->pos/8));
   if (opts & MY_OPT_XTERM)
      printf("%s", MY_TERM_RESET);

   // prints spaces
   for(s = 0; s < offset; s++)
      printf("         ");

   // prints each byte
   len = len ? (len - (file->pos-offset)) : 8;
   max = (len > ((size_t)file->code)) ? (size_t)file->code : len;
   for(s = 0; s < max; s++)
         printf(" %s", my_byte2str(file->data[s], buff));
   printf("\n");

   file->pos += max;

   return(1);
}


/// performs read upon file
/// @param[in]  file       file to use for operations
/// @param[in]  offset     offset
/// @param[in]  len        len
int my_read(BinDumpFile * file, size_t offset, size_t len, uint32_t verbose)
{
   size_t max;

   if (file->fd == -1)
      return(0);

   file->code = 0;

   max = 8 - offset;
   if (len)
      max = ((len - file->read) < max) ? len - file->read : max;

   if ((file->code = read(file->fd, file->data, max)) == -1)
   {
      perror(PROGRAM_NAME ": read()");
      my_close(file, verbose);
      return(-1);
   };

   if (file->code < 1)
      file->eof = 1;
   else
      file->read += file->code;

   if (len)
      if (file->read > len)
         file->eof = 1;

   return(file->code);
}


/// displays usage information
void my_usage()
{
   // TRANSLATORS: The following strings provide usage for command. These
   // strings are displayed if the program is passed `--help' on the command
   // line. The two strings referenced are: PROGRAM_NAME, and
   // PACKAGE_BUGREPORT
   printf(_("Usage: %s [options] file\n"
         "  -d                        print only differing data\n"
         "  -h, --help                print this help and exit\n"
         "  -l bytes                  length of data to display\n"
         "  -o bytes                  offset to start reading data\n"
         "  -V, --version             print verbose messages\n"
         "  -v, --verbose             print version number and exit\n"
         "  -x                        disables Xterm output\n"
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


/// displays a byte in binary notation
/// @param[in]  data    8 bits to translate into an ASCII string buffer
/// @param[out] buff    ASCII string buffer to hold the result
char * my_byte2str(uint8_t data, char * buff)
{
   uint32_t b;
   for(b = 0; b < 8; b++)
      buff[b] = (data & (0x01 << b)) ? '1' : '0';
   buff[b] = '\0';
   return(buff);
}

// end of source file
