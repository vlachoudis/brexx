/*
 * $Id: bmem.h,v 1.2 2001/06/25 18:52:04 bnv Exp $
 * $Log: bmem.h,v $
 * Revision 1.2  2001/06/25 18:52:04  bnv
 * Header -> Id
 *
 * Revision 1.1  1998/07/02 17:35:50  bnv
 * Initial revision
 *
 */

#ifndef __BMEM_H__
#define __BMEM_H__

#include <stdlib.h>
#include <malloc.h>

#ifdef __DEBUG__
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
void	*mem_malloc(size_t size, char *desc);
void	*mem_realloc(void *ptr, size_t size);
void	mem_free(void *ptr);
void	mem_list(void);
int	mem_chk(void);
long	mem_allocated(void);
#endif

#endif
