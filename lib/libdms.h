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


#endif /* end of header file */
