/*
 * $Header: /home/bnv/tmp/brexx/lstring/RCS/countstr.c,v 1.1 1998/07/02 17:17:00 bnv Exp $
 * $Log: countstr.c,v $
 * Revision 1.1  1998/07/02 17:17:00  bnv
 * Initial revision
 *
 */

#include <lstring.h>

/* ----------------- Lcountstr ------------------- *
 * Counts the appearances of the first string 'target'
 * in the second argument 'source'
 */
long
Lcountstr( const PLstr target, const PLstr source )
{
	long	output=0, position;

	position = Lpos(target,source,0);
	while (position>0) {
		output++;
		position = Lpos(target,source,position+LLEN(*target));
	}
	return output;
} /* Lcountstr */
