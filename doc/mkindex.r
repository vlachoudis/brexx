sortfile = "sortidx.html"

say "This program creates the" sortfile "index file from the html"

parse source os .
if os=="WINCE" then do
	call load "wince.r"
	docdir = "\Temp\Rexx\new\"	/* "\Program Files\Rexx\Doc\" */
	pathsep = "\"
	files = ListFile(docdir"*.html")
end; else do
	docdir = "./"
	pathsep = "/"
	/*files = "ls"(docdir"*.html")*/
	"ls" docdir"*.html (stack"
	files = ""
	do queued()
		parse pull file
		files = files||file||"0A"x
	end
end
say "HTML dir="docdir

/* --- find all the possible targets --- */
say "Processing files:"

link. = ""
link.0 = 0
htmlfile. = ""
Do until files=""
	parse var files fn "0A"x files
	p = lastpos(pathsep,fn)
	if p>0 then fn = substr(fn,p+1)
	if fn=sortfile then iterate
	call FindTarget docdir,fn
End

fout = open(sortfile,"w")

say
say
say "Writing the HTML file:"
/* --- Prepare the header --- */
call lineout fout,'<HTML>'
call lineout fout,'<HEAD>'
call lineout fout,'<TITLE>BREXX, General Index</TITLE>'
call lineout fout,'<META NAME="Responsible"    CONTENT="Vasilis.Vlachoudis@cern.ch">'
call lineout fout,'<META NAME="Created"        CONTENT="1998-3-2">'
call lineout fout,'<META NAME="NextRevision"   CONTENT="2000-1-1">'
call lineout fout,'<META NAME="DC.Creator"     CONTENT="VI">'
call lineout fout,'<META NAME="DC.Subject"     CONTENT="BREXX Manual">'
call lineout fout,'<META name="DC.Description" CONTENT="BREXX General Index">'
call lineout fout,'<META NAME="DC.Publisher"   CONTENT="Vasilis.Vlachoudis@cern.ch">'
call lineout fout,'<META NAME="DC.Contributor" CONTENT="Vasilis.Vlachoudis@cern.ch">'
date = insert('-',insert('-',date('s'),4),7)
call lineout fout,'<META NAME="DC.Date"        CONTENT="'date'">'
call lineout fout,'<META NAME="DC.Type"        CONTENT="Public">'
call lineout fout,'<META NAME="DC.Language"    CONTENT="English">'
call lineout fout,'</HEAD>'
call lineout fout,'<BODY BGCOLOR="#FFFFFF">'
call lineout fout,'<TABLE>'
call lineout fout,'<TD><A HREF="implemen.html">Previous</A>'
call lineout fout,'<TD><A HREF="index.html">Top</A>'
call lineout fout,'</TABLE>'
call lineout fout,'<HR>'
call lineout fout
call lineout fout,'<CENTER><H1>General Index</H1></CENTER>'
call lineout fout,'<P>'

/* --- create links for the alphabet --- */
call lineout fout,'<CENTER>'
call alpha_links fout
call lineout fout,"<P>"

/* ---- create the links ---- */
call lineout fout,"<TABLE BORDER WIDTH='70%'>"
lastletter = ""
do i=1 to link.0
	letter = translate(left(link.i,1))
	if letter <> lastletter then do
		call lineout fout,'<TR><TD COLSPAN=2 ALIGN=CENTER>'
		call lineout fout,'<A NAME="'letter'"></A><A HREF="'sortfile'"><H2>'letter'</H2></A></TD></TR>'
		lastletter = letter
	end
	link = link.i
	file = htmlfile.i
	htmllink = file".html#"link
	call lineout fout,'<TR><TD><A HREF="'htmllink'">'link'</A></TD>'
	call lineout fout,'    <TD>'file'</TD></TR>'
end
call lineout fout,'</TABLE>'

/* --- create links for the alphabet --- */
call alpha_links fout
call lineout fout,"<P>"

/* --- Prepare the footer --- */
call lineout fout,'</CENTER>'
call lineout fout,"<P>"
call lineout fout,'<HR>'
call lineout fout,'<TABLE>'
call lineout fout,'<TD><A HREF="implemen.html">Previous</A>'
call lineout fout,'<TD><A HREF="index.html">Top</A>'
call lineout fout,'</TABLE>'
call lineout fout,'</BODY>'
call lineout fout,'</HTML>'

call close fout
say "Exiting"
Exit

/* --- alpha_links --- */
alpha_links: procedure expose link.
	parse arg fout
	do i=1 to 26
		letter = substr(xrange('A','Z'),i,1)
		found = 0
		do j=1 to link.0
			if translate(left(link.j,1))==letter then do
				found = 1
				leave
			end
		end
		if found
			then call lineout fout,'<A HREF="#'letter'">'letter'</A>'
			else call lineout fout,letter
	end
return

/* --- FindTarget --- */
FindTarget: procedure expose link. htmlfile.
	parse arg dir,filename

	call write ,filename' '
	fin = open(dir||filename,"r")
	if fin<0 then do
		say
		say "Error in opening file '"dir||filename"'."
		return
	end
	do forever
		line = read(fin)
		if eof(fin) then leave
		if pos("<a name=",line) <> 0 then do
			parse var line . '"' target '"' .
			call AddSorted target,left(filename,length(filename)-5)
		end
	end
	call close fin
return

/* ----- Add to link. array but sorted (case insensitive) ----- */
AddSorted: procedure expose link. htmlfile.
	parse arg target,file
	n = link.0
	utarget = translate(target)
	do i=1 to n
		if utarget<translate(link.i) then
			leave
	end
	do j=n to i by -1
		k = j+1
		link.k = link.j
		htmlfile.k = htmlfile.j
	end
	link.i = target
	htmlfile.i = file
	link.0 = n + 1
return
