trace o
"cat links (stack"
links.0 = queued()
do i=1 to links.0
	parse pull link.i text.i
end
"mkdir new"

do l=2 to links.0
	say "File:" link.l
	fin = open(link.l,"r")
	fout = open("new/"link.l,"w")

	lp = l - 1
	ln = l + 1

	do forever
		line = read(fin)
		if eof(fin) then leave
		if abbrev(strip(line),"<BODY",5) then do
			call lineout fout,line
			call WriteLinks
			call lineout fout,'<HR>'
			/* skip everything until <HR> */
			do until line="<HR>"
				line = read(fin)
			end
		end; else
		if line="<!-- Link End -->" then do
			call lineout fout,line
			call lineout fout,'<HR>'
			call WriteLinks
			do until line="</TABLE>"
				line = read(fin)
			end
		end; else
			call lineout fout,line
	end

	call close fin
	call close fout
end
exit

WriteLinks:
	call lineout fout,'<TABLE>'
	call lineout fout,'<TD><A HREF="'link.lp'">Previous</A>'
	call lineout fout,'<TD><A HREF="'link.1'">Top</A>'
	call lineout fout,'<TD><A HREF="sortidx.html">Index</A>'
	if ln <= links.0 then
		call lineout fout,'<TD><A HREF="'link.ln'">Next</A>'
	call lineout fout,'</TABLE>'
return
