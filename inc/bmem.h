/*
 * $Id: bmem.h,v 1.11 2011/06/29 08:32:12 bnv Exp $
 * $Log: bmem.h,v $
 * Revision 1.11  2011/06/29 08:32:12  bnv
 * Added android
 *
 * Revision 1.10  2011/06/20 08:32:28  bnv
 * Added android
 *
 * Revision 1.9  2009/06/02 09:41:43  bnv
 * MVS/CMS corrections
 *
 * Revision 1.8  2008/07/15 14:54:05  bnv
 * *** empty log message ***
 *
 * Revision 1.7  2008/07/15 07:40:07  bnv
 * MVS, CMS support
 *
 * Revision 1.6  2008/07/14 13:09:21  bnv
 * MVS,CMS support
 *
 * Revision 1.5  2004/08/16 15:30:15  bnv
 * Added: Checking for WCE
 *
 * Revision 1.4  2003/02/26 16:30:16  bnv
 * Added: config.h
 *
 * Revision 1.3  2002/06/11 12:37:56  bnv
 * Added: CDECL
 *
 * Revision 1.2  2001/06/25 18:52:04  bnv
 * Header -> Id
 *
 * Revision 1.1  1998/07/02 17:35:50  bnv
 * Initial revision
 *
 */

#ifndef __BMEM_H__
#define __BMEM_H__

#include "os.h"
#include <stdlib.h>
#if !defined(__CMS__) && !defined(__MVS__) && !defined(__MACH__) && !defined(__APPLE__)
#	include <malloc.h>
#endif

#if defined(__DEBUG__) && !defined(WCE)
#	define	MALLOC(s,d)	mem_malloc(s,d)
#	define	REALLOC(p,s)	mem_realloc(p,s)
#	define	FREE		mem_free
#else
#	if defined(__BORLANDC__) && (defined(__HUGE__) || defined(__LARGE__))
#		define	MALLOC(s,d)	farmalloc(s)
#		define	REALLOC(p,s)	farrealloc(p,s)
#		define	FREE		farfree
#	else
#		define	MALLOC(s,d)	malloc(s)
#		define	REALLOC(p,s)	realloc(p,s)
#		define	FREE		free
#	endif
#endif

/* ------ function prototypes --------- */
#ifdef __DEBUG__
void	__CDECL *mem_malloc(size_t size, char *desc);
void	__CDECL *mem_realloc(void *ptr, size_t size);
void	__CDECL mem_free(void *ptr);
void	__CDECL mem_list(void);
int	__CDECL mem_chk(void);
long	__CDECL mem_allocated(void);
#endif

#endif
