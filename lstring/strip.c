/*
 * $Header: /home/bnv/tmp/brexx/lstring/RCS/strip.c,v 1.1 1998/07/02 17:18:00 bnv Exp $
 * $Log: strip.c,v $
 * Revision 1.1  1998/07/02 17:18:00  bnv
 * Initial Version
 *
 */

#include <string.h>
#include <lstring.h>

/* --------------- Lstrip ------------------ */
void
Lstrip( const PLstr to, const PLstr str, const char action, const char pad)
{
	char	*cf, *cl;
	size_t	l;

	if (LTYPE(*str)!=LSTRING_TY) {
		Lstrcpy(to,str);
		return;
	}

	l = LLEN(*str);
	if (!l) return;

	cf = LSTR(*str);
	if (action==LBOTH || action==LLEADING) {
		while (l && (*cf==pad)) { cf++; l--; }
		if (!l) {
			LZEROSTR(*to);
			return;
	}	}

	l = LLEN(*str);
	cl = LSTR(*str)+ l - 1;
	if (action==LBOTH || action==LTRAILING) {
		while (l && (*cl==pad)) {cl--; l--; }
		if (!l) {
			LZEROSTR(*to);
			return;
	}	}

	l = (int)( (long)(char huge *)cl - (long)(char huge *)cf + 1);

	if (l<LLEN(*str)) {
		Lfx(to,l);
		MEMMOVE(LSTR(*to),cf,l);
		LLEN(*to) = l;
		LTYPE(*to) = LSTRING_TY;
	} else
		Lstrcpy(to,str);
} /* Lstrip */
