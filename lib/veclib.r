/*
 * $Id: veclib.r,v 1.1 2011/06/29 08:33:29 bnv Exp bnv $
 * $Log: veclib.r,v $
 * Revision 1.1  2011/06/29 08:33:29  bnv
 * Initial revision
 *
 *
 * Vector library for BREXX
 * Each vector is defined as a tree number string  v=x y z,
 *  ie.  v=0 0 0, is the origin
 * Call first the Vec_Init to initialize some basic vectors
 * Array and vector index are ZERO based
 */
return

/* --- Initialise Vectors --- */
VecInit:
	VecO  = 0. 0. 0.
	VecXo = 1. 0. 0.
	VecYo = 0. 1. 0.
	VecZo = 0. 0. 1.
return

/* --- Return Vectors components --- */
Vec:	return word(arg(1),arg(2)+1)
VecX:	return word(arg(1),1)
VecY:	return word(arg(1),2)
VecZ:	return word(arg(1),3)

/* --- Useless function --- */
VecMake: return	arg(1) arg(2) arg(3)

/* --- Negate a Vector (-V) --- */
VecNeg: procedure
	parse arg x y z
return	(-x) (-y) (-z)

/* --- Dot product of 2 vectors (A.B) --- */
VecDot: procedure
	parse arg Ax Ay Az, Bx By Bz
	Ax=Ax+0.0
	Ay=Ay+0.0
	Az=Az+0.0
return	Ax*Bx + Ay*By + Az*Bz

/* --- Length of a vector --- */
VecLen: procedure
	parse arg x y z
	x = x+0.0
	y = y+0.0
	z = z+0.0
return	sqrt(0.0 + x*x + y*y + z*z)

/* --- Normalise a vector --- */
VecNorm: procedure
	parse arg x y z
	x = x+0.0
	y = y+0.0
	z = z+0.0
	invlen = 1/sqrt(x*x + y*y + z*z)
return	invlen*x   invlen*y   invlen*z

/* --- Random Vector --- */
VecRnd: procedure
	max = 32000
	z = 2*(random(0,max)/max-0.5)
	atr = sqrt(1-z*z)
	phi = 6.283185*random(0,max)/max
	x = atr * cos(phi)
	y = atr * sin(phi)
return x y z

/* --- Add two vectors (A+B) --- */
VecAdd: procedure
	parse arg Ax Ay Az, Bx By Bz
return	Ax+Bx    Ay+By    Az+Bz

/* --- Subtract two vectors (A-B) --- */
VecSub: procedure
	parse arg Ax Ay Az, Bx By Bz
return	Ax-Bx    Ay-By    Az-Bz

/* --- Combine two vectors (a*A+b*B) --- */
VecComb: procedure
	parse arg af, Ax Ay Az, bf, Bx By Bz
	Ax=Ax+0.0
	Ay=Ay+0.0
	Az=Az+0.0
return	af*Ax+bf*Bx    af*Ay+bf*By    af*Az+bf*Bz

/* --- Scale a vector (s*A) --- */
VecS: procedure
	parse arg s, x y z
return	s*x   s*y   s*z

/* --- Scale and Add two vector (s*A+B) --- */
VecAddS: procedure
	parse arg s,  Ax Ay Az,  Bx By Bz
	s=s+0.0
return	s*Ax+Bx    s*Ay+By    s*Az+Bz

/* --- Multiply two vector (A*B) --- */
VecMult: procedure
	parse arg Ax Ay Az, Bx By Bz
return	(Ax+0.0)*Bx    (Ay+0.0)*By    (Az+0.0)*Bz

/* --- Cross product of two vector (AxB) --- */
VecCross: procedure
	parse arg Ax Ay Az, Bx By Bz
	Ax=Ax+0.0
	Ay=Ay+0.0
	Az=Az+0.0
return	Ay*Bz-Az*By    Az*Bx-Ax*Bz     Ax*By-Ay*Bx

/* --- Format a Vector --- */
VecFormat: procedure
	parse arg x y z,b,a
return format(x,b,a) format(y,b,a) format(z,b,a)

/* --- Create a 4x4 transformation matrix from 3 vectors (everything in a row) --- */
MatMake: return arg(1) 0 arg(2) 0 arg(3) 0 0 0 0 1

/* --- Return Matrix item Mat(row,col) --- */
Mat: return word(arg(1),arg(2)*4+arg(3)+1)

/* --- Set a Matrix item SetMat(row,col) --- */
SetMat: procedure
	parse arg mat,row,col,val
	pos = row*4+col
	if pos>0	then before = subword(mat,1,pos)
			else before = ""
	after = subword(mat,pos+2)
return before val after

/* --- Unary Matrix --- */
MatUnary: procedure
	m = ""
	do j=0 to 3
		do i=0 to 3
			m = m (i==j)
		end
	end
return m

/* --- Zero Matrix --- */
MatZero: return copies("0 ",16)

/* --- Vectror multiplied with the transformation vatrix --- */
MatXVec: procedure
	parse arg m,x y z
	nx = x*Mat(m,0,0) + y*Mat(m,0,1) + z*Mat(m,0,2) + Mat(m,0,3)
	ny = x*Mat(m,1,0) + y*Mat(m,1,1) + z*Mat(m,1,2) + Mat(m,1,3)
	nz = x*Mat(m,2,0) + y*Mat(m,2,1) + z*Mat(m,2,2) + Mat(m,2,3)
