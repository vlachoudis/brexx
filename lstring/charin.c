/*
 * $Header: /home/bnv/tmp/brexx/lstring/RCS/charin.c,v 1.2 1999/11/26 09:53:14 bnv Exp $
 * $Log: charin.c,v $
 * Revision 1.2  1999/11/26 09:53:14  bnv
 * Changed: To use the new macros.
 *
 * Revision 1.1  1998/07/02 17:17:00  bnv
 * Initial revision
 *
 */

#include <lstring.h>

/* ---------------- Lcharin ------------------- */
void
Lcharin( FILEP f, const PLstr line, const long start, const long length )
{
	if (start>=1)
		FSEEK(f,start-1,SEEK_SET);
	if (length<=0) {
		LZEROSTR(*line);
		return;
	} 
	Lread(f,line,length);
} /* Lcharin */
