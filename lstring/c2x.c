/*
 * $Header: /home/bnv/tmp/brexx/lstring/RCS/c2x.c,v 1.1 1998/07/02 17:16:35 bnv Exp $
 * $Log: c2x.c,v $
 * Revision 1.1  1998/07/02 17:16:35  bnv
 * Initial revision
 *
 */

#include <lstring.h>

/* ------------------- Lc2x ------------------- */
void
Lc2x( const PLstr to, const PLstr from )
{
	char	*re, *ar;
	int	i,r;

	L2STR(from);

	Lfx(to,2*LLEN(*from));
	re = LSTR(*to); ar = LSTR(*from);

	for (i=0,r=0; i<LLEN(*from); i++) {
		re[r++] = chex[(ar[i] >> 4) & 0x0F];
		re[r++] = chex[ar[i] & 0x0F];
	}
	LTYPE(*to) = LSTRING_TY;
	LLEN(*to) = r;
} /* Lc2x */
