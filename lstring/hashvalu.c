/*
 * $Header: /home/bnv/tmp/brexx/lstring/RCS/hashvalu.c,v 1.2 1999/11/26 09:56:55 bnv Exp $
 * $Log: hashvalu.c,v $
 * Revision 1.2  1999/11/26 09:56:55  bnv
 * Added: Error message fror CE
 *
 * Revision 1.1  1998/07/02 17:18:00  bnv
 * Initial Version
 *
 */

#include <lstring.h>

/* --------------- Lhashvalue ------------------ */
word
Lhashvalue( const PLstr str )
{
#ifdef WCE
#	error "Lhashvalue is not used!"
#endif
	word	value = 0;
	size_t	i,l;

	if (LISNULL(*str)) return 0;

	switch (LTYPE(*str)) {
		case LINTEGER_TY: l = sizeof(long); break;
		case LREAL_TY:    l = sizeof(double); break;
		case LSTRING_TY:  l = MIN(255,LLEN(*str)); break;
	}
	for (i=0; i<l; i++) {
		value ^= LSTR(*str)[i];
#ifdef __BORLANDC__
		value = _rotl(value,1);
#else
		value = ((value>>1) | (value<<7)) & 0xFF;
#endif
	}
	return value;
} /* Lhashvalue */
