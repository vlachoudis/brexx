parse arg msg
/*file = "C:\DOS\EGAGK.CPI"*/
file = "/C/dos/egagk.cpi"
file = "/mnt/net/misg140/mnt/dos/dos/egagk.cpi"
if left(word(msg,1),1) == '-' then do
	file = substr(word(msg,1),2)
	parse var msg . msg
end
f = open(file,"rb")
line. = ''
do i = 1 to length(msg)
	call letter substr(msg,i,1)
	do j = 1 to 16
		line.j = line.j row.j
	end
end
do i = 1 to 16
	say line.i
end
exit

letter: procedure expose row. f
	call seek f,64+c2d(arg(1))*16
	drop row.
	row. = ''
	do i = 1 to 16
		c = c2d(read(f,1))
		do while c > 0
			if c // 2 then row.i = '#' || row.i
				else row.i = ' ' || row.i
			c = c % 2
		end
		row.i = right(row.i,8)
	end
return
