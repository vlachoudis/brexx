/*
 * $Header: /home/bnv/tmp/brexx/lstring/RCS/write.c,v 1.2 1999/02/10 15:45:16 bnv Exp $
 * $Log: write.c,v $
 * Revision 1.2  1999/02/10 15:45:16  bnv
 * RXANSI support by Generoso Martello
 *
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
#ifdef RXANSI
		if (f==stdout) putch(*c++);
		else
#endif
		fputc(*c++,f);
	if (newline)
		fputc('\n',f);
} /* Lwrite */
