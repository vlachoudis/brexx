/*
 * $Header: /home/bnv/tmp/brexx/lstring/RCS/d2x.c,v 1.1 1998/07/02 17:17:00 bnv Exp $
 * $Log: d2x.c,v $
 * Revision 1.1  1998/07/02 17:17:00  bnv
 * Initial revision
 *
 */

#include <lstring.h>

/* ------------------ Ld2x ------------------ */
void
Ld2x( const PLstr to, const PLstr from, long length)
{
	Lstr	tmp,tmp2;
	long	n;

	LINITSTR(tmp);
	LINITSTR(tmp2);

	if (length<0) {
		Ld2c(&tmp,from,-1);
		Lc2x(&tmp2,&tmp);
		length = LLEN(tmp2) - (LSTR(tmp2)[0]=='0');
	} else {
		n = length/2 + (length&1);
		Ld2c(&tmp,from,n);
		Lc2x(&tmp2,&tmp);
	}
	Lright(to,&tmp2,length,' ');

/********
	if (length<0) length = 0;

	n = length/2 + (length&1);
	Ld2c(&tmp,from,n);
	if (length) {
		Lc2x(&tmp2,&tmp);
		Lright(to,&tmp2,length,' ');
	} else
		Lc2x(to,&tmp);
******/

	LFREESTR(tmp);
	LFREESTR(tmp2);
} /* Ld2x */
