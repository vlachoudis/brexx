/* interrupt test */

/* print msg via int 21, ah = 9 */
msg = "Hello world" || "0A0D"x || "$"
seg = d2x(addr('msg') % 16)
ofs = d2x(addr('msg') // 16)
regs = intr( x2d("21"), "ax=0900 dx="ofs "ds="seg )
say regs