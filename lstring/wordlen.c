/*
 * $Header: /home/bnv/tmp/brexx/lstring/RCS/wordlen.c,v 1.1 1998/07/02 17:20:58 bnv Exp $
 * $Log: wordlen.c,v $
 * Revision 1.1  1998/07/02 17:20:58  bnv
 * Initial Version
 *
 */

#include <lstring.h>

/* --------------- Lwordlength ----------------- */
long
Lwordlength( const PLstr from, long n )
{
	long	i;

	i = Lwordindex(from,n);
	if (i==0) return 0;
	n = i--;
	LSKIPWORD(*from,i);
	return i-n+1;
} /* Lwordlength */
