/*
 * $Header: /home/bnv/tmp/brexx/lstring/RCS/neg.c,v 1.1 1998/07/02 17:18:00 bnv Exp $
 * $Log: neg.c,v $
 * Revision 1.1  1998/07/02 17:18:00  bnv
 * Initial Version
 *
 */

#include <lstring.h>

/* ------------------- Lneg ------------------ */
void
Lneg( const PLstr to, const PLstr num )
{
	L2NUM(num);

	if (LTYPE(*num)==LINTEGER_TY) {
		LINT(*to)  = -LINT(*num);
		LTYPE(*to) = LINTEGER_TY;
		LLEN(*to)  = sizeof(long);
	} else {
		LREAL(*to) = -LREAL(*num);
		LTYPE(*to) = LREAL_TY;
		LLEN(*to)  = sizeof(double);
	}
} /* Lneg */
