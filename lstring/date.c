/*
 * $Id: date.c,v 1.7 2008/07/15 07:40:54 bnv Exp $
 * $Log: date.c,v $
 * Revision 1.7  2008/07/15 07:40:54  bnv
 * #include changed from <> to ""
 *
 * Revision 1.6  2004/08/16 15:25:20  bnv
 * float to double
 *
 * Revision 1.5  2002/06/11 12:37:15  bnv
 * Added: CDECL
 *
 * Revision 1.4  2001/06/25 18:49:48  bnv
 * Header changed to Id
 *
 * Revision 1.3  2000/10/09 07:17:19  bnv
 * Corrected the year format
 *
 * Revision 1.2  1999/11/26 09:53:28  bnv
 * Added Windows CE support
 *
 * Revision 1.1  1998/07/02 17:17:00  bnv
 * Initial revision
 *
 */

#ifndef WCE
#	include <time.h>
#endif
#ifdef MSDOS
#	include <dos.h>
#endif
#include "lerror.h"
#include "lstring.h"

#ifdef WCE
static	BYTE	month_days[12] = {
			31, 28, 31, 30, 31, 30,
			31, 31, 30, 31, 30, 31
		};

/* ---- day of year ---- */
static int
day_of_year(LPSYSTEMTIME time)
{
	int	m, d = 0;
	/* fix the february */
	if ((time->wYear%4==0) && (time->wYear%400 != 0))
		month_days[1] = 29;
	else
		month_days[1] = 28;
	for (m=1; m<time->wMonth; m++)
		d += month_days[m-1];
	return d+time->wDay;
} /* day_of_year */
#endif

static TCHAR *WeekDays[] = {
	TEXT("Sunday"), TEXT("Monday"), TEXT("Tuesday"), TEXT("Wednesday"),
	TEXT("Thursday"), TEXT("Friday"), TEXT("Saturday") };

static TCHAR *months[] = {
	TEXT("January"), TEXT("February"), TEXT("March"),
	TEXT("April"), TEXT("May"), TEXT("June"),
	TEXT("July"), TEXT("August"), TEXT("September"),
	TEXT("October"), TEXT("November"), TEXT("December") };

/* ----------------- Ldate ------------------ */
void __CDECL
Ldate( const PLstr datestr, char option )
{
	static	TCHAR *fmt = TEXT("%02d/%02d/%02d");
	static	TCHAR *iso = TEXT("%4d%02d%02d");
	long	length;
	TCHAR	*chptr;

#ifndef WCE
	time_t now ;
	struct tm *tmdata ;
#else
	TCHAR	buf[30];
	SYSTEMTIME	time;
#endif

	option = l2u[(byte)option];
	Lfx(datestr,30); LZEROSTR(*datestr);

#ifndef WCE
	now = time(NULL);
	tmdata = localtime(&now) ;
#else
	GetLocalTime(&time);
#endif

	switch (option) {
		case 'C':
#ifndef WCE
			length = tmdata->tm_yday + 1 +
			(long)(((double)tmdata->tm_year-1)*365.25) + 365 ;
			sprintf(LSTR(*datestr),"%ld",length) ;
#else
			length = day_of_year(&time) +
			(long)((time.wYear%100-1)*365.25) + 365;
			swprintf(buf,TEXT("%ld"),length) ;
#endif
			break;

		case 'D':
#ifndef WCE
			sprintf(LSTR(*datestr), "%d", tmdata->tm_yday+1) ;
#else
			swprintf(buf, TEXT("%d"), day_of_year(&time)) ;
#endif
			break;

		case 'E':
#ifndef WCE
			sprintf(LSTR(*datestr), fmt, tmdata->tm_mday,
				tmdata->tm_mon+1, tmdata->tm_year%100) ;
#else
			swprintf(buf, fmt, time.wDay,
				time.wMonth, time.wYear%100) ;
#endif
			break;

		case 'J':
#ifndef WCE
			sprintf(LSTR(*datestr),"%02d%03d",
				tmdata->tm_year%100, tmdata->tm_yday+1);
#else
			swprintf(buf,TEXT("%02d%03d"),
				time.wYear%100, day_of_year(&time));
#endif
			break;

		case 'M':
#ifndef WCE
			STRCPY(LSTR(*datestr),months[tmdata->tm_mon]);
#else
			wcscpy(buf,months[time.wMonth-1]);
#endif
			break;

		case 'N':
#ifndef WCE
			chptr = months[tmdata->tm_mon] ;
			sprintf(LSTR(*datestr),"%d %c%c%c %4d",
				tmdata->tm_mday, chptr[0], chptr[1],
				chptr[2], tmdata->tm_year+1900) ;
#else
			chptr = months[time.wMonth-1] ;
			swprintf(buf,TEXT("%d %c%c%c %4d"),
				time.wDay, chptr[0], chptr[1],
				chptr[2], time.wYear) ;
#endif
			break;

		case 'O':
#ifndef WCE
			sprintf(LSTR(*datestr), fmt, tmdata->tm_year%100,
				tmdata->tm_mon+1, tmdata->tm_mday);
#else
			swprintf(buf, fmt, time.wYear%100,
				time.wMonth, time.wDay);
#endif
			break;

		case 'S':
#ifndef WCE
			sprintf(LSTR(*datestr), iso, tmdata->tm_year+1900,
				tmdata->tm_mon+1, tmdata->tm_mday) ;
#else
			swprintf(buf, iso, time.wYear,
				time.wMonth, time.wDay) ;
#endif
			break;

		case 'U':
#ifndef WCE
			sprintf(LSTR(*datestr), fmt, tmdata->tm_mon+1,
				tmdata->tm_mday, tmdata->tm_year%100 ) ;
#else
			swprintf(buf, fmt, time.wMonth,
				time.wDay, time.wYear%100 ) ;
#endif
			break;

		case 'W':
#ifndef WCE
			STRCPY(LSTR(*datestr),WeekDays[tmdata->tm_wday]);
#else
			wcscpy(buf, WeekDays[time.wDayOfWeek]);
#endif
			break;

		default:
			Lerror(ERR_INCORRECT_CALL,0);
	}
#ifndef WCE
	LLEN(*datestr) = STRLEN(LSTR(*datestr));
#else
	wcstombs(LSTR(*datestr),buf,LLEN(*datestr)=wcslen(buf));
#endif
} /* Ldate */
