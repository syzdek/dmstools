/*
 *  DMS Tools and Utilities
 *  Copyright (C) 2008 David M. Syzdek <david@syzdek.net>.
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
 *  @file src/codetagger.c Utility for making bulk changes to tqgged text files.
 */
/*
 *  Simple Build:
 *     gcc -W -Wall -O2 -c codetagger.c
 *     gcc -W -Wall -O2 -o codetagger   codetagger.o
 *
 *  GNU Libtool Build:
 *     libtool --mode=compile gcc -W -Wall -g -O2 -c codetagger.c
 *     libtool --mode=link    gcc -W -Wall -g -O2 -o codetagger codetagger.lo
 *
 *  GNU Libtool Install:
 *     libtool --mode=install install -c codetagger /usr/local/bin/codetagger
 *
 *  GNU Libtool Clean:
 *     libtool --mode=clean rm -f codetagger.lo codetagger
 */
#define _DMSTOOLS_SRC_CODINGTAGGER_C 1

///////////////
//           //
//  Headers  //
//           //
///////////////
// MARK: - Headers

#ifdef HAVE_COMMON_H
#include "common.h"
#endif

#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <dirent.h>
#include <getopt.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include <stdarg.h>
#include <regex.h>
#include <errno.h>


///////////////////
//               //
//  Definitions  //
//               //
///////////////////
// MARK: - Definitions

#ifndef PROGRAM_NAME
#define PROGRAM_NAME "codetagger"
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

#define CODETAGGER_OPT_CONTINUE    0x0001
#define CODETAGGER_OPT_DEBUG       0x0002
#define CODETAGGER_OPT_FORCE       0x0004
#define CODETAGGER_OPT_HIDDEN      0x0008
#define CODETAGGER_OPT_LINKS       0x0010
#define CODETAGGER_OPT_QUIET       0x0020
#define CODETAGGER_OPT_RECURSE     0x0040
#define CODETAGGER_OPT_TEST        0x0080
#define CODETAGGER_OPT_VERBOSE     0x0100

#undef  CODETAGGER_STR_LEN
#define CODETAGGER_STR_LEN  ((size_t)512)

#ifndef PARAMS
#define PARAMS(protos) protos
#endif


/////////////////
//             //
//  Datatypes  //
//             //
/////////////////
// MARK: - Datatypes

/// tag data
typedef struct codetagger_data CodeTaggerData;
struct codetagger_data
{
   char     * name;
   char    ** contents;
   regex_t    regex;
};


/// config data
typedef struct codetagger_config CodeTagger;
struct codetagger_config
{
   unsigned          opts;
   unsigned          tagCount;
   size_t            buff_size;          ///< size of each file buffer
   ssize_t           len_orig;           ///< length of the content of the original file
   int               pos_orig;           ///< current location in buffer for original file
   int               pos_modd;           ///< current location in buffer for updated file
   char            * buff_orig;          ///< buffer for original file
   char            * buff_modd;          ///< buffer for updated file
   const char      * tagFile;
   const char      * leftTagString;
   const char      * rightTagString;
   regex_t           generic_tag;        ///< generic regex for finding start tag
   CodeTaggerData ** tagList;
};


//////////////
//          //
//  Macros  //
//          //
//////////////
// MARK: - Macros

#define codetagger_debug(cnf)           codetagger_debug_trace(cnf, __func__, NULL)
#define codetagger_debug_ext(cnf, ...)  codetagger_debug_trace(cnf, __func__, __VA_ARGS__)


//////////////////
//              //
//  Prototypes  //
//              //
//////////////////
// MARK: - Prototypes

static int
codetagger_buffer_resize(
         CodeTagger *                  cnf,
         size_t                        size,
         const char *                  error_prefix );


static int
codetagger_buffer_write(
         CodeTagger *                  cnf,
         const char *                  src,
         size_t                        len,
         const char *                  filename );


static void
codetagger_debug_trace(
         CodeTagger *                  cnf,
         const char *                  func,
         const char *                  fmt,
         ... );


static void
codetagger_error(
         CodeTagger *                  cnf,
         const char *                  fmt,
         ... );


static int
codetagger_escape_string(
         CodeTagger *                  cnf,
         char *                        buff,
         const char *                  str,
         size_t                        len );


static void
codetagger_free_filedata(
         CodeTagger *                  cnf,
         char **                       lines );


static void
codetagger_free_tag(
         CodeTagger *                  cnf,
         CodeTaggerData *              tag );


static void
codetagger_free_taglist(
         CodeTagger *                  cnf,
         CodeTaggerData **             taglist );


static char **
codetagger_get_file_contents(
         CodeTagger *                  cnf,
         const char *                  file );


static int
codetagger_parse_tag_file(
         CodeTagger *                  cnf );


static int
codetagger_prepare_regex(
         CodeTagger *                  cnf );


