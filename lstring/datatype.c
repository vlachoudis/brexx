/*
 * $Header: /home/bnv/tmp/brexx/lstring/RCS/datatype.c,v 1.1 1998/07/02 17:17:00 bnv Exp $
 * $Log: datatype.c,v $
 * Revision 1.1  1998/07/02 17:17:00  bnv
 * Initial revision
 *
 */

#include <ctype.h>
#include <lstring.h>

/* --------------- Ldatatype ----------------- */
/* returns -1 on error type                    */
int
Ldatatype( const PLstr str, char type )
{
	Lstr	ref;
	int	t,j,digits;
	char	*c;

	type = l2u[(byte)type];

	LINITSTR(ref);

	if (type!='T' && type!='N') {
		L2STR(str);
		if (!LLEN(*str))	/* special type */
			return (type=='X') || (type=='B');
	}

	switch (type) {
		case 'A':
			Lscpy(&ref,clower);
			Lcat(&ref,cUPPER);
			Lcat(&ref,cdigits);
			break;
		case 'B':
			/* check blanks in allowed places */
			if (isspace(LSTR(*str)[0])) return 0;
			c = LSTR(*str) + (LLEN(*str)-1);
			if (isspace(*c)) return 0;
			digits = 0;
			for (j=LLEN(*str); j>0; j--, c--) {
				if (isspace(*c)) {
					/* Blanks need four Binary
						Digits to the right of them */
					if (digits%4!=0) return 0;
				} else {
					if (*c!='0' && *c!='1') return 0;
					digits++;
				}
			}
			return 1;
		case 'L':
			Lscpy(&ref,clower);
			break;
		case 'M':
			Lscpy(&ref,clower);
			Lcat(&ref,cUPPER);
			break;
		case 'N':
			if (LTYPE(*str)==LSTRING_TY)
				return (_Lisnum(str) != LSTRING_TY);
			else
				return TRUE;
		case 'S':
			Lscpy(&ref,clower);
			Lcat(&ref,cUPPER);
			Lcat(&ref,cdigits);
			Lcat(&ref,crxsymb);
			break;
		case 'T':
			return LTYPE(*str);       /* mine special type */
		case 'U':
			Lscpy(&ref,cUPPER);
			break;
		case 'W':
			Lscpy(&ref,cdigits);
			break;
		case 'X':
			/* check blanks in allowed places */
			if (isspace(LSTR(*str)[0])) return 0;
			c = LSTR(*str) + (LLEN(*str)-1);
			if (isspace(*c)) return 0;
			digits = 0;
			for (j=LLEN(*str); j>0; j--, c--) {
				if (isspace(*c)) {
					/* Blanks need a pair of Hex
						Digits to the right of them */
					if (digits%2!=0) return 0;
				} else {
					if (STRCHR(chex,*c)==NULL) return 0;
					digits++;
				}
			}
			return 1;
			break;
		default:
			return -1;
	}
	t = (Lverify(str,&ref,FALSE,1)==LNOTFOUND);
	LFREESTR(ref);
	return t;
} /* Ldatatype */
