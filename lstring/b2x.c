/*
 * $Header: /home/bnv/tmp/brexx/lstring/RCS/b2x.c,v 1.1 1998/07/02 17:16:35 bnv Exp $
 * $Log: b2x.c,v $
 * Revision 1.1  1998/07/02 17:16:35  bnv
 * Initial revision
 *
 */

#include <ctype.h>
#include <lerror.h>
#include <lstring.h>

/* ------------------- Lb2x ------------------- */
void
Lb2x( const PLstr to, const PLstr from )
{
	long	i;
	int	j,k;
	char	*c;

	Lstrcpy(to,from);
	Lreverse(to);
	c = LSTR(*to);

	for (i=j=k=0; i<LLEN(*to); i++) {
		if (isspace(LSTR(*to)[i])) continue;
		if (LSTR(*to)[i]<'0' || LSTR(*to)[i]>'1')
			Lerror(ERR_INVALID_HEX_CONST,0);

		j |= ((LSTR(*to)[i]=='1')&1) << k;
		if (++k==4) {
			*c++ = chex[j];
			j=k=0;
		}
	}
	if (k) *c++ = chex[j];
	*c = 0;
	LLEN(*to) = STRLEN(LSTR(*to));
	LTYPE(*to) = LSTRING_TY;
	Lreverse(to);
} /* Lb2x */
