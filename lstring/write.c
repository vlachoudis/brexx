/*
 * $Header: /home/bnv/tmp/brexx/lstring/RCS/write.c,v 1.1 1998/07/02 17:20:58 bnv Exp $
 * $Log: write.c,v $
 * Revision 1.1  1998/07/02 17:20:58  bnv
 * Initial Version
 *
 */

#include <stdio.h>
#include <lstring.h>

/* ---------------- Lwrite ------------------- */
void
Lwrite( FILE *f, const PLstr line, const bool newline)
{
	long	l;
	char	*c;

	L2STR(line);
	c = LSTR(*line);
	l = LLEN(*line);
	while (l--)
		fputc(*c++,f);
	if (newline)
		fputc('\n',f);
} /* Lwrite */
