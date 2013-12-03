/*
 *  DMS Tools and Utilities
 *  Copyright (C) 2009 David M. Syzdek <david@syzdek.net>.
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
 *  @file src/macaddrutil.c converts string into octal, decimal, hex, and bin
 */
/*
 *  Simple Build:
 *     gcc -W -Wall -O2 -c macaddrutil.c
 *     gcc -W -Wall -O2 -o macaddrutil   macaddrutil.o
 *
 *  GNU Libtool Build:
 *     libtool --mode=compile gcc -W -Wall -g -O2 -c macaddrutil.c
 *     libtool --mode=link    gcc -W -Wall -g -O2 -o macaddrutil macaddrutil.lo
 *
 *  GNU Libtool Install:
 *     libtool --mode=install install -c macaddrutil /usr/local/bin/macaddrutil
 *
 *  GNU Libtool Clean:
 *     libtool --mode=clean rm -f macaddrutil.lo macaddrutil
 */
#define _DMSTOOLS_SRC_MACADDRUTIL_C 1

///////////////
//           //
//  Headers  //
//           //
///////////////
#ifdef DMSTOOLS_PMARK
#pragma mark Headers
#endif

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <stdio.h>
#include <inttypes.h>
#include <stdlib.h>
#include <getopt.h>
#include <string.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/uio.h>
#include <unistd.h>
#include <stdarg.h>
#include <assert.h>


///////////////////
//               //
//  Definitions  //
//               //
///////////////////
#ifdef DMSTOOLS_PMARK
#pragma mark - Definitions
#endif

#ifndef PROGRAM_NAME
#define PROGRAM_NAME "macaddrutil"
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


#define MAU_GETOPT_SHORT "cDdhlqRuVv"

#define MAU_GETOPT_LONG {"colon",            no_argument,       NULL, 'c'}, \
                        {"dot",              no_argument,       NULL, 'D'}, \
                        {"dash",             no_argument,       NULL, 'd'}, \
                        {"help",             no_argument,       NULL, 'h'}, \
                        {"lower",            no_argument,       NULL, 'l'}, \
                        {"quiet",            no_argument,       NULL, 'q'}, \
                        {"raw",              no_argument,       NULL, 'R'}, \
                        {"silent",           no_argument,       NULL, 'q'}, \
                        {"upper",            no_argument,       NULL, 'u'}, \
                        {"version",          no_argument,       NULL, 'V'}, \
                        {"verbose",          no_argument,       NULL, 'v'}

#define MAU_MASK_CASE        0xf0000000
#define MAU_MASK_NOTATION    0x000000FF
#define MAU_CASE_LOWER       0x00000000
#define MAU_CASE_UPPER       0x10000000
#define MAU_NOTATION_COLON   0x00000000
#define MAU_NOTATION_DASH    0x00000001
#define MAU_NOTATION_DOT     0x00000002
#define MAU_NOTATION_RAW     0x00000003

#define MAU_CASE(notation)                (notation & MAU_MASK_CASE)
#define MAU_NOTATION(notation)            (notation & MAU_MASK_NOTATION)
#define MAU_SET_CASE(notation, flag)      ((notation & (~ MAU_MASK_CASE))     | (flag & MAU_MASK_CASE))
#define MAU_SET_NOTATION(notation, flag)  ((notation & (~ MAU_MASK_NOTATION)) | (flag & MAU_MASK_NOTATION))


//////////////////
//              //
//  Data Types  //
//              //
//////////////////
#ifdef DMSTOOLS_PMARK
#pragma mark - Data Types
#endif

typedef uint8_t mauaddr_t[6];
typedef uint8_t mauoui_t[3];
typedef char    maustr_t[18];

typedef struct mau_config mau_config;
typedef struct mau_command mau_command;

struct mau_config
{
   int                  quiet;
   int                  verbose;
   int                  notation;
   int                  cmd_argc;
   char              ** cmd_argv;
   const char         * cmd_name;
   size_t               cmd_len;
   const mau_command  * cmd;
};


struct mau_command
{
   const char * cmd_name;
   int  (*cmd_func)(mau_config * cnf, int argc, char * argv[]);
   void (*cmd_usage)(void);
   const char * cmd_desc;
};


