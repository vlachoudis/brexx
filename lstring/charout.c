/*
 * $Id: charout.c,v 1.3 2001/06/25 18:49:48 bnv Exp $
 * $Log: charout.c,v $
 * Revision 1.3  2001/06/25 18:49:48  bnv
 * Header changed to Id
 *
 * Revision 1.2  1999/11/26 09:53:28  bnv
 * Changed: To use the new macros.
 *
 * Revision 1.1  1998/07/02 17:17:00  bnv
 * Initial revision
 *
 */

#include <lstring.h>

/* ---------------- Lcharout ------------------- */
void
Lcharout( FILEP f, const PLstr line, const long start )
{
	if (start>=0)
		FSEEK(f,start,SEEK_SET);

	Lwrite(f,line,FALSE);
} /* Lcharout */
