/*
 * $Header: /home/bnv/tmp/brexx/lstring/RCS/equal.c,v 1.2 1998/11/10 13:36:14 bnv Exp $
 * $Log: equal.c,v $
 * Revision 1.2  1998/11/10 13:36:14  bnv
 * Comparison for reals is done with fabs(a-b)<smallnumber
 *
 * Revision 1.1  1998/07/02 17:18:00  bnv
 * Initial Version
 *
 */

#include <math.h>
#include <ctype.h>
#include <string.h>
#include <lstring.h>

/* -------------------- Lequal ----------------- */
int
Lequal(const PLstr A, const PLstr B)
{
	int	ta, tb;
	char	*a, *b;		/* start position in string */
	char	*ae, *be;	/* ending position in string */
	double	ra, rb;

	if (LTYPE(*A)==LSTRING_TY)
		ta = _Lisnum(A);
	else
		ta = LTYPE(*A);

	/* check to see if the first argument is string? */
	if (ta == LSTRING_TY) {
		L2STR(B);	/* make string and the second	*/
		goto eq_str;	/* go and check strings		*/
	}

	if (LTYPE(*B)==LSTRING_TY)
		tb = _Lisnum(B);
	else
		tb = LTYPE(*B);

	/* is B also a number */
	if (tb != LSTRING_TY) {
		if (LTYPE(*A) == LSTRING_TY)
			ra = strtod(LSTR(*A),NULL);
		else
			ra = TOREAL(*A);

		if (LTYPE(*B) == LSTRING_TY)
			rb = strtod(LSTR(*B),NULL);
		else
			rb = TOREAL(*B);

		if (fabs(ra-rb)<=1E-14)
			return 0;
		else
		if (ra>rb)
			return 1;
		else
			return -1;
	}

	/* nope it was a string */
	L2STR(A);		/* convert A string */
eq_str:
	a = LSTR(*A);
	ae = a + LLEN(*A);
	for(; (a<ae) && isspace(*a); a++) ;

	b = LSTR(*B);
	be = b + LLEN(*B);
	for(; (b<be) && isspace(*b); b++) ;

	for(;(a<ae) && (b<be) && (*a==*b); a++,b++) ;
               
	for(; (a<ae) && isspace(*a);a++) ;
	for(; (b<be) && isspace(*b);b++) ;

	if (a==ae && b==be)
		return 0;
	else
	if (a<ae && b<be)
		return (*a<*b) ? -1 : 1 ;
	else 
		return (a<ae) ? 1 : -1 ;
} /* Lequal */
