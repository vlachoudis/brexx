/*
 * $Header: /home/bnv/tmp/brexx/lstring/RCS/date.c,v 1.1 1998/07/02 17:17:00 bnv Exp $
 * $Log: date.c,v $
 * Revision 1.1  1998/07/02 17:17:00  bnv
 * Initial revision
 *
 */

#include <time.h>
#include <stdio.h>
#ifdef MSDOS
#	include <dos.h>
#endif
#include <lerror.h>
#include <lstring.h>

static char *WeekDays[] = {
	"Sunday", "Monday", "Tuesday", "Wednesday",
	"Thursday", "Friday", "Saturday" } ;

static char *months[] = {
	"January", "February", "March", "April", "May", "June",
	"July", "August", "September", "October", "November", "December" } ;

/* ----------------- Ldate ------------------ */
void
Ldate( const PLstr datestr, char option )
{
	static char *fmt = "%02d/%02d/%02d" ;
	static char *iso = "%4d%02d%02d" ;
	long   length;
	char   *chptr;

	time_t now ;
	struct tm *tmdata ;

	option = l2u[(byte)option];
	Lfx(datestr,30); LZEROSTR(*datestr);

	now = time(NULL);
	tmdata = localtime(&now) ;

	switch (option) {
		case 'C':
			length = tmdata->tm_yday + 1 +
			(long)(((float)tmdata->tm_year-1)*365.25) + 365 ;
			sprintf(LSTR(*datestr),"%ld",length) ;
			break;

		case 'D':
			sprintf(LSTR(*datestr), "%d", tmdata->tm_yday+1) ;
			break;

		case 'E':
			sprintf(LSTR(*datestr), fmt, tmdata->tm_mday,
				tmdata->tm_mon+1, tmdata->tm_year) ;
			break;

		case 'J':
			sprintf(LSTR(*datestr),"%02d%03d",
				tmdata->tm_year, tmdata->tm_yday+1);
			break;

		case 'M':
			STRCPY(LSTR(*datestr),months[tmdata->tm_mon]);
			break;

		case 'N':
			chptr = months[tmdata->tm_mon] ;
			sprintf(LSTR(*datestr),"%d %c%c%c %4d",
				tmdata->tm_mday, chptr[0], chptr[1],
				chptr[2], tmdata->tm_year+1900) ;
			break;

		case 'O':
			sprintf(LSTR(*datestr), fmt, tmdata->tm_year,
				tmdata->tm_mon+1, tmdata->tm_mday);
			break;

		case 'S':
			sprintf(LSTR(*datestr), iso, tmdata->tm_year+1900,
				tmdata->tm_mon+1, tmdata->tm_mday) ;
			break;

		case 'U':
			sprintf(LSTR(*datestr), fmt, tmdata->tm_mon+1,
				tmdata->tm_mday, tmdata->tm_year ) ;
			break;

		case 'W':
			STRCPY(LSTR(*datestr),WeekDays[tmdata->tm_wday]);
			break;

		default:
			Lerror(ERR_INCORRECT_CALL,0);
	}
	LLEN(*datestr) = STRLEN(LSTR(*datestr));
} /* Ldate */
