/*
 * $Header: /home/bnv/tmp/brexx/lstring/RCS/x2b.c,v 1.2 1999/11/26 12:52:25 bnv Exp $
 * $Log: x2b.c,v $
 * Revision 1.2  1999/11/26 12:52:25  bnv
 * Changed: To use the new macros
 *
 * Revision 1.1  1998/07/02 17:20:58  bnv
 * Initial Version
 *
 */

#include <ctype.h>
#include <lerror.h>
#include <lstring.h>

/* ------------------- Lx2b ------------------- */
void
Lx2b( const PLstr to, const PLstr from )
{
	long	i;
	int	k;
	char	*c,d;

	L2STR(from);
	Lfx(to,4*LLEN(*from));
	c = LSTR(*to);

	for (i=0; i<LLEN(*from); i++) {
		if (ISSPACE(LSTR(*from)[i])) continue;
		if (!ISXDIGIT(LSTR(*from)[i]))
			Lerror(ERR_INVALID_HEX_CONST,0);

		d = HEXVAL(LSTR(*from)[i]);
		for (k=8; k; k>>=1)
			*c++ = (d&k)?'1':'0';
	}
	*c = 0;
	LLEN(*to) = STRLEN(LSTR(*to));
	LTYPE(*to) = LSTRING_TY;
} /* Lx2b */
