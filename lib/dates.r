/* dates.r */
parse source . calltype .
if calltype ^== "COMMAND" then return

arg day month year
say DayName(DayOfWeek(day,month,year))
say 'Easter = 'Easter(year)'/04/'year
say 'Days = 'DaysOfMonth(month,year)
say 'Dates Library'
exit

/* --- DayName --- */
/* base 0=Sun */
DayName: procedure
	parse arg dow
return word('Sunday Monday Tuesday Wednesday Thursday Friday Saturday',dow+1)

/* --- MonthName --- */
MonthName: procedure
	parse arg month
return word('January February March April May June July August',
		'September October November December',month)

/* ------------------ DayOfWeek -------------------- */
/* return day of week 0=Sun, 1=Mon, ... 6=Sat  */
DayOfWeek: procedure
	if arg() = 3	then parse arg day,month,year
			else parse arg day '/' month '/' year
	if month < 3 then do
		month = month + 10
		year  = year - 1
	end;
	else month = month - 2
	century = year % 100
	Yr = year // 100
	Dw = (((26*Month - 2) % 10) + Day + Yr + (Yr % 4) +,
		(Century % 4) - (2 * Century)) // 7;
	if Dw < 0	then DayOfWeek = Dw + 7
			else DayOfWeek = Dw
return DayOfWeek

/* ------------------ Easter In April ----------------------- */
/* if Easter > 30 then Month = May, Easter = Easter - 30      */
/* this algoritm works only for Greek Orthodox Easter         */
Easter: procedure
	arg year
	a = year // 19;
	b = year // 4;
	c = year // 7;
	d = (19*a+16) // 30;
	e = (2*b + 4*c + 6*d) // 7;
return 3 + d + e;

/* ------------------ Catholic Easter ----------------------- */
/* if Easter > 30 then Month = May, Easter = Easter - 30      */
/*  EASTER.C - Determine the date of Easter for any given year*/
/*  public domain by Ed Bernal                                */
CatholicEaster: procedure
	/*
	**  Gauss' famous algorithm (I don't know how or why it works,
	**  so there's no commenting)
	*/
	year=arg(1)

	a = year // 19
	b = year%100
	c = year//100
	z = b%4
	e = b//4
	g = (8*b+13)%25
	h = (19*a + b - z - g + 15) // 30
	u = (a + 11*h) % 319
	i = c%4
	k = c//4
	x = (2*e + 2*i - k - h + u + 32)//7
	month = (h-u+x+90)%25
	day = (h-u+x + month +19)//32
	/*return day"/"month*/
	if month>4 then day = day + 30
return day

/* -------------------- daysOfMonth ---------------------- */
/* With the correction of "Byron <bjhinkle@worldnet.att.net>"
 * for the leap years
 */
daysOfMonth: procedure
daysInMonth = '312831303130313130313031'
	if arg() = 2	then parse arg month,year
			else parse arg month '/' year
	Days = substr(DaysInMonth,Month*2-1,2)
	if Month=2 then
	if Year//4=0 & Year//100^=0 | Year//400=0
		then Days = 29
		else Days = 28

return Days

/* ------------------ addMonths ------------------- */
addMonths: procedure
	if arg() = 2 then parse arg day '/' month '/' year, months
		else parse arg day,month,year,months

	if months = '' then months = 1
	month = month + months
	if months > 0 then do
		do while month > 12
			month = month - 12
			year = year + 1
		end
	end
	else do
		do while month < 1
			month = month + 12
			year = year - 1
		end
	end
return day'/'month'/'year

/* ----------------------- addDays ------------------- */
addDays: procedure
	if arg() = 2 then parse arg day '/' month '/' year, days
			else parse arg day,month,year,days
	if days = '' then days = 1
	day = day + days
	if days > 0 then do
		do while day > DaysOfMonth(month,year)
			day = day - DaysOfMonth(month,year)
			parse value addMonths(day'/'month'/'year,1) with day '/' month '/' year
		end
	end
	else do
		do while day < 1
			parse value addMonths(day'/'month'/'year,-1) with day '/' month '/' year
			day = day + DaysOfMonth(month,year)
		end
	end
return day'/'month'/'year

/* -------------------- swapDate ----------------- */
swapDate: procedure
	if arg()=1 then parse arg day '/' month '/' year
		else parse arg day,month,year
return month'/'day'/'year

/* ----------------- GMTime -------------------- */
/* Expand a unix date format time_t to
 *		year/month/day hour":"min":"sec
 * The date is an integer representing the number of seconds since 1/1/1970
 * Vasilis Vlachoudis 2003
 */
GMTime: procedure
	time = arg(1)

	sec = time // 60	/* seconds */
	time = time % 60

	min = time // 60	/* minutes */
	time = time % 60

	i = time % (1461 * 24)	/* Number of 4 year blocks */
	year = i * 4
	year = year + 70
	cumdays = 1461 * i
	time = time // (1461 * 24) /* Hours since end of last 4 year block */

	do forever
		hpery = 365 * 24
		if year//4 = 0 then hpery = hpery + 24
		if time < hpery then leave
		cumdays = cumdays + hpery / 24;
		year = year + 1;
		time = time - hpery;
	end	/* at end, time is number of hours into current year */

	/*
		if  (dst && daylight &&
			__isDST( (int)(time % 24),
			(int)(time / 24), 0, year-70)) {
				time++;
				isdst = 1;
		} else
			isdst = 0;
	*/

	hour = time // 24
	time = time % 24	/* Time in days */
	yday = time
	cumdays = cumdays + time + 4;
	wday = cumdays // 7
	time = time + 1

	year = year+1900

	if year//4 == 0 then do
		if time > 60 then
			time = time - 1
		else
		if time = 60 then do
			mon = 2;
			mday = 29;
			return year"/"mon"/"mday hour":"min":"sec
		end
	end

	days = "31 28 31 30 31 30 31 31 30 31 30 31"
	do mon = 1 while word(days,mon) < time
		time = time - word(days,mon)
	end

	mday = time
