/*
 * $Header: /home/bnv/tmp/brexx/lstring/RCS/reradix.c,v 1.2 1999/11/26 12:52:25 bnv Exp $
 * $Log: reradix.c,v $
 * Revision 1.2  1999/11/26 12:52:25  bnv
 * Nothing important.
 *
 * Revision 1.1  1998/07/02 17:18:00  bnv
 * Initial Version
 *
 */

#include <lstring.h>

/* --------------- Lreradix ----------------- */
void
Lreradix( const PLstr to, const PLstr subject,
	const int fromradix, const int toradix )
{
	/* and suppose that subject is string */
	/* radix must be from 2 to 16 */
	long	integer=0;
	int	j;

	for (j=0; j<LLEN(*subject); j++)
		integer = integer*fromradix + HEXVAL(LSTR(*subject)[j]);

	/* ... incomplete ... */
} /* Lreradix */
