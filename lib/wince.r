/* ------  Windows CE dedicated functions ---- */
/* --- Initialise some common variables --- */
CeInit:
/* --- Messabe Box options --- */
MB_OK=0
MB_OKCANCEL=1
MB_ABORTRETRYIGNORE=2
MB_YESNOCANCEL=3
MB_YESNO=4
MB_RETRYCANCEL=5
MB_ICONHAND=16
MB_ICONQUESTION=32
MB_ICONEXCLAMATION=48
MB_ICONASTERISK=64

/* --- Predefined Clipboard formats --- */
CF_TEXT=1
CF_BITMAP=2
CF_SYLK=4
CF_DIF=5
CF_TIFF=6
CF_OEMTEXT=7
CF_DIB=8
CF_PALETTE=9
CF_PENDATA=10
CF_RIFF=11
CF_WAVE=12
CF_UNICODETEXT=13
return

/* --- CEListFile --- */
CEListFile: procedure
	trace o
	dirfiles = Dir(arg(1))
	files = ""
	do forever
		parse var files . +36 fn "0A"x files
		files = files || fn
		if dirfile<>""
			then files = files"0A"x
			else leave
	end
return

/* --- Color --- */
Color: procedure
	parse arg fg,bg
	colors ="BLACK RED GREEN YELLOW BLUE MAGENTA CYAN GRAY WHITE"

	fgofs = 7
	select
		when left(fg,4)=="DARK" then do
			fgofs=0
			fg = substr(fg,5)
		end
		when left(fg,4)=="BOLD"  then fg = substr(fg,5)
		when left(fg,5)=="LIGHT" then fg = substr(fg,6)
	end

	f = wordpos(fg,colors)-1
	if f<0 then return "ERROR"
	if f=8 then f=15
	else
	if f=7 & fgofs=7 then f=8
	else
		f = f+fgofs

	if bg<>"" then do
		bgofs = 7
		select
			when left(bg,4)=="DARK" then do
				bgofs=0
				bg = substr(bg,5)
			end
			when left(bg,4)=="BOLD"  then bg = substr(bg,5)
			when left(bg,5)=="LIGHT" then bg = substr(bg,6)
		end

		b = wordpos(bg,colors)-1
		if b<0 then return "ERROR"
		if b=8 then b=15
		else
		if b=7 & bgofs=7 then b=8
		else
			b = b+bgofs

		call setcolor f,b
	end; else
		call setcolor f
return
