parse arg path
path=strip(path)

call windowtitle strip("List Files" path)
fout = open("\Temp\ls.out","w")
call listdir path
call close fout
return

/* display the size in kb of each directory and the sub-directories */
listdir: procedure expose fout
	parse arg path

	say "Directory:" path
	call lineout fout,"Directory:" path

	if right(path,1) ^== "\" then path = path"\"
	dir=dir(path"*")
	subdirs=""
	dirsize = 0
	do while dir<>""
		parse var dir filedata "0A"x dir
		parse var filedata attrs size date time file

		say filedata
		call lineout fout,filedata

		if left(attrs,1)=='D' then subdirs=subdirs"|"file
		dirsize=dirsize+size
	end
	say "Total Size:" dirsize
	call lineout fout,"Total Size:" dirsize
	do while subdirs<>""
		parse var subdirs dir "|" subdirs
		if dir="" then iterate
		call listdir path||dir
	end
return
