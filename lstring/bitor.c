/*
 * $Header: /home/bnv/tmp/brexx/lstring/RCS/bitor.c,v 1.1 1998/07/02 17:16:35 bnv Exp $
 * $Log: bitor.c,v $
 * Revision 1.1  1998/07/02 17:16:35  bnv
 * Initial revision
 *
 */

#include <lstring.h>

/* ---------------- Lbitor ------------------ */
void
Lbitor( const PLstr to, const PLstr s1, const PLstr s2,
	const bool usepad, const char pad )
{
	long	i;

	L2STR(s1);
	L2STR(s2);

	if (LLEN(*s1) < LLEN(*s2)) {
		Lstrcpy(to,s2);
		for (i=0; i<LLEN(*s1); i++)
			LSTR(*to)[i] = LSTR(*s1)[i] | LSTR(*s2)[i];

		if (usepad)
			for (; i<LLEN(*s2); i++)
				LSTR(*to)[i] = LSTR(*s2)[i] | pad;
	} else {
		Lstrcpy(to,s1);

		for (i=0; i<LLEN(*s2); i++)
			LSTR(*to)[i] = LSTR(*s1)[i] | LSTR(*s2)[i];

		if (usepad)
			for (; i<LLEN(*s1); i++)
				LSTR(*to)[i] = LSTR(*s1)[i] | pad;
	}
} /* Lbitor */
