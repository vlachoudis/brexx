/*
 * $Header: /home/bnv/tmp/brexx/lstring/RCS/time.c,v 1.5 1999/11/26 12:52:25 bnv Exp $
 * $Log: time.c,v $
 * Revision 1.5  1999/11/26 12:52:25  bnv
 * Added: Windows CE support
 *
 * Revision 1.4  1999/03/10 16:55:55  bnv
 * Added MSC support
 *
 * Revision 1.3  1999/03/01 11:06:33  bnv
 * Change in the format to long int for tv.tv_usec just to keep compiler happy
 *
 * Revision 1.2  1998/11/06 08:57:08  bnv
 * Added: in the includes the time.h and sys/time.h
 *
 * Revision 1.1  1998/07/02 17:18:00  bnv
 * Initial Version
 *
 */

#if defined(MSDOS) && !defined(__WIN32__) && !defined(_MSC_VER) && !defined(WCE)
#	include <dos.h>
#	include <time.h>
#elif defined(WCE)
#elif defined(_MSC_VER)
#	include <time.h>
#	include <sys/types.h>
#	include <sys/timeb.h>
#else
/* Load both of them time.h and sys/time.h, you never now where the 
 * struct timeval is
 */
#	include <time.h>
#	include <sys/time.h>
#	include <unistd.h>
#endif
#include <lerror.h>
#include <lstring.h>

static double elapsed=0.0;

/* ------------------ _Ltimeinit ----------------- */
void
_Ltimeinit( void )
{
#if defined(MSDOS) && !defined(__WIN32__) && !defined(_MSC_VER)
	struct time	t;
#elif defined(WCE)
	/* nothing to declare */
#elif defined(_MSC_VER)
	struct _timeb tb;
#else
	struct timeval	tv;
	struct timezone	tz;
#endif

#ifdef WCE
	elapsed = (double)GetTickCount() / 1000.0;
#elif defined(MSDOS) && !defined(__WIN32__) && !defined(_MSC_VER)
	gettime(&t);
	elapsed = (double)t.ti_hour*3600 + (double)t.ti_min*60 +
		(double)t.ti_sec + (double)t.ti_hund/100.0;
#elif defined(_MSC_VER)
	_ftime(&tb);
	elapsed = (double)tb.time + (double)tb.millitm/1000.0;
#else
	gettimeofday(&tv,&tz);
	elapsed = tv.tv_sec + (double)tv.tv_usec/1000000.0;
#endif
} /* _Ltimeinit */

