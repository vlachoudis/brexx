/* ----------------------------------------------BNV 1991 --- */
/* prototype to call asm int2e back door of command.com       */
/* int2e, executes a dos command without loading command.com  */
/* but uses the parent proccess command interpreter           */
/* ---------------------------------------------------------- */
extern void far int2e(char far *);