static CodeTaggerData *
codetagger_retrieve_tag_data(
         CodeTagger *                  cnf,
         const char *                  tagName,
         const char *                  fileName,
         int                           lineNumber );


static int
codetagger_scan_directory(
         CodeTagger *                  cnf,
         const char *                  origin );


static int
codetagger_scan_file(
         CodeTagger *                  cnf,
         const char *                  file,
         size_t *                      countp,
         char ***                      queuep,
         size_t *                      sizep );


static int
codetagger_update_file(
         CodeTagger *                  cnf,
         const char *                  filename,
         struct stat *                 sbp );


static void
codetagger_usage(
         void );


static void
codetagger_verbose(
         CodeTagger *                  cnf,
         const char *                  fmt,
         ... );


static void
codetagger_version(
         void );


static int
codetagger_process_tag(
         CodeTagger *                  cnf,
         char *                        name,
         char **                       data,
         size_t                        len,
         int                           pos );


extern int
main(
         int                           argc,
         char *                        argv[] );


/////////////////
//             //
//  Functions  //
//             //
/////////////////
// MARK: - Functions

/// resizes file buffers
/// @param[in]  cnf   pointer to config data structure
/// @param[in]  size   requested buffer size
int
codetagger_buffer_resize(
         CodeTagger *                  cnf,
         size_t                        size,
         const char *                  error_prefix )
{
   void * ptr;

   codetagger_debug(cnf);

   if (!(cnf))
      return(0);

   if (cnf->buff_size > size)
      return(0);

   if (size > (1024*1024*10))
   {
      if (error_prefix)
         codetagger_error(cnf, "%s: exceeded memory limits\n", error_prefix);
      else
         codetagger_error(cnf, "exceeded memory limits\n");
      return(1);
   };

   size += 1024 * 20; // pads buffer by 10K to reduce calls to realloc()
   codetagger_debug_ext(cnf, "%i", size);

   if (!(ptr = realloc(cnf->buff_orig, size)))
   {
      codetagger_error(NULL, "out of virtual memory\n");
      return(-1);
   };
   cnf->buff_orig = ptr;

   if (!(ptr = realloc(cnf->buff_modd, size)))
   {
      codetagger_error(NULL, "out of virtual memory\n");
      return(-1);
   };
   cnf->buff_modd = ptr;

   cnf->buff_size = size;

   return(0);
}


// appends source data to buffer of modified file
/// @param[in]  cnf       pointer to config data structure
/// @param[in]  src       source buffer to copy
/// @param[in]  len       length of data
/// @param[in]  filename  filename being processed
int
codetagger_buffer_write(
         CodeTagger *                  cnf,
         const char *                  src,
         size_t                        len,
         const char *                  filename )
{
   int err;

   if ((unsigned)(cnf->pos_modd+len) >= cnf->buff_size)
         if ((err = codetagger_buffer_resize(cnf, cnf->pos_modd+len, filename)))
            return(err);

   memcpy(&cnf->buff_modd[cnf->pos_modd], src, len);

   cnf->pos_modd += len;

   return(0);
}


/// prints debug messages
/// @param[in]  cnf   pointer to config data structure
/// @param[in]  func  name of calling function
/// @param[in]  fmt   the format string of the message
/// @param[in]  ...   the format arguments of the message
void
codetagger_debug_trace(
         CodeTagger *                  cnf,
         const char *                  func,
         const char *                  fmt,
         ... )
{
   char    buff[1024];
   va_list ap;

   if (!(cnf))
      return;
   if (!(cnf->opts & CODETAGGER_OPT_DEBUG))
      return;
   if (!(fmt))
   {
      fprintf(stderr, "codetagger: %s\n", func);
      return;
   };
   va_start(ap, fmt);
      vsnprintf(buff, (size_t)1023, fmt, ap);
   va_end(ap);
   buff[1023] = '\0';
   if (!(func))
      fprintf(stderr, "codetagger: %s\n", buff);
   else
      fprintf(stderr, "codetagger: %s: %s\n", func, buff);
   return;
}


/// prints error message
/// @param[in]  cnf    pointer to config data structure
/// @param[in]  fmt    the format string of the message
/// @param[in]  ...    the format arguments of the message
void
codetagger_error(
         CodeTagger *                  cnf,
         const char *                  fmt,
         ... )
{
   char    buff[1024];
   va_list ap;

   codetagger_debug(cnf);

   if (!(fmt))
      return;
   if (cnf)
      if ((cnf->opts & CODETAGGER_OPT_CONTINUE) && (cnf->opts & CODETAGGER_OPT_QUIET))
         return;

   va_start(ap, fmt);
      vsnprintf(buff, (size_t)1023, fmt, ap);
   va_end(ap);
   buff[1023] = '\0';

   fprintf(stderr, PROGRAM_NAME ": %s", buff);

   return;
}


