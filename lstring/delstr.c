/*
 * $Header: /home/bnv/tmp/brexx/lstring/RCS/delstr.c,v 1.1 1998/07/02 17:17:00 bnv Exp $
 * $Log: delstr.c,v $
 * Revision 1.1  1998/07/02 17:17:00  bnv
 * Initial revision
 *
 */

#include <lstring.h>

/* ---------------- Ldelstr ------------------- */
void
Ldelstr( const PLstr to, const PLstr from, long start, long length )
{
	Lstr	tmp;

	L2STR(from);

	start--;
	if (start<0) start = 0;
	if (start>=LLEN(*from)) {
		Lstrcpy(to,from);
		return;
	}

	if (start)
		_Lsubstr(to,from,1,(size_t)start);
	else
		LZEROSTR(*to);

	if (length>0) 
	if (start+length < LLEN(*from)) {
		LINITSTR(tmp);
		_Lsubstr(&tmp, from, (size_t)(start+length+1), 0);
		Lstrcat(to,&tmp);
		LFREESTR(tmp);
	}
} /* Ldelstr */
