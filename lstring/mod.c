/*
 * $Id: mod.c,v 1.2 2001/06/25 18:49:48 bnv Exp $
 * $Log: mod.c,v $
 * Revision 1.2  2001/06/25 18:49:48  bnv
 * Header changed to Id
 *
 * Revision 1.1  1998/07/02 17:18:00  bnv
 * Initial Version
 *
 */

#include <lerror.h>
#include <lstring.h>

/* ------------------ Lmod ----------------- */
void
Lmod( const PLstr to, const PLstr A, const PLstr B )
{
	L2REAL(A);
	L2REAL(B);

	if (LREAL(*B) == 0) Lerror(ERR_ARITH_OVERFLOW,0);

	LREAL(*to) = (double) (LREAL(*A) - (long)(LREAL(*A) / LREAL(*B)) * LREAL(*B));
	LTYPE(*to) = LREAL_TY;
	LLEN(*to)  = sizeof(double);
} /* Lmod */
