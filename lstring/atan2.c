/*
 * $Id: atan2.c,v 1.2 2001/06/25 18:49:48 bnv Exp $
 * $Log: atan2.c,v $
 * Revision 1.2  2001/06/25 18:49:48  bnv
 * Header changed to Id
 *
 * Revision 1.1  1998/07/02 17:16:35  bnv
 * Initial revision
 *
 */

#include <math.h>
#include <lstring.h>

/* ----------------- Lpow --------------------- */
void
Latan2( const PLstr to, const PLstr x, const PLstr y )
{
	Lrcpy(to, atan2(Lrdreal(x),Lrdreal(y)));
} /* Latan2 */
