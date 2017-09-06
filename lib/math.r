/* extended math library */
C  = 299792458.0		/* m/s */
h  = 6.6260755e-34		/* J s */
pi = 3.14159265358979323846
qe = 1.6021773349e-19		/* C */
me = 0.51099906			/* MeV */
mp = 938.27231			/* MeV */
mn = 939.56563			/* MeV */
fwhm = 2.355
na = 6.022e23			/* Avogadro */

in = 25.4			/* mm */
nm = 1852			/* m */
ft = 0.3048			/* m */
return

d2r: return arg(1)/180*PI
r2d: return arg(1)/PI*180

sind: return sin(d2r(arg(1)))
cosd: return cos(d2r(arg(1)))
tand: return tan(d2r(arg(1)))

asind: return r2d(asin(arg(1)))
acosd: return r2d(acos(arg(1)))
atand: return r2d(atan(arg(1)))

b2g: return 1/sqrt(1-arg(1)**2)
g2b: return sqrt(1-1/arg(1)**2)
