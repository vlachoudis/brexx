/*
 * $Header: /home/bnv/tmp/brexx/lstring/RCS/translat.c,v 1.1 1998/07/02 17:18:00 bnv Exp $
 * $Log: translat.c,v $
 * Revision 1.1  1998/07/02 17:18:00  bnv
 * Initial Version
 *
 */

#include <lstring.h>

/* ---------------- Ltranslate ------------------- */
void
Ltranslate( const PLstr to, const PLstr from,
	const PLstr tableout, const PLstr tablein, const char pad )
{
	char     table[256];
	int      i;

	Lstrcpy(to,from);	L2STR(to);

	for (i=0; i<256; i++)
		table[i] = i;

	if (tableout)	L2STR(tableout);
	if (tablein)	L2STR(tablein);

	if (tablein) {
		for (i=LLEN(*tablein)-1; i>=0; i--)
			if (tableout) {
				if (i>=LLEN(*tableout))
					table[(byte)LSTR(*tablein)[i]]=pad;
				else
					table[(byte)LSTR(*tablein)[i]]=LSTR(*tableout)[i];
			} else
				table[(byte)LSTR(*tablein)[i]] = pad;
	} else {
		for (i=0; i<256; i++)
			if (tableout)
				if (i >= LLEN(*tableout))
					table[i] = pad;
				else
					table[i] = LSTR(*tableout)[i];
	}

	for (i=0; i<LLEN(*to); i++)
		LSTR(*to)[i] = table[ (byte) LSTR(*to)[i] ];
} /* Ltranslate */
