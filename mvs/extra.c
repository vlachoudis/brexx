/*
 * $Id: extra.c,v 1.1 2008/07/16 08:01:45 bnv Exp $
 * $Log: extra.c,v $
 * Revision 1.1  2008/07/16 08:01:45  bnv
 * Initial revision
 *
 */

#include <stdio.h>
#include <stdlib.h>

/* -------------- putenv ---------------- */
int
putenv(char *envval){
	printf(" Env String = %s\n",envval);
	return 0;
} /* putenv */

/* -------------- gcvt ---------------- */
char *
gcvt(double num, size_t ndigits, char *result)
{
	double b;
	int i,exp,pdigits,format;
	char sign, work[45];

	/* save original data & set sign */
	/* printf(" gcvt entered ndigits=<%d>\n",ndigits);*/

	if (num < 0) {
		b = -num;
		sign = '-';
	} else {
		b = num;
		sign = '+';
	}

	/* Now scale to get exponent */
	exp = 0;
	if(b > 1.0) {
		while(b >= 10.0) {
			++exp;
			b=b / 10.0;
		}
	} else
	if (b == 0.0)
		exp=0;
	else
	if (b < 1.0) {
		while(b <= 1.0)
			{
			--exp;
			b=b*10.0;
			}
		}

	/* printf(" After Scaling b=<%e> exp=<%d>\n",b,exp);*/

	/*
	  now decide how to print and save in FORMAT.
	     -1 => we need leading digits
	      0 => print in exp
	     +1 => we have digits before dp.
	*/
	if ( exp >= 0 ){
		/* Original Number > 1 */
		if( ndigits > exp ) {
			/* printf(" we can print as normal - >1 \n"); */
			format=1;
		} else {
			/* printf(" we need to print in exp\n"); */
			format=0;
		}
	} else {
		if ( ndigits > (-(exp+1) ) ) {
			/* printf( " we can print as normal <1\n"); */
			format=-1;
		} else {
			/* printf( " we need to print in exp <1\n"); */
			format=0;
		}
	}

	/* Now extract the requesite number of digits */
	if (format==-1) {
		/* Number < 1.0 so we need to print the "0."
		 * and the leading zeros... */
		result[0]=sign;
		result[1]='0';
		result[2]='.';
		result[3]=0x00;
		while(++exp){
			--ndigits;
			strcat(result,"0");
		}
		i=b;
		++ndigits;
		sprintf(work,"%d%",i);
	} else
	if (format==+1) {
		/* Number >= 1.0 just print the first digit */
		i = b;
		result[0] = sign;
		result[1] = '\0';
		sprintf(work,"%d",i);
		strcat(result,work);
	} else  {
		/* printing in standard form */
		i = b;
		result[0] = sign;
		result[1] = '\0';
		sprintf(work,"%d",i);
		strcat(result,work);
		strcat(result,".");
	}

	pdigits = ndigits;

	while (--pdigits) {
		b -= i;
		b *= 10.0;
		i = b;
		sprintf(work,"%d",i);
		strcat(result,work);
		if ((ndigits-pdigits)==exp) strcat(result,".");
	}

	if(format==0){ /* exp format - put exp on end */
		sprintf(work,"e%3.2d", exp);
		strcat(result, work);
	}

	/* printf(" Final Answer = <%s> fprintf goves=%g\n", result,num); */
	return result;
} /* gcvt */


