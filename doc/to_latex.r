/*
 * This file reads all the files which are links from the main rx.html
 * (except the sortidx.html) and creates a latex file called brexx.tex
 */
latexfile = "brexx.tex"
dir="\Temp\Rexx\new\"
dir="./"
mainfile = "rx.html"

say "Creating a latex output file '"latexfile"' from the html documents."
say

fout = open(dir||latexfile,"w")

/* --- prepare the header ---- */
call lineout fout,"\documentclass[a4paper,12pt]{book}"
call lineout fout,"\begin{document}"
/**call lineout fout,"\setlength{\indent}{0pt}"**/

/* find order of files from main file */
fin = open(dir||mainfile,"r")
do until eof(fin)
	line = read(fin)
	if abbrev(line,"<LI><A HREF=") then do
		parse var line . '"' file '">' title "</A>" .
		if file=="sortidx.html" then iterate
		say "File:" file
		call Process fout,dir||file
	end
end

/* --- write the footer --- */
call lineout fout,"\end{document}"
call close fout
call close fin
Exit

/* ---- Process HTML file ---- */
Process: procedure
	parse arg fout,fn
	file = read(fn,"F")
	call close fn
	ptr = 0

	write = 1

	link = 0
	bold = 0
	table = 0
	tableptr = 0
	tablecols = 0
	tableTRCnt = 0
	tableAfterTR = 0
	italic = 0
	underline = 0
	ulist = 0
	olist = 0

	do while ptr>=0
		str = StringUntilNextTag(ptr)
		if ptr<0 then leave
		call WriteStr fout,str
		parse upper value TagOpt() with tag opt
		EndTag = "/"tag

		select
			when tag=="HTML" then nop
			when tag=="/HTML" then leave
			when tag=="HEAD" then Call SkipUntil EndTag
			when tag=="BODY" then nop
			when tag=="/BODY" then nop
			when tag=="A" then nop
			when tag=="/A" then nop
			when tag=="H1" then do
				str = StrUntil(EndTag)
				if write then call lineout fout,"\section{"strip(str)"}"
			end
			when tag=="H2" then do
				str = StrUntil(EndTag)
				if write then call lineout fout,"\subsection{"strip(str)"}"
			end
			when tag=="H3" then do
				str = StrUntil(EndTag)
				if write then call lineout fout,"\subsubsection{"strip(str)"}"
			end
			when tag=="HR" then nop
			when tag=="BR" & write then call lineout fout,"\\"
			when tag=="P" then do
				if write then call lineout fout
				if write then call lineout fout
			end
			when tag=="B" then do
				if bold then Call Error "Unexpected tag" tag
				bold = 1
				if write then call write fout,"{\bf "
			end
			when tag=="/B" then do
				if ^bold then Call Error "Unexpected tag" tag
				bold = 0
				if write then call write fout,"}"
			end
			when tag=="I" then do
				if italic then Call Error "Unexpected tag" tag
				italic = 1
				if write then call write fout,"{\it "
			end
			when tag=="/I" then do
				if ^italic then Call Error "Unexpected tag" tag
				italic = 0
				if write then call write fout,"}"
			end
			when tag=="CENTER" then do
				if write then call lineout fout,"\begin{center}"
			end
			when tag=="/CENTER" then do
				if write then call lineout fout,"\end{center}"
			end
			when tag=="BLOCKQUOTE" then do
				if write then call lineout fout,"\begin{quote}"
				if write then call lineout fout,"\setlength{\rightmargin}{0pt}"
			end
			when tag=="/BLOCKQUOTE" then do
				if write then call lineout fout,"\end{quote}"
			end

			when tag=="TABLE" then do
	/* !!! FOR THE MOMENT NESTING TABLES ARE NOT ALLOWED !!! */
				if table^=0 then Call Error "Nesting tables are not allowed"
				table = -1
				/* first try to estimate the columns */
				tableptr = ptr
				tablecols=0
				tableAfterTR=0
				tableTRCnt=0
				write = 0
			end
			when tag=="/TABLE" then do
				if table=0 then Call Error "Unexpected tag" tag
				if table=1 then
					if write then call lineout fout,"\end{tabular}"
				table = 0
				write = 1
				if tableTRCnt=0 then Call Error "Table with no TR"
			end

			when tag=="TR" then do
				if table=0 then Call Error "Unexpected tag" tag
				else
				if table=1 & write then do
					if tableTRCnt<>0 then call lineout fout,"\\"
					tableTRCnt=tableTRCnt+1
				end; else
				if table=-1 & tablecols>0 then do
					table=1
					write=1
					tableTRCnt=0
					if write then call lineout fout,"\begin{tabular}{"copies("l",tablecols)"}"
					ptr = tableptr
				end
				tableAfterTR = 1
			end
			when tag=="/TR" then nop
			when tag=="TD" | tag=="TH" then do
				if table=0 then Call Error "Unexpected tag" tag
				else
				if table=-1 then do
					parse var opt . "COLSPAN=" cols .
					if cols="" then cols=1
					tablecols = tablecols + cols
				end; else
				if ^tableAfterTR & write then call write fout,"&"
				tableAfterTR = 0
			end
			when tag=="/TD" then nop

			when tag=="UL" then do
				ulist = 1
				if write then call lineout fout,"\begin{itemize}"
			end
			when tag=="/UL" then do
				if ^ulist then Call Error "Unexpected tag" tag
				ulist = 0
				if write then call lineout fout,"\end{itemize}"
			end
			when tag=="LI" then do
				if ^ulist & ^olist then
					Call Error "Unexpected tag" tag
				if write then call write fout,"\item "
			end
			when tag=="/LI" then nop

			otherwise
				Call Error "Unexpected tag" tag
		end
	end
