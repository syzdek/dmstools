/*
 *   Recursively list files in a directory.
 */


#include <stdio.h>
#include <stdlib.h>
#include <getopt.h>
#include <string.h>
#include <errno.h>
#include <sys/stat.h>
#include <dirent.h>

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

//typedef struct recurse_data Recurse;
//struct recurse_data
//{
//   char ** 
//};


int main(int argc, char * argv[]);
void my_version(void);
void my_usage(void);
int recurse_directory(int * lenp, char *** listp, int opts);
int recurse_list(const char * src, int opts);



int main(int argc, char * argv[])
{
   int        c;
   int        i;
   int        err;
   int        opts;
   int        option_index;
   int        len;
   void     * ptr;
   char    ** list;

   static char   short_options[] = "achLqrvV";
   static struct option long_options[] = 
   {
      {"help",          no_argument, 0, 'h'},
      {"quiet",         no_argument, 0, 'q'},
      {"silent",        no_argument, 0, 'q'},
      {"verbose",       no_argument, 0, 'v'},
      {"version",       no_argument, 0, 'V'},
      {NULL,            0,           0, 0  }
   };

   len          = 0;
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
   {
/*
      len++;
      if (!(ptr = realloc(list, (sizeof(char *)*len))))
      {
         fprintf(stderr, "out of virtual memory\n");
         for(i = 0; i < (len-1); i++)
            free(list[0]);
         free(list);
         return(1);
      };
      list = ptr;
      if (!(list[len-1] = strdup(argv[i])))
      {
         fprintf(stderr, "out of virtual memory\n");
         for(i = 0; i < (len-1); i++)
            free(list[0]);
         free(list);
         return(1);
      };
*/
      if ((recurse_list(argv[i], opts)))
         return(1);
   };

   err = recurse_list(&len, &list, opts);

   for(i = 0; i < len; i++)
      free(list[i]);
   free(list);

   return(err);
}


/* displays usage */
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


int recurse_directory(int * lenp, char *** listp, int opts)
{
   int             err;
   DIR           * d;
   void          * ptr;
   char          * dir;
   char          * file;
   size_t          dir_len;
   size_t          file_len;
   struct stat     sb;
   struct dirent * dp;

   // points to directory name
   if (!(dir = (*listp)[(*lenp)-1]))
      return(0);
   //(*listp)[(*lenp)-1] = NULL;
   //(*lenp)--;
   
   // opens directory for reading
   if (!(d = opendir(dir)))
   {
      if (opts & MY_OPT_QUIET)
         fprintf(stderr, "%s: %s, %s\n", PROGRAM_NAME, dir, strerror(errno) );
      free(dir);
      (*listp)[(*lenp)-1] = NULL;
      if (!(opts & MY_OPT_CONTINUE))
         return(0);
   };

   // loops through directory
   for(dp = readdir(d); dp; dp = readdir(d))
   {
      // skips hidden files, current directory, and parent
      if (dp->d_name[0] == '.')
      {
         if (dp->d_name[1] == '\0')
            continue;
         if ( (dp->d_name[1] == '.') && (dp->d_name[2] == '\0') )
            continue;
         if (!(opts & MY_OPT_HIDDEN))
            continue;
      };

      // builds file name
      dir_len  = strlen(dir);
      file_len = strlen(dp->d_name);
      if (!(file = malloc(sizeof(char) * (dir_len + file_len + 2))))
      {
         fprintf(stderr, "%s: out of virtual memory\n", PROGRAM_NAME);
         free(dir);
         return(1);
      };
      strcpy(&file[0],         dir);
      strcpy(&file[dir_len],   "/");
      strcpy(&file[dir_len+1], dp->d_name);
      file[dir_len+file_len+1] = '\0';

      // retrieves file information
      if ((opts & MY_OPT_LINKS))
         err = stat(file, &sb);
      else
         err = lstat(file, &sb);
      if (err == -1)
         if (opts & MY_OPT_QUIET)
            fprintf(stderr, "%s: %s, %s\n", PROGRAM_NAME, file, strerror(errno) );

      // checks file type
      switch(sb.st_mode & (S_IFDIR|S_IFREG|S_IFLNK))
      {
         case S_IFDIR:
            if (!(ptr = realloc(*listp, sizeof(char *) * ((*lenp)+1))))
            {
               fprintf(stderr, "%s: out of virtual memory\n", PROGRAM_NAME);
               free(dir);
               free(file);
               closedir(d);
               return(1);
            };
            *listp = ptr;
            (*listp)[(*lenp)] = file;
            (*lenp)++;
            printf("%s\n", file);
            break;
         case S_IFREG:
            //if (!(opts & MY_OPT_QUIET))
               printf("%s\n", file);
            free(file);
            break;
         //case S_IFLNK:
         //   free(file);
         //   break;
         default:
            //if (!( (opts & MY_OPT_QUIET) && (opts & MY_OPT_CONTINUE) ))
            //   fprintf(stderr, "%s: %s: invalid file type\n", PROGRAM_NAME, file);
            free(file);
            //if (!(opts & MY_OPT_CONTINUE))
            //{
            //   closedir(d);
            //   return(1);
            //};
            break;
      };
   };

   closedir(d);

   free(dir);

   return(0);
}


int recurse_list(const char * src, int opts)
{
   int     len;
   char ** list;
   
   len  = 0;
   list = NULL;
   
   while(len > 0)
   {
      printf("%s\n", list[len-1]);
      //if ((recurse_directory(lenp, listp, opts)))
      //   return(1);
      len--;
      if (list[len])
         free(list[len]);
      list[len] = NULL;
   };
   return(0);
}

/* end of source file */
