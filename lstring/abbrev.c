/*
 * $Id: abbrev.c,v 1.2 2001/06/25 18:49:48 bnv Exp $
 * $Log: abbrev.c,v $
 * Revision 1.2  2001/06/25 18:49:48  bnv
 * Header changed to Id
 *
 * Revision 1.1  1998/07/02 17:16:15  bnv
 * Initial revision
 *
 */

#include <lstring.h>

/* ------------------- Labbrev -------------------- */
bool
Labbrev(const PLstr information, const PLstr info, long length)
{
	long	i;
	bool	more;

	L2STR(information);
	L2STR(info);

	if (length<=0) length = LLEN(*info);
	if ((LLEN(*information) < LLEN(*info)) ||
		(LLEN(*info) < length))
			return FALSE;
	if (LLEN(*info)==0)
		return TRUE;
	i = 0; more = TRUE;
	while ( (i<length) && more )
		if (LSTR(*information)[i] == LSTR(*info)[i])
			i++;
		else
			more = FALSE;
	return more;
} /* Labbrev */
