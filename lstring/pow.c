/*
 * $Id: pow.c,v 1.2 2001/06/25 18:49:48 bnv Exp $
 * $Log: pow.c,v $
 * Revision 1.2  2001/06/25 18:49:48  bnv
 * Header changed to Id
 *
 * Revision 1.1  1998/07/02 17:18:00  bnv
 * Initial Version
 *
 */

#include <math.h>
#include <lstring.h>

/* ----------------- Lpow --------------------- */
void
Lpow( const PLstr to, const PLstr num, const PLstr p )
{
	L2REAL(num);
	L2REAL(p);
	Lrcpy(to, pow(LREAL(*num),LREAL(*p)));
} /* Lpow */