/// creates a regex safe string (escapes chracters meaning full in regex)
/// @param[in]  cnf   pointer to config data structure
/// @param[in]  buff  buffer into which escape the string
/// @param[in]  str   string to escape and concatenate from
/// @param[in]  len   size of buffer
int
codetagger_escape_string(
         CodeTagger *                  cnf,
         char *                        buff,
         const char *                  str,
         size_t                        len )
{
   size_t strpos;
   size_t buffpos;

   codetagger_debug(cnf);

   len--;
   buff[len] = '\0';
   
   buffpos = strlen(buff) + 2;
   for(strpos = 0; ( (strpos < strlen(str)) && (buffpos < len)); strpos++)
   {
      buff[buffpos-2] = '\\';
      buff[buffpos-1] = str[strpos];
      buffpos += 2;
   };
   buff[buffpos] = '\0';
   return(0);
}


/// frees memory used to hold file contents
/// @param[in]  cnf    pointer to config data structure
/// @param[in]  lines  array of lines to free
void
codetagger_free_filedata(
         CodeTagger *                  cnf,
         char **                       lines )
{
   if (!(lines))
      return;

   codetagger_debug(cnf);

   if (lines[0])
      free(lines[0]);
   lines[0] = NULL;

   free(lines);

   return;
}


/// frees memory used store tag
/// @param[in]  cnf  pointer to config data structure
/// @param[in]  tag  tag data structure to free
void
codetagger_free_tag(
         CodeTagger *                  cnf,
         CodeTaggerData *              tag )
{
   int i;

   codetagger_debug(cnf);

   if (!(tag))
      return;

   if (tag->name)
      free(tag->name);

   if (tag->contents)
   {
      regfree(&tag->regex);
      for(i = 0; tag->contents[i]; i++)
         free(tag->contents[i]);
      free(tag->contents);
   };

   free(tag);

   return;
}


/// frees memory used store an array of tags
/// @param[in]  cnf      pointer to config data structure
/// @param[in]  taglist  array of tags to free
void
codetagger_free_taglist(
         CodeTagger *                  cnf,
         CodeTaggerData **             taglist )
{
   int i;
   codetagger_debug(cnf);
   if (!(taglist))
      return;
   for(i = 0; taglist[i]; i++)
      codetagger_free_tag(cnf, taglist[i]);
   free(taglist);
   return;
}


/// reads file into an array
/// @param[in]  cnf   pointer to config data structure
/// @param[in]  file  file name of file to process
char **
codetagger_get_file_contents(
         CodeTagger *                  cnf,
         const char *                  file )
{
   int            i;
   int            fd;
   ssize_t        len;
   int            count;
   char         * buff;
   char        ** lines;
   struct stat    sb;

   codetagger_debug(cnf);

   if ((stat(file, &sb)) == -1)
   {
      perror(PROGRAM_NAME ": stat()");
      return(NULL);
   };

   if ((fd = open(file, O_RDONLY)) == -1)
   {
      perror(PROGRAM_NAME ": open()");
      return(NULL);
   };

   if (!(buff = (char *) malloc((size_t)(sb.st_size + 1))))
   {
      fprintf(stderr, PROGRAM_NAME ": out of virtual memory\n");
      close(fd);
      return(NULL);
   };

   if ((len = read(fd, buff, (size_t)sb.st_size)) == -1)
   {
      perror(PROGRAM_NAME ": read()");
      close(fd);
      return(NULL);
   };
   buff[len] = '\0';

   close(fd);

   count = 0;
   for(i = 0; i < len; i++)
   {
      if (buff[i] == '\n')
      {
         count++;
         buff[i] = '\0';
      };
   };

   count++;
   if (!(lines = (char **) malloc(sizeof(char *) * count)))
   {
      perror(PROGRAM_NAME ": read()");
      free(buff);
      return(NULL);
   };

   lines[0] = buff;
   count = 0;
   for(i = 0; i < len; i++)
   {
      if (buff[i] == '\0')
      {
         count++;
         lines[count] = &buff[i+1];
      };
   };
   lines[count] = NULL;

   return(lines);
}


