/*
 * $Header: /home/bnv/tmp/brexx/lstring/RCS/time.c,v 1.1 1998/07/02 17:18:00 bnv Exp $
 * $Log: time.c,v $
 * Revision 1.1  1998/07/02 17:18:00  bnv
 * Initial Version
 *
 */

#include <time.h>
#include <stdio.h>
#if defined(MSDOS) || defined(__WIN32__)
#	include <dos.h>
#endif
#include <lerror.h>
#include <lstring.h>

static double elapsed=0;

/* ------------------ _Ltimeinit ----------------- */
void
_Ltimeinit( void )
{
#if defined(MSDOS) || defined(__WIN32__)
	struct time	t;
#else
	struct timeval	tv;
	struct timezone	tz;
#endif

#if defined(MSDOS) || defined(__WIN32__)
	gettime(&t);
	elapsed = (double)t.ti_hour*3600 + (double)t.ti_min*60 +
		(double)t.ti_sec + (double)t.ti_hund/100.0;
#else
	gettimeofday(&tv,&tz);
	elapsed = tv.tv_sec + (double)tv.tv_usec/1000000.0;
#endif
} /* _Ltimeinit */

/* -------------------- Ltime ---------------------- */
void
Ltime( const PLstr timestr, char option )
{
	char	*ampm;
	int	hour;
	time_t	now;
	double	unow;
	struct tm *tmdata ;
#if defined(MSDOS) || defined(__WIN32__)
	struct time t;
#else
	struct timeval tv;
	struct timezone tz;
#endif

	option = l2u[(byte)option];
	Lfx(timestr,30); LZEROSTR(*timestr);

	now = time(NULL);
	tmdata = localtime(&now) ;

	switch (option) {
		case 'C':
			hour = tmdata->tm_hour ;
			ampm = (hour>11) ? "pm" : "am" ;
			if ((hour=hour%12)==0)  hour = 12 ;
			sprintf(LSTR(*timestr),"%d:%02d%s",
				hour, tmdata->tm_min, ampm) ;
			break;

		case 'E':
#if defined(MSDOS) || defined(__WIN32__)
			gettime(&t);
			unow = (double)t.ti_hour*3600 + (double)t.ti_min*60 +
				(double)t.ti_sec + (double)t.ti_hund/100.0;
#else
			gettimeofday(&tv,&tz);
			unow = (double)tv.tv_sec + (double)tv.tv_usec/1000000.0;
#endif
			LREAL(*timestr) = unow-elapsed;
			LTYPE(*timestr) = LREAL_TY;
			LLEN(*timestr) = sizeof(double);
			return;

		case 'H':
			sprintf(LSTR(*timestr), "%d", tmdata->tm_hour) ;
			break;

		case 'L':
#if defined(MSDOS) || defined(__WIN32__)
			gettime(&t);
			sprintf(LSTR(*timestr), "%02d:%02d:%02d.%02d",
				t.ti_hour, t.ti_min, t.ti_sec, t.ti_hund);
#else
			gettimeofday(&tv,&tz);
			sprintf(LSTR(*timestr), "%02d:%02d:%02d.%06d",
				tmdata->tm_hour, tmdata->tm_min,
				tmdata->tm_sec, tv.tv_usec) ;
#endif
			break;

		case 'M':
			sprintf(LSTR(*timestr), "%d",
				tmdata->tm_hour*60 + tmdata->tm_min) ;
			break;

		case 'N':
			sprintf(LSTR(*timestr), "%02d:%02d:%02d",
				tmdata->tm_hour, tmdata->tm_min,
				tmdata->tm_sec ) ;
			break;

		case 'R':
#if defined(MSDOS) || defined(__WIN32__)
			gettime(&t);
			unow = (double)t.ti_hour*3600 + (double)t.ti_min*60 +
				(double)t.ti_sec + (double)t.ti_hund/100.0;
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
			sprintf(LSTR(*timestr), "%ld",
				(long)((long)(tmdata->tm_hour*60L)+tmdata->tm_min)
				*60L + (long)tmdata->tm_sec) ;
			break;

		default:
			Lerror(ERR_INCORRECT_CALL,0);
	}
	LLEN(*timestr) = STRLEN(LSTR(*timestr));
} /* Ltime */
