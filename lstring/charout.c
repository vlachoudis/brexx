/*
 * $Header: /home/bnv/tmp/brexx/lstring/RCS/charout.c,v 1.1 1998/07/02 17:17:00 bnv Exp $
 * $Log: charout.c,v $
 * Revision 1.1  1998/07/02 17:17:00  bnv
 * Initial revision
 *
 */

#include <stdio.h>
#include <lstring.h>

/* ---------------- Lcharout ------------------- */
void
Lcharout( FILE *f, const PLstr line, const long start )
{
	if (start>=0)
		fseek(f,start,SEEK_SET);

	Lwrite(f,line,FALSE);
} /* Lcharout */
