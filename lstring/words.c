/*
 * $Header: /home/bnv/tmp/brexx/lstring/RCS/words.c,v 1.1 1998/07/02 17:20:58 bnv Exp $
 * $Log: words.c,v $
 * Revision 1.1  1998/07/02 17:20:58  bnv
 * Initial Version
 *
 */

#include <lstring.h>

/* ----------------- Lwords ------------------ */
long
Lwords( const PLstr from )
{
	long	i=0,r=0;

	L2STR(from);
	for (;;) {
		LSKIPBLANKS(*from,i);
		if (i>=LLEN(*from)) return r;
		r++;
		LSKIPWORD(*from,i);
	}
} /* Lwords */
