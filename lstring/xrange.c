/*
 * $Id: xrange.c,v 1.2 2001/06/25 18:49:48 bnv Exp $
 * $Log: xrange.c,v $
 * Revision 1.2  2001/06/25 18:49:48  bnv
 * Header changed to Id
 *
 * Revision 1.1  1998/07/02 17:20:58  bnv
 * Initial Version
 *
 */

#include <lstring.h>

/* ------------------ Lxrange -------------------- */
void
Lxrange( const PLstr to, byte start, byte stop )
{
	word	c;
	char	*r;

	start = start & 0xFF;
	stop  = stop  & 0xFF;

	if (start <= stop) {
		Lfx(to, stop-start+1 );
		r = LSTR(*to);
		for (c=start; c<=stop; c++)
			*r++ = (char)c;
		LLEN(*to) = stop-start+1;
	} else {
		Lfx( to, 257 - (start-stop));
		r = LSTR(*to);
		for (c=start; c<=0xFF; c++)
			*r++ = (char)c;
		for (c=0x00; c<=stop; c++)
			*r++ = (char)c;
		LLEN(*to) = 257 - (start-stop);
	}
	LTYPE(*to) = LSTRING_TY;
} /* R_xrange */
