**********************************************************************
*                                                                    *
*  THIS PROGRAM WRITTEN BY DAVE WADE.                                *
*  RELEASED TO THE PUBLIC DOMAIN                                     *
*                                                                    *
**********************************************************************
*                                                                    *
**********************************************************************
*                                                                    *
*  DIAG24 - RETURNS THE TERMINAL  LINE LENGTH                        *
*                                                                    *
**********************************************************************
         COPY  PDPTOP
         REGEQU
         ENTRY DIAG24
DIAG24   EQU   *
         SAVE  (14,12),,DIAG24
         LR    R12,R15
         USING DIAG24,R12
*
         L     R2,=F'-1'
         DC    X'83',X'24',XL2'0024'
*
         N     R5,=X'000000FF'
         LR    R15,R5
*
         RETURN (14,12),RC=(15)
         LTORG
**********************************************************************
*                                                                    *
*  GETUSR(CHAR USER[8]) - GET CURRENT USERID                         *
*                                                                    *
**********************************************************************
         ENTRY GETUSR
GETUSR   EQU   *
         SAVE  (14,12),,GETUSR
         LR    R12,R15
         USING GETUSR,R12
*
         L     R9,0(R1)          SAVE ADDRESS OF USER ID
         LA    R2,BUFFER
         L     R3,=F'40'
         DC    X'83',X'23',XL2'0000'
*       LINEDIT  TEXT='SYSTEM=........ VERSION=.. LEVEL=.. PLC=..',    X
               SUB=(CHAR8A,BUFFER,HEXA,VMVER,HEXA,VMLVL,               X
               HEXA,VMPLC),DOT=NO
*       LINEDIT  TEXT='CPUID=.. MECL=.... CPU ADDRESS=..',             X
               SUB=(HEXA,CPUID,HEXA,MECL,HEXA,CPUAD),DOT=NO
*       LINEDIT  TEXT='USERID =(.........)',SUB=(CHAR8A,USERID),DOT=NO
         MVC   0(8,R9),USERID
         LR    R15,R5
*
         RETURN (14,12),RC=(15)
* BUFFERS
BUFFER   DS    0D
OPSYS    DS    1F
VMVER    DS    1X
VMLVL    DS    1X
VMPLC    DS    1X
CPUID    DS    1X
MECL     DS    2X
CPUAD    DS    2X
PPFLAG   DS    CL4
USERID   DS    CL8
         DS    CL240
         LTORG
**********************************************************************
*                                                                    *
*  REXCLOCK - RETURNS TIME AS SECS & MILLISECS                       *
*                                                                    *
**********************************************************************
         ENTRY REXCLOCK
REXCLOCK EQU   *
         SAVE  (14,12),,REXCLOCK
         LR    R12,R15
         USING REXCLOCK,R12
*
         L     R2,0(R1)
         STCK  0(R2)
         L     R4,0(R2)
         L     R5,4(R2)
         SRDL  R4,12
         SL    R4,=X'0007D910'
         ST    R5,4(R2)  * SAVE MICRO SECONDS IN SECOND WORD
         D     R4,=F'1000000'
         SL    R5,=F'1220'
         ST    R5,0(R2)     * SAVE SECONDS IN FIRST WORD OF ARGS
         LR    R15,R5
*
         RETURN (14,12),RC=(15)
*
**********************************************************************
*                                                                    *
*  DIAG60 - RETURNS THE VIRTUAL MEMORY SIZE                          *
*                                                                    *
**********************************************************************
         ENTRY DIAG60
DIAG60   EQU   *
         SAVE  (14,12),,DIAG60
         LR    R12,R15
         USING DIAG60,R12
*
         DC    X'83',X'24',XL2'0060'
*
         LR    R15,R2
*
         RETURN (14,12),RC=(15)
         LTORG
         END
