/*
 * $Id: reverse.c,v 1.2 2001/06/25 18:49:48 bnv Exp $
 * $Log: reverse.c,v $
 * Revision 1.2  2001/06/25 18:49:48  bnv
 * Header changed to Id
 *
 * Revision 1.1  1998/07/02 17:18:00  bnv
 * Initial Version
 *
 */

#include <lstring.h>

/* ------------------ Lreverse ------------------- */
void
Lreverse( const PLstr s )
{
	long	i;
	char	c,*cf,*cl;

	L2STR(s);
	if (LLEN(*s)==0)
		return;
	cf = LSTR(*s);  cl = cf + LLEN(*s) - 1;
	i = LLEN(*s) / 2;
	while (i--) {
		c = *cf;
		*cf++ = *cl;
		*cl-- = c;
	}
} /* Lreverse */