//////////////////
//              //
//  Prototypes  //
//              //
//////////////////
#ifdef DMSTOOLS_PMARK
#pragma mark - Prototypes
#endif

// main statement
int main(int argc, char * argv[]);


int mau_cmd_format(mau_config * cnf, int argc, char * argv[]);
void mau_cmd_format_usage(void);

// generate command
int mau_cmd_generate(mau_config * cnf, int argc, char * argv[]);

// generate command usage
void mau_cmd_generate_usage(void);

int mau_cmd_info(mau_config * cnf, int argc, char * argv[]);
void mau_cmd_info_usage(void);

int mau_getopt(mau_config * cnf, int argc, char * const * argv,
   const char * short_opt, const struct option * long_opt, int * opt_index);

// print error message
void mau_log_err(mau_config * cnf, const char * fmt, ...);

// print verbose information
void mau_log_verbose(mau_config * cnf, const char * fmt, ...);

// print warning message
void mau_log_warn(mau_config * cnf, const char * fmt, ...);

void mau_logv(mau_config * cnf, const char * fmt, va_list args);

int mau_mac2str(mau_config * cnf, const mauaddr_t addr, maustr_t str);
int mau_str2mac(mau_config * cnf, const maustr_t str, mauaddr_t addr);

//displays usage information
void mau_usage(mau_config * cnf);

// displays version information
void mau_version(void);



/////////////////
//             //
//  Variables  //
//             //
/////////////////
#ifdef DMSTOOLS_PMARK
#pragma mark - Variables
#endif

extern const int errno;

const mau_command mau_cmdmap[] =
{
   { "format",              mau_cmd_format,    mau_cmd_format_usage,    "reformat MAC address using notation style flags" },
   { "generate",            mau_cmd_generate,  mau_cmd_generate_usage,  "generate random MAC address" },
   { "information",         mau_cmd_info,      mau_cmd_info_usage,      "display meta information about MAC address" },
   { NULL, NULL, NULL, NULL }
};


/////////////////
//             //
//  Functions  //
//             //
/////////////////
#ifdef DMSTOOLS_PMARK
#pragma mark - Functions
#endif

/// main statement
/// @param[in] argc   number of arguments
/// @param[in] argv   array of arguments
int main(int argc, char * argv[])
{
   int            i;
   int            c;
   int            opt_index;
   mau_config     cnf;

   // getopt options
   static char   short_opt[] = "+" MAU_GETOPT_SHORT;
   static struct option long_opt[] =
   {
      MAU_GETOPT_LONG,
      {NULL, 0, NULL, 0 }
   };

   memset(&cnf, 0, sizeof(cnf));

   while((c = mau_getopt(&cnf, argc, argv, short_opt, long_opt, &opt_index)) != -1)
   {
      switch(c)
      {
         case -2: /* captured by common options */
         case -1:	/* no more arguments */
         case 0:	/* long options toggles */
         break;

         case 1:
         return(0);

         case 2:
         return(1);

         case '?':
         fprintf(stderr, "Try `%s --help' for more information.\n", PROGRAM_NAME);
         return(1);

         default:
         fprintf(stderr, "%s: unrecognized option `--%c'\n", PROGRAM_NAME, c);
         fprintf(stderr, "Try `%s --help' for more information.\n", PROGRAM_NAME);
         return(1);
      };
   };

   if ((argc - optind) < 1)
   {
      fprintf(stderr, "%s: missing required argument\n", PROGRAM_NAME);
      fprintf(stderr, "Try `%s --help' for more information.\n", PROGRAM_NAME);
      return(1);
   };
   cnf.cmd_argc = (argc - optind);
   cnf.cmd_argv = &argv[optind];
   cnf.cmd_name = cnf.cmd_argv[0];
   cnf.cmd_len  = strlen(cnf.cmd_name);

   for(i = 0; (mau_cmdmap[i].cmd_name != NULL); i++)
   {
      if ((strncmp(cnf.cmd_name, mau_cmdmap[i].cmd_name, cnf.cmd_len)))
         continue;

      if ((cnf.cmd))
      {
         fprintf(stderr, "%s: ambiguous command -- \"%s\"\n", PROGRAM_NAME, cnf.cmd_name);
         fprintf(stderr, "Try `%s --help' for more information.\n", PROGRAM_NAME);
         return(1);
      };

      cnf.cmd = &mau_cmdmap[i];
   };
   if (!(cnf.cmd))
   {
      fprintf(stderr, "%s: unknown command -- \"%s\"\n", PROGRAM_NAME, cnf.cmd_name);
      fprintf(stderr, "Try `%s --help' for more information.\n", PROGRAM_NAME);
      return(1);
   };
   if (!(cnf.cmd->cmd_func))
   {
      fprintf(stderr, "%s: command not implemented -- \"%s\"\n", PROGRAM_NAME, cnf.cmd->cmd_name);
      fprintf(stderr, "Try `%s --help' for more information.\n", PROGRAM_NAME);
      return(1);
   };

   optind = 1;
   return(cnf.cmd->cmd_func(&cnf, cnf.cmd_argc, cnf.cmd_argv));
}


