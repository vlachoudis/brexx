/* exts.r, displays all extensions (+info) in a directory */
/* Bill N. Vlachoudis (c) May 1996 */
if arg(1) = "?" then do
	say "syntax: exts [<dir>]"
	say "desc:   Displays a summary of file extensions in the current directory"
	say "author: Bill N. Vlachoudis"
	say "date:   May 1996"
	exit
end
parse source system .
if system == "UNIX" then
	'ls -l' arg(1) '(stack'
else do
	'dir' arg(1) '(stack'
	/* skip first lines */
	c = 0
	do while queued()>0
		parse pull line
		if line="" then c=c+1
		if c=2 then leave
	end
end
size. = 0
count. = 0
exts = ""
totsize = 0
files = 0
maxtype = 0
do while queued()>0
	if system=="UNIX" then
		parse pull . . . . size . . . name '.' type .
	else do
		parse pull name type size fdate ftime
		if ftime = '' then do
			size = type
			type = 'No_EXT'  /* file has '*. ' type of name */
		end

		if type = "file(s)" then do
			do queued()
				parse pull .
			end
			leave
		end
		if type="<DIR>" then iterate
		p = pos(',',size)
		do while p>0
			size = delstr(size,p,1)
			p = pos(',',size)
		end
		if datatype(size) ^= "NUM" then iterate
	end
	if datatype(size,'NUM') then do
		if type="" then type="(none)"
		size.type = size.type + size
		count.type = count.type + 1
		totsize = totsize + size
		files = files + 1
		if count.type = 1 then exts = exts type
		maxtype = max(maxtype,length(type))
	end
end
maxtype = maxtype + 3
line=left("Extensions",maxtype) " Files" right("Size",10)

say line
say copies('-',length(line))
do i = 1 to words(exts)
	type = word(exts,i)
	say left(type,maxtype) format(count.type,6) format(size.type,10)
end

say copies('-',length(line))
say words(exts) "Extensions, " files "Files"
say "Total Size =" totsize"."