return

/* ---- Write String --- */
WriteStr: procedure expose write
	parse arg fout,str
	outstr = ""
	do while str^==""
		if left(str,1) == "&"
			then parse var str ch ";" str
			else parse var str ch +1 str
		select
			when ch=="\" then ch="\texttt{\char92}"
			when ch=="^" then ch="\texttt{\char94}"
			when ch=="~" then ch="\texttt{\char126}"

			when ch=="&lt" then ch="$<$"
			when ch=="&gt" then ch="$>$"
			when ch=="&amp" then ch="\&"
			when ch=="&nbsp" then ch="~"

			when	ch=="#" |,
				ch=="$" |,
				ch=="_" |,
				ch=="%" |,
				ch=="@" then ch="{\"ch"}"
			
			when	ch=="[" |,
				ch=="]" |,
				ch=="|" |,
				ch=="/" |,
				ch=="\" |,
				ch=="*"
			then do
				if right(outstr,1)=="$"
					then do
						outstr = left(outstr,length(outstr)-1)
						ch=ch"$"
					end
					else ch="$"ch"$"
			end
		end
		outstr = outstr || ch
	end
	if write then call write fout,outstr
return 


/* ---- StrUntil ---- */
StrUntil: procedure expose fn file ptr
	trace o
	parse arg EndTag
	oldptr = ptr
	do forever
		str = StringUntilNextTag()
		p = ptr
		parse upper value TagOpt() with tag opt
		if tag == EndTag then leave
	end
return substr(file,oldptr+1,p-oldptr-1)

/* ---- SkipUntil ---- */
/* Skip everything until the ending tag */
SkipUntil: procedure expose fn file ptr
	trace o
	parse arg EndTag
	do forever
		str = StringUntilNextTag()
		parse upper value TagOpt() with tag opt
		if tag == EndTag then leave
	end
return

/* ----------- StringUntilNextTag ------------ */
StringUntilNextTag: procedure expose fn file ptr
	trace o
	p = pos("<",file,ptr+1)
	if p = 0 then do
		ptr = -1
		return ""
	end
	str = substr(file,ptr+1,p-ptr-1)
	ptr = p
return str

/* ------------ TagOpt -------------- */
TagOpt: procedure expose fn file ptr
	trace o
	p = pos(">",file,ptr+1)
	if p = 0 then do
		ptr = -1
		return ""
	end
	tagopt = substr(file,ptr+1,p-ptr-1)
	ptr = p
return tagopt

/* --------- display an error message ---- */
Error:
	linenum = countstr("0A"x,left(file,ptr))+1
	start = lastpos("0A"x,file,ptr)+1
	stop = pos("0A"x,file,ptr)
	line = substr(file,start,stop-start)
	call lineout "<STDERR>","ERROR:" arg(1)
	call lineout "<STDERR>","In File:" fn", Line="Linenum '"'line'"'
	exit