int mau_cmd_format(mau_config * cnf, int argc, char * argv[])
{
   int            c;
   int            opt_index;
   maustr_t       str;
   mauaddr_t      addr;

   // getopt options
   static char   short_opt[] = MAU_GETOPT_SHORT;
   static struct option long_opt[] =
   {
      MAU_GETOPT_LONG,
      {NULL, 0, NULL, 0 }
   };


   while((c = mau_getopt(cnf, argc, argv, short_opt, long_opt, &opt_index)) != -1)
   {
      switch(c)
      {
         case -2: /* captured by common options */
         case -1:	/* no more arguments */
         case 0:	/* long options toggles */
         break;

         case 1:
         return(0);
         case 2:
         return(1);

         case '?':
         fprintf(stderr, "Try `%s format --help' for more information.\n", PROGRAM_NAME);
         return(1);

         default:
         fprintf(stderr, "%s: unrecognized option `--%c'\n", PROGRAM_NAME, c);
         fprintf(stderr, "Try `%s format --help' for more information.\n", PROGRAM_NAME);
         return(1);
      };
   };

   if ((argc - optind) < 1)
   {
      fprintf(stderr, "%s: missing MAC address\n", PROGRAM_NAME);
      fprintf(stderr, "Try `%s format --help' for more information.\n", PROGRAM_NAME);
      return(1);
   };
   if ((optind+1) != argc)
   {
      fprintf(stderr, "%s: unrecognized argument `-- %s'\n", PROGRAM_NAME, argv[optind+1]);
      fprintf(stderr, "Try `%s format --help' for more information.\n", PROGRAM_NAME);
      return(1);
   };

   if ((mau_str2mac(cnf, argv[optind], addr)))
      return(1);

   mau_mac2str(cnf, addr, str);

   printf("%s\n", str);

   return(0);
}


/// generate command usage
void mau_cmd_format_usage(void)
{
   printf(
      (
         "Usage: %s format [OPTIONS] mac_address\n"
      ), PROGRAM_NAME
   );
   return;
}


