/*
 * $Header: /home/bnv/tmp/brexx/lstring/RCS/div.c,v 1.1 1998/07/02 17:18:00 bnv Exp $
 * $Log: div.c,v $
 * Revision 1.1  1998/07/02 17:18:00  bnv
 * Initial Version
 *
 */

#include <lerror.h>
#include <lstring.h>

/* ------------------- Ldiv ----------------- */
void
Ldiv( const PLstr to, const PLstr A, const PLstr B )
{
	double	b;

	b = Lrdreal(B);
	if (b == 0) Lerror(ERR_ARITH_OVERFLOW,0);
	LREAL(*to) = Lrdreal(A) / b;
	LTYPE(*to) = LREAL_TY;
	LLEN(*to)  = sizeof(double);
} /* Ldiv */
