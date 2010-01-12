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
 *  @file lib/libdms.h internal directives for libdms.la
 */
#ifndef _OTADM_LIB_LIBDMS_H
#define _OTADM_LIB_LIBDMS_H 1

///////////////
//           //
//  Headers  //
//           //
///////////////

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <dms.h>


///////////////////
//               //
//  Definitions  //
//               //
///////////////////

#ifndef DMS_LIBS_DYNAMIC
#define DMS_LIBS_DYNAMIC 1
#endif

#ifndef PACKAGE_TARNAME
#define PACKAGE_TARNAME "dmstools"
#endif

#ifndef SYSCONFDIR
#define SYSCONFDIR "/usr/local/etc"
#endif

#ifndef LIBEXECDIR
#define LIBEXECDIR "/usr/local/libexec"
#endif

#ifndef MODULEDIR
#define MODULEDIR "/usr/local/libexec/otadm"
#endif


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

#endif /* end of header file */
