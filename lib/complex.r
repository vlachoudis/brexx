/*
 * $Id: complex.r,v 1.1 2011/06/29 08:33:29 bnv Exp bnv $
 * $Log: complex.r,v $
 * Revision 1.1  2011/06/29 08:33:29  bnv
 * Initial revision
 *
 *
 * Compex number library for BREXX
 * Each complex number is defined as a string  v=re im,
 */
return arg(1) arg(2)

i: return "0 1"

/* --- Re/Im --- */
Re: return word(arg(1),1)
Im: procedure
	parse arg re im
	if im="" then return 0
return im

/* --- Operations --- */
CAdd: procedure
	parse arg reA imA, reB imB
	if imA="" then imA=0
	if imB="" then imB=0
return (reA+reB) (imA+imB)

CSub: procedure
	parse arg reA imA, reB imB
	if imA="" then imA=0
	if imB="" then imB=0
return (reA-reB) (imA-imB)

CMult: procedure
	parse arg reA imA, reB imB
	if imA="" then imA=0
	if imB="" then imB=0
return (reA*reB-imA*imB) (reA*imB+imA*reB)

CDiv: procedure
	parse arg reA imA, reB imB
	if imA="" then imA=0
	if imB="" then imB=0
	len2 = reB**2 + imB**2
return (reA*reB+imA*imB)/len2 (imA*reB-reA*imB)/len2

CNeg: procedure
	parse arg re im
	if im="" then im=0
return (-re) (-im)

CAbs: procedure
	parse arg re im
	if im="" then im=0
return sqrt(re**2 + im**2)

CExp: procedure
	parse arg re im
	if im="" then im=0
	e = exp(re)
return (e*cos(im)) (e*sin(im))

CArg: procedure
	parse arg re im
	if im="" then im=0
return atan2(im,re)

CSign: procedure
	parse arg re im
	if im="" then im=0
	len = sqrt(re**2 + im**2)
return re/len im/len

Conj: procedure
	parse arg re im
	if im="" then im=0
return re (-im)

/* --- CFormat --- */
CFormat: procedure
	parse arg re im,before,after
	str = format(re,before,after)

	if im="" then im = 0

	if im<0 then do
		str = str"-i*"
		im = -im
	end; else
		str = str"+i*"

return str||format(im,before,after)
