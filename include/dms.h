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
 *  @file lib/dms.h defines API for libdms.la
 */
#ifndef _DMS_H
#define _DMS_H 1

//////////////
//          //
//  Macros  //
//          //
//////////////

/*
 * The macros "BEGIN_C_DECLS" and "END_C_DECLS" are taken verbatim
 * from section 7.1 of the Libtool 1.5.14 manual.
 */
/* BEGIN_C_DECLS should be used at the beginning of your declarations,
   so that C++ compilers don't mangle their names. Use END_C_DECLS at
   the end of C declarations. */
#undef BEGIN_C_DECLS
#undef END_C_DECLS
#if defined(__cplusplus) || defined(c_plusplus)
#   define BEGIN_C_DECLS  extern "C" {    ///< exports as C functions
#   define END_C_DECLS    }               ///< exports as C functions
#else
#   define BEGIN_C_DECLS  /* empty */     ///< exports as C functions
#   define END_C_DECLS    /* empty */     ///< exports as C functions
#endif


/*
 * The macro "PARAMS" is taken verbatim from section 7.1 of the
 * Libtool 1.5.14 manual.
 */
/* PARAMS is a macro used to wrap function prototypes, so that
   compilers that don't understand ANSI C prototypes still work,
   and ANSI C compilers can issue warnings about type mismatches. */
#undef PARAMS
#if defined (__STDC__) || defined (_AIX) \
        || (defined (__mips) && defined (_SYSTYPE_SVR4)) \
        || defined(WIN32) || defined (__cplusplus)
# define PARAMS(protos) protos   ///< wraps function arguments in order to support ANSI C
#else
# define PARAMS(protos) ()       ///< wraps function arguments in order to support ANSI C
#endif


/*
 * The following macro is taken verbatim from section 5.40 of the GCC
 * 4.0.2 manual.
 */
#if __STDC_VERSION__ < 199901L
# if __GNUC__ >= 2
# define __func__ __FUNCTION__
# else
# define __func__ "<unknown>"
# endif
#endif


// Exports function type
#ifdef WIN32
#   ifdef DMS_LIBS_DYNAMIC
#      define DMS_F(type)   extern __declspec(dllexport) type   ///< used for library calls
#      define DMS_V(type)   extern __declspec(dllexport) type   ///< used for library calls
#   else
#      define DMS_F(type)   extern __declspec(dllimport) type   ///< used for library calls
#      define DMS_V(type)   extern __declspec(dllimport) type   ///< used for library calls
#   endif
#else
#   ifdef DMS_LIBS_DYNAMIC
#      define DMS_F(type)   type                                ///< used for library calls
#      define DMS_V(type)   type                                ///< used for library calls
#   else
#      define DMS_F(type)   extern type                         ///< used for library calls
#      define DMS_V(type)   extern type                         ///< used for library calls
#   endif
#endif


///////////////
//           //
//  Headers  //
//           //
///////////////

#include <inttypes.h>
#include <dmsversion.h>


///////////////////
//               //
//  Definitions  //
//               //
///////////////////

#define DMS_TRUE   0x01
#define DMS_FALSE  0x00
#define DMS_YES    0x01
#define DMS_NO     0x00


/////////////////
//             //
//  Datatypes  //
//             //
/////////////////


//////////////////
//              //
//  Prototypes  //
//              //
//////////////////
BEGIN_C_DECLS

// returns the binary GNU Libtool LIB_VERSION_INFO value
DMS_F(int) dms_lib_version PARAMS((void));

// returns the GNU Libtool LIB_VERSION_AGE value
DMS_F(int) dms_lib_version_age PARAMS((void));

// checks for library binary compatability
DMS_F(int) dms_lib_version_check PARAMS((int version));

// returns the GNU Libtool LIB_VERSION_CURRENT value
DMS_F(int) dms_lib_version_current PARAMS((void));

// returns the GNU Libtool LIB_VERSION_INFO string
DMS_F(const char *) dms_lib_version_info PARAMS((void));

// returns the GNU Libtool LIB_VERSION_REVISION value
DMS_F(int) dms_lib_version_revision PARAMS((void));

// returns version info string for library
DMS_F(const char *) dms_version PARAMS((void));


END_C_DECLS
#endif /* end of header file */