/// generate array of tags from file
/// @param[in]  cnf   pointer to config data structure
int
codetagger_parse_tag_file(
         CodeTagger *                  cnf )
{
   int           i;
   int           err;
   size_t        lineCount;
   char          regstr[CODETAGGER_STR_LEN];
   char          errmsg[CODETAGGER_STR_LEN];
   char          tagName[CODETAGGER_STR_LEN];
   char       ** data;
   regex_t       regex;
   regmatch_t    match[5];

   codetagger_debug(cnf);

   memset(regstr,   0, CODETAGGER_STR_LEN);
   memset(errmsg,   0, CODETAGGER_STR_LEN);
   memset(tagName,  0, CODETAGGER_STR_LEN);

   if (!(data = codetagger_get_file_contents(cnf, cnf->tagFile)))
      return(-1);

   for(lineCount = 0; data[lineCount]; lineCount++);

   // compiles regex used to locate tags
   strncat(regstr,       "^",              CODETAGGER_STR_LEN);
   codetagger_escape_string(cnf, regstr, cnf->leftTagString,  CODETAGGER_STR_LEN);
   strncat(regstr,       "([_[:blank:][:alnum:]]+)START",   CODETAGGER_STR_LEN);
   codetagger_escape_string(cnf, regstr, cnf->rightTagString, CODETAGGER_STR_LEN);
   if ((err = regcomp(&regex, regstr, REG_EXTENDED|REG_ICASE)))
   {
      regerror(err, &regex, errmsg, (CODETAGGER_STR_LEN-1));
      printf("regex error: %s\n", errmsg);
      return(-1);
   };

   // loops through looking for tags
   codetagger_verbose(cnf, "indexing \"%s\"\n", cnf->tagFile);
   for(i = 0; i < (int)lineCount; i++)
   {
      if (!(regexec(&regex, data[i], (size_t)5, match, 0)))
      {
         data[i][(int)match[1].rm_eo] = '\0';
         strncpy(tagName, &data[i][(int)match[1].rm_so], (CODETAGGER_STR_LEN-1));
         i++;
         if ((i = codetagger_process_tag(cnf, tagName, data, lineCount, i)) == -1)
         {
            codetagger_free_filedata(cnf, data);
            regfree(&regex);
            return(-1);
         };
         cnf->tagCount++;
      };
   };

   codetagger_free_filedata(cnf, data);
   regfree(&regex);

   return(0);
}


/// prepares generic regular expressions
/// @param[in]  cnf   pointer to config data structure
int
codetagger_prepare_regex(
         CodeTagger *                  cnf )
{
   int           err;
   char          errmsg[CODETAGGER_STR_LEN];
   char          regstr[CODETAGGER_STR_LEN];

   codetagger_debug(cnf);

   memset(regstr, 0, CODETAGGER_STR_LEN);
   memset(errmsg, 0, CODETAGGER_STR_LEN);

   strncat(regstr, "(.*)", CODETAGGER_STR_LEN);
   codetagger_escape_string(cnf, regstr, cnf->leftTagString,  CODETAGGER_STR_LEN);
   strncat(regstr, "([_[:blank:][:alnum:]]+)START", CODETAGGER_STR_LEN);
   codetagger_escape_string(cnf, regstr, cnf->rightTagString, CODETAGGER_STR_LEN);

   if ((err = regcomp(&cnf->generic_tag, regstr, REG_EXTENDED|REG_ICASE)))
   {
      regerror(err, &cnf->generic_tag, errmsg, (CODETAGGER_STR_LEN-1));
      fprintf(stderr, PROGRAM_NAME ": regex error: %s\n", errmsg);
      return(1);
   };

   return(0);
}


/// retrieves a specific tag from the tag list
/// @param[in]  cnf         pointer to config data structure
/// @param[in]  tagName     name of tag to find
/// @param[in]  fileName    file currently being updated
/// @param[in]  lineNumber  line number of the current file being updated
CodeTaggerData *
codetagger_retrieve_tag_data(
         CodeTagger *                  cnf,
         const char *                  tagName,
         const char *                  fileName,
         int                           lineNumber )
{
   int i;

   codetagger_debug(cnf);

   if (!(cnf))
      return(NULL);
   if (!(cnf->tagList))
      return(NULL);
   if (!(tagName))
      return(NULL);
   if (!(fileName))
      return(NULL);

   for(i = 0; cnf->tagList[i]; i++)
      if (!(strcasecmp(tagName, cnf->tagList[i]->name)))
         return(cnf->tagList[i]);

   codetagger_verbose(cnf, PROGRAM_NAME ": %s: %i: unknown tag \"%s\"\n", fileName, lineNumber, tagName);

   return(NULL);
}