return year"/"right(mon,2,0)"/"right(mday,2,0) right(hour,2)":"right(min,2,0)":"right(sec,2,0)

/* ----------------- MKTime -------------------- */
/* Converts a date expressed in local time to the number of seconds
 *  elapsed since year 70 (1970) in calendar time.
 * Vasilis Vlachoudis 2003
 */
MKTime: procedure
	if arg()=1 then
		parse arg year "/" month "/" day hour ":" min ":" sec
	else
		parse arg year,month,day,hour,min,sec
	if ^datatype(year,"N") | ^datatype(month,"N") | ^datatype(day,"N") |,
	   ^datatype(hour,"N") | ^datatype(min,"N") | ^datatype(sec,"N") then
		return -1

	year = year - 1900

	if year < 70 | year > 138 then return -1

	month = month - 1	/* make zero based */
	day   = day - 1

	min  = min + sec % 60
	sec  = sec // 60	/* Seconds are normalized */
	hour = hour + min % 60
	min  = min // 60	/* Minutes are normalized */
	day  = day + hour % 24
	hour = hour // 24	/* Hours are normalized   */

	year = year + month % 12	/* Normalize month */
	month = month // 12

	days = "31 28 31 30 31 30 31 31 30 31 30 31"
	Ydays = "0 31 59 90 120 151 181 212 243 273 304 334"

	do while day >= word(days,month+1)
		if year//4=0 & month=1 then do
			if day > 28 then do
				day = day - 29
				month = month + 1
			end; else
				leave
		end; else do
			day = day - word(Days,month+1)
			month = month + 1
		end
		year = year + month % 12	/* Normalize month */
		month = month // 12
	end

	year = year - 70
	leaps = (year + 2) % 4

	if ((year+70)//4)=0 & month<2 then leaps = leaps-1

	days = year*365 + leaps + word(YDays,month+1) + day

	secs = days*86400 + hour*3600 + min*60 + sec /*+ timezone*/

	/*if (daylight && __isDST(hour, day, month+1, year))
		secs -= 3600*/

	if secs<0 then secs = -1
return secs

/***********************************************************************
 *                                                                     *
 * JULIAN -- Ordered to Julian and Julian to Ordered date conversion   *
 *                                                                     *
 *         (c) Eric Thomas 1986          (ERIC at FRECP11)             *
 *                                                                     *
 * This file was last updated on Saturday, June the 14th of 1986       *
 *                                                                     *
 *                                                                     *
 * This program will  convert a date from yy/mm/dd to  yyddd format or *
 *  vice-versa. It  can be invoked either  as a REXX function  or as a *
 *  command. The input  parameter is assumed to be  of correct syntax; *
 *  although some basic checks are performed, it is most probable that *
 *  a REXX error  will occur if the argument is  incorrect (eg 85999). *
 *                                                                     *
 * Examples:                                                           *
 *                                                                     *
 * (from CMS mode): Julian 84249                                       *
 *              --> 84249 = 84/09/05                                   *
 *                  Julian 84/09/05                                    *
 *              --> 84/09/05 = 84249                                   *
 *                                                                     *
 * (from REXX pgm): Say Julian(85123)                                  *
 *              --> 85/05/03                                           *
 *                  Say Julian('85/05/03')                             *
 *              --> 85123                                              *
 *                                                                     *
 ***********************************************************************/
Julian: procedure
Parse source . calltype .
normdays = '31 28 31 30 31 30 31 31 30 31 30 31'
bissdays = '31 29 31 30 31 30 31 31 30 31 30 31'

if arg()=3 then
	Parse arg dd,mm,yy
else
	Parse arg yy'/'mm'/'dd
If dd ^= ''
 Then
  Do
    If ^Datatype(yy||mm||dd,'W') Then Do
                                        Say "Invalid yy/mm/dd date:" Arg(1)
                                        Exit
                                      End
    yy = Right(yy,2,0)
    If yy // 4 = 0 Then days = bissdays
                   Else days = normdays
    Do i = 1 to mm-1
      dd = dd + Word(days,i)
    End
    result = yy||Right(dd,3,0)
  End
 Else
  Do
    If ^Datatype(yy,'W') | Length(yy) ^= 5 Then
     Do
       Say "Invalid yyddd date:" Arg(1)
       Exit
     End
    ddd = Right(yy,3)
    yy = Left(yy,2)
    If yy // 4 = 0 Then days = bissdays
                   Else days = normdays
    mm = 1
    Do i = 1
      md = Word(days,i)
      If md >= ddd Then Leave
      mm = mm + 1
      ddd = ddd - md
    End
    result = yy'/'Right(mm,2,0)'/'Right(ddd,2,0)
  End

If calltype ^== 'COMMAND' Then Return result
Say Arg(1) "=" result
Return result
