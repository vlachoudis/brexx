/*
 * $Header: /home/bnv/tmp/brexx/lstring/RCS/trunc.c,v 1.1 1998/07/02 17:18:00 bnv Exp $
 * $Log: trunc.c,v $
 * Revision 1.1  1998/07/02 17:18:00  bnv
 * Initial Version
 *
 */

#include <stdio.h>
#include <lstring.h>

/* ---------------- Ltrunc ----------------- */
void
Ltrunc( const PLstr to, const PLstr from, long n)
{
	if (n<0) n = 0;

	if (!n) {
		Lstrcpy(to,from);
		L2REAL(to);
		LINT(*to)  = (long)LREAL(*to);
		LTYPE(*to) = LINTEGER_TY;
		LLEN(*to)  = sizeof(long);
	} else {
		L2REAL(from);
		Lfx(to,n+15);
		sprintf(LSTR(*to),"%.*f", (int)n, LREAL(*from));
		LTYPE(*to) = LSTRING_TY;
		LLEN(*to)  = STRLEN(LSTR(*to));
	}
} /* R_trunc */