/* -------------------- Ltime ---------------------- */
void
Ltime( const PLstr timestr, char option )
{
	double	unow;
	int	hour;
#ifdef WCE
	SYSTEMTIME	time;
	TCHAR	buf[30];
	TCHAR	*ampm;
#else
	time_t	now;
	char	*ampm;
	struct tm *tmdata ;
#	if defined(MSDOS) && !defined(__WIN32__) && !defined(_MSC_VER)
		struct time t;
#	elif defined(_MSC_VER)
		struct _timeb tb;
#	else
		struct timeval tv;
		struct timezone tz;
#	endif
#endif

	option = l2u[(byte)option];
	Lfx(timestr,30); LZEROSTR(*timestr);

#ifndef WCE
	now = time(NULL);
	tmdata = localtime(&now) ;
#else
	GetLocalTime(&time);
#endif

	switch (option) {
		case 'C':
#ifndef WCE
			hour = tmdata->tm_hour ;
			ampm = (hour>11) ? "pm" : "am" ;
			if ((hour=hour%12)==0)  hour = 12 ;
			sprintf(LSTR(*timestr),"%d:%02d%s",
				hour, tmdata->tm_min, ampm) ;
#else
			hour = time.wHour ;
			ampm = (hour>11) ? TEXT("pm") : TEXT("am");
			if ((hour=hour%12)==0)  hour = 12 ;
			swprintf(buf,TEXT("%d:%02d%s"),
				hour, time.wMinute, ampm) ;
#endif
			break;

		case 'E':
#ifdef WCE
			unow = (double)GetTickCount() / 1000.0;
#elif defined(MSDOS) && !defined(__WIN32__) && !defined(_MSC_VER)
			gettime(&t);
			unow = (double)t.ti_hour*3600 + (double)t.ti_min*60 +
				(double)t.ti_sec + (double)t.ti_hund/100.0;
#elif defined(_MSC_VER)
			_ftime(&tb);
			unow = (double)tb.time + (double)tb.millitm/1000.0;
#else
			gettimeofday(&tv,&tz);
			unow = (double)tv.tv_sec + (double)tv.tv_usec/1000000.0;
#endif
			LREAL(*timestr) = unow-elapsed;
			LTYPE(*timestr) = LREAL_TY;
			LLEN(*timestr) = sizeof(double);
			return;

		case 'H':
#ifndef WCE
			sprintf(LSTR(*timestr), "%d", tmdata->tm_hour) ;
#else
			swprintf(buf, TEXT("%d"), time.wHour) ;
#endif
			break;

		case 'L':
#ifdef WCE
			swprintf(buf, TEXT("%02d:%02d:%02d.%03d"), time.wHour,
					time.wMinute, time.wSecond, time.wMilliseconds) ;
#elif defined(MSDOS) && !defined(__WIN32__) && !defined(_MSC_VER)
			gettime(&t);
			sprintf(LSTR(*timestr), "%02d:%02d:%02d.%02d",
				t.ti_hour, t.ti_min, t.ti_sec, t.ti_hund);
#elif defined(_MSC_VER)
			_ftime(&tb);
			sprintf(LSTR(*timestr), "%02d:%02d:%02d.%03ld",
				tmdata->tm_hour, tmdata->tm_min,
				tmdata->tm_sec, tb.millitm) ;
#else
			gettimeofday(&tv,&tz);
			sprintf(LSTR(*timestr), "%02d:%02d:%02d.%06ld",
				tmdata->tm_hour, tmdata->tm_min,
				tmdata->tm_sec, tv.tv_usec) ;
#endif
			break;

		case 'M':
#ifndef WCE
			sprintf(LSTR(*timestr), "%d",
				tmdata->tm_hour*60 + tmdata->tm_min) ;
#else
			swprintf(buf, TEXT("%d"),
				time.wHour*60 + time.wMinute);
#endif
			break;

		case 'N':
#ifndef WCE
			sprintf(LSTR(*timestr), "%02d:%02d:%02d",
				tmdata->tm_hour, tmdata->tm_min,
				tmdata->tm_sec ) ;
#else
			swprintf(buf, TEXT("%02d:%02d:%02d"),
				time.wHour, time.wMinute, time.wSecond);
#endif
			break;

		case 'R':
#ifdef WCE
			unow = (double)GetTickCount() / 1000.0;
#elif defined(MSDOS) && !defined(__WIN32__) && !defined(_MSC_VER)
			gettime(&t);
			unow = (double)t.ti_hour*3600 + (double)t.ti_min*60 +
				(double)t.ti_sec + (double)t.ti_hund/100.0;
#elif defined(_MSC_VER)
			_ftime(&tb);
			unow = (double)tb.time + (double)tb.millitm/1000.0;
#else
			gettimeofday(&tv,&tz);
			unow = (double)tv.tv_sec + (double)tv.tv_usec/1000000.0;
#endif
			LREAL(*timestr) = unow-elapsed;
			elapsed = unow;
			LTYPE(*timestr) = LREAL_TY;
			LLEN(*timestr) = sizeof(double);
			return;

		case 'S':
#ifndef WCE
			sprintf(LSTR(*timestr), "%ld",
				(long)((long)(tmdata->tm_hour*60L)+tmdata->tm_min)
				*60L + (long)tmdata->tm_sec) ;
#else
			swprintf(buf, TEXT("%ld"),
				(long)((long)(time.wHour*60L)+time.wMinute)
				*60L + (long)time.wSecond) ;
#endif
			break;

		default:
			Lerror(ERR_INCORRECT_CALL,0);
	}
#ifndef WCE
	LLEN(*timestr) = STRLEN(LSTR(*timestr));
#else
	wcstombs(LSTR(*timestr), buf, LLEN(*timestr)=wcslen(buf));
#endif
} /* Ltime */
