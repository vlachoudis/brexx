/****************************************************
 PS commands
	General
	~~~~~~~
		Unit of measure 1/72 of inch
 number		any kind of number integer or real
 string		will be substitute if there is a definition
		otherwise as it is
 (string)	litteral
 {1 2 (hello)}	array of objects

	STACK Operators
	~~~~~~~~~~~~~~~
 clear		Remove all stack contents		ob1 ob2 => -
 dup		duplicate top of stack			ob => ob ob
 exch		Reverse order of top two objects	ob1 ob2 => ob2 ob1
 pop		Remove top of stack			ob1 ob2 => ob1
 n j roll	Rotate n elements j times		ob1..obj n j => ob1..obj
 % comment	Line comments

	MATH Operators
	~~~~~~~~~~~~~~
 add		Add two numbers				a b => a+b
 div		Divide					a b => a/b
 idiv		Integer Divide				a b => int(a/b)
 mod		Modulus					a b => mod(a/b)
 mul		Multiply				a b => mod(a/b)
 sub		Subtract				a b => a-b

	RELATIONAL Operators
	~~~~~~~~~~~~~~~~~~~~
 eq		Test for equality			a b => bool
 ne		Test for inequality			a b => bool
 gt		Test for greater than			a/str1 b/str2 => bool
 ge		Test for greater or equal		a/str1 b/str2 => bool
 lt		Test for less than			a/str1 b/str2 => bool
 le		Test for less or equal			a/str1 b/str2 => bool

	STRING Operators
	~~~~~~~~~~~~~~~~
 string		Create string of length n		n => str
 cvs		Convert to string			ob str => str

	INTERACTIVE Operators
	~~~~~~~~~~~~~~~~~~~~~
 ==		Destructively display top of stack	ob => -
 pstack		Display stack contents

	PATH Construction Operators
	~~~~~~~~~~~~~~~~~~~~~~~~~~~
 closepath	Closes the current path with a straight line to the last
		"moveto" point
 lineto		Continue the path with line to (x,y)	x y => -
 moveto		Set the current point to (x,y)		x y => -
 newpath	Clear the current path
 rlineto	Relative "lineto" (curpoint + (x,y))	x y => -
 rmoveto	Relative "moveto"			x y => -
 arc		Add counter-clockwise arc to		x y r ang1 ang2 => -
		current path
 arcn		Add clockwise arc to current path	x y r ang1 ang2 => -
 arcto		Built tangent arc			x1 y1 x2 y2 r => xt1 yt1 xt2 yt2
 currentpoint	return coordinates of current point	- => x y

	DICTIONARY Operators
	~~~~~~~~~~~~~~~~~~~~
 key value def	Associate key with value in the current dictionary
		/inch {72 mul} def

	PAINTING Operators
	~~~~~~~~~~~~~~~~~~
 fill		Fill current path with the current color
 setgray	Set the current gray-color (0-1)	n => -
 setlinewidth	Set the current line width		n => -
 stroke		Paint the current path with the current color and line width

	FONTS Operators
	~~~~~~~~~~~~~~~
 findfont	Return dictionary for named font	key => fdict
		Fonts: Times, Helvetica, Courier, Symbol
		Families: Roman, Italic,Bold and so on.
		Faces: Extended, Condensed, Obliqued.

 scalefont	Return new scaled font			n => fdict
		dictionary fdict
 setfont	Set current font			fdict => -
 show		Print str on the current page		str => -
 stringwidth	Returns width of str			str => x y

	CONTROL Operators
	~~~~~~~~~~~~~~~~~
 repeat		Repeat "proc" n-times			n proc => -
 exit		Exit innermost for,loop or repeat	- => -
 for		For i=j to l step k do proc		j k l proc => -
 if		If bool is true, then do proc		bool proc => -
 ifelse		If b is true do proct else procf	b proct procf => -
 loop		Repeat proc for ever			proc => -



	COORDINATE System Operators
	~~~~~~~~~~~~~~~~~~~~~~~~~~~
 rotate		Rotate user space "angle" deg.		angle => -
		counter-clockwise about origin
 scale		Scale user space by sx horizontally	sx sy => -
		and by sy vertically
 translate	Move origin of user space to (tx,ty)	tx ty => -


	GRAPHICS State Operators
	~~~~~~~~~~~~~~~~~~~~~~~~
 grestore	Restore graphics state from		- => -
		matching "gsave"
 gsave		Save current graphics state		- => -


	OUTPUT Operators
	~~~~~~~~~~~~~~~~
 showpath	Transfer the current page to the output device
*********************************************************/

/* ============================================================== */
gsinit: procedure
	parse arg pipefile,device,opts
	"mkfifo" pipefile
	call value "_pipefile",pipefile,0
	gsoptions  = "-dNOPLATFONTS -sDEVICE="device" -dQUIET -dNOPAUSE -dSAFER"
	"gs" opts gsoptions "- <" pipefile "&"
return psinit(pipefile,,"w+")

psinit: procedure
	parse arg filename,comment
	if arg(3,"E") then
		_fps = open(filename,arg(3))
	else
		_fps = open(filename,"w")
	call value "_fps",_fps,0
	call lineout _fps,"%!PS-Adobe-2.0"
	call lineout _fps,"%%Title:" comment
	call lineout _fps,"%%EndComments"
	call lineout _fps,"/inch {72 mul} def"
	call lineout _fps,"/cm {"||(72/2.54) "mul} def"
	call lineout _fps,"/mm {"||(72/25.4) "mul} def"
	call lineout _fps,"/Helvetica {/Helvetica findfont 12 scalefont} def"
	call lineout _fps,"/Times {/Times findfont 12 scalefont} def"
	call lineout _fps,"/Courier {/Courier findfont 12 scalefont} def"
	call lineout _fps,"/popn {{pop} repeat} def"
	call flush _fps
return _fps

gsclose: procedure
	_fps = value("_fps",,0)
	call lineout _fps,"quit"
	call flush _fps
	call close _fps
	"rm" value("_pipefile",,0)
return

psclose: procedure
	_fps = value("_fps",,0)
	call lineout _fps,"showpage"
	call lineout _fps,"%%EOF"
	call close _fps
return

psfont:
	call ps "findfont",arg(1)
	call ps "scalefont",arg(2)
	call ps "setfont"
return

psgray:
	call ps "setgray",arg(1)
return

psrgb:
	call ps "setrgbcolor",arg(1),arg(2),arg(3)
return

psflush:
	call lineout value("_fps",,0)
	call flush value("_fps",,0)
return

/*** General command ***/
ps: procedure
	cmd = ""
	do i=2 to arg()
		cmd = cmd arg(i)
	end
	cmd = strip(cmd arg(1))
	call lineout value("_fps",,0),cmd
return

psshow:
	call ps "show","("changestr("(",changestr(")",arg(1),"\)"),"\(")")"
return

psmove:
	call ps "moveto",arg(1),arg(2)
return

/*** pscircle(x,y,r) ***/
pscircle: procedure
	_fps = value("_fps",,0)
	call lineout _fps,"newpath"
	call lineout _fps,arg(1) arg(2) arg(3) 0 360 "arc"
	call lineout _fps,"stroke"
return

/*** psline ***/
psline: procedure
	_fps = value("_fps",,0)
	call lineout _fps,"newpath"
	call lineout _fps,arg(1) arg(2) "moveto"
	call lineout _fps,arg(3) arg(4) "lineto"
	call lineout _fps,"stroke"
return
