/*
 * $Header: /home/bnv/tmp/brexx/lstring/RCS/insert.c,v 1.1 1998/07/02 17:18:00 bnv Exp $
 * $Log: insert.c,v $
 * Revision 1.1  1998/07/02 17:18:00  bnv
 * Initial Version
 *
 */

#include <lstring.h>

/* ------------------ Linsert ------------------- */
void
Linsert( const PLstr to, const PLstr newstr, const PLstr target, 
                   long n, long length, const char pad)
{
	Lstr	tmp;

	LINITSTR(tmp);

	L2STR(newstr);
	L2STR(target);

	if (n<0) n = 0;

	if (length==0) {
		Lstrcpy(to,target);
		return;
	}

	if (length<0) length = LLEN(*newstr);

	if (n>=1)
		Lsubstr(to,target,1,n,pad);
	else
		LZEROSTR(*to);

	Lsubstr(&tmp,newstr,1,length,pad);
	Lstrcat(to,&tmp);
	if (n <= LLEN(*target)) {
		_Lsubstr(&tmp, target, (size_t)n+1, 0);
		Lstrcat(to,&tmp);
	}

	LFREESTR(tmp);
} /* Linsert */
