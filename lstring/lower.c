/*
 * $Header: /home/bnv/tmp/brexx/lstring/RCS/lower.c,v 1.1 1998/07/02 17:18:00 bnv Exp $
 * $Log: lower.c,v $
 * Revision 1.1  1998/07/02 17:18:00  bnv
 * Initial Version
 *
 */

#include <lstring.h>

/* --------------- Llower ----------------- */
void
Llower( const PLstr s )
{
	size_t	i;

	L2STR(s);
	for (i=0; i<LLEN(*s); i++)
		LSTR(*s)[i] = u2l[ (byte) LSTR(*s)[i] ];
} /* Llower */
