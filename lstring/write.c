/*
 * $Header: /home/bnv/tmp/brexx/lstring/RCS/write.c,v 1.5 1999/11/26 12:52:25 bnv Exp $
 * $Log: write.c,v $
 * Revision 1.5  1999/11/26 12:52:25  bnv
 * Changed: To use the new macros
 *
 * Revision 1.4  1999/05/26 16:47:42  bnv
 * Gene corrections in RXCONIO
 *
 * Revision 1.3  1999/03/10 16:55:55  bnv
 * Added MSC support
 *
 * Revision 1.2  1999/02/10 15:45:16  bnv
 * RXCONIO support by Generoso Martello
 *
 * Revision 1.1  1998/07/02 17:20:58  bnv
 * Initial Version
 *
 */

#include <lstring.h>

/* ---------------- Lwrite ------------------- */
void
Lwrite( FILEP f, const PLstr line, const bool newline)
{
	long	l;
	char	*c;

	L2STR(line);
	c = LSTR(*line);
	l = LLEN(*line);
	while (l--)
#ifdef RXCONIO
		if (f==STDOUT) {
			putch(*c++);
		} else
#endif
		FPUTC(*c++,f);
	if (newline)
		FPUTC('\n',f);
} /* Lwrite */