return nx ny nz

VecXMat: return MatXVec(arg(2),arg(1))

/* --- Matrix Transform --- */
MatT: procedure
	parse arg m
	mt = ""
	do j=0 to 3
		do i=0 to 3
			mt = mt Mat(m,i,j)
		end
	end
return mt

/* --- Create a Rotation Matrix, axis can be "x y z" and angle in radian --- */
MatRot: procedure
	parse arg axis,angle
	upper axis
	ax = wordpos(axis,"X Y Z")
	if axis=0 then do
		say "ERROR MatRot: Invalid Axis" axis
		return MatUnary()
	end

	mat = MatUnary()
        m1 = (ax//3)+1
        m2 = m1//3
	m1 = m1 - 1
        c = cos(angle)
        s = sin(angle)

	mat = SetMat(mat,m1,m1,c)
	mat = SetMat(mat,m2,m2,c)
	mat = SetMat(mat,m2,m1,s)
	mat = SetMat(mat,m1,m2,-s)
return mat

/* --- Create a translation matrix --- */
MatTrans: procedure
	parse arg dx, dy, dz
	if dy="" then parse var dx dx dy dz
	mat = MatUnary()
	mat = SetMat(mat,0,3,dx)
	mat = SetMat(mat,1,3,dy)
	mat = SetMat(mat,2,3,dz)
return mat

/* --- Scale Matrix --- */
MatScale: procedure
	parse arg sx, sy, sz
	if sy="" then parse var sx sx sy sz
	if sy="" then sy=sx
	if sz="" then sz=sx
	mat = MatUnary()
	mat = SetMat(mat,0,0,sx)
	mat = SetMat(mat,1,1,sy)
	mat = SetMat(mat,2,2,sz)
return mat

/* --- MatMult --- */
MatMult: procedure
	parse arg a,b
	m = ""
	do row=0 to 3
		do col=0 to 3
			sum = 0
			do k=0 to 3
				sum = sum + Mat(a,row,k)*Mat(b,k,col)
			end
			m = m sum
		end
	end
return m

/* --- Print Matrix --- */
MatPrint: procedure
	parse arg m,prefix,b,a,suffix
	do i=0 to 3
		line = ""
		do j=0 to 3
			line = line format(Mat(m,i,j),b,a)
		end
		say prefix||line||suffix
	end
return

/* --- MatInv --- */
MatInv: procedure
	mat = _ludcmp(arg(1))	/* matrix lu decomposition */
	y = MatZero()
	do j=0 to 3		/* matrix inversion */
		do i=0 to 3
			col.i = (j=i)
		end
		call _lubksb mat
		do i=0 to 3
			y = SetMat(y,i,j,col.i)
		end
	end
return y

/****************************************************************
 * _ludcmp(Mat4 a, int *indx, Flt *d)
 * LU decomposition.
 * Parameteres
 *      Mat4    a               input matrix. gets thrashed
 *      int     *indx           row permutation record
 *      Flt     *d              +/- 1.0 (even or odd # of row interchanges
 *****************************************************************/
_ludcmp: procedure expose d indx.
	parse arg a

	d = 1.0
	do i=0 to 3
		big = 0
		do j=0 to 3
			big = max(abs(Mat(a,i,j)),big)
		end
		if big = 0 then do
			say "ERROR _ludcmp(): singular matrix found..."
			exit
		end
		vv.i = 1.0/big
	end

	do j=0 to 3
		do i=0 to j-1
			sum = Mat(a,i,j)
			do k=0 to i-1
				sum = sum - Mat(a,i,k) * Mat(a,k,j)
			end
			a = SetMat(a,i,j,sum)
		end
		big = 0
		do i=j to 3
			sum = Mat(a,i,j)
			do k=0 to j-1
				sum = sum - Mat(a,i,k) * Mat(a,k,j)
			end
			a = SetMat(a,i,j,sum)
			dum = vv.i*abs(sum)
			if dum >= big then do
				big = dum
				imax = i
			end
		end
		if j ^= imax then do
			do k=0 to 3
				dum = Mat(a,imax,k)
				a = SetMat(a,imax,k, Mat(a,j,k))
				a = SetMat(a,j,k, dum)
			end
			d = -d
			vv.imax = vv.j
		end
		indx.j = imax
		if Mat(a,j,j) = 0.0 then a = SetMat(a,j,j, 1E-20)
		if j ^= 3 then do
			dum = 1/Mat(a,j,j)
			do i=j+1 to 3
				a = SetMat(a,i,j, Mat(a,i,j)*dum)
			end
		end
	end
return a

/****************************************************************
 * _lubksb(Mat4 a, int *indx, Flt b[])
 * backward substitution
 *      Mat4    a       input matrix
 *      int     *indx   row permutation record
 *      Flt     col     right hand vector (?)
 *****************************************************************/
_lubksb: procedure expose col. indx.
	parse arg a

	ii = -1

	do i=0 to 3
		ip = indx.i
		sum = col.ip
		col.ip = col.i
		if ii >= 0 then
			do j=ii to i-1
				sum = sum - Mat(a,i,j) * col.j
			end
		else
		if sum ^= 0.0 then ii = i
		col.i = sum
	end
	do i=3 to 0 by -1
		sum = col.i
		do j=i+1 to 3
			sum = sum - Mat(a,i,j) * col.j
		end
		col.i = sum/Mat(a,i,i)
	end
return
