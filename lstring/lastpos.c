/*
 * $Header: /home/bnv/tmp/brexx/lstring/RCS/lastpos.c,v 1.1 1998/07/02 17:18:00 bnv Exp $
 * $Log: lastpos.c,v $
 * Revision 1.1  1998/07/02 17:18:00  bnv
 * Initial Version
 *
 */

#include <lstring.h>

/* ---------------- Llastpos --------------- */
long
Llastpos( const PLstr needle, const PLstr haystack, long p)
{
	Lstr	N;
	Lstr	H;

	L2STR(needle);
	L2STR(haystack);

	if (p<0 || p>=LLEN(*haystack)) p = LLEN(*haystack);

	if (!LLEN(*needle)) {
		if (LLEN(*haystack))
			return (p>0)?p:1;
		else
			return LNOTFOUND;
	}

	LINITSTR(N);	Lstrcpy(&N,needle);
	LINITSTR(H);	Lstrcpy(&H,haystack);

	Lreverse(&N);
	Lreverse(&H);

	if (p>0) p = LLEN(H) - p + 1;

	p = Lindex(&H, &N, p);
	if (p!=LNOTFOUND)  p = LLEN(H)-(p+LLEN(N))+2;

	LFREESTR(N);
	LFREESTR(H);

	return p;
} /* Llastpos */
