/*
 * $Header: /home/bnv/tmp/brexx/lstring/RCS/charin.c,v 1.1 1998/07/02 17:17:00 bnv Exp $
 * $Log: charin.c,v $
 * Revision 1.1  1998/07/02 17:17:00  bnv
 * Initial revision
 *
 */

#include <stdio.h>
#include <lstring.h>

/* ---------------- Lcharin ------------------- */
void
Lcharin( FILE *f, const PLstr line, const long start, const long length )
{
	if (start>=1)
		fseek(f,start-1,SEEK_SET);
	if (length<=0) {
		LZEROSTR(*line);
		return;
	} 
	Lread(f,line,length);
} /* Lcharin */
