/*
 *  DMS Tools and Utilities
 *  Copyright (C) 2010 David M. Syzdek <david@syzdek.net>.
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
#define MY_OPT_REVERSEBIT  0x08


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
int my_close PARAMS((BinDumpFile * file, unsigned verbose));

// preforms lseek on file
int my_lseek PARAMS((BinDumpFile * file, size_t offset, unsigned verbose));

// determines the max number of bytes to dislpay
size_t my_max PARAMS((ssize_t code, size_t len));

// opens a file
int my_open PARAMS((BinDumpFile * file, unsigned verbose));

// displays the diff between two files
size_t my_print_diff PARAMS((BinDumpFile * file1, BinDumpFile * file2,
   size_t offset, size_t len, unsigned opts));

// displays one line 8 byte chunk of data
size_t my_print_dump PARAMS((BinDumpFile * file, size_t offset, size_t len,
   unsigned opts));

// performs read upon file
ssize_t my_read PARAMS((BinDumpFile * file, size_t offset, size_t len,
   unsigned verbose));

//displays usage information
void my_usage PARAMS((void));

// displays version information
void my_version PARAMS((void));

// displays data in binary notation
char * my_byte2str PARAMS((signed data, char * buff, unsigned opts));


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
   size_t      len;
   size_t      line;
   size_t      line_add;
   unsigned    opts;
   unsigned    verbose;
   BinDumpFile  file1;
   BinDumpFile  file2;

   // getopt options
   static char   short_opt[] = "dhl:o:rvVx";
   static struct option long_opt[] =
   {
      {"help",          no_argument, 0, 'h'},
      {"verbose",       no_argument, 0, 'v'},
      {"version",       no_argument, 0, 'V'},
      {NULL,            0,           0, 0  }
   };

#ifdef HAVE_GETTEXT
   setlocale (LC_ALL, "");
   bindtextdomain (PACKAGE, LOCALEDIR);
   textdomain (PACKAGE);
#endif

   len          = 0;
   opts         = (MY_OPT_XTERM | MY_OPT_ALL);
   line         = 0;
   offset       = 0;
   offset_mod   = 0;
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
            offset_mod = (offset % 8);
            break;
         case 'r':
            opts = opts | MY_OPT_REVERSEBIT;
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
            fprintf(stderr, "Try `%s --help' for more information.\n", PROGRAM_NAME);
            return(1);
         default:
            fprintf(stderr, "%s: unrecognized option `--%c'\n", PROGRAM_NAME, c);
            fprintf(stderr, "Try `%s --help' for more information.\n", PROGRAM_NAME);
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
         fprintf(stderr, "%s: missing required argument\n", PROGRAM_NAME);
         fprintf(stderr, "Try `%s --help' for more information.\n", PROGRAM_NAME);
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
   printf("%soffset        00       01       02       03       04       05       06       07     01234567%s\n", ((opts & MY_OPT_XTERM) ? MY_TERM_BOLD : ""), ((opts & MY_OPT_XTERM) ? MY_TERM_RESET : ""));
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
      if ((my_read(&file1, (size_t)0, len, verbose) == -1))
         return(my_close(&file2, verbose));
      if ((my_read(&file2, (size_t)0, len, verbose) == -1))
         return(my_close(&file1, verbose));
      if (file2.filename)
         line_add = my_print_diff(&file1, &file2, (size_t)0, len, opts);
      else
         line_add = my_print_dump(&file1, (size_t)0, len, opts);
      line += line_add;
      if ( (!(line %  22)) && (line_add) )
      {
         if (file2.filename)
            printf("  ");
         printf("%soffset        00       01       02       03       04       05       06       07     01234567%s\n", ((opts & MY_OPT_XTERM) ? MY_TERM_BOLD : ""), ((opts & MY_OPT_XTERM) ? MY_TERM_RESET : ""));
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
int my_close(BinDumpFile * file, unsigned verbose)
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
int my_lseek(BinDumpFile * file, size_t offset, unsigned verbose)
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


/// determines the max number of bytes to dislpay
/// @param[in]  pos        pos
/// @param[in]  code       code
/// @param[in]  len        len
/// @param[in]  offset     offset
size_t my_max(ssize_t code, size_t len)
{
   size_t max;
   if (code < 1)
      return(0);
   len = len ? len : 8;
   max = (len > (size_t)code) ? (size_t)code : len;
   return(max);
}


/// opens a file
/// @param[in]  file       file to use for operations
/// @param[in]  verbose    verbose level of messages to display
int my_open(BinDumpFile * file, unsigned verbose)
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
   size_t len, unsigned opts)
{
   char    buff[9];
   size_t  s;
   size_t  max1;
   size_t  max2;
   size_t  line;
   uint8_t diff;
   uint8_t diff1[9];
   uint8_t diff2[9];

   line = 0;
   max1 = my_max(file1->code, len);
   max2 = my_max(file2->code, len);

   file1->pos += max1;
   file2->pos += max2;

   if ( (!(max1)) && (!(max2)) )
      return(0);

   diff = 0;
   memset(diff1, 0, (size_t)9);
   memset(diff2, 0, (size_t)9);
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
         if (max1)
            diff1[8] = 1;
      } else if (s < max1) {
         diff = 1;
         diff1[s] = 1;
         diff1[8] = 1;
         if (max2)
            diff2[8] = 1;
      };
   };

   if (!(diff))
      return(0);

   // prints line offset
   if ((max1) && (diff1[8]))
   {
      printf("%s< 0%07zo0:%s", ((opts & MY_OPT_XTERM) ? MY_TERM_BOLD : ""),
                               ((file1->pos-max1)/8),
                               ((opts & MY_OPT_XTERM) ? MY_TERM_RESET : ""));

      // print leading spaces for offset
      for(s = 0; s < offset; s++)
         printf("         ");

      // print each byte
      for(s = 0; s < max1; s++)
      {
         if (diff1[s])
         {
            printf(" %s%s%s", ((opts & MY_OPT_XTERM) ? MY_TERM_DIFF : ""),
                              my_byte2str(file1->data[s], buff, opts),
                              ((opts & MY_OPT_XTERM) ? MY_TERM_RESET : ""));
         } else {
            printf(" %s", (opts & MY_OPT_ALL) ? my_byte2str(file1->data[s], buff, opts) : "        ");
         };
      };
      for(s = max1+offset; s < 8; s++)
         printf("         ");

      // prints summary
      printf("  ");
      for(s = 0; s < offset; s++)
         printf(" ");
      for(s = 0; s < max1; s++)
      {
         if ( (diff1[s]) && (opts & MY_OPT_XTERM) )
            printf("%s", MY_TERM_DIFF);
         if ( (file1->data[s] >= 0x21) && (file1->data[s] <= 0x7e) )
            printf("%c", file1->data[s]);
         else
            printf(".");
         if ( (diff1[s]) && (opts & MY_OPT_XTERM) )
            printf("%s", MY_TERM_RESET);
      };

      printf("\n");

      line++;
   };

   if ((max2) && (diff2[8]))
   {
      printf("%s> 0%07zo0:%s", ((opts & MY_OPT_XTERM) ? MY_TERM_BOLD : ""),
                               ((file2->pos-max2)/8),
                               ((opts & MY_OPT_XTERM) ? MY_TERM_RESET : ""));

      // print leading spaces for offset
      for(s = 0; s < offset; s++)
         printf("         ");

      // print each byte
      for(s = 0; s < max2; s++)
      {
         if (diff2[s])
         {
            printf(" %s%s%s", ((opts & MY_OPT_XTERM) ? MY_TERM_DIFF : ""),
                              my_byte2str(file2->data[s], buff, opts),
                              ((opts & MY_OPT_XTERM) ? MY_TERM_RESET : ""));
         } else {
            printf(" %s", (opts & MY_OPT_ALL) ? my_byte2str(file2->data[s], buff, opts) : "        ");
         };
      };
      for(s = max2+offset; s < 8; s++)
         printf("         ");

      // prints summary
      printf("  ");
      for(s = 0; s < offset; s++)
         printf(" ");
      for(s = 0; s < max2; s++)
      {
         if ( (diff2[s]) && (opts & MY_OPT_XTERM) )
            printf("%s", MY_TERM_DIFF);
         if ( (file2->data[s] >= 0x21) && (file2->data[s] <= 0x7e) )
            printf("%c", file2->data[s]);
         else
            printf(".");
         if ( (diff2[s]) && (opts & MY_OPT_XTERM) )
            printf("%s", MY_TERM_RESET);
      };

      printf("\n");

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
   unsigned opts)
{
   char   buff[9];
   size_t s;
   size_t max;

   max = my_max(file->code, len);

   if (!(max))
      return(0);

   // prints line offset
   printf("%s0%07zo0:%s", ((opts & MY_OPT_XTERM) ? MY_TERM_BOLD : ""),
                          (file->pos/8),
                          ((opts & MY_OPT_XTERM) ? MY_TERM_RESET : ""));

   // prints spaces
   for(s = 0; s < offset; s++)
      printf("         ");

   // prints each byte
   for(s = 0; s < max; s++)
         printf(" %s", my_byte2str(file->data[s], buff, opts));
   for(s = max+offset; s < 8; s++)
      printf("         ");

   // prints summary
   printf("  ");
   for(s = 0; s < offset; s++)
      printf(" ");
   for(s = 0; s < max; s++)
   {
      if ( (file->data[s] >= 0x21) && (file->data[s] <= 0x7e) )
         printf("%c", file->data[s]);
      else
         printf(".");
   };

   printf("\n");

   file->pos += max;

   return(1);
}


/// performs read upon file
/// @param[in]  file       file to use for operations
/// @param[in]  offset     offset
/// @param[in]  len        len
ssize_t my_read(BinDumpFile * file, size_t offset, size_t len, unsigned verbose)
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
   printf("Usage: %s [options] file\n", PROGRAM_NAME);
   printf("  -d                        print only differing data\n");
   printf("  -h, --help                print this help and exit\n");
   printf("  -l bytes                  length of data to display\n");
   printf("  -o bytes                  offset to start reading data\n");
   printf("  -r                        display in reverse bit order\n");
   printf("  -V, --version             print verbose messages\n");
   printf("  -v, --verbose             print version number and exit\n");
   printf("  -x                        disables Xterm output\n");
   printf("\n");
   printf("Report bugs to <%s>.\n", PACKAGE_BUGREPORT);
   return;
}


/// displays version information
void my_version(void)
{
   printf("%s (%s) %s\n", PROGRAM_NAME, PACKAGE_NAME, PACKAGE_VERSION);
   printf("Written by David M. Syzdek.\n");
   printf("\n");
   printf("%s\n", PACKAGE_COPYRIGHT);
   printf("This is free software; see the source for copying conditions.  There is NO\n");
   printf("warranty; not even for MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.\n");
   return;
}


/// displays a byte in binary notation
/// @param[in]  data    8 bits to translate into an ASCII string buffer
/// @param[out] buff    ASCII string buffer to hold the result
/// @param[in]  opts    output options
char * my_byte2str(signed data, char * buff, unsigned opts)
{
   unsigned b;
   if (!(opts & MY_OPT_REVERSEBIT))
   {
      for(b = 0; b < 8; b++)
         buff[b] = (data & (0x01 << (7-b))) ? '1' : '0';
   }
   else
   {
      for(b = 0; b < 8; b++)
         buff[b] = (data & (0x01 << b)) ? '1' : '0';
   };
   buff[b] = '\0';
   return(buff);
}

// end of source file
