#!/usr/local/bin/rexx

parse arg args
if args = "" then do
	say "syntax: htmlmacro.r [options] file"
	say "desc: Filters the input file and creates a rexx file for"
	say "	generating the output"
	say "	Commands accepted:"
	say "		<rexx>"
	say "		rexx-commands"
	say "		<rexx>"
	say "	Inlining: with $"
	say "Options:"
	say "	-s[Char] - define the special char for inline (default=$)"
	say "	-v	- only dump on screen the result rexx file"
	say "	-x	- [default] execute the rexx file"
	say "Author: Vasilis.Vlachoudis@cern.ch 2001"
	exit
end

filename = ""
specialCh = "$"
execute = 1
do while args <> ""
	parse var args opt args
	select
		when left(opt,2)=="-s" then do
			specialCh=substr(opt,3,1)
			if specialCh==" " then specialCh="$"
		end
		when opt=="-x" then execute=1
		when opt=="-v" then execute=0
		otherwise
			filename = opt
	end
end

if filename <> "" then
	fin = open(filename,"r")
else
	fin = 0

incode = 0
firstline = 1
line = ""
do forever
	if line="" then do
		line = read(fin)
		if eof(fin) then leave
		if firstLine then do
			if left(line,2)=="#!" then iterate
			firstLine = 0
		end
	end

	if incode then do
		if pos("</rexx>",line) <> 0 then do
			parse var line before "</rexx>" after
			queue before
			incode = 0
			line = after
		end; else do
			queue line
			line = ""
		end
	end; else do
		if pos("<rexx>",line) <> 0 then do
			parse var line before "<rexx>" after
			if before <> "" then call filter before
			line = after
			incode = 1
		end; else do
			call filter line
			line = ""
		end
	end
end
if execute then "stack> rexx -"
else do queued()
	parse pull line
	say line
end
exit

/* --- Filter --- */
Filter: procedure expose specialCh
	parse arg line
	lineout = ""
	do while line <> ""
		parse var line before (specialCh) inside (specialCh) after
		if right(before,1)=="\" then do
			lineout = lineout || before || specialCh
			line = inside || specialCh|| after
		end; else do
			lineout = changestr('"',lineout||before,'""')
			if lineout <> "" then
				queue 'call write 1,"'lineout'"'
			if inside <> "" then
				queue 'call write 1,'inside
			line = after
			lineout = ""
		end
	end
	queue "call write 1"
return