/// generate command
int mau_cmd_generate(mau_config * cnf, int argc, char * argv[])
{
   int            c;
   int            opt_index;
   int            fd;
   const char   * rnd_file;
   int            use_oui;
   mauoui_t       oui;
   mauaddr_t      addr;
   maustr_t       str;

   // getopt options
   static char   short_opt[] = "hr:x" MAU_GETOPT_SHORT;
   static struct option long_opt[] =
   {
      MAU_GETOPT_LONG,
      // command long options
      {"vmware",           no_argument,       NULL, 0},
      {"xen",              no_argument,       NULL, 0},
      {NULL, 0, NULL, 0 }
   };

   rnd_file = "/dev/random";
   use_oui  = 0;

   while((c = mau_getopt(cnf, argc, argv, short_opt, long_opt, &opt_index)) != -1)
   {
      switch(c)
      {
         case -2: /* captured by common options */
         case -1:	/* no more arguments */
         break;

         case 0:	/* long options toggles */
         if (!(strcmp(long_opt[opt_index].name, "vmware")))
         {
            use_oui = 1;
            oui[0] = 0x00;
            oui[1] = 0x50;
            oui[2] = 0x56;
         };
         if (!(strcmp(long_opt[opt_index].name, "xen")))
         {
            use_oui = 1;
            oui[0] = 0x00;
            oui[1] = 0x16;
            oui[2] = 0x3e;
         };
         break;

         case 1:
         return(0);
         case 2:
         return(1);

         case 'r':
         rnd_file = optarg;
         break;

         case '?':
         fprintf(stderr, "Try `%s generate --help' for more information.\n", PROGRAM_NAME);
         return(1);

         default:
         fprintf(stderr, "%s: unrecognized option `--%c'\n", PROGRAM_NAME, c);
         fprintf(stderr, "Try `%s generate --help' for more information.\n", PROGRAM_NAME);
         return(1);
      };
   };
   if (optind != argc)
   {
      fprintf(stderr, "%s: unrecognized argument `-- %s'\n", PROGRAM_NAME, argv[optind]);
      fprintf(stderr, "Try `%s generate --help' for more information.\n", PROGRAM_NAME);
      return(1);
   };

   // generates random MAC
   mau_log_verbose(cnf, "opening file (%s)\n", rnd_file);
   if ((fd = open(rnd_file, O_RDONLY)) == -1)
   {
      mau_log_err(cnf, "%s: %s\n", rnd_file, strerror(errno));
      return(1);
   };
   mau_log_verbose(cnf, "%s: reading random data\n", PROGRAM_NAME);
   if (read(fd, addr, sizeof(addr)) == -1)
   {
      mau_log_err(cnf, "%s: %s\n", rnd_file, strerror(errno));
      mau_log_verbose(cnf, "%s: closing file\n", PROGRAM_NAME);
      close(fd);
      return(1);
   };
   mau_log_verbose(cnf, "%s: closing file\n", PROGRAM_NAME);
   close(fd);
   addr[0] = (addr[0] & 0xFC) | 0x02;


   // applies OUI
   if ((use_oui))
      memcpy(addr, oui, 3);

   mau_mac2str(cnf, addr, str);
   printf("%s\n", str);

   return(0);
}


/// generate command usage
void mau_cmd_generate_usage(void)
{
   printf(
      (
         "Usage: %s generate [OPTIONS]\n"
         "COMMAND OPTIONS:\n"
         "  -r randomdev              a file containing random data\n"
         "  --vmware                  generate MAC address for VMWare\n"
         "  --xen                     generate MAC address for Xen\n"
         "\n"
      ), PROGRAM_NAME
   );
   return;
}


int mau_cmd_info(mau_config * cnf, int argc, char * argv[])
{
   int            c;
   int            opt_index;
   maustr_t       str;
   mauaddr_t      addr;

   // getopt options
   static char   short_opt[] = MAU_GETOPT_SHORT;
   static struct option long_opt[] =
   {
      MAU_GETOPT_LONG,
      {NULL, 0, NULL, 0 }
   };


   while((c = mau_getopt(cnf, argc, argv, short_opt, long_opt, &opt_index)) != -1)
   {
      switch(c)
      {
         case -2: /* captured by common options */
         case -1:	/* no more arguments */
         case 0:	/* long options toggles */
         break;

         case 1:
         return(0);
         case 2:
         return(1);

         case '?':
         fprintf(stderr, "Try `%s info --help' for more information.\n", PROGRAM_NAME);
         return(1);

         default:
         fprintf(stderr, "%s: unrecognized option `--%c'\n", PROGRAM_NAME, c);
         fprintf(stderr, "Try `%s info --help' for more information.\n", PROGRAM_NAME);
         return(1);
      };
   };

   if ((argc - optind) < 1)
   {
      fprintf(stderr, "%s: missing MAC address\n", PROGRAM_NAME);
      fprintf(stderr, "Try `%s info --help' for more information.\n", PROGRAM_NAME);
      return(1);
   };
   if ((optind+1) != argc)
   {
      fprintf(stderr, "%s: unrecognized argument `-- %s'\n", PROGRAM_NAME, argv[optind+1]);
      fprintf(stderr, "Try `%s info --help' for more information.\n", PROGRAM_NAME);
      return(1);
   };

   if ((mau_str2mac(cnf, argv[optind], addr)))
      return(1);

   mau_mac2str(cnf, addr, str);

   printf("MAC Address:    %s\n", str);
   printf("U/L Bit:        %s\n", ((addr[0] & 0x02) == 0x02) ? "1 (locally administered)" : "0 (universally administered)");
   printf("Multicast Bit:  %s\n", ((addr[0] & 0x01) == 0x01) ? "1 (multicast)" : "0 (unicast)");

   return(0);
}


