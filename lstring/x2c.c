/*
 * $Header: /home/bnv/tmp/brexx/lstring/RCS/x2c.c,v 1.1 1998/07/02 17:20:58 bnv Exp $
 * $Log: x2c.c,v $
 * Revision 1.1  1998/07/02 17:20:58  bnv
 * Initial Version
 *
 */

#include <ctype.h>
#include <lerror.h>
#include <lstring.h>

/* ------------------ Lx2c ------------------ */
void
Lx2c( const PLstr to, const PLstr from )
{
	int	i,j,r;
	char	*t,*f;

	L2STR(from);
	Lfx(to,LLEN(*from)/2+1);    /* a rough estimation */

	t = LSTR(*to);	f = LSTR(*from);

	for (i=r=0; i<LLEN(*from); )  {
		for (; isspace(f[i]) && (i<LLEN(*from)); i++) ;; /*skip spaces*/
		for (j=i; isxdigit(f[j]) && (j<LLEN(*from)); j++) ;; /* find hexdigits */

		if ((i<LLEN(*from)) && (j==i)) {	/* Ooops wrong character */
			Lerror(ERR_INVALID_HEX_CONST,0);
			LZEROSTR(*to);		/* return null when error occures */
			return;
		}

		if ((j-i)&1)  { t[r++] = HEXVAL(f[i]); i++ ;}
		for (; i<j; i+=2)
		t[r++] = (HEXVAL(f[i])<<4) | HEXVAL(f[i+1]);
	}

	LTYPE(*to) = LSTRING_TY;
	LLEN(*to) = r;
} /* Lx2c */
