path = "\Storage Card"
KB = 1024
MB = KB**2
CardSize = 32*MB

call windowtitle strip("Storage Card")

size = dirsize(path)

say "Storage Card Compact Flash Lexar"
say "Space Occupied:" size/KB "("trunc(100*size/CardSize)"%)"
free= CardSize-Size
say "Free Space:" Free/KB "("trunc(100*Free/CardSize)"%)"
exit

/* display the size in kb of each directory and the sub-directories */
dirsize: procedure
	parse arg path
	if right(path,1) ^== "\" then path = path"\"
	dir=dir(path"*")
	totalsize = 0
	dirsize = 0
	do while dir<>""
		parse var dir attrs size date time file "0A"x dir
		if left(attrs,1)=='D' then
			totalsize = totalsize + dirsize(path||file)
		dirsize=dirsize+size
	end
return  totalsize + dirsize