/*
 * $Header: /home/bnv/tmp/brexx/lstring/RCS/sub.c,v 1.1 1998/07/02 17:18:00 bnv Exp $
 * $Log: sub.c,v $
 * Revision 1.1  1998/07/02 17:18:00  bnv
 * Initial Version
 *
 */

#include <lerror.h>
#include <lstring.h>

/* ---------------- Lsub ------------------- */
void
Lsub( const PLstr to, const PLstr A, const PLstr B)
{
	int	ta, tb;
	double	r;

	if (LTYPE(*A) == LSTRING_TY) {
		ta = _Lisnum(A);
		if (ta==LSTRING_TY) Lerror(ERR_BAD_ARITHMETIC,0);
		r = strtod(LSTR(*A),NULL);
	} else {
		ta = LTYPE(*A);
		r = TOREAL(*A);
	}

	if (LTYPE(*B) == LSTRING_TY) {
		tb = _Lisnum(B);
		if (tb==LSTRING_TY) Lerror(ERR_BAD_ARITHMETIC,0);
		r -= strtod(LSTR(*B),NULL);
	} else {
		tb = LTYPE(*B);
		r -= TOREAL(*B);
	}

	if ( (ta == LINTEGER_TY) && (tb == LINTEGER_TY) ) {
		LINT(*to)  = (long)r;
		LTYPE(*to) = LINTEGER_TY;
		LLEN(*to)  = sizeof(long);
	} else {
		LREAL(*to) = r;
		LTYPE(*to) = LREAL_TY;
		LLEN(*to)  = sizeof(double);
	}
} /* Lsub */