/// recursively scans directory for files to update
/// @param[in]  cnf      pointer to config data structure
/// @param[in]  origin   starting point of the directory recursion
int
codetagger_scan_directory(
         CodeTagger *                  cnf,
         const char *                  origin )
{
   int             err;
   DIR           * d;
   char          * file;
   char         ** queue;
   size_t          len_dir;
   size_t          len_file;
   size_t          size;
   size_t          count;
   struct dirent * dp;

   size  = 0;
   count = 0;
   queue = NULL;

   // seeds queue with first file/directory
   if ((err = codetagger_scan_file(cnf, origin, &count, &queue, &size)))
      return(err);

   if (!(cnf->opts & CODETAGGER_OPT_RECURSE))
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
      codetagger_verbose(cnf, "scanning \"%s\"\n", queue[count-1]);

      // opens directory for processing
      if (!(d = opendir(queue[count-1])))
      {
         codetagger_error(cnf, "%s: %s\n", queue[count-1], strerror(errno));
         if (!(cnf->opts & CODETAGGER_OPT_CONTINUE))
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

         if ( (dp->d_name[0] == '.') && (cnf->opts & CODETAGGER_OPT_HIDDEN) )
         {
            if ( (dp->d_name[1] != '/') &&
                 (dp->d_name[1] != '\0') &&
                 (dp->d_name[1] != '.') )
            {
               switch (codetagger_scan_file(cnf, file, &count, &queue, &size))
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
                  if (!(cnf->opts & CODETAGGER_OPT_CONTINUE))
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
            switch (codetagger_scan_file(cnf, file, &count, &queue, &size))
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
               if (!(cnf->opts & CODETAGGER_OPT_CONTINUE))
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


/// scan file to determine wether to attempt and update
/// @param[in]  cnf      pointer to config data structure
/// @param[in]  file     file/directory to either update or queue for scanning
/// @param[in]  countp   pointer to number of directories waiting to be scanned
/// @param[in]  queuep   pointer to array of directories waiting to be scanned
/// @param[in]  sizep    pointer to size of array of directories waiting to be scanned
int
codetagger_scan_file(
         CodeTagger *                  cnf,
         const char *                  file,
         size_t *                      countp,
         char ***                      queuep,
         size_t *                      sizep )
{
   int             err;
   void          * ptr;
   ssize_t         u;
   struct stat     sb;

   if (cnf->opts & CODETAGGER_OPT_LINKS)
      err = stat(file, &sb);
   else
      err = lstat(file, &sb);
   if (err == -1)
   {
      codetagger_error(cnf, "%s: %s\n", file, strerror(errno));
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
      return(codetagger_update_file(cnf, file, &sb));

      default:
      codetagger_error(cnf, "%s: unknown file type\n", file);
      return(1);
   };

   return(0);
}


/// updates original file by inserting/expanding tags
/// @param[in]  cnf       pointer to config data structure
/// @param[in]  filename  name of file to process
int
codetagger_update_file(
         CodeTagger *                  cnf,
         const char *                  filename,
         struct stat *                 sbp )
{
   int          fd;
   int          err;
   long long    pos;
   int          changed;
   long long    len_margin;
   long long    len_stripped;
   long long    len_tagname;
   char         margin[CODETAGGER_STR_LEN];
   char         stripped[CODETAGGER_STR_LEN];
   char         tagname[CODETAGGER_STR_LEN];
   char       * bol;
   regmatch_t   match[5];
   CodeTaggerData * tag;
   struct stat sb;

   codetagger_debug(cnf);
   codetagger_debug_ext(cnf, filename);
   codetagger_verbose(cnf, "processing \"%s\"\n", filename);

   changed = 0;

   if ((err = codetagger_buffer_resize(cnf, (size_t)sbp->st_size, filename)))
      return(err);

   // imports original file into buffer
   if ((lstat(filename, &sb)))
   {
      codetagger_error(cnf, "%s: %s\n", filename, strerror(errno));
      return(1);
   };
   if ((fd = open(filename, O_RDONLY)) == -1)
   {
      codetagger_error(cnf, "%s: %s\n", filename, strerror(errno));
      return(1);
   };
   if ((cnf->len_orig = read(fd, cnf->buff_orig, (size_t)sbp->st_size)) == -1)
   {
      codetagger_error(cnf, "%s: %s\n", filename, strerror(errno));
      close(fd);
      return(1);
   };
   cnf->buff_orig[sbp->st_size] = '\0';
   close(fd);

   cnf->pos_modd = 0;
   bol           = &cnf->buff_orig[0];

   for(cnf->pos_orig = 0; cnf->pos_orig < cnf->len_orig; cnf->pos_orig++)
   {
      if ((unsigned)(cnf->pos_modd+1) >= cnf->buff_size)
         if ((err = codetagger_buffer_resize(cnf, (size_t)cnf->pos_modd+1, filename)))
            return(err);

      cnf->buff_modd[cnf->pos_modd] = cnf->buff_orig[cnf->pos_orig];
      cnf->pos_modd++;
      if (cnf->buff_orig[cnf->pos_orig] != '\n')
         continue;

      cnf->buff_orig[cnf->pos_orig] = '\0';
      err = regexec(&cnf->generic_tag, bol, (size_t)5, match, 0);
      cnf->buff_orig[cnf->pos_orig] = '\n';

      if (err != 0)
      {
         bol = &cnf->buff_orig[cnf->pos_orig+1];
         continue;
      };

      len_margin  = match[1].rm_eo - match[1].rm_so;
      len_tagname = match[2].rm_eo - match[2].rm_so;

      memcpy(margin,  &bol[(int)match[1].rm_so], (size_t)len_margin);
      memcpy(tagname, &bol[(int)match[2].rm_so], (size_t)len_tagname);

      margin[len_margin]   = '\0';
      tagname[len_tagname] = '\0';

      len_stripped = 0;
      strncpy(stripped, margin, (size_t)len_margin);
      for(pos = len_margin; ((pos > 0) && (!(len_stripped))); pos--)
         if ((stripped[pos-1] != ' ') && (stripped[pos-1] != '\t'))
            len_stripped = pos;
      stripped[len_stripped] = '\0';

      bol = &cnf->buff_orig[cnf->pos_orig+1];

      if (!(tag = codetagger_retrieve_tag_data(cnf, tagname, filename, cnf->pos_orig)))
         continue;

      // fast forwards to end tag in original file
      if ((regexec(&tag->regex, &cnf->buff_orig[cnf->pos_orig+1], (size_t)5, match, 0)))
      {
         codetagger_error(cnf, "%s: missing \"%sEND\" tag\n", filename, tagname);
         return(1);
      };
      cnf->pos_orig += match[1].rm_eo;
      bol = &cnf->buff_orig[cnf->pos_orig];

      for(pos = 0; tag->contents[pos]; pos++)
      {
         if (!(tag->contents[pos][0]))
         {
            if ((err = codetagger_buffer_write(cnf, stripped, (size_t)len_stripped, filename)))
               return(err);
         } else {
            if ((err = codetagger_buffer_write(cnf, margin, (size_t)len_margin, filename)))
               return(err);
         };
         if ((err = codetagger_buffer_write(cnf, tag->contents[pos], strlen(tag->contents[pos]), filename)))
            return(err);
         if ((unsigned)(cnf->pos_modd+1) >= cnf->buff_size)
            if ((err = codetagger_buffer_resize(cnf, (size_t)cnf->pos_modd+1, filename)))
               return(err);
         cnf->buff_modd[cnf->pos_modd] = '\n';
         cnf->pos_modd++;
      };

      if ((err = codetagger_buffer_write(cnf, margin, (size_t)len_margin, filename)))
         return(err);
      if ((err = codetagger_buffer_write(cnf, cnf->leftTagString, strlen(cnf->leftTagString), filename)))
         return(err);
      if ((err = codetagger_buffer_write(cnf, tagname, (size_t)len_tagname, filename)))
         return(err);
      if ((err = codetagger_buffer_write(cnf, "END", (size_t)3, filename)))
         return(err);
   };

   if (cnf->pos_modd != cnf->pos_orig)
      changed = 1;
   else
      for(pos = 0; ( (pos < cnf->pos_orig) && (!(changed)) ); pos++)
         if (cnf->buff_modd[pos] != cnf->buff_orig[pos])
            changed = 1;
   if (!(changed))
      return(0);

   if (!(cnf->opts & CODETAGGER_OPT_QUIET))
      printf("updating \"%s\"\n", filename);

   if ((fd = open(filename, O_WRONLY|O_TRUNC|O_CREAT, sb.st_mode)) == -1)
   {
      codetagger_error(cnf, "%s: %s\n", filename, strerror(errno));
      close(fd);
      return(1);
   };
   if (write(fd, cnf->buff_modd, (size_t)cnf->pos_modd) == -1)
   {
      codetagger_error(cnf, "%s: %s\n", filename, strerror(errno));
      close(fd);
      return(1);
   };
   close(fd);

   return(0);
}


/// displays usage
void
codetagger_usage(
         void )
{
   printf("Usage: %s [OPTIONS] files\n", PROGRAM_NAME);
   printf("  -a                        include hidden files\n");
   printf("  -c                        continue on error\n");
   printf("  -d                        enter debug mode\n");
   printf("  -f                        force writes\n");
   printf("  -h, --help                print this help and exit\n");
   printf("  -i file                   file containing tags\n");
   printf("  -L                        follow symbolic links\n");
   printf("  -l str                    left enclosing string for tags\n");
   printf("  -q, --quiet, --silent     do not print messages\n");
   printf("  -r str                    right enclosing string for tags\n");
   printf("  -R                        recursively follow directories\n");
   printf("  -t, --test                show what would be done\n");
   printf("  -v, --verbose             print verbose messages\n");
   printf("  -V, --version             print version number and exit\n");
   printf("\n");
   printf("Report bugs to <%s>.\n", PACKAGE_BUGREPORT);
   return;
}


/// prints verbose messages
/// @param[in] cnf   pointer to config data structure
/// @param[in] fmt   the format string of the message
/// @param[in] ...   the format arguments of the message
void
codetagger_verbose(
         CodeTagger *                  cnf,
         const char *                  fmt,
         ... )
{
   va_list arg;

   if (!(cnf))
      return;
   if (!(cnf->opts & CODETAGGER_OPT_VERBOSE))
      return;

   va_start (arg, fmt);
      vprintf(fmt, arg);
   va_end (arg);

   return;
}


/// displays version information
void
codetagger_version(
         void )
{
   printf("%s (%s) %s\n", PROGRAM_NAME, PACKAGE_NAME, PACKAGE_VERSION);
   printf("Written by David M. Syzdek.\n");
   printf("\n");
   printf("%s\n", PACKAGE_COPYRIGHT);
   printf("This is free software; see the source for copying conditions.  There is NO\n");
   printf("warranty; not even for MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.\n");
   return;
}


/// processes tag from tag file
/// @param[in]  cnf       pointer to config data structure
/// @param[in]  name      name of tag to process
/// @param[in]  data      data from which to extract the tag
/// @param[in]  len       length of data array
/// @param[in]  pos       current position within the data
int
codetagger_process_tag(
         CodeTagger *                  cnf,
         char *                        name,
         char **                       data,
         size_t                        len,
         int                           pos )
{
   /* declares local vars */
   int           i;
   int           err;
   int           count;
   int           tagCount;
   int           line_count;
   CodeTaggerData     * tag;
   void        * ptr;
   char          regstr[CODETAGGER_STR_LEN];
   char          errmsg[CODETAGGER_STR_LEN];
   regmatch_t    match[5];

   codetagger_debug(cnf);

   if (!(name))
      return(-1);
   if (!(data))
      return(-1);
   if (!(cnf))
      return(-1);
      
   memset(regstr, 0, CODETAGGER_STR_LEN);
   memset(errmsg, 0, CODETAGGER_STR_LEN);

   // counts tags in taglist
   tagCount = 0;
   if (cnf->tagList)
      for(tagCount = 0; cnf->tagList[tagCount]; tagCount++);

   // allocates memory for tag
   tagCount++;
   if (!(tag = (CodeTaggerData *) malloc(sizeof(CodeTaggerData))))
   {
      fprintf(stderr, "%s: out of virtual memory\n", PROGRAM_NAME);
      return(-1);
   };
   memset(tag, 0, sizeof(CodeTaggerData));

   // allocates memory for array
   if (!(ptr = realloc(cnf->tagList, sizeof(CodeTaggerData) * (tagCount + 1))))
   {
      fprintf(stderr, "%s: out of virtual memory\n", PROGRAM_NAME);
      codetagger_free_tag(cnf, tag);
      return(-1);
   };
   cnf->tagList             = ptr;
   cnf->tagList[tagCount-1] = NULL;
   cnf->tagList[tagCount]   = NULL;

   // saves tag name
   if (!(tag->name = strdup(name)))
   {
      fprintf(stderr, "%s: out of virtual memory\n", PROGRAM_NAME);
      codetagger_free_tag(cnf, tag);
      return(-1);
   };

   // compiles regular expression
   codetagger_escape_string(cnf, regstr, cnf->leftTagString,  CODETAGGER_STR_LEN);
   strncat(regstr,       "(",                 CODETAGGER_STR_LEN);
   strncat(regstr,       name,                CODETAGGER_STR_LEN);
   strncat(regstr,       "END",               CODETAGGER_STR_LEN);
   strncat(regstr,       ")",                 CODETAGGER_STR_LEN);
   codetagger_escape_string(cnf, regstr, cnf->rightTagString, CODETAGGER_STR_LEN);
   if ((err = regcomp(&tag->regex, regstr, REG_EXTENDED|REG_ICASE)))
   {
      regerror(err, &tag->regex, errmsg, CODETAGGER_STR_LEN-1);
      printf("regex error: %s\n", errmsg);
      codetagger_free_tag(cnf, tag);
      return(-1);
   };

   // loops through file
   err = 1;
   for(count = pos; ((count < (int)len) && (err)); count++)
      err = regexec(&tag->regex, data[count], (size_t)5, match, 0);

   // checks for error
   if (err)
   {
      codetagger_free_tag(cnf, tag);
      return(-1);
   };

   // calculate number of lines in tag
   line_count = count - pos - 1;

   // allocate memory for tag contents
   if (!(tag->contents = (char **) malloc(sizeof(char *) * (line_count + 1))))
   {
      codetagger_free_tag(cnf, tag);
      return(-1);
   };
   memset(tag->contents, 0, (sizeof(char *) * (line_count + 1)));

   // copies lines into tag
   for(i = pos; i < (pos + line_count); i++)
   {
      if (!(tag->contents[i-pos] = strdup(data[i])))
      {
         codetagger_free_tag(cnf, tag);
         return(-1);
      };
   };

   cnf->tagList[tagCount-1] = tag;

   return(i);
}


/// main statement
/// @param[in]  argc  number of arguments passed to program
/// @param[in]  argv  array of arguments passed to program
int
main(
         int                           argc,
         char *                        argv[] )
{
   int           c;
   int           i;
   int           opt_index;
   CodeTagger        cnf;

   static char   short_opt[] = "acdfhi:Ll:qRr:tvV";
   static struct option long_opt[] =
   {
      {"continue",      no_argument, 0, 'c'},
      {"help",          no_argument, 0, 'h'},
      {"silent",        no_argument, 0, 'q'},
      {"quiet",         no_argument, 0, 'q'},
      {"test",          no_argument, 0, 't'},
      {"verbose",       no_argument, 0, 'v'},
      {"version",       no_argument, 0, 'V'},
      {NULL,            0,           0, 0  }
   };

#ifdef HAVE_GETTEXT
   setlocale (LC_ALL, "");
   bindtextdomain (PACKAGE, LOCALEDIR);
   textdomain (PACKAGE);
#endif

   memset(&cnf, 0, sizeof(CodeTagger));
   cnf.leftTagString  = "@";
   cnf.rightTagString = "@";
   cnf.tagFile        = NULL;
   opt_index          = 0;

   while((c = getopt_long(argc, argv, short_opt, long_opt, &opt_index)) != -1)
   {
      switch(c)
      {
         case -1:	/* no more arguments */
         case 0:	/* long options toggles */
         break;

         case 'a':
         cnf.opts |= CODETAGGER_OPT_HIDDEN;
         break;

         case 'c':
         cnf.opts |= CODETAGGER_OPT_CONTINUE;
         break;

         case 'd':
         cnf.opts |= CODETAGGER_OPT_DEBUG;
         break;

         case 'f':
         cnf.opts |= CODETAGGER_OPT_FORCE;
         break;

         case 'h':
         codetagger_usage();
         return(0);

         case 'i':
         cnf.tagFile = optarg;
         break;

         case 'L':
         cnf.opts |= CODETAGGER_OPT_LINKS;
         break;

         case 'l':
         cnf.leftTagString = optarg;
         break;

         case 'q':
         cnf.opts |= CODETAGGER_OPT_QUIET;
         cnf.opts &= cnf.opts & (~CODETAGGER_OPT_VERBOSE);
         break;

         case 'R':
         cnf.opts |= CODETAGGER_OPT_RECURSE;
         break;

         case 'r':
         cnf.rightTagString = optarg;
         break;

         case 't':
         cnf.opts |= CODETAGGER_OPT_TEST;
         break;

         case 'V':
         codetagger_version();
         return(0);

         case 'v':
         cnf.opts |= CODETAGGER_OPT_VERBOSE;
         cnf.opts &= cnf.opts & (~CODETAGGER_OPT_QUIET);
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

   // verifies arguments were passed on the command line
   if ((optind == argc) || (optind == 1))
   {
      codetagger_usage();
      return(1);
   };
   if (!(cnf.tagFile ))
   {
      fprintf(stderr, "%s: missing required option `-f'\n", PROGRAM_NAME);
      fprintf(stderr, "Try `%s --help' for more information.\n", PROGRAM_NAME);
      return(1);
   };

   codetagger_debug(&cnf);

   codetagger_debug_ext(&cnf, "Debug Mode:        %s", (cnf.opts & CODETAGGER_OPT_DEBUG)    ? "yes" : "no");
   codetagger_debug_ext(&cnf, "Continue on Error: %s", (cnf.opts & CODETAGGER_OPT_CONTINUE) ? "yes" : "no");
   codetagger_debug_ext(&cnf, "Hidden Files:      %s", (cnf.opts & CODETAGGER_OPT_HIDDEN)   ? "yes" : "no");
   codetagger_debug_ext(&cnf, "Follow Symlinks:   %s", (cnf.opts & CODETAGGER_OPT_LINKS)    ? "yes" : "no");
   codetagger_debug_ext(&cnf, "Quiet Mode:        %s", (cnf.opts & CODETAGGER_OPT_QUIET)    ? "yes" : "no");
   codetagger_debug_ext(&cnf, "Recurse Mode:      %s", (cnf.opts & CODETAGGER_OPT_RECURSE)  ? "yes" : "no");
   codetagger_debug_ext(&cnf, "Test Mode:         %s", (cnf.opts & CODETAGGER_OPT_TEST)     ? "yes" : "no");
   codetagger_debug_ext(&cnf, "Verbose Mode:      %s", (cnf.opts & CODETAGGER_OPT_VERBOSE)  ? "yes" : "no");
   codetagger_debug_ext(&cnf, "Left Bracket:      %s", cnf.leftTagString);
   codetagger_debug_ext(&cnf, "Right Bracket:     %s", cnf.rightTagString);

   if ((codetagger_parse_tag_file(&cnf)))
      return(0);

   if (codetagger_prepare_regex(&cnf))
      return(1);

   if (!(cnf.tagCount))
   {
      fprintf(stderr, "%s: no tag definitions were found in \"%s\"\n", PROGRAM_NAME, cnf.tagFile);
      return(1);
   };

   // loops through files to be tagged
   for(i = optind; i < argc; i++)
      switch (codetagger_scan_directory(&cnf, argv[i]))
      {
         case -1: return(1);
         case 0:  break;

         default:
         if (!(cnf.opts & CODETAGGER_OPT_CONTINUE))
            return(1);
         break;
      };

   codetagger_free_taglist(&cnf, cnf.tagList);

   return(0);
}


/* end of source file */
