/* display random stars */
call load "ansi.r"
call AnsiCls
signal on halt
colors = "BLUE GREEN MAGENTA RED CYAN YELLOW WHITE BLACK"
step = ". o # * "
steps = words(step)

star. = ""
nmax = 30
do i=1 to nmax
	x = random(0,79)
	y = random(0,24)
	call AnsiColor word(colors,random(1,words(colors)))
	call Ansigoto x,y
	call write ,word(step,1)
	call flush "<STDOUT>"
		/* store it's location */
	star.i = x y
	call delay 0.2
end
do forever
	/* peek a random star */
	i = random(1,nmax)
	parse var star.i x y
	if random(1,10)>4 then
		do i=2 to steps
			call AnsiGoto x,y
			call AnsiColor word(colors,random(1,words(colors)))
			call write ,word(step,i)
			call flush "<STDOUT>"
			call delay 0.15
		end
	call AnsiGoto x,y
	call write ,' '

	/* create a new one */
	x = random(0,79)
	y = random(0,24)
	call AnsiColor word(colors,random(1,words(colors)))
	call AnsiGoto x,y
	call write ,word(step,1)
	star.i = x y
	call delay 0.2
	call flush "<STDOUT>"
end
halt:
call AnsiCls
call AnsiAttr "normal"
exit

delay:
	call time 'r'
	do while arg(1)>time('e')
		nop
	end
return