/// generate command usage
void mau_cmd_info_usage(void)
{
   printf(
      (
         "Usage: %s info [OPTIONS] mac_address\n"
      ), PROGRAM_NAME
   );
   return;
}


int mau_getopt(mau_config * cnf, int argc, char * const * argv,
   const char * short_opt, const struct option * long_opt, int * opt_index)
{
   int            c;

   c = getopt_long(argc, argv, short_opt, long_opt, opt_index);

   switch(c)
   {
      case 'c':
      cnf->notation = MAU_SET_NOTATION(cnf->notation, MAU_NOTATION_COLON);
      return(-2);

      case 'D':
      cnf->notation = MAU_SET_NOTATION(cnf->notation, MAU_NOTATION_DOT);
      return(-2);

      case 'd':
      cnf->notation = MAU_SET_NOTATION(cnf->notation, MAU_NOTATION_DASH);
      return(-2);

      case 'h':
      mau_usage(cnf);
      return(1);

      case 'l':
      cnf->notation = MAU_SET_CASE(cnf->notation, MAU_CASE_LOWER);
      return(-2);

      case 'q':
      cnf->quiet++;
      return(-2);

      case 'R':
      cnf->notation = MAU_SET_NOTATION(cnf->notation, MAU_NOTATION_RAW);
      return(-2);

      case 'u':
      cnf->notation = MAU_SET_CASE(cnf->notation, MAU_CASE_UPPER);
      return(-2);

      case 'V':
      mau_version();
      return(1);

      case 'v':
      cnf->verbose++;
      return(-2);

      default:
      break;
   };
   return(c);
}


// print error message
void mau_log_err(mau_config * cnf, const char * fmt, ...)
{
   va_list args;
   if (cnf->verbose > 1)
      return;
   va_start(args, fmt);
   mau_logv(cnf, fmt, args);
   va_end(args);
   return;
}


// print verbose information
void mau_log_verbose(mau_config * cnf, const char * fmt, ...)
{
   va_list args;
   if (cnf->verbose < 1)
      return;
   va_start(args, fmt);
   mau_logv(cnf, fmt, args);
   va_end(args);
   return;
}


// print warning message
void mau_log_warn(mau_config * cnf, const char * fmt, ...)
{
   va_list args;
   if ((cnf->verbose))
      return;
   va_start(args, fmt);
   mau_logv(cnf, fmt, args);
   va_end(args);
   return;
}


// logs message
void mau_logv(mau_config * cnf, const char * fmt, va_list args)
{
   if ((cnf->cmd))
      fprintf(stderr, "%s %s: ", PROGRAM_NAME, cnf->cmd->cmd_name);
   else
      fprintf(stderr, "%s: ", PROGRAM_NAME);
   vfprintf(stderr, fmt, args);
   return;
}


int mau_mac2str(mau_config * cnf, const mauaddr_t addr, maustr_t str)
{
   switch (cnf->notation | (MAU_MASK_NOTATION & MAU_MASK_CASE))
   {
      case (MAU_NOTATION_COLON | MAU_CASE_LOWER):
      snprintf(str, sizeof(maustr_t), "%02x:%02x:%02x:%02x:%02x:%02x", addr[0],addr[1],addr[2],addr[3],addr[4],addr[5]);
      break;

      case (MAU_NOTATION_COLON | MAU_CASE_UPPER):
      snprintf(str, sizeof(maustr_t), "%02X:%02X:%02X:%02X:%02X:%02X", addr[0],addr[1],addr[2],addr[3],addr[4],addr[5]);
      break;

      case (MAU_NOTATION_DASH | MAU_CASE_LOWER):
      snprintf(str, sizeof(maustr_t), "%02x-%02x-%02x-%02x-%02x-%02x", addr[0],addr[1],addr[2],addr[3],addr[4],addr[5]);
      break;

      case (MAU_NOTATION_DASH | MAU_CASE_UPPER):
      snprintf(str, sizeof(maustr_t), "%02X-%02X-%02X-%02X-%02X-%02X", addr[0],addr[1],addr[2],addr[3],addr[4],addr[5]);
      break;

      case (MAU_NOTATION_DOT | MAU_CASE_LOWER):
      snprintf(str, sizeof(maustr_t), "%02x%02x.%02x%02x.%02x%02x", addr[0],addr[1],addr[2],addr[3],addr[4],addr[5]);
      break;

      case (MAU_NOTATION_DOT | MAU_CASE_UPPER):
      snprintf(str, sizeof(maustr_t), "%02X%02X.%02X%02X.%02X%02X", addr[0],addr[1],addr[2],addr[3],addr[4],addr[5]);
      break;

      case (MAU_NOTATION_RAW | MAU_CASE_LOWER):
      snprintf(str, sizeof(maustr_t), "%02x%02x%02x%02x%02x%02x", addr[0],addr[1],addr[2],addr[3],addr[4],addr[5]);
      break;

      case (MAU_NOTATION_RAW | MAU_CASE_UPPER):
      snprintf(str, sizeof(maustr_t), "%02X%02X%02X%02X%02X%02X", addr[0],addr[1],addr[2],addr[3],addr[4],addr[5]);
      break;

      default:
      snprintf(str, sizeof(maustr_t), "%02x:%02x:%02x:%02x:%02x:%02x", addr[0],addr[1],addr[2],addr[3],addr[4],addr[5]);
   };
   return(0);
}


