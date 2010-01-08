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
   int         opt_index;
   size_t      offset;
   size_t      offset_mod;
   size_t      offset_div;
   size_t      len;
   size_t      line;
   uint32_t    verbose;
   BinDumpFile  file1;

   // getopt options
   static char   short_opt[] = "hl:o:vV";
   static struct option long_opt[] =
   {
      {"help",          no_argument, 0, 'h'},
      {"verbose",       no_argument, 0, 'v'},
      {"version",       no_argument, 0, 'V'},
      {NULL,            0,           0, 0  }
   };

   len      = 0;
   offset   = 0;
   offset_mod  = 0;
   offset_div  = 0;
   verbose  = 0;
   memset(&file1, 0, sizeof(BinDumpFile));
   
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
      case 1:
         if ((strcmp(argv[optind+0], "-")))
            file1.filename = argv[optind+0];
         break;
      case 0:
         break;
      default:
         fprintf(stderr, _("%s: missing required argument\n"
               "Try `%s --help' for more information.\n"
            ),  PROGRAM_NAME, PROGRAM_NAME
         );
         return(1);
   };
   if ( (!(file1.filename)) && (offset) )
   {
      fprintf(stderr, PROGRAM_NAME ": unable to specify an offset with STDIN\n");
      return(1);
   };

   // show them who we are
   if (verbose > 1)
      printf("%s (%s) %s\n", PROGRAM_NAME, PACKAGE_NAME, PACKAGE_VERSION);

   // open file for reading or set to STDIN
   file1.fd = STDIN_FILENO;
   if (!(file1.filename))
      if (verbose > 2)
         printf("using stdin...\n");
   if (file1.filename)
   {
      if (verbose > 2)
         printf("opening %s...\n", file1.filename);
      if ((file1.fd = open(file1.filename, O_RDONLY)) == -1)
      {
         perror(PROGRAM_NAME ": open()");
         return(1);
      };
   };

   // move to the specified offset
   if (offset)
   {
      if (verbose > 2)
         printf("offsetting by %zi bytes...\n", offset);
      if ((my_lseek(&file1, offset, verbose) == -1))
         return(1);
   };

   line = 0;

   // fill in white space so the offset markings align with the position in the file
   if (verbose > 2)
      printf("reading data...\n");
   printf("offset     00       01       02       03       04       05       06       07 \n");
   if (offset_mod)
   {
      if ((my_read(&file1, offset_mod, len, verbose) == -1))
         return(1);
      line += my_print_dump(&file1, offset_mod, len, 0);
   };

   // read data from file handle
   while(!(file1.eof))
   {
      if ((my_read(&file1, 0, len, verbose) == -1))
         return(1);
      line += my_print_dump(&file1, 0, (len - (file1.pos-offset)), 0);
      if (!(line %  23))
         printf("offset     00       01       02       03       04       05       06       07 \n");
   };

   // close file and finish up
   my_close(&file1, verbose);
   if (verbose > 0)
      printf("read %zu bytes\n", (file1.read));
   printf("\n");

   return(0);
}


/// closes a file
/// @param[in]  file       file to use for operations
/// @param[in]  verbose    verbose level of messages to display
int my_close(BinDumpFile * file, uint32_t verbose)
{
   if (!(file->filename))
      return(0);
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
   if ((lseek(file->fd, (off_t)offset, SEEK_SET) == -1))
   {
      perror(PROGRAM_NAME ": lseek()");
      my_close(file, verbose);
      return(-1);
   };
   file->pos += offset;
   return(0);
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

   if (opts)
      return(0);

   if (file->code < 1)
      return(0);

   // prints line offset
   printf("0%04zo0:", (file->pos/8));

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

   file->code = 0;

   max = 8 - offset;
   if (len)
      max = ((len - file->read) < max) ? len - file->read : max;

   if ((file->code = read(file->fd, file->data, max)) == -1)
   {
      perror(PROGRAM_NAME ": read()");
      my_close(file, verbose);
      if (verbose > 0)
         printf("read %zu bytes\n", (file->read));
      return(1);
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
         "  -h, --help                print this help and exit\n"
         "  -l bytes                  length of data to display\n"
         "  -o bytes                  offset to start reading data\n"
         "  -V, --version             print verbose messages\n"
         "  -v, --verbose             print version number and exit\n"
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
