;-----------------------------------------------------------------
; $Id: int2e.asm,v 1.1 2009/02/02 09:25:52 bnv Exp $
;      INT2E.ASM -- Execute a DOS command via the undocument
;      interrupt 0x2e (Back door for the command.com)
;      Vasilis.Vlachoudis@cern.ch
;-----------------------------------------------------------------
            TITLE 'Interrupt 2E  back door for COMMAND.COM'
            NAME  INT2E
;
;   void far int2e(char far *cmd);
;
;   Caller Borland C
;
CMDoff      EQU  word ptr [bp+06h]          ; standard stack frame
CMDseg      EQU  word ptr [bp+08h]

_TEXT       SEGMENT byte public 'CODE'
            ASSUME cs:_TEXT,ds:_TEXT,es:_TEXT


            PUBLIC _int2e
_int2e      PROC far
            push bp                 ; preserve caller registers
            mov  bp,sp
            push ds
            push es

            push ax
            push bx
            push cx
            push dx
            push si
            push di

            mov  ax,CMDseg
            mov  ds,ax              ; DS -> segment of ASCIIZ with the cmd
            mov  dx,CMDoff          ; DX -> offset  of ASCIIZ
;
; Let's point our stack
;
            mov   ax,ss             ; Save old stack segment
            mov   cs:OldSS,ax       ;
            mov   cs:OldSP,sp       ; and stack pointer
            cli                     ; turn off interupts while switching
            mov   ax,cs
            mov   ss,ax             ; Stack segment
            mov   sp,offset STAK    ; point our stack
            sti                     ; enable interupts
;
; Copy command to the our area and found its length
;
	    mov   ax,ds
            mov   es,ax
            mov   di,dx
            mov   cx,0ffffh
            xor   ax,ax
            cld
      repne scasb                    ; Find ascii'Z' in es:di -> argument
            neg   cx
            sub   cx,2
            mov   cs:CMDLEN,cl
            mov   si,dx
	    mov   di,offset CMD
	    mov   ax,cs
	    mov   es,ax
	rep movsb                    ; move from DS:SI to ES:DI
	    xor   bx,bx
	    mov   bl,cs:CMDLEN
	    mov   cs:CMD[bx],0Dh
;
	    mov   ax,cs
	    mov   ds,ax
	    mov   si,offset CMDLEN
            int   2eh
;
;  Restore old stack
;
	    mov   ax,cs:OldSS
	    cli
	    mov   ss,ax             ; Stack segment
	    mov   sp,cs:OldSP       ; and Stack pointer
	    sti

            pop  di
            pop  si
            pop  dx
            pop  cx
            pop  bx
            pop  ax

            pop  es
            pop  ds                 ; restore caller registers
	    mov  sp,bp
            pop  bp
            ret


            db    32 DUP ('STACK___')     ; 512 bytes stack
STAK        label byte

OldSS       dw    ?
OldSP       dw    ?

CMDLEN      db    ?
CMD         db    130 DUP (00h)

_int2e      ENDP

_TEXT       ENDS
            END
