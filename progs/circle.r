/* ---------------- some mcga functions ---------------- */
parse arg xc yc r .
if xc = '' then do
	say 'syntax: circle x y r'
	say 'ie.  circle 100 100 50'
	say '! RXLIB environment variable must be set, or CONIO.R must be in'
	say '  current directory'
	exit 1
end

call load "conio.r"
call setmode x2d(13)

call time 'r'
do f = 0.0 to 6.28 by 0.025
	x = xc + r * cos(f)
	y = yc + r * sin(f)
	call plot x,y,15
end
t = time('e')
say "press enter to continue"
pull .
call setmode 3
say "time to draw circle =" t "secs"
exit

/* ------------- plot(x,y,color)--- returns prev color ---- */
plot: procedure
addr = "A0000"h + 320 * trunc(arg(2)) + trunc(arg(1))
return storage(addr,1,d2c(arg(3)))
