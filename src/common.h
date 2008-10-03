/*
 *  $Id: $
 */
/*
 *  Syzdek Coding Tools
 *  Copyright (c) 2008 David M. Syzdek <david@syzdek.net>.
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
 *  @file src/common.h common includes and definitions
 */
#ifndef _SCT_SRC_COMMON_H
#define _SCT_SRC_COMMON_H 1

//////////////
//          //
//  Macros  //
//          //
//////////////

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
# define PARAMS(protos) protos
#else
# define PARAMS(protos) ()
#endif


///////////////
//           //
//  Headers  //
//           //
///////////////


#ifdef HAVE_CONFIG_H
#include <config.h>
#endif


///////////////////
//               //
//  Definitions  //
//               //
///////////////////

#ifndef PACKAGE_BUGREPORT
#define PACKAGE_BUGREPORT "david@syzdek.net"
#endif

#ifndef PACKAGE_NAME
#define PACKAGE_NAME "Syzdek Coding Tools"
#endif

#ifndef PACKAGE_COPYRIGHT
#define PACKAGE_COPYRIGHT "Copyright (c) 2008 David M. Syzdek <david@syzdek.net>."
#endif

#ifndef PACKAGE_VERSION
#define PACKAGE_VERSION ""
#endif

#ifndef PROGRAM_NAME
#define PROGRAM_NAME ""
#endif

#endif
/* end of header */
