/* Standard Rexx Format */
RexxFormat: procedure
	/*
	call CheckArgs, 'rNUM oWHOLE>=0 oWHOLE>=0 oWHOLE>=0 oWHOLE>=0'
	*/
	if arg(2,"E") then Before = arg(2)
	if arg(3,"E") then After = arg(3)
	if arg(4,"E") then Expp = arg(4)
	if arg(5,"E") then Expt = arg(5)

	/* In the simplest case the first is the only argument. */
	Number=arg(1)
	if arg() < 2 then return Number

	/* Dissect the Number. It is in the normal Rexx format. */
	parse upper var Number Mantissa 'E' Exponent
	if Exponent == '' then Exponent = 0

	Sign = 0
	if left(Mantissa,1) == '-' then do
		Sign = 1
		Mantissa = substr(Mantissa,2)
	end

	parse var Mantissa Befo '.' Afte
	/* Count from the left for the decimal point. */
	Point = length(Befo)
	/* Sign Mantissa and Exponent now reflect the Number.
	   Befo Afte and Point reflect Mantissa. */

	/* The fourth and fifth arguments allow for exponential notation. */
	/* Decide whether exponential form to be used, setting ShowExp. */
	ShowExp = 0
	if arg(4,"E") | arg(5,"E") then do
		if ^arg(5,"E") then Expt = Digits()

		/* Decide whether exponential form to be used. */
		if (Point + Exponent) > Expt then ShowExp = 1

		/* Digits before rule. */
		LeftOfPoint = 0

		if length(Befo) > 0 then LeftOfPoint = Befo /* Value left of the point */

		/* Digits after point rule for exponentiation: */
		/* Count zeros to right of point. */
		z = 0
		do while substr(Afte,z+1,1) == '0'
			z = z + 1
		end

		if LeftOfPoint = 0 & (z - Exponent) > 5 then ShowExp = 1

		/* An extra rule for exponential form: */
		if arg(4,"E") then
			if Expp = 0 then ShowExp = 0

		/* Construct the exponential part of the result. */
		if ShowExp then do
			Exponent = Exponent + ( Point - 1 ) Point = 1
			/* As required for 'SCIENTIFIC' */
			if form() == 'ENGINEERING' then
				do while Exponent//3 ^= 0
					Point = Point+1
					Exponent = Exponent-1
				end
		end

		if ^ShowExp then Point = Point + Exponent
	end /* Expp or Expt given */
	else do
		/* Even if Expp and Expt are not given, exponential notation
		will be used if the original Number+0 done by CheckArgs led to it. */
		if Exponent ^= 0 then do
			ShowExp = 1
		end
	end

	/* ShowExp now indicates whether to show an exponent,
	Exponent is its value. */

	/* Make this a Number without a point. */
	Integer = Befo||Afte

	/* Make sure Point position isn't disjoint from Integer. */
	if Point<1 then do /* Extra zeros on the left. */
		Integer = copies('0',1 - Point) || Integer
		Point = 1
	end

	/* And maybe on the right. */
	if Point > length(Integer) then Integer = left(Integer,Point,'0')

	/* Deal with right of decimal point first since that can affect the left.
	Ensure the requested number of digits there. */
	Afters = length(Integer)-Point
	if arg(3,"E") = 0 then After = Afters /* Note default. */

	/* Make Afters match the requested After */
	do while Afters < After
		Afters = Afters+1
		Integer = Integer'0'
	end

	if Afters > After then do
		/* Round by adding 5 at the right place. */
		r=substr(Integer, Point + After + 1, 1)
		Integer = left(Integer, Point + After)

		if r >= '5' then Integer = Integer + 1

		/* This can leave the result zero. */
		If Integer = 0 then Sign = 0


		/* The case when rounding makes the integer longer is
		   an awkward one. The exponent will have to be adjusted. */
		if length(Integer) < Point + After then Integer=right(Integer,Point+After,"0")
		else
		if length(Integer) > Point + After then Point = Point+1

		if ShowExp = 1 then do
			Exponent=Exponent + (Point - 1)
			Point = 1
			/* As required for 'SCIENTIFIC' */
			if form() = 'ENGINEERING' then
				do while Exponent//3 ^= 0
					Point = Point+1
					Exponent = Exponent-1
				end
		end
		t = Point-length(Integer)
		if t > 0 then Integer = Integer||copies('0',t)
	end /* Rounded */

	/* Right part is final now. */
	if After > 0 then
		Afte = '.'||substr(Integer,Point+1,After)
	else
		Afte = ''

	/* Now deal with the integer part of the result. */
	Integer = left(Integer,Point)
	if arg(2,"E") = 0 then Before = Point + Sign /* Note default. */

	/* Make Point match Before */
	if Point > Before - Sign then call Raise 40.38, 2, arg(1)
	do while Point<Before
		Point = Point+1
		Integer = '0'Integer
	end

	/* Find the Sign position and blank leading zeroes. */
	r = ''
	Triggered = 0
	do j = 1 to length(Integer)
		Digit = substr(Integer,j,1)
		/* Triggered is set when sign inserted or blanking finished. */
		if Triggered = 1 then do
			r = r||Digit
			iterate
		end
		/* If before sign insertion point then blank out zero. */
		if Digit = '0' then
			if substr(Integer,j+1,1) = '0' & j+1<length(Integer) then do
				r = r||' '
				iterate
			end

		/* j is the sign insertion point. */
		if Digit = '0' & j ^= length(Integer) then Digit = ' '
		if Sign = 1 then Digit = '-'
		r = r||Digit
		Triggered = 1
	end j

	Number = r||Afte
	if ShowExp = 1 then do /* Format the exponent. */
		Expart = ''
		SignExp = 0
		if Exponent<0 then do
			SignExp = 1
			Exponent = -Exponent
		end

		/* Make the exponent to the requested width. */
		if arg(4,"E") = 0 then Expp = length(Exponent)
		if length(Exponent) > Expp then call Raise 40.38, 4, arg(1)
		Exponent=right(Exponent,Expp,'0')
		if Exponent = 0 then do
			if arg(4,"E") then Expart = copies(' ',expp+2)
		end
		else if SignExp = 0 then
			Expart = 'E+'Exponent
		else
			Expart = 'E-'Exponent

		Number = Number||Expart
	end
return Number
