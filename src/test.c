#include "../inc/bnv.h"
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>

void main()
{
	int	i,c,r;

	c = 0;
	for (i=0; i<10000000; i++) {
		r = random()%256;
//		if (IN_RANGE('0',r,'9')) c++;
		if (isdigit(r)) c++;
	}
	printf("count = %d\n",c);
}
