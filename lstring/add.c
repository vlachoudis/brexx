/*
 * $Header: /home/bnv/tmp/brexx/lstring/RCS/add.c,v 1.1 1998/07/02 17:16:35 bnv Exp $
 * $Log: add.c,v $
 * Revision 1.1  1998/07/02 17:16:35  bnv
 * Initial revision
 *
 */

#include <ctype.h>
#include <lerror.h>
#include <lstring.h>

/* ---------------- Ladd ------------------- */
void
Ladd( const PLstr to, const PLstr A, const PLstr B)
{
	int	ta, tb;
	double	r;
	char	*eptr;

	if (LTYPE(*A) == LSTRING_TY) {
/****
		ta = _Lisnum(A);
		if (ta==LSTRING_TY) Lerror(ERR_BAD_ARITHMETIC,0);
		r = strtod(LSTR(*A),NULL);
/// With this code with have a problem when a string contains NULL char ie
///      123\00 , then it is treated as normal number
****/
		LASCIIZ(*A);
		r = strtod(LSTR(*A),&eptr);
		if (*eptr) {
			for (; isspace(*eptr); eptr++);
			if (*eptr) Lerror(ERR_BAD_ARITHMETIC,0);
		}
	} else {
		ta = LTYPE(*A);
		r = TOREAL(*A);
	}

	if (LTYPE(*B) == LSTRING_TY) {
/****
		tb = _Lisnum(B);
		if (tb==LSTRING_TY) Lerror(ERR_BAD_ARITHMETIC,0);
		r += strtod(LSTR(*B),NULL);
****/
		LASCIIZ(*B);
		r += strtod(LSTR(*B),&eptr);
		if (*eptr) {
			for (; isspace(*eptr); eptr++);
			if (*eptr) Lerror(ERR_BAD_ARITHMETIC,0);
		}
	} else {
		tb = LTYPE(*B);
		r += TOREAL(*B);
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

/********
	if ( (ta == LINTEGER_TY) && (tb == LINTEGER_TY) ) {
		if (LTYPE(*A) == LINTEGER_TY)
			i = LINT(*A);
		else {
			LASCIIZ(*A);
			i = atol(LSTR(*A));
		}
		if (LTYPE(*B) == LINTEGER_TY)
			i += LINT(*B);
		else {
			LASCIIZ(*B);
			i += atol(LSTR(*B));
		}
		LINT(*to)  = i;
		LTYPE(*to) = LINTEGER_TY;
		LLEN(*to)  = sizeof(long);
	} else {
		if (LTYPE(*A) == LSTRING_TY) {
			LASCIIZ(*A);
			r = strtod( LSTR(*A), NULL );
		} else
			r = TOREAL(*A);

		if (LTYPE(*B) == LSTRING_TY) {
			LASCIIZ(*B);
			r += strtod( LSTR(*B), NULL );
		} else
			r += TOREAL(*B);

		LREAL(*to) = r;
		LTYPE(*to) = LREAL_TY;
		LLEN(*to)  = sizeof(double);
	}
*****
	L2NUM(A);
	L2NUM(B);

	if ( (LTYPE(*A) == LINTEGER_TY) && (LTYPE(*B) == LINTEGER_TY) ) {
		LINT(*to)  = LINT(*A) + LINT(*B);
		LTYPE(*to) = LINTEGER_TY;
		LLEN(*to)  = sizeof(long);
	} else {
		LREAL(*to) = TOREAL(*A) + TOREAL(*B);
		LTYPE(*to) = LREAL_TY;
		LLEN(*to)  = sizeof(double);
	}
*****/
} /* Ladd */
