/*
 * $Header: /home/bnv/tmp/brexx/lstring/RCS/intdiv.c,v 1.1 1998/07/02 17:18:00 bnv Exp $
 * $Log: intdiv.c,v $
 * Revision 1.1  1998/07/02 17:18:00  bnv
 * Initial Version
 *
 */

#include <lerror.h>
#include <lstring.h>

/* ---------------- Lintdiv ---------------- */
void
Lintdiv( const PLstr to, const PLstr A, const PLstr B )
{
	double	b;

	b = Lrdreal(B);

	if (b == 0) Lerror(ERR_ARITH_OVERFLOW,0);

	LINT(*to)  = (long) (Lrdreal(A) / b);
	LTYPE(*to) = LINTEGER_TY;
	LLEN(*to)  = sizeof(long);
} /* Lintdiv */
