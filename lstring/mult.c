/*
 * $Id: mult.c,v 1.4 2008/07/15 07:40:54 bnv Exp $
 * $Log: mult.c,v $
 * Revision 1.4  2008/07/15 07:40:54  bnv
 * #include changed from <> to ""
 *
 * Revision 1.3  2002/06/11 12:37:15  bnv
 * Added: CDECL
 *
 * Revision 1.2  2001/06/25 18:49:48  bnv
 * Header changed to Id
 *
 * Revision 1.1  1998/07/02 17:18:00  bnv
 * Initial Version
 *
 */

#include "lstring.h"

/* ------------------- Lmult ----------------- */
void __CDECL
Lmult( const PLstr to, const PLstr A, const PLstr B)
{
	L2NUM(A);
	L2NUM(B);

	if ((LTYPE(*A)==LINTEGER_TY) && (LTYPE(*B)==LINTEGER_TY)) {
		LINT(*to)  = LINT(*A) * LINT(*B);
		LTYPE(*to) = LINTEGER_TY;
		LLEN(*to)  = sizeof(long);
	} else {
		LREAL(*to) = TOREAL(*A) * TOREAL(*B);
		LTYPE(*to) = LREAL_TY;
		LLEN(*to)  = sizeof(double);
	}
} /* Lmult */
