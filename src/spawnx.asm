;-----------------------------------------------------------------
; $Id$
;      SPAWNX.ASM -- Execute a program w/o Overlay
;-----------------------------------------------------------------
_TEXT	SEGMENT	BYTE PUBLIC 'CODE'
	ASSUME	CS:_TEXT
_TEXT	ENDS
_DATA	SEGMENT	WORD PUBLIC 'DATA'
_DATA	ENDS

	ASSUME	CS:_TEXT, DS:_DATA

;	External References
        EXTRN   __psp : WORD
_psp@   equ     __psp

	SUBTTL  _SpawnX Function
	PAGE
;*-----------------------------------------------------
;* int far _spawnx(pathP,cmdP,envP);
;*     char far *pathP;
;*     char far *cmdP;
;*     char far *envP;
;*
;*-----------------------------------------------------

pathP		equ	6
cmdP		equ	pathP + 4
envP		equ	cmdP + 4

Fcb2Ptr		equ	4
Fcb1Ptr		equ	Fcb2Ptr + 4
CmdPtr		equ	Fcb1Ptr + 4
EnvSeg		equ	CmdPtr + 2
Fcb2		equ	EnvSeg + 16
Fcb1		equ	Fcb2 + 16

_TEXT	SEGMENT BYTE PUBLIC 'CODE'
	ASSUME	CS:_TEXT

SaveSP		dw	?
SaveSS		dw	?

		public	__spawnx
__spawnx	proc	far
		push	bp
		mov	bp, sp
		sub	sp, Fcb1
		push	si
		push	ds
		push	di
		push	es

	;/* All we need to access with ES is in the stack */

		push	ss
		pop	es

	;/* Use envP and make with it a segment address */

		mov	ax, [bp+envP]
		mov	dx, [bp+envP+2]

		mov	cl, 4
		shr	ax, cl
		add	ax, dx
		mov	[bp-EnvSeg], ax

	;/* Parse the command line for FCBs */

		push	ds
		lds	si, [bp+cmdP]
		mov	[bp-CmdPtr], si
		mov	[bp-CmdPtr+2], ds
		mov	ax, 2901h
		lea	di, [bp-Fcb1]
		mov	[bp-Fcb1Ptr], di
		mov	[bp-Fcb1Ptr+2], es
		inc	si
		int	21h		;Build FCB 1 in PSP

FindWhite	label	near
		mov	al,[si]
		cmp	al,20h		; Space
		je	NextFCB
		cmp	al,09h		; Tab
		je	NextFCB
		cmp	al,0dh		; Carriage Return
		je	NextFCB
		inc	si
		jmp	FindWhite

NextFCB		label	near
		mov	ax, 2901h
		lea	di, [bp-Fcb2]
		mov	[bp-Fcb2Ptr], di
		mov	[bp-Fcb2Ptr+2], es
		int	21h		;Build FCB 2 in PSP
		pop	ds

	;/* DOS 2.xx likes DS to point to PSP, so copy pathP to PSP:80 which
	;/* allows us to move DS to point to the PSP

		push	ds
		push	es
		mov	ax, seg _psp@	; Get segment of _psp variable
		mov	ds, ax		; Setup DS to point to it
		mov	es, _psp@	; Get PSP segment
		lds	si, [bp+pathP]	; Pathname of program to load
		mov	di, 80h		; Point to argument area
		mov	cx, 7Fh		; Limit length so we don't trash things
		cld			; Forward copy
CopyPathP:	lodsb			; Get a char
		test	al, al		; End of string yet ?
		jz	CopyPathPDone	; Yes
		stosb			; Nope, copy character
		loop	CopyPathP	; Next character
		mov	al, 0		; Place null at end of string
CopyPathPDone:	stosb
		pop	es
		pop	ds

	;/* Load and Execute the program */

		push	bp		; Save our context
		push	ds
		lea	bx, [bp-EnvSeg]	; Load descriptor
		mov	ax, seg _psp@	; Get segment of _psp variable
		mov	ds, ax
		mov	ds, _psp@	; Pathname is in the PSP now
		mov	dx, 80h
		mov	ax, 4B00h	; Load and Execute a program
		push	word ptr ss:[02Eh] ; DOS 2.xx destroy these 2 words
		push	word ptr ss:[030h]
		mov	SaveSS, ss	; SS and SP also
		mov	SaveSP, sp
		int	21h
		cli
		mov	ss, SaveSS	; Restore our stack pointer
		mov	sp, SaveSP
		sti
		pop	word ptr ss:[030h]
		pop	word ptr ss:[02Eh]
		pop	ds
		pop	bp

	;/* If no error, get child exit code */

		jnc	SpawnOk
		push	ax
		jmp	short SpawnExit
SpawnOk		label	near
		mov	ah, 4dh
		int	21h
SpawnExit	label	near
		pop	es
		pop	di
		pop	ds
		pop	si
		mov     sp, bp
		pop	bp
		ret
		endp
_TEXT	ENDS
	END
