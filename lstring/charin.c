/*
 * $Id: charin.c,v 1.4 2002/06/11 12:37:15 bnv Exp $
 * $Log: charin.c,v $
 * Revision 1.4  2002/06/11 12:37:15  bnv
 * Added: CDECL
 *
 * Revision 1.3  2001/06/25 18:49:48  bnv
 * Header changed to Id
 *
 * Revision 1.2  1999/11/26 09:53:14  bnv
 * Changed: To use the new macros.
 *
 * Revision 1.1  1998/07/02 17:17:00  bnv
 * Initial revision
 *
 */

#include <lstring.h>

/* ---------------- Lcharin ------------------- */
void __CDECL
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