int mau_str2mac(mau_config * cnf, const maustr_t str, mauaddr_t addr)
{
   int      buff[6];
   int      len;
   int      i;

   assert(str  != NULL);
   assert(addr != NULL);

   len = sscanf(str, "%02x:%02x:%02x:%02x:%02x:%02x", &buff[0],&buff[1],&buff[2],&buff[3],&buff[4],&buff[5]);
   if (len != 6)
      len = sscanf(str, "%02x-%02x-%02x-%02x-%02x-%02x", &buff[0],&buff[1],&buff[2],&buff[3],&buff[4],&buff[5]);
   if (len != 6)
      len = sscanf(str, "%02X%02X.%02X%02X.%02X%02X", &buff[0],&buff[1],&buff[2],&buff[3],&buff[4],&buff[5]);
   if (len != 6)
      len = sscanf(str, "%02X%02X%02X%02X%02X%02X", &buff[0],&buff[1],&buff[2],&buff[3],&buff[4],&buff[5]);
   if (len != 6)
   {
      mau_log_err(cnf, "invalid MAC address\n");
      return(1);
   };

   for(i = 0; i < 6; i++)
      addr[i] = (uint8_t) buff[i];

   return(0);
}


/// displays usage information
void mau_usage(mau_config * cnf)
{
   int i;

   if ((cnf->cmd))
      cnf->cmd->cmd_usage();
   else
   {
      printf("Usage: %s [OPTIONS] COMMAND ...\n", PROGRAM_NAME);
      printf("COMMANDS:\n");
      for(i = 0; mau_cmdmap[i].cmd_name != NULL; i++)
         printf("  %-25s %s\n", mau_cmdmap[i].cmd_name, mau_cmdmap[i].cmd_desc);
      printf("\n");
   };

   printf(
      (
         "COMMON OPTIONS:\n"
         "  -c, --colon               print MAC addresses in colon notation\n"
         "  -D, --dot                 print MAC addresses in dot notation\n"
         "  -d, --dash                print MAC addresses in dash notation\n"
         "  -h, --help                print this help and exit\n"
         "  -l, --lower               print MAC addresses in lower case\n"
         "  -q, --quiet, --silent     do not print messages\n"
         "  -R, --raw                 print MAC addresses in raw hex notation\n"
         "  -u, --upper               print MAC addresses in upper case\n"
         "  -V, --version             print version number and exit\n"
         "  -v, --verbose             print verbose messages\n"
         "\n"
      )
   );
   return;
}


/// displays version information
void mau_version(void)
{
   printf(
      (
         "%s (%s) %s\n"
         "Written by David M. Syzdek.\n"
         "\n"
         "%s\n"
         "This is free software; see the source for copying conditions.  There is NO\n"
         "warranty; not even for MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.\n"
      ), PROGRAM_NAME, PACKAGE_NAME, PACKAGE_VERSION, PACKAGE_COPYRIGHT
   );
   return;
}


/* end of source code */
