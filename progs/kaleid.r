/*===================================================================*/
/*     Bill N. Vlachoudis                                     o o    */
/*     Computer adDREss.......V.Vlachoudis@cern.ch          ____oo_  */
/*     HomE AdDreSs...........Parodos Filippoy 51          /||    |\ */
/*   !                        TK 555-35 Pylaia              ||    |  */
/* !  tHe bESt Is             ThesSALONIKI, GREECE          '.___.'  */
/*  Y e t  to   Co mE !                                     MARMITA  */
/*===================================================================*/
call load 'ansi.r'
signal on halt
colors = 'BLUE GREEN MAGENTA RED CYAN YELLOW WHITE BLACK'
colors = 'BLACK MAGENTA BLUE RED GREEN CYAN YELLOW WHITE'
do i=1 to words(colors)
	col.i = word(colors,i)
end
                                                               
hd.1  = "   ******  ****              ****                         ****"
hd.2  = "    ***     **                 **              **          ***"
hd.3  = "     **    **                  **              **           **"
hd.4  = "    **    **                  **                           **"
hd.5  = "    **  **         **** **    **      ****   ***      **** **"
hd.6  = "    *****         **  ***    **      **  **   **     ** ****"
hd.7  = "   *****         **    **    **     **   **   **    **    **"
hd.8  = "   **  **        **    **    **    **   **   **    **     **"
hd.9  = "   **  **       **    **    **    *******    **    **     **"
hd.10 = "  **    **      **    **    **    **         **   **     **"
hd.11 = "  **     **     **   ***   **     **        **    **     **"
hd.12 = " ***     ***    **  **** * ** *   **   **   ** *  **   **** *"
hd.13 = "*****   ******   **** ***  ****    *****    ****   ***** ****"
hd.0 = 13

n = copies(' ',68);
call AnsiCls
say n "   o o   "
say n " ____oo_ "
say n "/||    |\"
say n " ||    | "
say n " `.___.' "
say n " MARMITA "
drop n
do i=1 to hd.0
	say '      ' hd.i
end
text.1 = center('by bill vlachoudis V.Vlachoudis@cern.ch (c)1988',79)
text.2 = center('for DOS ANSI.SYS is required.',79)
text.3 = center('Press <ENTER> to continue,  Control-C to stop',79)
do i=1 to 3
	say text.i
end

/* scroll a little bit the colors */
do i=1 to 80
	do j=1 to hd.0
		p = i-j
		if p>0 then do
			c = substr(hd.j,p,1)
			if c=='*' then do
				call AnsiColor "BOLDYELLOW"
				call AnsiGoto p+7,j+6
				call write ,'*'
			end
		end
		p = i-j-4
		if p>0 then do
			c = substr(hd.j,p,1)
			if c=='*' then do
				call AnsiColor "RED"
				call AnsiGoto p+7,j+6
				call write ,'*'
			end
		end
	end j
	/* delay a while */
	call time 'r'
	do while time('e')<0.012
		nop
	end
	call flush "<STDOUT>"
end i

/* put random colors to text */
do i=1 to 3
	do j=1 to length(text.i)
		ch = substr(text.i,j,1)
		if ch ^== " " then do
			c = random(0,15)
			if c>7 then col="BOLD"; else col=""
			c = c//7+1
			call Ansicolor col||col.c
			call AnsiGoto j,i+19
			call write ,ch
			call flush "<STDOUT>"
		end
	end j
end i

pull .
call AnsiCls
Do w = 3 to 50
	Do i = 1 to 39
		Do j = 1 to 39
			k = i + j
			c = trunc(j * 3 / (i + 7) + i * w / 24)
			if c > 15 then c = c - 15
			xl = i
			xr = 80 - xl
			yt = trunc(k/4 + 0.5)
			yb = 21 - yt
			if c > 7 then col = 'BOLD'; else col = ''
			cc = c//7+1
			col = col col.cc 
			Call AnsiColor col,col.cc

			call AnsiGoto xl,yt; call write ,'#'
			call AnsiGoto xr,yt; call write ,'#'
			call AnsiGoto xl,yb; call write ,'#'
			call AnsiGoto xr,yb; call write ,'#'
			yt = trunc(i/4 + 0.5)
			yb = 21 - yt
			xl = trunc(k)
			xr = 80 - xl
			call AnsiGoto xl,yt; call write ,'#'
			call AnsiGoto xr,yt; call write ,'#'
			call AnsiGoto xl,yb; call write ,'#'
			call AnsiGoto xr,yb; call write ,'#'
			call flush "<STDOUT>"
		End
	End
End
 
exit:
halt:
call AnsiCls
call AnsiAttr "normal"
exit 0
