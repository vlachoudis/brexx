parse arg path
path=strip(path)
if left(path,1)=='-' then do
	ignore = substr(path,2)
	path = ""
end; else
	ignore = ""

call windowtitle strip("Disk Usage" path)
say " Total   Dir  Name"
say "====== ======" copies('=',40)
call dirsize path
exit

/* display the size in kb of each directory and the sub-directories */
dirsize: procedure expose ignore
	parse arg path
	if right(path,1) ^== "\" then path = path"\"
	dir=dir(path"*")
	totalsize = 0
	dirsize = 0
	do while dir<>""
		parse var dir attrs size date time file "0A"x dir
		if left(attrs,1)=='D' then do
			newdir = path||file
			if newdir ^= ignore then
				totalsize = totalsize + dirsize(path||file)
		end
		dirsize=dirsize+size
	end
	totalsize = totalsize + dirsize
	say right(trunc(totalsize/1024),6) right(trunc(dirsize/1024),6) path
return totalsize