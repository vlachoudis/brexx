/*
        A small library of files routines.
        Bill N. Vlachoudis   V.Vlachoudis@cern.ch
*/

say "This is a library file, and cannot run alone..."
exit 1

/* ----------- return file size, or -1 if file not found --------------- */
filesize: procedure
	trace o
	file = open(arg(1),"rb")
	if file = -1 then return -1
	size = seek(file,0,"eof")
	call close file
return size

/* ----------------- return 0 or 1 if file exists -------------- */
state: procedure
	trace o
	file = open(arg(1),"rb")
	if file = -1 then return 0
	call close file
return 1

/* --------------------- basename ------------------------------ */
basename: procedure
	trace o
	parse arg path
	parse source os .
	if os=="UNIX" then
		sep = "/"
	else
		sep = "\"
	lp = lastpos(sep,path)
return substr(path,lp+1)

/* ---------------------- dirname ------------------------------ */
dirname: procedure
	trace o
	parse arg path
	parse source os .
	if os=="UNIX" then
		sep = "/"
	else
		sep = "\"
	lp = lastpos(sep,path)
	if lp=0 then return path
return left(path,lp-1)

/* --------------------- readword ------------------------------ */
/* Read one word from file */
readword: procedure
	trace o
	fin = arg(1)
	word = ""
	do forever
		if eof(fin) then leave
		ch = read(fin,1)
		if c2d(ch) <= 32 then do
			if word ^== "" then leave
		end; else
			word = word || ch
	end
return word

/* the same as state */
exist: return state(arg(1))

/* ---------------- Read a file in an array ------------------ */
ReadStem: procedure
	parse arg filename,array,pool,opt
	if pool = "" then pool = 0
	up = left(translate(opt),1)="U"
	fin = open(filename,"r")
	if fin < 0 then do
		call value array"0",0,pool
		return
	end
	do i=1
		line = linein(fin)
		if up then upper line
		if eof(fin) then leave
		call value array||i,line,pool
	end
	call value array"0",i-1,pool
	call close fin
return

/* ------------ Write an array to a file -------------------- */
WriteStem: procedure
	parse arg filename,array,pool
	if pool = "" then pool = 0
	fout = open(filename,"w")
	do i=1 to value(array"0",,pool)
		call lineout fout,value(array||i,,pool)
	end
	call close fout
return
