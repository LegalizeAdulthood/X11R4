C***********************************************************************GKS00010
C***********************************************************************GKS00020
C*		Copyright IBM Corporation 1989
C*
C*                      All Rights Reserved
C*
C* Permission to use, copy, modify, and distribute this software and its
C* documentation for any purpose and without fee is hereby granted,
C* provided that the above copyright notice appear in all copies and that
C* both that copyright notice and this permission notice appear in
c* supporting documentation, and that the name of IBM not be
C* used in advertising or publicity pertaining to distribution of the
C* software without specific, written prior permission.
C*
C* IBM DISCLAIMS ALL WARRANTIES WITH REGARD TO THIS SOFTWARE, INCLUDING
C* ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS, IN NO EVENT SHALL
C* IBM BE LIABLE FOR ANY SPECIAL, INDIRECT OR CONSEQUENTIAL DAMAGES OR
C* ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS,
C* WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION,
C* ARISING OUT OF OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS
C* SOFTWARE.
C*
C*****                                                               ***GKS00030
C*****   Product:     graPHIGS GKS-CO Demonstration Program          ***GKS00040
C*****                                                               ***GKS00050
C*****   Module Name: gksdemo.f                                      ***GKS00060
C*****                                                               ***GKS00070
C*****   Module Type: FORTRAN                                        ***GKS00080
C*****                                                               ***GKS00090
C*****   Descriptive Name: GKS Demonstration Program                 ***GKS00100
C*****                                                               ***GKS00110
C*****   Module Function:  Demonstration package written to          ***GKS00120
C*****                     show the programming techniques of        ***GKS00130
C*****                     writing graphics applications using       ***GKS00140
C*****                     the device independent graphics GKS       ***GKS00150
C*****                     standard, via the graPHIGS GKS-CO         ***GKS00160
C*****                     implementation of that standard.          ***GKS00170
C*****                                                               ***GKS00180
C*****                                                               ***GKS00190
C*****                                                               ***GKS00200
C*****   Internal Flow:       GKSDEMO                                ***GKS00210
C*****     ___________________| | | |____________________            ***GKS00220
C*****     |             _______| |_______              |            ***GKS00230
C*****     |             |               |              |            ***GKS00240
C*****     |             |               |              |            ***GKS00250
C*****  MAPDEM         PRIMIT          COLOR          INTER          ***GKS00260
C*****         ________||||||________                  ||||_____     ***GKS00270
C*****         |        ||||        |                 / ||     |     ***GKS00280
C*****     DEMOPM  _____||||_____ DEMOGD             /  ||   INTDEL  ***GKS00290
C*****             |     ||     |                   /   ||____       ***GKS00300
C*****         DEMOPL ___||___ DEMOCA              /    |    |       ***GKS00310
C*****         _|     |      |                    /     |  INTTRA    ***GKS00320
C*****        |    DEMOTX   DEMOFA             INTCRE   |___         ***GKS00330
C*****    DRWHLS                ________________||||       |         ***GKS00340
C***** .....................    |      __________|||     INTINS      ***GKS00350
C***** .Common Subroutines .    |      |       ___||____             ***GKS00360
C***** . Used by Many:     .    |      |       |       |             ***GKS00370
C***** ..................... CREPLN  CREPMK  CREFA  CRETXT           ***GKS00380
C***** .  CRSEGM FINSHF    .     \        \   /       /              ***GKS00390
C***** .  RCHOI  RMENU     .      \________\ /_______/               ***GKS00400
C***** .  SETCOL SETUPF    .              CHCOL                      ***GKS00410
C***** .....................                                         ***GKS00420
C*****                                                               ***GKS00430
C*****                                                               ***GKS00440
C*****                                                               ***GKS00450
C*****   Entry Point: GKSDEM                                         ***GKS00460
C*****                                                               ***GKS00470
C*****   Subroutines:  CHCOL  - Allows User to choose a color        ***GKS00480
C*****                 COLOR  - Demonstrates the Color Facilities    ***GKS00490
C*****                 CREFA  - Create Fill Area in Segment          ***GKS00500
C*****                 CREPLN - Create Polyline in Segment           ***GKS00510
C*****                 CREPMK - Create Polymarker in Segment         ***GKS00520
C*****                 CRSEGM - Create Pickable Segment for RETURN   ***GKS00530
C*****                 CRETXT - Create Text in Segment               ***GKS00540
C*****                 DEMOCA - Demonstrates cell arrays             ***GKS00550
C*****                 DEMOFA - Demonstrates fill areas              ***GKS00560
C*****                 DEMOGD - Demonstrates GDPs Available          ***GKS00570
C*****                 DEMOPL - Demonstrates polylines               ***GKS00580
C*****                 DEMOPM - Demonstrates polymarkers             ***GKS00590
C*****                 DEMOTX - Demonstrates the text primitive      ***GKS00600
C*****                 DRWHLS - Draws the wheels for the car frame   ***GKS00610
C*****                 FINSHF - Finishes the screen frame borders    ***GKS00620
C*****                 INTCRE - Create Segment                       ***GKS00630
C*****                 INTDEL - Delete a Segment                     ***GKS00640
C*****                 INTER  - Main Interactive Demo Menu           ***GKS00650
C*****                 INTINS - Insert Segment(s) into New Segment   ***GKS00660
C*****                 INTTRA - Transform Segment Data               ***GKS00670
C*****                 MAPDEM - Draws high level picture of GKS-CO   ***GKS00680
C*****                 PRIMIT - Main output primitive menu           ***GKS00690
C*****                 RCHOI  - Waits on input from choice device    ***GKS00700
C*****                 RMENU  - Sets up choice prompts               ***GKS00710
C*****                 SETCOL - Sets up the color table              ***GKS00720
C*****                 SETUPF - Draws the screen frame borders       ***GKS00730
C*****                                                               ***GKS00740
C*****   Include Files:  gkspar.inc                                  ***GKS00750
C*****                                                               ***GKS00760
C*****   Environment Dependencies: IBM RT PC with lpfks, tablet,     ***GKS00770
C*****                             and dial devices attached.        ***GKS00780
C*****                                                               ***GKS00790
C***********************************************************************GKS00800
C***********************************************************************GKS00810
                                                                        GKS00820
      PROGRAM GKSDEM                                                    GKS00830
      INCLUDE 'gkspar.inc'
                                                                        GKS00850
C***********************************************************************GKS00860
C*****                                                               ***GKS00870
C*****   Declare all Variables                                       ***GKS00880
C*****                                                               ***GKS00890
C***********************************************************************GKS00900
                                                                        GKS00910
      INTEGER*4     ERRFIL,WKID,CONID,WTYPE,TNR                         GKS00920
      INTEGER*4     ERRIND,DCUNIT,LX,LY,NOERR                           GKS00930
      INTEGER*4     I,J,IA(32),MENLEN(5),SGNA                           GKS00940
      INTEGER*4     YES,NO,LDRCDU,LASP(13)                              GKS00950
                                                                        GKS00960
      REAL*4        RX,RY,STARTX,STARTY,PX(10),PY(10)                   GKS00970
      REAL*4        SELBLK(8),ARROW(14),TXTBLK(8),EMPTRP(1)             GKS00980
      REAL*4        TXTX,TXTY,XMIN,XMAX,YMIN,YMAX                       GKS00990
                                                                        GKS01000
      CHARACTER*1   CHAR                                                GKS01020
      CHARACTER*1   DUMMYA(10)                                          GKS01030
      CHARACTER*20  MENTXT(5)                                           GKS01040
      CHARACTER*80  DRCDUM(4)                                           GKS01050
                                                                        GKS01060
C***********************************************************************GKS01070
C*****                                                               ***GKS01080
C*****   Define COMMON block variables                               ***GKS01090
C*****                                                               ***GKS01100
C***********************************************************************GKS01110
                                                                        GKS01120
      COMMON        /WINF/ WKID,WTYPE                                   GKS01130
      COMMON        /LIMITS/ XMIN,XMAX,YMIN,YMAX                        GKS01140
      COMMON        /TEXT/ TXTX,TXTY                                    GKS01150
                                                                        GKS01160
C***********************************************************************GKS01170
C*****                                                               ***GKS01180
C*****   Initialize Variables                                        ***GKS01190
C*****                                                               ***GKS01200
C***********************************************************************GKS01210
                                                                        GKS01220
      DATA ERRFIL /0/                                                   GKS01230
      DATA WKID /1/                                                     GKS01240
      DATA CONID /1/                                                    GKS01250
      DATA WTYPE /4/                                                    GKS01260
      DATA TNR /1/                                                      GKS01270
      DATA LASP /1,1,1,1,1,1,1,1,1,1,1,1,1/                             GKS01280
      DATA NOERR /0/                                                    GKS01290
      DATA NO /0/                                                       GKS01300
      DATA YES /1/                                                      GKS01310
                                                                        GKS01320
C***********************************************************************GKS01330
C*****                                                               ***GKS01340
C*****  Set up the coordinates for the select, arrow and text        ***GKS01350
C*****  blocks.                                                      ***GKS01360
C*****                                                               ***GKS01370
C***********************************************************************GKS01380
                                                                        GKS01390
      DATA SELBLK /0.000,0.100,0.100,0.000,0.100,0.100,0.000,0.000/     GKS01400
      DATA ARROW  /0.000,0.140,0.140,0.185,0.140,0.140,0.000,           GKS01410
     *             0.016,0.016,0.000,0.024,0.048,0.032,0.032/           GKS01420
      DATA TXTBLK /0.000,0.535,0.535,0.000,0.100,0.100,0.000,0.000/     GKS01430
                                                                        GKS01440
C***********************************************************************GKS01450
C*****                                                               ***GKS01460
C*****  Text for the menu options and the length of each text        ***GKS01470
C*****  string.                                                      ***GKS01480
C*****                                                               ***GKS01490
C***********************************************************************GKS01500
                                                                        GKS01510
      DATA MENTXT(1)  /'XGKS'/,                                         GKS01520
     *     MENTXT(2)  /'PRIMITIVES'/,                                   GKS01530
     *     MENTXT(3)  /'COLOR'/,                                        GKS01540
     *     MENTXT(4)  /'INTERACTION'/,                                  GKS01550
     *     MENTXT(5)  /'END'/                                           GKS01560
      DATA MENLEN /4,10,5,11,3/                                         GKS01570
                                                                        GKS01580
      DATA EMPTRP /0.0/                                                 GKS01590
      DATA CHAR   /' '/                                                 GKS01600
                                                                        GKS01610
C***********************************************************************GKS01620
C*****                                                               ***GKS01630
C*****   Open GKS, open the workstation and activate it.             ***GKS01640
C*****   Set the deferral mode to 'bnil' which will produce a        ***GKS01650
C*****   visual effect of each function appearing before the next    ***GKS01660
C*****   interaction locally. Set the implicit regeneration mode     ***GKS01670
C*****   to suppressed.                                              ***GKS01680
C*****                                                               ***GKS01690
C***********************************************************************GKS01700
                                                                        GKS01710
      CALL GOPKS(ERRFIL,0)                                              GKS01720
      CALL GOPWK(WKID,CONID,WTYPE)                                      GKS01730
      CALL GACWK(WKID)                                                  GKS01740
      CALL GSDS(WKID,GBNIL,GSUPPD)                                      GKS01750
                                                                        GKS01760
C***********************************************************************GKS01770
C*****                                                               ***GKS01780
C*****   Set all the aspect source flags to individual.              ***GKS01790
C*****   Set the normalization transformation for transformation     ***GKS01800
C*****   number one.                                                 ***GKS01810
C*****                                                               ***GKS01820
C***********************************************************************GKS01830
                                                                        GKS01840
      CALL GSASF(LASP)                                                  GKS01850
      CALL GSWN(TNR,0.0,1.0,0.0,1.0)                                    GKS01860
      CALL GSVP(TNR,0.0,1.0,0.0,1.0)                                    GKS01870
                                                                        GKS01880
C***********************************************************************GKS01890
C*****                                                               ***GKS01900
C*****    Set the workstation transformation after inquiring         ***GKS01910
C*****    the display information.                                   ***GKS01920
C*****                                                               ***GKS01930
C***********************************************************************GKS01940
                                                                        GKS01950
      CALL GQDSP(WTYPE,ERRIND,DCUNIT,RX,RY,LX,LY)                       GKS01960
      XMIN = (RX - RY) / 2.0                                            GKS01970
      XMAX = XMIN + RY                                                  GKS01980
      YMIN = 0.000                                                      GKS01990
      YMAX = RY                                                         GKS02000
      CALL GSWKWN(WKID,0.0,1.0,0.0,1.0)                                 GKS02010
      CALL GSWKVP(WKID,XMIN,XMAX,YMIN,YMAX)                             GKS02020
      CALL GSELNT(TNR)                                                  GKS02030
                                                                        GKS02040
C***********************************************************************GKS02050
C*****                                                               ***GKS02060
C*****   Call Subroutine SETCOL to set up the color table for        ***GKS02070
C*****   this program.                                               ***GKS02080
C*****                                                               ***GKS02090
C***********************************************************************GKS02100
                                                                        GKS02110
      CALL SETCOL                                                       GKS02120
                                                                        GKS02130
C***********************************************************************GKS02140
C*****                                                               ***GKS02150
C*****   Build the main menu from which all paths are chosen.        ***GKS02160
C*****   Main loop: keep drawing the main menu until the user        ***GKS02170
C*****   chooses to exit the routine (by choosing the segment        ***GKS02180
C*****   'end' - segment 5) via the LPFK or pick device.             ***GKS02190
C*****                                                               ***GKS02200
C***********************************************************************GKS02210
                                                                        GKS02220
      ENDIT = NO                                                        GKS02230
                                                                        GKS02240
C*************************************************************          GKS02250
C*****                                                   *****          GKS02260
C*****   Call subroutine SETUPF to draw the menu         *****          GKS02270
C*****   border. Add the text to the menu and call       *****          GKS02280
C*****   subroutine FINSHF to complete the menu          *****          GKS02290
C*****   border.                                         *****          GKS02300
C*****                                                   *****          GKS02310
C*************************************************************          GKS02320
                                                                        GKS02330
   10 CALL SETUPF(.TRUE.,.FALSE.)                                       GKS02340
      CALL GSCHH(0.065)                                                 GKS02350
      CALL GSCHSP(0.2)                                                  GKS02450
      CALL GTXS(TXTX,0.835,9,'XGKS DEMO')                               GKS02360
      CALL FINSHF(.TRUE.)                                               GKS02370
                                                                        GKS02380
      CALL GSFAIS(GSOLID)                                               GKS02390
      CALL GSFACI(GYELOW)                                               GKS02400
      CALL GSTXFP(GFONT2,GCHARP)                                        GKS02410
      CALL GSTXCI(GRED)                                                 GKS02420
      CALL GSCHH(0.060)                                                 GKS02430
      CALL GSCHXP(0.9)                                                  GKS02440
      CALL GSCHSP(0.1)                                                  GKS02450
                                                                        GKS02460
C*************************************************************          GKS02470
C*****                                                   *****          GKS02480
C*****   Set all LPFKs to off (1).                       *****          GKS02490
C*****                                                   *****          GKS02500
C*************************************************************          GKS02510
                                                                        GKS02520
      DO 20 I=1,32                                                      GKS02530
        IA(I)=GPROFF                                                    GKS02540
   20 CONTINUE                                                          GKS02550
                                                                        GKS02560
C*************************************************************          GKS02570
C*****                                                   *****          GKS02580
C*****   For each of the 5 selectable segments,          *****          GKS02590
C*****   calculate the 4 points that indicate its        *****          GKS02600
C*****   boundary.                                       *****          GKS02610
C*****                                                   *****          GKS02620
C*************************************************************          GKS02630
                                                                        GKS02640
C     DO 60 I=0,4                                                       GKS02650
C       STARTX = 0.065                                                  GKS02660
C       STARTY = 0.590 - I * 0.130                                      GKS02670
C       DO 30 J=1,4                                                     GKS02680
C         PX(J) = SELBLK(J) + STARTX                                    GKS02690
C         PY(J) = SELBLK(J+4) + STARTY                                  GKS02700
C  30   CONTINUE                                                        GKS02710
                                                                        GKS02720
C*************************************************************          GKS02730
C*****                                                   *****          GKS02740
C*****  Display pickable choices on the main menu.       *****          GKS02750
C*****                                                   *****          GKS02760
C*************************************************************          GKS02770
                                                                        GKS02780
C       CALL GCRSG(I+1)                                                 GKS02790
C       CALL GFA(4,PX,PY)                                               GKS02800
C       WRITE(CH,'(I1)')I+1                                             GKS02810
C       CALL GSTXAL(GACENT,GAHALF)                                      GKS02820
C       CALL GTXS(STARTX+0.05,STARTY+0.05,1,CH)                         GKS02830
C       CALL GSTXAL(GAHNOR,GAVNOR)                                      GKS02840
                                                                        GKS02850
C*************************************************************          GKS02860
C*****                                                   *****          GKS02870
C*****   Set the LPFK for this segment to on (2).        *****          GKS02880
C*****   Close the segment and make it detectable.       *****          GKS02890
C*****                                                   *****          GKS02900
C*************************************************************          GKS02910
                                                                        GKS02920
C       IA(I+1) = GPRON                                                 GKS02930
C       CALL GCLSG                                                      GKS02940
C       CALL GSDTEC(I+1,1)                                              GKS02950
                                                                        GKS02960
C*************************************************************          GKS02970
C*****                                                   *****          GKS02980
C*****   Draw arrows on menu to indicate which LPFK      *****          GKS02990
C*****   goes with which choice.                         *****          GKS03000
C*****                                                   *****          GKS03010
C*************************************************************          GKS03020
                                                                        GKS03030
C       STARTX = 0.190                                                  GKS03040
C       STARTY = 0.606 - I * 0.130                                      GKS03050
C       DO 40 J = 1,7                                                   GKS03060
C         PX(J) = ARROW(J) + STARTX                                     GKS03070
C         PY(J) = ARROW(J+7) + STARTY                                   GKS03080
C  40   CONTINUE                                                        GKS03090
C       CALL GPL(7,PX,PY)                                               GKS03100
C       STARTX = 0.400                                                  GKS03110
C       STARTY = 0.590 - I * 0.130                                      GKS03120
C       DO 50 J=1,4                                                     GKS03130
C         PX(J) = TXTBLK(J) + STARTX                                    GKS03140
C         PY(J) = TXTBLK(J+4) + STARTY                                  GKS03150
C  50   CONTINUE                                                        GKS03160
C                                                                       GKS03170
C*************************************************************          GKS03180
C*****                                                   *****          GKS03190
C*****   Indicate choices for each segment as follows:   *****          GKS03200
C*****      1 -> GKS-CO                                  *****          GKS03210
C*****      2 -> Primitives                              *****          GKS03220
C*****      3 -> Color                                   *****          GKS03230
C*****      4 -> Interaction                             *****          GKS03240
C*****      5 -> End                                     *****          GKS03250
C*****                                                   *****          GKS03260
C*************************************************************          GKS03270
                                                                        GKS03280
C       CALL GFA(4,PX,PY)                                               GKS03290
C       STARTX = STARTX + 0.015                                         GKS03300
C       STARTY = STARTY + 0.020                                         GKS03310
C       CALL GTXS(STARTX,STARTY,MENLEN(I+1),MENTXT(I+1))                GKS03320
C  60 CONTINUE                                                          GKS03330
C                                                                       GKS03340
C*************************************************************          GKS03350
C*****                                                   *****          GKS03360
C*****   Initialize both the choice (lpfks) and the      *****          GKS03370
C*****   pick (tablet) input types. Set both devices     *****          GKS03380
C*****   in input mode and put out a message to tell     *****          GKS03390
C*****   the user to select an item.                     *****          GKS03400
C*****                                                   *****          GKS03410
C*************************************************************          GKS03420
                                                                        GKS03430
      CALL GSTXFP(GFONT1,GSTRKP)                                        GKS03440
      CALL GSCHSP(0.10)                                                 GKS03450
      CALL GPREC(0,IA,0,EMPTRP,0,DUMMYA,CHAR,4,ERRIND,LDRCDU,DRCDUM)    GKS03460
      CALL GINPK(WKID,1,1,1,1,1,XMIN,XMAX,YMIN,YMAX,                    GKS03470
     *           LDRCDU,DRCDUM)                                         GKS03480
      CALL GPREC(32,IA,0,EMPTRP,0,DUMMYA,CHAR,4,ERRIND,LDRCDU,DRCDUM)   GKS03490
C     CALL GINCH(WKID,1,1,1,2,XMIN,XMAX,YMIN,YMAX,                      GKS03500
C    *           LDRCDU,DRCDUM)                                         GKS03510
C     CALL GSCHM(WKID,1,2,1)                                            GKS03520
C     CALL GSPKM(WKID,1,2,1)                                            GKS03530
C     CALL GMSGS(WKID,31,'    PICK SQUARES TO SELECT ITEM')             GKS03540
                                                                        GKS03550
C*************************************************************          GKS03560
C*****                                                   *****          GKS03570
C*****   Loop forever until input from either the        *****          GKS03580
C*****   lpfks or tablet is received.                    *****          GKS03590
C*****                                                   *****          GKS03600
C*************************************************************          GKS03610
                                                                        GKS03620
C  70 CALL GWAIT(600.0,WKID,ICLASS,IDEV)                                GKS03630
C     IF (ICLASS.EQ.4) THEN                                             GKS03640
C       CALL GGTCH(STAT,SGNA)                                           GKS03650
C     ELSE                                                              GKS03660
C       IF (ICLASS.EQ.5) THEN                                           GKS03670
C         CALL GGTPK(STAT,SGNA,PCID)                                    GKS03680
C       ELSE                                                            GKS03690
C         GOTO 70                                                       GKS03700
C       ENDIF                                                           GKS03710
C     ENDIF                                                             GKS03720
                                                                        GKS03730
C*************************************************************          GKS03740
C*****                                                   *****          GKS03750
C*****   First, prepare for further input by enabling    *****          GKS03760
C*****   the choice and pick devices and then flushing   *****          GKS03770
C*****   their queues.                                   *****          GKS03780
C*****                                                   *****          GKS03790
C*************************************************************          GKS03800
                                                                        GKS03810
      CALL GSCHM(WKID,1,0,1)                                            GKS03820
      CALL GSPKM(WKID,1,0,1)                                            GKS03830
      CALL GFLUSH(WKID,4,1)                                             GKS03840
      CALL GFLUSH(WKID,5,1)                                             GKS03850
      CALL GMSGS(WKID,1,' ')                                            GKS03860
      STARTX = 0.190                                                    GKS03870
      STARTY = 0.736 - SGNA * 0.130                                     GKS03880
      DO 80 J = 1,7                                                     GKS03890
        PX(J) = ARROW(J) + STARTX                                       GKS03900
        PY(J) = ARROW(J+7) + STARTY                                     GKS03910
   80 CONTINUE                                                          GKS03920
      CALL GMSGS(WKID,1,' ')                                            GKS03930
                                                                        GKS03940
C*************************************************************          GKS03950
C*****                                                   *****          GKS03960
C*****   Check input choice and call subroutine to       *****          GKS03970
C*****   handle the request. If 'END' was selected,      *****          GKS03980
C*****   exit the input loop and leave the program       *****          GKS03990
C*****   gracefully.                                     *****          GKS04000
C*****                                                   *****          GKS04010
C*************************************************************          GKS04020
                                                                        GKS04030
      CALL PRIMIT                                                       GKS04080
                                                                        GKS04210
C*************************************************************          GKS04220
C*****                                                   *****          GKS04230
C*****   If 'END' was not selected, wait for next        *****          GKS04240
C*****   choice from the main menu.                      *****          GKS04250
C*****                                                   *****          GKS04260
C*************************************************************          GKS04270
                                                                        GKS04280
      GOTO 10                                                           GKS04290
                                                                        GKS04300
C***********************************************************************GKS04310
C*****                                                               ***GKS04320
C*****   Otherwise, exit the program gracefully.                     ***GKS04330
C*****   Deactivate and close the workstation, close gks.            ***GKS04340
C*****                                                               ***GKS04350
C***********************************************************************GKS04360
                                                                        GKS04370
   90 CALL GDAWK(WKID)                                                  GKS04380
      CALL GCLWK(WKID)                                                  GKS04390
      CALL GCLKS                                                        GKS04400
      END                                                               GKS04410
                                                                        GKS04420
                                                                        GKS04430
                                                                        GKS04440
C***********************************************************************GKS04450
C***********************************************************************GKS04460
C*****                                                               ***GKS04470
C*****   Subroutine Name: CHCOL                                      ***GKS04480
C*****                                                               ***GKS04490
C*****   Subroutine Function: Choose a color from the color table.   ***GKS04500
C*****                                                               ***GKS04510
C*****   Calls Subroutines:   RCHOI                                  ***GKS04520
C*****                                                               ***GKS04530
C***********************************************************************GKS04540
C***********************************************************************GKS04550
                                                                        GKS04560
      SUBROUTINE CHCOL(COLI)                                            GKS04570
      INCLUDE 'gkspar.inc'
                                                                        GKS04590
      INTEGER*4    WKID,WTYPE,I,J,CHNR,COLI,ERRIND                      GKS04600
      INTEGER*4    NELCOL,LCHCOL(16),LDRCOL                             GKS04610
                                                                        GKS04620
      REAL*4       STARTX,STARTY,PX(4),PY(4),COLBOX(8)                  GKS04630
      REAL*4       XMIN,XMAX,YMIN,YMAX                                  GKS04640
                                                                        GKS04650
      CHARACTER*23 MENCOL                                               GKS04660
      CHARACTER*80 DTRCOL(16)                                           GKS04670
                                                                        GKS04680
      COMMON      /WINF/   WKID,WTYPE                                   GKS04690
      COMMON      /LIMITS/ XMIN,XMAX,YMIN,YMAX                          GKS04700
                                                                        GKS04710
      DATA MENCOL /'12345678910111213141516'/                           GKS04720
      DATA LCHCOL /1,1,1,1,1,1,1,1,1,2,2,2,2,2,2,2/                     GKS04730
      DATA NELCOL /16/                                                  GKS04740
                                                                        GKS04750
      DATA COLBOX /0.000,0.030,0.030,0.000,0.000,0.000,0.012,0.012/     GKS04760
                                                                        GKS04770
C***********************************************************************GKS04780
C*****                                                               ***GKS04790
C*****   Draw the color table on the upper right hand side of the    ***GKS04800
C*****   screen using colored solid fill areas. Each color repre-    ***GKS04810
C*****   sents the corresponding choice number.                      ***GKS04820
C*****                                                               ***GKS04830
C***********************************************************************GKS04840
                                                                        GKS04850
      CALL GCRSG(1000)                                                  GKS04860
      CALL GSFAIS(GSOLID)                                               GKS04870
      STARTX = 0.965                                                    GKS04880
      DO 110 I=1,16                                                     GKS04890
        CALL GSFACI(I)                                                  GKS04900
        STARTY = 1.012 - I * 0.028                                      GKS04910
        DO 100 J=1,4                                                    GKS04920
          PX(J) = COLBOX(J) + STARTX                                    GKS04930
          PY(J) = COLBOX(J+4) + STARTY                                  GKS04940
  100   CONTINUE                                                        GKS04950
        CALL GFA(4,PX,PY)                                               GKS04960
  110 CONTINUE                                                          GKS04970
      CALL GSFACI(GGREEN)                                               GKS04980
      CALL GSFAIS(GSOLID)                                               GKS04990
      CALL GCLSG                                                        GKS05000
                                                                        GKS05010
C***********************************************************************GKS05020
C*****                                                               ***GKS05030
C*****   Initialize the choice device to prompt for colors 1-16.     ***GKS05040
C*****   Call subroutine to wait for valid choice output and then    ***GKS05050
C*****   assign the color picked to 'COLI' and return it to the      ***GKS05060
C*****   calling routine.                                            ***GKS05070
C*****                                                               ***GKS05080
C***********************************************************************GKS05090
                                                                        GKS05100
      CALL GPREC(0,0,0,0.0,NELCOL,LCHCOL,MENCOL,NELCOL,                 GKS05110
     *           ERRIND,LDRCOL,DTRCOL)                                  GKS05120
C     CALL GINCH(WKID,1,1,1,3,XMIN,XMAX,YMIN,YMAX,LDRCOL,DTRCOL)        GKS05130
      CALL GMSGS(WKID,18,'SELECT COLOR INDEX')                          GKS05140
      CALL RCHOI(WKID,16,CHNR)                                          GKS05150
      COLI = CHNR                                                       GKS05160
      CALL GDSG(1000)                                                   GKS05170
      RETURN                                                            GKS05180
      END                                                               GKS05190
                                                                        GKS05200
                                                                        GKS05210
                                                                        GKS05220
C***********************************************************************GKS05230
C***********************************************************************GKS05240
C*****                                                               ***GKS05250
C*****   Subroutine Name: COLOR                                      ***GKS05260
C*****                                                               ***GKS05270
C*****   Subroutine Function: This subroutine will demonstrate the   ***GKS05280
C*****                        color facilities supported by GKS-CO   ***GKS05290
C*****                        for the given workstation.             ***GKS05300
C*****                                                               ***GKS05310
C***********************************************************************GKS05320
C***********************************************************************GKS05330
                                                                        GKS05340
                                                                        GKS10710
C***********************************************************************GKS10720
C***********************************************************************GKS10730
C*****                                                               ***GKS10740
C*****   Subroutine Name: CREFA                                      ***GKS10750
C*****                                                               ***GKS10760
C*****   Subroutine Function: This subroutine allows the user to     ***GKS10770
C*****                        create fill areas in new segments.     ***GKS10780
C*****                        The user can choose to change the      ***GKS10790
C*****                        fill area color and interior style.    ***GKS10800
C*****                        The user can then draw the fill area   ***GKS10810
C*****                        shape using the stroke device.         ***GKS10820
C*****                                                               ***GKS10830
C*****   Calls Subroutines:   RCHOI, CHCOL                           ***GKS10840
C*****                                                               ***GKS10850
C***********************************************************************GKS10860
C***********************************************************************GKS10870
                                                                        GKS10880
      SUBROUTINE CREFA                                                  GKS10890
      INCLUDE 'gkspar.inc'
                                                                        GKS10910
      INTEGER*4    WKID,WTYPE,CHNR,ERRIND,STAT,TNR,I,J,IA(4),COLI       GKS10920
      INTEGER*4    NELFLA,LCHFLA(4),LDRFLA                              GKS10930
      INTEGER*4    NELDUM,LCHDUM(1),LDRDUM                              GKS12570
      INTEGER*4    NELFIS,LCHFIS(4),LDRFIS                              GKS10940
                                                                        GKS10950
      REAL*4       XMIN,XMAX,YMIN,YMAX,LCX(337),LCY(337)                GKS10960
      REAL*4       IPX(1),IPY(1),SKIVAL(4)                              GKS10970
                                                                        GKS10980
      CHARACTER*30 MENFLA                                               GKS10990
      CHARACTER*80 DTRFLA(4)                                            GKS11000
      CHARACTER*23 MENFIS                                               GKS11010
      CHARACTER*80 DTRFIS(4)                                            GKS11020
      CHARACTER*1  MENDUM                                               GKS12670
                                                                        GKS11030
      LOGICAL      SGNAME(50)                                           GKS11040
                                                                        GKS11050
      COMMON      /WINF/   WKID,WTYPE                                   GKS11060
      COMMON      /SEGM/   SGNAME                                       GKS11070
      COMMON      /LIMITS/ XMIN,XMAX,YMIN,YMAX                          GKS11080
                                                                        GKS11090
      DATA MENDUM /' '/                                                 GKS12800
      DATA LCHDUM /1/                                                   GKS12810
      DATA NELDUM /4/                                                   GKS12820
                                                                        GKS12830
      DATA MENFLA /'FILL AREACOLORINT. STYLERETURN'/                    GKS11100
      DATA LCHFLA /9,5,10,6/                                            GKS11110
      DATA NELFLA /4/                                                   GKS11120
                                                                        GKS11130
      DATA MENFIS /'HOLLOWSOLIDPATTERNHATCH'/                           GKS11140
      DATA LCHFIS /6,5,7,5/                                             GKS11150
      DATA NELFIS /4/                                                   GKS11160
                                                                        GKS11170
      DATA IA     /0,0,0,0/                                             GKS11180
      DATA SKIVAL /0.005,0.005,0.005,1.000/                             GKS11190
                                                                        GKS11200
C***********************************************************************GKS11210
C*****                                                               ***GKS11220
C*****   Use Pack Data Record to set up the prompts for the choice   ***GKS11230
C*****   device. The user can then choose to: draw fill area, set    ***GKS11240
C*****   the fill area color, set the fill area interior style,      ***GKS11250
C*****   or return to the main create panel.                         ***GKS11260
C*****                                                               ***GKS11270
C***********************************************************************GKS11280
                                                                        GKS11290
      CALL GPREC(0,0,0,0.0,NELFLA,LCHFLA,MENFLA,NELFLA,                 GKS11300
     *           ERRIND,LDRFLA,DTRFLA)                                  GKS11310
  300 CALL GINCH(WKID,1,1,1,3,XMIN,XMAX,YMIN,YMAX,LDRFLA,DTRFLA)        GKS11320
      CALL GMSGS(WKID,32,'DRAW FILL AREA OR SET ATTRIBUTES')            GKS11330
      CALL RCHOI(WKID,4,CHNR)                                           GKS11340
                                                                        GKS11350
C***********************************************************************GKS11360
C*****                                                               ***GKS11370
C*****   Find the first unused segment number (segment 1-6 are re-   ***GKS11380
C*****   served for use by GKSDEMO). Allow the user to draw the      ***GKS11390
C*****   fill area using the stroke device (user holds button one    ***GKS11400
C*****   down on the locator while drawing and releases the button   ***GKS11410
C*****   when fill area boundaries are complete).                    ***GKS11420
C*****                                                               ***GKS11430
C***********************************************************************GKS11440
                                                                        GKS11450
      IF (CHNR.EQ.1) THEN                                               GKS11460
        DO 310 I=7,50                                                   GKS11470
          IF (.NOT.(SGNAME(I))) GOTO 320                                GKS11480
  310   CONTINUE                                                        GKS11490
        CALL GMSGS(WKID,32,'NO MORE THAN 50 SEGMENTS ALLOWED')          GKS11500
        GOTO 340                                                        GKS11510
  320   J = 10                                                          GKS11520
        IPX(1) = 0.0                                                    GKS11530
        IPY(1) = 0.0                                                    GKS11540
	IA(1) = 0
	IA(2) = 1
        CALL GPREC(2,IA,3,SKIVAL,0,LCHDUM,MENDUM,NELDUM,                GKS11550
     *             ERRIND,LDRDUM,DTRDUM)                                GKS11560
        CALL GINSK(WKID,1,1,1,IPX,IPY,1,XMIN,XMAX,YMIN,YMAX,            GKS11570
     *             337,LDRDUM,DTRDUM)                                   GKS11580
        CALL GMSGS(WKID,20,'ENTER POINT OR BREAK')                      GKS11590
  330   CALL GRQSK(WKID,1,337,STAT,TNR,J,LCX,LCY)                       GKS11600
        CALL GMSGS(WKID,1,' ')                                          GKS11610
                                                                        GKS11620
C***********************************************************************GKS11630
C*****                                                               ***GKS11640
C*****   Once valid stroke data is received, create a segment to     ***GKS11650
C*****   hold the fill area and then place the fill area in the      ***GKS11660
C*****   segment. If valid stroke data wasn't received, try ob-      ***GKS11670
C*****   taining valid input again.                                  ***GKS11680
C*****                                                               ***GKS11690
C***********************************************************************GKS11700
                                                                        GKS11710
        IF (STAT.EQ.1) THEN                                             GKS11720
          CALL GCRSG(I)                                                 GKS11730
          CALL GSDTEC(I,1)                                              GKS11740
          SGNAME(I) = .TRUE.                                            GKS11750
          CALL GFA(J,LCX,LCY)                                           GKS11760
          CALL GCLSG                                                    GKS11770
        ELSE                                                            GKS11780
          GOTO 330                                                      GKS11790
        ENDIF                                                           GKS11800
      ELSE                                                              GKS11810
                                                                        GKS11820
C***********************************************************************GKS11830
C*****                                                               ***GKS11840
C*****   Call subroutine CHCOL to display a panel and allow the      ***GKS11850
C*****   user to choose a color from the current color table (color  ***GKS11860
C*****   index passed back in variable COLI). Set the fill area      ***GKS11870
C*****   color using the color index returned.                       ***GKS11880
C*****                                                               ***GKS11890
C***********************************************************************GKS11900
                                                                        GKS11910
        IF (CHNR.EQ.2) THEN                                             GKS11920
          CALL CHCOL(COLI)                                              GKS11930
          CALL GSFACI(COLI)                                             GKS11940
        ELSE                                                            GKS11950
                                                                        GKS11960
C***********************************************************************GKS11970
C*****                                                               ***GKS11980
C*****   Use pack data record to set up the prompt for the choice    ***GKS11990
C*****   device. User can choose an interior style of either:        ***GKS12000
C*****   hollow, solid, pattern, or hatch. When a valid choice       ***GKS12010
C*****   has been made, set the fill area interior style based on    ***GKS12020
C*****   the choice number minus (0=hollow,1=solid,2=pattern,        ***GKS12030
C*****   3=hatch).                                                   ***GKS12040
C*****                                                               ***GKS12050
C***********************************************************************GKS12060
                                                                        GKS12070
          IF (CHNR.EQ.3) THEN                                           GKS12080
            CALL GPREC(0,0,0,0.0,NELFIS,LCHFIS,MENFIS,NELFIS,           GKS12090
     *                 ERRIND,LDRFIS,DTRFIS)                            GKS12100
            CALL GINCH(WKID,1,1,1,3,XMIN,XMAX,YMIN,YMAX,LDRFIS,         GKS12110
     *                 DTRFIS)                                          GKS12120
            CALL GMSGS(WKID,21,'SELECT INTERIOR STYLE')                 GKS12130
            CALL RCHOI(WKID,4,CHNR)                                     GKS12140
            CALL GSFAIS(CHNR-1)                                         GKS12150
                                                                        GKS12160
C***********************************************************************GKS12170
C*****                                                               ***GKS12180
C*****   User chose lpfk 4 - RETURN. So exit back to the main        ***GKS12190
C*****   create segment choice menu.                                 ***GKS12200
C*****                                                               ***GKS12210
C***********************************************************************GKS12220
                                                                        GKS12230
          ELSE                                                          GKS12240
            GOTO 340                                                    GKS12250
          ENDIF                                                         GKS12260
        ENDIF                                                           GKS12270
      ENDIF                                                             GKS12280
      GOTO 300                                                          GKS12290
  340 RETURN                                                            GKS12300
      END                                                               GKS12310
                                                                        GKS12320
                                                                        GKS12330
                                                                        GKS12340
C***********************************************************************GKS12350
C***********************************************************************GKS12360
C*****                                                               ***GKS12370
C*****   Subroutine Name: CREPLN                                     ***GKS12380
C*****                                                               ***GKS12390
C*****   Subroutine Function: This subroutine allows the user to     ***GKS12400
C*****                        create a polyline in a new segment.    ***GKS12410
C*****                        The user can choose to change the      ***GKS12420
C*****                        polyline color, linewidth and line-    ***GKS12430
C*****                        type. The user can then use the        ***GKS12440
C*****                        stroke device to draw the line.        ***GKS12450
C*****                                                               ***GKS12460
C*****   Calls Subroutines:   RCHOI, CHCOL                           ***GKS12470
C*****                                                               ***GKS12480
C***********************************************************************GKS12490
C***********************************************************************GKS12500
                                                                        GKS12510
      SUBROUTINE CREPLN                                                 GKS12520
      INCLUDE 'gkspar.inc'
                                                                        GKS12540
      INTEGER*4    WKID,WTYPE,CHNR,ERRIND,STAT,TNR,I,J,IA(4),COLI       GKS12550
      INTEGER*4    NELLIN,LCHLIN(5),LDRLIN                              GKS12560
      INTEGER*4    NELDUM,LCHDUM(1),LDRDUM                              GKS12570
      INTEGER*4    NELCLT,LCHCLT(4),LDRCLT                              GKS12580
                                                                        GKS12590
      REAL*4       XMIN,XMAX,YMIN,YMAX,SCFACT                           GKS12600
      REAL*4       IPX(1),IPY(1),LCX(337),LCY(337),SKIVAL(4)            GKS12610
                                                                        GKS12620
      CHARACTER*27 MENCLT                                               GKS12630
      CHARACTER*80 DTRCLT(4)                                            GKS12640
      CHARACTER*36 MENLIN                                               GKS12650
      CHARACTER*80 DTRLIN(4)                                            GKS12660
      CHARACTER*1  MENDUM                                               GKS12670
      CHARACTER*80 DTRDUM(4)                                            GKS12680
                                                                        GKS12690
      LOGICAL      SGNAME(50)                                           GKS12700
                                                                        GKS12710
      COMMON      /WINF/   WKID,WTYPE                                   GKS12720
      COMMON      /SEGM/   SGNAME                                       GKS12730
      COMMON      /LIMITS/ XMIN,XMAX,YMIN,YMAX                          GKS12740
                                                                        GKS12750
      DATA MENLIN /'POLYLINECOLORLINEWIDTHLINETYPERETURN'/              GKS12760
      DATA LCHLIN /8,5,9,8,6/                                           GKS12770
      DATA NELLIN /5/                                                   GKS12780
                                                                        GKS12790
      DATA MENDUM /' '/                                                 GKS12800
      DATA LCHDUM /1/                                                   GKS12810
      DATA NELDUM /4/                                                   GKS12820
                                                                        GKS12830
      DATA MENCLT /'SOLIDDASHEDDOTTEDDASHDOTTED'/                       GKS12840
      DATA LCHCLT /5,6,6,10/                                            GKS12850
      DATA NELCLT /4/                                                   GKS12860
                                                                        GKS12870
      DATA IA     /0,0,0,0/                                             GKS12880
      DATA SKIVAL /0.005,0.005,0.005,1.000/                             GKS12890
                                                                        GKS12900
C***********************************************************************GKS12910
C*****                                                               ***GKS12920
C*****   Use Pack Data Record to set up the prompts for the choice   ***GKS12930
C*****   device. The user can then choose to: draw a polyline, set   ***GKS12940
C*****   the polyline color, set the linewidth, set the linetype,    ***GKS12950
C*****   or return to the main create panel.                         ***GKS12960
C*****                                                               ***GKS12970
C***********************************************************************GKS12980
                                                                        GKS12990
      CALL GPREC(0,0,0,0.0,NELLIN,LCHLIN,MENLIN,NELLIN,                 GKS13000
     *           ERRIND,LDRLIN,DTRLIN)                                  GKS13010
  400 CALL GMSGS(WKID,1,' ')                                            GKS13020
      CALL GINCH(WKID,1,1,1,3,XMIN,XMAX,YMIN,YMAX,LDRLIN,DTRLIN)        GKS13030
      CALL GMSGS(WKID,31,'DRAW POLYLINE OR SET ATTRIBUTES')             GKS13040
      CALL RCHOI(WKID,5,CHNR)                                           GKS13050
                                                                        GKS13060
C***********************************************************************GKS13070
C*****                                                               ***GKS13080
C*****   Find the first unused segment number (segment 1-6 are re-   ***GKS13090
C*****   served for use by GKSDEMO). Allow the user to draw the      ***GKS13100
C*****   polyline using the stroke device (user holds button one     ***GKS13110
C*****   down on the locator while drawing and releases the button   ***GKS13120
C*****   when polyline is complete).                                 ***GKS13130
C*****                                                               ***GKS13140
C***********************************************************************GKS13150
                                                                        GKS13160
      IF (CHNR.EQ.1) THEN                                               GKS13170
        DO 410 I=7,50                                                   GKS13180
          IF (.NOT.(SGNAME(I))) GOTO 420                                GKS13190
  410   CONTINUE                                                        GKS13200
        CALL GMSGS(WKID,32,'NO MORE THAN 50 SEGMENTS ALLOWED')          GKS13210
        GOTO 440                                                        GKS13220
C demo modification  TRG  J = 10 not J = 0
  420   J = 10                                                          GKS13230
        IPX(1) = 0.5                                                    GKS13240
        IPY(1) = 0.5                                                    GKS13250
	IA(1) = 0
	IA(2) = 1
        CALL GPREC(2,IA,3,SKIVAL,0,LCHDUM,MENDUM,NELDUM,                GKS11550
     *             ERRIND,LDRDUM,DTRDUM)                                GKS11560
        CALL GINSK(WKID,1,1,1,IPX,IPY,1,XMIN,XMAX,YMIN,YMAX,            GKS13280
     *             337,LDRDUM,DTRDUM)                                   GKS13290
        CALL GMSGS(WKID,20,'ENTER POINT OR BREAK')                      GKS13300
  430   CALL GRQSK(WKID,1,337,STAT,TNR,J,LCX,LCY)                       GKS13310
        CALL GMSGS(WKID,1,' ')                                          GKS13320
                                                                        GKS13330
C***********************************************************************GKS13340
C*****                                                               ***GKS13350
C*****   Once valid stroke data is received, create a segment to     ***GKS13360
C*****   hold the polyline and then place the polyline in the        ***GKS13370
C*****   segment. If valid stroke data wasn't received, try ob-      ***GKS13380
C*****   taining valid input again.                                  ***GKS13390
C*****                                                               ***GKS13400
C***********************************************************************GKS13410
                                                                        GKS13420
        IF (STAT.EQ.1) THEN                                             GKS13430
          CALL GCRSG(I)                                                 GKS13440
          CALL GSDTEC(I,1)                                              GKS13450
          SGNAME(I) = .TRUE.                                            GKS13460
          CALL GPL(J,LCX,LCY)                                           GKS13470
          CALL GCLSG                                                    GKS13480
        ELSE                                                            GKS13490
          GOTO 430                                                      GKS13500
        ENDIF                                                           GKS13510
      ELSE                                                              GKS13520
                                                                        GKS13530
C***********************************************************************GKS13540
C*****                                                               ***GKS13550
C*****   Call subroutine CHCOL to display a panel and allow the      ***GKS13560
C*****   user to choose a color from the current color table (color  ***GKS13570
C*****   index passed back in variable COLI). Set the polyline       ***GKS13580
C*****   color using the color index returned.                       ***GKS13590
C*****                                                               ***GKS13600
C***********************************************************************GKS13610
                                                                        GKS13620
        IF (CHNR.EQ.2) THEN                                             GKS13630
          CALL CHCOL(COLI)                                              GKS13640
          CALL GSPLCI(COLI)                                             GKS13650
        ELSE                                                            GKS13660
                                                                        GKS13670
C***********************************************************************GKS13680
C*****                                                               ***GKS13690
C*****   Use the valuator device to select the linewidth scale       ***GKS13700
C*****   factor (user turns valuator to desired position and then    ***GKS13710
C*****   hits the enter key).                                        ***GKS13720
C*****                                                               ***GKS13730
C***********************************************************************GKS13740
                                                                        GKS13750
          IF (CHNR.EQ.3) THEN                                           GKS13760
            CALL GMSGS(WKID,31,'EVALUATE LINEWIDTH SCALE FACTOR')       GKS13770
            CALL GRQVL(WKID,1,STAT,SCFACT)                              GKS13780
            CALL GMSGS(WKID,1,' ')                                      GKS13790
            CALL GSLWSC(SCFACT)                                         GKS13800
          ELSE                                                          GKS13810
                                                                        GKS13820
C***********************************************************************GKS13830
C*****                                                               ***GKS13840
C*****   Use the choice device to select the linetype. Valid         ***GKS13850
C*****   choices are: solid, dashed, dotted, and dash-dotted.        ***GKS13860
C*****                                                               ***GKS13870
C***********************************************************************GKS13880
                                                                        GKS13890
            IF (CHNR.EQ.4) THEN                                         GKS13900
              CALL GPREC(0,0,0,0.0,NELCLT,LCHCLT,MENCLT,NELCLT,         GKS13910
     *                   ERRIND,LDRCLT,DTRCLT)                          GKS13920
              CALL GINCH(WKID,1,1,1,3,XMIN,XMAX,YMIN,YMAX,LDRCLT,       GKS13930
     *                   DTRCLT)                                        GKS13940
              CALL GMSGS(WKID,15,'SELECT LINETYPE')                     GKS13950
              CALL RCHOI(WKID,4,CHNR)                                   GKS13960
              CALL GSLN(CHNR)                                           GKS13970
                                                                        GKS13980
C***********************************************************************GKS13990
C*****                                                               ***GKS14000
C*****   User chose lpfk 5 - RETURN. So exit back to the main        ***GKS14010
C*****   create segment choice menu.                                 ***GKS14020
C*****                                                               ***GKS14030
C***********************************************************************GKS14040
                                                                        GKS14050
            ELSE                                                        GKS14060
              GOTO 440                                                  GKS14070
            ENDIF                                                       GKS14080
          ENDIF                                                         GKS14090
        ENDIF                                                           GKS14100
      ENDIF                                                             GKS14110
      GOTO 400                                                          GKS14120
  440 RETURN                                                            GKS14130
      END                                                               GKS14140
                                                                        GKS14150
                                                                        GKS14160
                                                                        GKS14170
C***********************************************************************GKS14180
C***********************************************************************GKS14190
C*****                                                               ***GKS14200
C*****   Subroutine Name: CREPMK                                     ***GKS14210
C*****                                                               ***GKS14220
C*****   Subroutine Function: This subroutine allows the user to     ***GKS14230
C*****                        create a polymarker in a new seg-      ***GKS14240
C*****                        ment. The user can choose to change    ***GKS14250
C*****                        the polymarker color, markersize       ***GKS14260
C*****                        and markertype. The user can then      ***GKS14270
C*****                        use the locator device to set the      ***GKS14280
C*****                        x,y coordinates of the polymarker      ***GKS14290
C*****                        and the polymarker segment is then     ***GKS14300
C*****                        created.                               ***GKS14310
C*****                                                               ***GKS14320
C*****   Calls Subroutines:   RCHOI,CHCOL                            ***GKS14330
C*****                                                               ***GKS14340
C***********************************************************************GKS14350
C***********************************************************************GKS14360
                                                                        GKS14370
      SUBROUTINE CREPMK                                                 GKS14380
      INCLUDE 'gkspar.inc'
                                                                        GKS14400
      INTEGER*4    WKID,WTYPE,CHNR,ERRIND,STAT,TNR,I,IA(1),COLI         GKS14410
      INTEGER*4    NELDUM,LCHDUM(1),LDRDUM                              GKS14420
      INTEGER*4    NELMRK,LCHMRK(5),LDRMRK                              GKS14430
      INTEGER*4    NELMKT,LCHMKT(5),LDRMKT                              GKS14440
                                                                        GKS14450
      REAL*4       XMIN,XMAX,YMIN,YMAX,SCFACT                           GKS14460
      REAL*4       LCX(337),LCY(337)                                    GKS14470
                                                                        GKS14480
      CHARACTER*1  MENDUM                                               GKS14490
      CHARACTER*80 DTRDUM(4)                                            GKS14500
      CHARACTER*41 MENMRK                                               GKS14510
      CHARACTER*80 DTRMRK(5)                                            GKS14520
      CHARACTER*36 MENMKT                                               GKS14530
      CHARACTER*80 DTRMKT(5)                                            GKS14540
                                                                        GKS14550
      LOGICAL      SGNAME(50)                                           GKS14560
                                                                        GKS14570
      COMMON      /WINF/   WKID,WTYPE                                   GKS14580
      COMMON      /SEGM/   SGNAME                                       GKS14590
      COMMON      /LIMITS/ XMIN,XMAX,YMIN,YMAX                          GKS14600
                                                                        GKS14610
      DATA MENDUM /' '/                                                 GKS14620
      DATA LCHDUM /1/                                                   GKS14630
      DATA NELDUM /4/                                                   GKS14640
                                                                        GKS14650
      DATA MENMRK /'POLYMARKERCOLORMARKERSIZEMARKERTYPERETURN'/         GKS14660
      DATA LCHMRK /10,5,10,10,6/                                        GKS14670
      DATA NELMRK /5/                                                   GKS14680
                                                                        GKS14690
      DATA MENMKT /'DOTPLUS SIGNASTERISKCIRCLEDIAG-CROSS'/              GKS14700
      DATA LCHMKT /3,9,8,6,10/                                          GKS14710
      DATA NELMKT /5/                                                   GKS14720
                                                                        GKS14730
      DATA IA     /0/                                                   GKS14740
                                                                        GKS14750
C***********************************************************************GKS14760
C*****                                                               ***GKS14770
C*****   Use Pack Data Record to set up the prompts for the choice   ***GKS14780
C*****   device. The user can then choose to: draw fill area, set    ***GKS14790
C*****   the fill area color, set the fill area interior style,      ***GKS14800
C*****   or return to the main create panel.                         ***GKS14810
C*****                                                               ***GKS14820
C***********************************************************************GKS14830
                                                                        GKS14840
      CALL GPREC(0,0,0,0.0,NELMRK,LCHMRK,MENMRK,NELMRK,                 GKS14850
     *           ERRIND,LDRMRK,DTRMRK)                                  GKS14860
  500 CALL GMSGS(WKID,1,' ')                                            GKS14870
      CALL GINCH(WKID,1,1,1,3,XMIN,XMAX,YMIN,YMAX,LDRMRK,DTRMRK)        GKS14880
      CALL GMSGS(WKID,33,'DRAW POLYMARKER OR SET ATTRIBUTES')           GKS14890
      CALL RCHOI(WKID,5,CHNR)                                           GKS14900
                                                                        GKS14910
C***********************************************************************GKS14920
C*****                                                               ***GKS14930
C*****   Find the first unused segment number (segment 1-6 are re-   ***GKS14940
C*****   served for use by GKSDEMO). Allow the user to draw the      ***GKS14950
C*****   polymarker by placing the locator echo at a certain point   ***GKS14960
C*****   and hitting a button on the locator. When user is done      ***GKS14970
C*****   inserting polymarker segments, it hits ALT-CANCEL (break    ***GKS14980
C*****   in RT lingo). This will send it back to the main            ***GKS14990
C*****   polymarker choice menu.                                     ***GKS15000
C*****                                                               ***GKS15010
C***********************************************************************GKS15020
                                                                        GKS15030
      IF (CHNR.EQ.1) THEN                                               GKS15040
        LCX(1) = 0.1                                                    GKS15050
        LCY(1) = 0.1                                                    GKS15060
  510   DO 520 I=7,50                                                   GKS15070
          IF (.NOT.(SGNAME(I))) GOTO 530                                GKS15080
  520   CONTINUE                                                        GKS15090
        CALL GMSGS(WKID,32,'NO MORE THAN 50 SEGMENTS ALLOWED')          GKS15100
        GOTO 540                                                        GKS15110
  530   CALL GPREC(0,IA,0,0.0,0,LCHDUM,MENDUM,NELDUM,ERRIND,            GKS15120
     *             LDRDUM,DTRDUM)                                       GKS15130
        CALL GINLC(WKID,1,1,LCX(1),LCY(1),2,XMIN,XMAX,YMIN,YMAX,        GKS15140
     *             LDRDUM,DTRDUM)                                       GKS15150
        CALL GMSGS(WKID,20,'ENTER POINT OR BREAK')                      GKS15160
        CALL GRQLC(WKID,1,STAT,TNR,LCX(1),LCY(1))                       GKS15170
        CALL GMSGS(WKID,1,' ')                                          GKS15180
        IF (STAT.EQ.1) THEN                                             GKS15190
          CALL GCRSG(I)                                                 GKS15200
          CALL GSDTEC(I,1)                                              GKS15210
          SGNAME(I) = .TRUE.                                            GKS15220
          CALL GPM(1,LCX(1),LCY(1))                                     GKS15230
          CALL GCLSG                                                    GKS15240
          GOTO 510                                                      GKS15250
        ENDIF                                                           GKS15260
      ELSE                                                              GKS15270
                                                                        GKS15280
C***********************************************************************GKS15290
C*****                                                               ***GKS15300
C*****   Call subroutine CHCOL to display a panel and allow the      ***GKS15310
C*****   user to choose a color from the current color table (color  ***GKS15320
C*****   index passed back in variable COLI). Set the polymarker     ***GKS15330
C*****   color using the color index returned.                       ***GKS15340
C*****                                                               ***GKS15350
C***********************************************************************GKS15360
                                                                        GKS15370
        IF (CHNR.EQ.2) THEN                                             GKS15380
          CALL CHCOL(COLI)                                              GKS15390
          CALL GSPMCI(COLI)                                             GKS15400
        ELSE                                                            GKS15410
          IF (CHNR.EQ.3) THEN                                           GKS15420
            CALL GMSGS(WKID,32,'EVALUATE MARKERSIZE SCALE FACTOR')      GKS15430
            CALL GRQVL(WKID,1,STAT,SCFACT)                              GKS15440
            CALL GMSGS(WKID,1,' ')                                      GKS15450
            CALL GSMKSC(SCFACT)                                         GKS15460
          ELSE                                                          GKS15470
            IF (CHNR.EQ.4) THEN                                         GKS15480
              CALL GPREC(0,0,0,0.0,NELMKT,LCHMKT,MENMKT,NELMKT,         GKS15490
     *                   ERRIND,LDRMKT,DTRMKT)                          GKS15500
              CALL GINCH(WKID,1,1,1,3,XMIN,XMAX,YMIN,YMAX,LDRMKT,       GKS15510
     *                   DTRMKT)                                        GKS15520
              CALL GMSGS(WKID,17,'SELECT MARKERTYPE')                   GKS15530
              CALL RCHOI(WKID,5,CHNR)                                   GKS15540
              CALL GSMK(CHNR)                                           GKS15550
                                                                        GKS15560
C***********************************************************************GKS15570
C*****                                                               ***GKS15580
C*****   User chose lpfk 5 - RETURN. So exit back to the main        ***GKS15590
C*****   create segment choice menu.                                 ***GKS15600
C*****                                                               ***GKS15610
C***********************************************************************GKS15620
                                                                        GKS15630
            ELSE                                                        GKS15640
              GOTO 540                                                  GKS15650
            ENDIF                                                       GKS15660
          ENDIF                                                         GKS15670
        ENDIF                                                           GKS15680
      ENDIF                                                             GKS15690
      GOTO 500                                                          GKS15700
  540 RETURN                                                            GKS15710
      END                                                               GKS15720
                                                                        GKS15730
                                                                        GKS15740
                                                                        GKS15750
C***********************************************************************GKS15760
C***********************************************************************GKS15770
C*****                                                               ***GKS15780
C*****   Subroutine Name: CRETXT                                     ***GKS15790
C*****                                                               ***GKS15800
C*****   Subroutine Function: This subroutine allows the user to     ***GKS15810
C*****                        create text strings in new segments.   ***GKS15820
C*****                        The user can choose to change the      ***GKS15830
C*****                        text color, size and font. The user    ***GKS15840
C*****                        can then input the text from the       ***GKS15850
C*****                        keyboard and select the text starting  ***GKS15860
C*****                        point via the locator device.          ***GKS15870
C*****                                                               ***GKS15880
C*****   Calls Subroutines:   RCHOI, CHCOL                           ***GKS15890
C*****                                                               ***GKS15900
C***********************************************************************GKS15910
C***********************************************************************GKS15920
                                                                        GKS15930
      SUBROUTINE CRETXT                                                 GKS15940
      INCLUDE 'gkspar.inc'
                                                                        GKS15960
      INTEGER*4    WKID,WTYPE,CHNR,ERRIND,STAT,TNR,I,IA(1),COLI,LSTR    GKS15970
      INTEGER*4    NELTXT,LCHTXT(5),LDRTXT                              GKS15980
      INTEGER*4    NELTXF,LCHTXF(11),LDRTXF                             GKS15990
      INTEGER*4    NELDUM,LCHDUM(1),LDRDUM                              GKS16000
                                                                        GKS16010
      REAL*4       XMIN,XMAX,YMIN,YMAX,SCFACT                           GKS16020
      REAL*4       LCX(337),LCY(337)                                    GKS16030
                                                                        GKS16040
      CHARACTER*31 MENTXT                                               GKS16050
      CHARACTER*80 DTRTXT(5)                                            GKS16060
      CHARACTER*44 MENTXF                                               GKS16070
      CHARACTER*80 DTRTXF(11)                                           GKS16080
      CHARACTER*1  MENDUM                                               GKS16090
      CHARACTER*80 DTRDUM(4)                                            GKS16100
      CHARACTER*80 STRBUF                                               GKS16110
                                                                        GKS16120
      LOGICAL      SGNAME(50)                                           GKS16130
                                                                        GKS16140
      COMMON      /WINF/   WKID,WTYPE                                   GKS16150
      COMMON      /SEGM/   SGNAME                                       GKS16160
      COMMON      /LIMITS/ XMIN,XMAX,YMIN,YMAX                          GKS16170
                                                                        GKS16180
      DATA MENTXT /'TEXTCOLORTEXTSIZETEXTFONTRETURN'/                   GKS16190
      DATA LCHTXT /4,5,8,8,6/                                           GKS16200
      DATA NELTXT /5/                                                   GKS16210
                                                                        GKS16220
      DATA MENTXF /'F  1F  2F  3F  4F  5F  6F  7F  8F  9F 10F 11'/      GKS16230
      DATA LCHTXF /4,4,4,4,4,4,4,4,4,4,4/                               GKS16240
      DATA NELTXF /11/                                                  GKS16250
                                                                        GKS16260
      DATA MENDUM /' '/                                                 GKS16270
      DATA LCHDUM /1/                                                   GKS16280
      DATA NELDUM /4/                                                   GKS16290
                                                                        GKS16300
      DATA IA     /0/                                                   GKS16310
                                                                        GKS16320
C***********************************************************************GKS16330
C*****                                                               ***GKS16340
C*****   Use Pack Data Record to set up the prompts for the choice   ***GKS16350
C*****   device. The user can then choose to: input text and pick    ***GKS16360
C*****   its starting point, set the text color, set the text size,  ***GKS16370
C*****   select the text font, or return to the main create segment  ***GKS16380
C*****   panel.                                                      ***GKS16390
C*****                                                               ***GKS16400
C***********************************************************************GKS16410
                                                                        GKS16420
      CALL GPREC(0,0,0,0.0,NELTXT,LCHTXT,MENTXT,NELTXT,                 GKS16430
     *           ERRIND,LDRTXT,DTRTXT)                                  GKS16440
  600 CALL GINCH(WKID,1,1,1,3,XMIN,XMAX,YMIN,YMAX,LDRTXT,DTRTXT)        GKS16450
      CALL GMSGS(WKID,27,'DRAW TEXT OR SET ATTRIBUTES')                 GKS16460
      CALL RCHOI(WKID,5,CHNR)                                           GKS16470
                                                                        GKS16480
C***********************************************************************GKS16490
C*****                                                               ***GKS16500
C*****   Find the first free segment number (segments 1-6 are        ***GKS16510
C*****   reserved for use by GKSDEMO). Request input from the        ***GKS16520
C*****   string device (keyboard) for the text to be displayed.      ***GKS16530
C*****   Find the starting point for the text by requesting input    ***GKS16540
C*****   from the locator device. Draw the new segment on the        ***GKS16550
C*****   screen and return to the text choice panel.                 ***GKS16560
C*****                                                               ***GKS16570
C***********************************************************************GKS16580
                                                                        GKS16590
      IF (CHNR.EQ.1) THEN                                               GKS16600
        DO 610 I=7,50                                                   GKS16610
          IF (.NOT.(SGNAME(I))) GOTO 620                                GKS16620
  610   CONTINUE                                                        GKS16630
        CALL GMSGS(WKID,32,'NO MORE THAN 50 SEGMENTS ALLOWED')          GKS16640
        GOTO 640                                                        GKS16650
  620   CALL GRQST(WKID,1,STAT,LSTR,STRBUF)                             GKS16660
        CALL GMSGS(WKID,1,' ')                                          GKS16670
        CALL GPREC(0,IA,0,0.0,0,LCHDUM,MENDUM,NELDUM,                   GKS16680
     *             ERRIND,LDRDUM,DTRDUM)                                GKS16690
        CALL GINLC(WKID,1,1,0.1,0.1,2,XMIN,XMAX,YMIN,YMAX,              GKS16700
     *             LDRDUM,DTRDUM)                                       GKS16710
        CALL GMSGS(WKID,25,'ENTER TEXT STARTING POINT')                 GKS16720
  630   CALL GRQLC(WKID,1,STAT,TNR,LCX(1),LCY(1))                       GKS16730
        CALL GMSGS(WKID,1,' ')                                          GKS16740
        IF (STAT.EQ.1) THEN                                             GKS16750
          CALL GCRSG(I)                                                 GKS16760
          CALL GSDTEC(I,1)                                              GKS16770
          SGNAME(I) = .TRUE.                                            GKS16780
          CALL GTXS(LCX(1),LCY(1),LSTR,STRBUF)                          GKS16790
          CALL GCLSG                                                    GKS16800
          GOTO 600                                                      GKS16810
        ELSE                                                            GKS16820
          GOTO 630                                                      GKS16830
        ENDIF                                                           GKS16840
      ELSE                                                              GKS16850
                                                                        GKS16860
C***********************************************************************GKS16870
C*****                                                               ***GKS16880
C*****   Call subroutine CHCOL to display a panel and allow the      ***GKS16890
C*****   user to choose a color from the current color table (color  ***GKS16900
C*****   index passed back in variable COLI). Set the text color     ***GKS16910
C*****   using the color index returned.                             ***GKS16920
C*****                                                               ***GKS16930
C***********************************************************************GKS16940
                                                                        GKS16950
        IF (CHNR.EQ.2) THEN                                             GKS16960
          CALL CHCOL(COLI)                                              GKS16970
          CALL GSTXCI(COLI)                                             GKS16980
        ELSE                                                            GKS16990
                                                                        GKS17000
C***********************************************************************GKS17010
C*****                                                               ***GKS17020
C*****   Obtain the character height expansion factor from the       ***GKS17030
C*****   valuator (user turns valuator to desired character height   ***GKS17040
C*****   and hits ENTER). Return to text choice panel.               ***GKS17050
C*****                                                               ***GKS17060
C***********************************************************************GKS17070
                                                                        GKS17080
          IF (CHNR.EQ.3) THEN                                           GKS17090
            CALL GMSGS(WKID,25,'EVALUATE CHARACTER HEIGHT')             GKS17100
            CALL GRQVL(WKID,1,STAT,SCFACT)                              GKS17110
            CALL GMSGS(WKID,1,' ')                                      GKS17120
            CALL GSCHH(0.1 * SCFACT)                                    GKS17130
          ELSE                                                          GKS17140
                                                                        GKS17150
C***********************************************************************GKS17160
C*****                                                               ***GKS17170
C*****   Use pack data record to set up the prompts for the choice   ***GKS17180
C*****   device. Call subroutine RCHOI to wait for a valid selec-    ***GKS17190
C*****   tion and then set the text font using the value returned.   ***GKS17200
C*****   Return to the text choice panel.                            ***GKS17210
C*****                                                               ***GKS17220
C***********************************************************************GKS17230
                                                                        GKS17240
            IF (CHNR.EQ.4) THEN                                         GKS17250
              CALL GPREC(0,0,0,0.0,NELTXF,LCHTXF,MENTXF,NELTXF,         GKS17260
     *                   ERRIND,LDRTXF,DTRTXF)                          GKS17270
              CALL GINCH(WKID,1,1,1,3,XMIN,XMAX,YMIN,YMAX,LDRTXF,       GKS17280
     *                   DTRTXF)                                        GKS17290
              CALL GMSGS(WKID,16,'SELECT TEXT FONT')                    GKS17300
              CALL RCHOI(WKID,11,CHNR)                                  GKS17310
              CALL GSTXFP(CHNR,GSTRKP)                                  GKS17320
                                                                        GKS17330
C***********************************************************************GKS17340
C*****                                                               ***GKS17350
C*****   User chose lpfk 5 - RETURN. So exit back to the main        ***GKS17360
C*****   create segment choice menu.                                 ***GKS17370
C*****                                                               ***GKS17380
C***********************************************************************GKS17390
                                                                        GKS17400
            ELSE                                                        GKS17410
              GOTO 640                                                  GKS17420
            ENDIF                                                       GKS17430
          ENDIF                                                         GKS17440
        ENDIF                                                           GKS17450
      ENDIF                                                             GKS17460
      GOTO 600                                                          GKS17470
  640 RETURN                                                            GKS17480
      END                                                               GKS17490
                                                                        GKS17500
                                                                        GKS17510
                                                                        GKS17520
C***********************************************************************GKS17530
C***********************************************************************GKS17540
C*****                                                               ***GKS17550
C*****   Subroutine Name: CRSEGM                                     ***GKS17560
C*****                                                               ***GKS17570
C*****   Subroutine Function: This subroutine will create a          ***GKS17580
C*****                        detectable segment (segment number     ***GKS17590
C*****                        6) and draw a red box with the word    ***GKS17600
C*****                        'RETURN' in it and then return to      ***GKS17610
C*****                        the caller. The caller is responsible  ***GKS17620
C*****                        for deleting segment 6 when it is      ***GKS17630
C*****                        finished using it.                     ***GKS17640
C*****                                                               ***GKS17650
C***********************************************************************GKS17660
C***********************************************************************GKS17670
                                                                        GKS17680
      SUBROUTINE CRSEGM                                                 GKS17690
      INCLUDE 'gkspar.inc'
                                                                        GKS17710
      REAL*4 PX(4),PY(4)                                                GKS17720
      DATA PX /0.01,0.15,0.15,0.01/                                     GKS17730
      DATA PY /0.94,0.94,0.99,0.99/                                     GKS17740
                                                                        GKS17750
      CALL GCRSG(6)                                                     GKS17760
      CALL GSDTEC(6,1)                                                  GKS17770
      CALL GSFACI(GRED)                                                 GKS17780
      CALL GSFAIS(GHOLLO)                                               GKS17790
      CALL GFA(4,PX,PY)                                                 GKS17800
      CALL GSCHH(0.020)                                                 GKS17810
      CALL GSTXCI(GRED)                                                 GKS17820
      CALL GSTXAL(GACENT,GAHALF)                                        GKS17830
      CALL GTXS(0.07,0.965,6,'RETURN')                                  GKS17840
      CALL GSTXAL(GAHNOR,GAVNOR)                                        GKS17850
      CALL GSTXCI(GYELOW)                                               GKS17860
      CALL GSPLCI(GYELOW)                                               GKS17870
      CALL GCLSG                                                        GKS17880
      RETURN                                                            GKS17890
      END                                                               GKS17900
                                                                        GKS17910
                                                                        GKS17920
                                                                        GKS17930
C***********************************************************************GKS17940
C***********************************************************************GKS17950
C*****                                                               ***GKS17960
C*****   Subroutine Name: DEMOCA                                     ***GKS17970
C*****                                                               ***GKS17980
C*****   Subroutine Function: Draw a castle tower and a tree to      ***GKS17990
C*****                        show how the cell array primitive      ***GKS18000
C*****                        and its attributes work.               ***GKS18010
C*****                                                               ***GKS18020
C*****   Calls Subroutines:   SETUPF,FINSHF,RMENU                    ***GKS18030
C*****                                                               ***GKS18040
C***********************************************************************GKS18050
C***********************************************************************GKS18060
                                                                        GKS18070
      SUBROUTINE DEMOCA                                                 GKS18080
      INCLUDE 'gkspar.inc'
                                                                        GKS18100
      INTEGER*4   WKID,WTYPE,I                                          GKS18110
      INTEGER*4   PIXEL1(9,9),PIXEL2(20,20)                             GKS18120
      REAL*4      X1,X2,Y1,Y2                                           GKS18130
      LOGICAL     CONT                                                  GKS18140
      COMMON      /WINF/ WKID,WTYPE                                     GKS18150
      COMMON      /TEXT/   TXTX,TXTY                                    GKS18160
      DATA  PIXEL1 / 8, 8,13,13,13,13,13, 8, 8,                         GKS18170
     *               8, 8, 8,13,13,13, 8, 8, 8,                         GKS18180
     *               8, 8, 8,13,13,13, 8, 8, 8,                         GKS18190
     *               8,12, 8,13,13,12, 8,12, 8,                         GKS18200
     *              12,12,12,12,12,12,12,12,12,                         GKS18210
     *              12,12,12,12,12,12,12,12,12,                         GKS18220
     *              12,12,12,12,12,12,12,12,12,                         GKS18230
     *               8,12,12,12,12,12,12,12, 8,                         GKS18240
     *               8, 8,12,12,12,12,12, 8, 8/                         GKS18250
      DATA PIXEL2 / 20*28,
     * 31,31,31,31,31,31,88,84,31,31,31,31,31,31,31,31,31,31,31,31,
     * 31,31,31,31,31,31,88,84,31,31,31,31,31,31,31,31,31,31,31,31,
     * 31,31,31,31,31,31,88,84,31,31,31,31,31,31,31,31,31,31,31,31,
     * 31,31,31,31,31,31,88,84,31,31,31,31,31,31,31,31,31,31,31,31,
     * 31,31,31,31,31,31,31,88,84,31,31,31,31,31,31,31,31,31,31,31,
     * 31,31,31,31,31,88,88,88,84,31,31,31,171,31,31,31,31,31,31,31,
     * 31,31,31,31,88,88,84,88,84,31,31,227,199,171,31,31,31,31,31,31,
     * 31,31,31,88,88,84,88,31,88,84,31,227,199,171,31,31,31,31,31,31,
     * 31,31,31,88,84,88,88,31,88,84,31,227,199,171,31,31,31,31,31,31,
     * 31,31,31,84,88,88,31,31,31,88,84,31,227,31,31,31,31,31,31,31,
     * 5*31,31,31,199,171,171,88,217,181,181,31,199,171,171,31,31,
     * 5*31,31,227,199,199,199,171,221,217,181,227,199,199,199,171,31,
     * 5*31,31,31,227,227,199,31,221,221,217,31,227,227,199,31,31,
     * 31,31,31,31,31,31,31,31,31,31,31,31,171,31,31,31,31,31,31,31,
     * 31,31,31,31,31,31,31,31,31,31,31,227,199,171,31,31,31,31,31,31,
     * 31,31,31,31,31,31,31,31,31,31,31,227,199,171,31,31,31,31,31,31,
     * 31,31,31,31,31,31,31,31,31,31,31,227,199,171,31,31,31,31,31,31,
     * 31,31,31,31,31,31,31,31,31,31,31,31,227,31,31,31,31,31,31,31,
     * 31,31,31,31,31,31,31,31,31,31,31,31,31,31,31,31,31,31,31,31/

                                                                        GKS18350
C***********************************************************************GKS18360
C*****                                                               ***GKS18370
C*****   Call subroutines SETUPF and FINSHF to draw the border and   ***GKS18380
C*****   border and title of the demo frame. Then draw the first     ***GKS18390
C*****   panel which is a picture drawn using cell arrays.           ***GKS18400
C*****                                                               ***GKS18410
C***********************************************************************GKS18420
                                                                        GKS18430
      CALL SETUPF(.TRUE.,.FALSE.)                                       GKS18440
      CALL GTXS(TXTX,TXTY,10,'CELL ARRAY')                              GKS18450
      CALL FINSHF(.TRUE.)                                               GKS18460
                                                                        GKS18470
C***********************************************************************GKS18480
C*****                                                               ***GKS18490
C*****   Use the values store in PIXEL2 array to display a castle    ***GKS18500
C*****   tower using the cell array primitive call.                  ***GKS18510
C*****                                                               ***GKS18520
C***********************************************************************GKS18530
                                                                        GKS18540
      CALL GCA(0.300,0.100,0.700,0.700,9,9,1,1,9,9,PIXEL1)              GKS18550
                                                                        GKS18560
C***********************************************************************GKS18570
C*****                                                               ***GKS18580
C*****   Call RMENU to use pack data record to indicate two prompts  ***GKS18590
C*****   for the lpfks (next and return). RMENU will call RCHOI      ***GKS18600
C*****   to wait for a valid choice selection and will pass that     ***GKS18610
C*****   selection back in CHNR.                                     ***GKS18620
C*****                                                               ***GKS18630
C***********************************************************************GKS18640
                                                                        GKS18650
		CONT = .TRUE.
	CALL SLEEP(4)
                                                                        GKS18670
C***********************************************************************GKS18680
C*****                                                               ***GKS18690
C*****   If user chose to continue viewing the cell array panels,    ***GKS18700
C*****   continue on. Otherwise, fall out the bottom and return      ***GKS18710
C*****   to the output primitive screen.                             ***GKS18720
C*****                                                               ***GKS18730
C***********************************************************************GKS18740
                                                                        GKS18750
      IF (CONT) THEN                                                    GKS18760
                                                                        GKS18770
C***********************************************************************GKS18780
C*****                                                               ***GKS18790
C*****   Call SETUPF and FINSHF to draw the border and title of      ***GKS18800
C*****   the demo frame.                                             ***GKS18810
C*****                                                               ***GKS18820
C***********************************************************************GKS18830
                                                                        GKS18840
        CALL SETUPF(.FALSE.,.FALSE.)                                    GKS18850
        CALL GTXS(TXTX,TXTY,16,'EXPAND RECTANGLE')                      GKS18860
        CALL FINSHF(.FALSE.)                                            GKS18870
                                                                        GKS18880
C***********************************************************************GKS18890
C*****                                                               ***GKS18900
C*****   Show how the cell array can be used easily to vary the      ***GKS18910
C*****   picture without changing the data.                          ***GKS18920
C*****                                                               ***GKS18930
C***********************************************************************GKS18940
                                                                        GKS18950
        Y1 = 0.150                                                      GKS18960
        DO 700 I=1,4                                                    GKS18970
          X1 = 0.500 - I * 0.050                                        GKS18980
          X2 = 0.500 + I * 0.050                                        GKS18990
          Y2 = 0.150 + I * 0.100                                        GKS19000
          CALL GCA(X1,Y1,X2,Y2,20,20,1,1,20,20,PIXEL2)                  GKS19010
  700   CONTINUE                                                        GKS19020
                                                                        GKS19030
C***********************************************************************GKS19040
C*****                                                               ***GKS19050
C*****   Call RMENU to use pack data record to indicate two prompts  ***GKS19060
C*****   for the lpfks (next and return). RMENU will call RCHOI      ***GKS19070
C*****   to wait for a valid choice selection and will pass that     ***GKS19080
C*****   selection back in CHNR.                                     ***GKS19090
C*****                                                               ***GKS19100
C***********************************************************************GKS19110
                                                                        GKS19120
	CONT = .TRUE.
	CALL SLEEP(4)
                                                                        GKS19140
C***********************************************************************GKS19150
C*****                                                               ***GKS19160
C*****   If user chose to continue viewing the cell array panels,    ***GKS19170
C*****   continue on. Otherwise, fall out the bottom and return      ***GKS19180
C*****   to the output primitive screen.                             ***GKS19190
C*****                                                               ***GKS19200
C***********************************************************************GKS19210
                                                                        GKS19220
        IF (CONT) THEN                                                  GKS19230
                                                                        GKS19240
C***********************************************************************GKS19250
C*****                                                               ***GKS19260
C*****   Call SETUPF and FINSHF to draw the border and title of      ***GKS19270
C*****   the demo frame.                                             ***GKS19280
C*****                                                               ***GKS19290
C***********************************************************************GKS19300
                                                                        GKS19310
          CALL SETUPF(.FALSE.,.FALSE.)                                  GKS19320
          CALL GTXS(TXTX,TXTY,22,'DIFFERENT ASPECT RATIO')              GKS19330
          CALL FINSHF(.FALSE.)                                          GKS19340
                                                                        GKS19350
C***********************************************************************GKS19360
C*****                                                               ***GKS19370
C*****   Draw three copies of the tree seen in the previous panel,   ***GKS19380
C*****   showing various aspect ratios.                              ***GKS19390
C*****                                                               ***GKS19400
C***********************************************************************GKS19410
                                                                        GKS19420
          CALL GCA(0.100,0.615,0.650,0.815,20,20,1,1,20,20,PIXEL2)      GKS19430
          CALL GCA(0.100,0.100,0.650,0.565,20,20,1,1,20,20,PIXEL2)      GKS19440
          CALL GCA(0.700,0.100,0.900,0.815,20,20,1,1,20,20,PIXEL2)      GKS19450
                                                                        GKS19460
C***********************************************************************GKS19470
C*****                                                               ***GKS19480
C*****   Call RMENU to use pack data record to indicate to prompt    ***GKS19490
C*****   for the lpfk (return). RMENU will call RCHOI to wait for    ***GKS19500
C*****   a valid choice selection and will pass that selection back  ***GKS19510
C*****   in CHNR. But in this case, the user could only select       ***GKS19520
C*****   return as a valid choice, so we will fall down to the       ***GKS19530
C*****   return statement and go back to the output primitves.       ***GKS19540
C*****                                                               ***GKS19550
C***********************************************************************GKS19560
                                                                        GKS19570
	  CONT = .TRUE.
	CALL SLEEP(4)
        ENDIF                                                           GKS19590
      ENDIF                                                             GKS19600
                                                                        GKS19610
      RETURN                                                            GKS19620
      END                                                               GKS19630
                                                                        GKS19640
                                                                        GKS19650
                                                                        GKS19660
C***********************************************************************GKS19670
C***********************************************************************GKS19680
C*****                                                               ***GKS19690
C*****   Subroutine Name: DEMOFA                                     ***GKS19700
C*****                                                               ***GKS19710
C*****   Subroutine Function: This subroutine demonstrates the       ***GKS19720
C*****                        fill area primitive and its            ***GKS19730
C*****                        attributes.                            ***GKS19740
C*****                                                               ***GKS19750
C*****   Calls Subroutines:   SETUPF,FINSHF,RMENU                    ***GKS19760
C*****                                                               ***GKS19770
C***********************************************************************GKS19780
C***********************************************************************GKS19790
                                                                        GKS19800
      SUBROUTINE DEMOFA                                                 GKS19810
      INCLUDE 'gkspar.inc'
                                                                        GKS19830
      INTEGER*4    WKID,WTYPE,I,J,K,START,II                            GKS19840
      REAL*4       STARTX,STARTY,ALPHA,PI,TM(2,3)                       GKS19850
      REAL*4       PX(20),PY(20),RX(20),RY(20),BLOCK(8)                 GKS19860
      REAL*4       EXOR3(6),RPLC3(6),EXOR4(8),RPLC4(8)                  GKS19870
      REAL*4       EXOR5(10),RPLC5(10),XMIN,XMAX,YMIN,YMAX              GKS19880
      LOGICAL      CONT                                                 GKS19890
      CHARACTER*10 TEXT(4)                                              GKS19900
      COMMON       /WINF/   WKID,WTYPE                                  GKS19910
      COMMON       /TEXT/   TXTX,TXTY                                   GKS19920
      COMMON       /LIMITS/ XMIN,XMAX,YMIN,YMAX                         GKS19930
                                                                        GKS19940
      DATA  PI      /3.141593/                                          GKS19950
                                                                        GKS19960
      DATA  TEXT(1) /'HOLLOW    '/                                      GKS19970
      DATA  TEXT(2) /'SOLID     '/                                      GKS19980
      DATA  TEXT(3) /'PATTERN   '/                                      GKS19990
      DATA  TEXT(4) /'HATCH     '/                                      GKS20000
                                                                        GKS20010
      DATA  BLOCK /0.00,0.00,0.50,0.50,0.00,0.12,0.12,0.00/             GKS20020
      DATA  EXOR3 /0.10,0.40,0.40,0.15,0.15,0.75/                       GKS20030
      DATA  RPLC3 /0.60,0.90,0.90,0.15,0.15,0.75/                       GKS20040
      DATA  EXOR4 /0.05,0.45,0.45,0.05,0.10,0.10,0.40,0.40/             GKS20050
      DATA  RPLC4 /0.55,0.95,0.95,0.55,0.10,0.10,0.40,0.40/             GKS20060
      DATA  EXOR5 /0.15,0.35,0.45,0.25,0.05,0.25,0.25,0.45,0.55,0.45/   GKS20070
      DATA  RPLC5 /0.65,0.85,0.95,0.75,0.55,0.25,0.25,0.45,0.55,0.45/   GKS20080
                                                                        GKS20090
C***********************************************************************GKS20100
C*****                                                               ***GKS20110
C*****   Call subroutines SETUPF and FINSHF to draw the border and   ***GKS20120
C*****   border and title of the demo frame. Then draw the first     ***GKS20130
C*****   panel which is a picture drawn using cell arrays.           ***GKS20140
C*****                                                               ***GKS20150
C***********************************************************************GKS20160
                                                                        GKS20170
      CALL SETUPF(.TRUE.,.FALSE.)                                       GKS20180
      CALL GTXS(TXTX,TXTY,9,'FILL AREA')                                GKS20190
      CALL FINSHF(.TRUE.)                                               GKS20200
                                                                        GKS20210
C***********************************************************************GKS20220
C*****                                                               ***GKS20230
C*****   Draw a hot pink, fancy nine pointed star using the fill     ***GKS20240
C*****   area primitive with a solid interior style.                 ***GKS20250
C*****                                                               ***GKS20260
C***********************************************************************GKS20270
                                                                        GKS20280
      CALL GSFAIS(GSOLID)                                               GKS20290
      CALL GSFACI(GLMGNT)                                               GKS20300
      ALPHA = 0.0                                                       GKS20310
      DO 800 I=1,9                                                      GKS20320
        RX(I) = 0.500 + 0.350 * SIN(ALPHA)                              GKS20330
        RY(I) = 0.375 + 0.350 * COS(ALPHA)                              GKS20340
        ALPHA = ALPHA + 2.0 * PI / 9.0                                  GKS20350
  800 CONTINUE                                                          GKS20360
      START = 0                                                         GKS20370
      DO 810 I=1,9                                                      GKS20380
        PX(I) = RX(START+1)                                             GKS20390
        PY(I) = RY(START+1)                                             GKS20400
        START = MOD(START+4,9)                                          GKS20410
  810 CONTINUE                                                          GKS20420
      CALL GFA(9,PX,PY)                                                 GKS20430
                                                                        GKS20440
C***********************************************************************GKS20450
C*****                                                               ***GKS20460
C*****   Call RMENU to use pack data record to indicate two prompts  ***GKS20470
C*****   for the lpfks (next and return). RMENU will call RCHOI      ***GKS20480
C*****   to wait for a valid choice selection and will pass that     ***GKS20490
C*****   selection back in CHNR.                                     ***GKS20500
C*****                                                               ***GKS20510
C***********************************************************************GKS20520
                                                                        GKS20530
	CONT = .TRUE.
	CALL SLEEP(4)
                                                                        GKS20550
C***********************************************************************GKS20560
C*****                                                               ***GKS20570
C*****   If user chose to continue viewing the cell array panels,    ***GKS20580
C*****   continue on. Otherwise, fall out the bottom and return      ***GKS20590
C*****   to the output primitive screen.                             ***GKS20600
C*****                                                               ***GKS20610
C***********************************************************************GKS20620
                                                                        GKS20630
      IF (CONT) THEN                                                    GKS20640
                                                                        GKS20650
C***********************************************************************GKS20660
C*****                                                               ***GKS20670
C*****   Call SETUPF and FINSHF to draw the border and title of      ***GKS20680
C*****   the demo frame.                                             ***GKS20690
C*****                                                               ***GKS20700
C***********************************************************************GKS20710
                                                                        GKS20720
        CALL SETUPF(.FALSE.,.FALSE.)                                    GKS20730
        CALL GTXS(TXTX,TXTY,15,'INTERIOR STYLES')                       GKS20740
        CALL FINSHF(.FALSE.)                                            GKS20750
                                                                        GKS20760
C***********************************************************************GKS20770
C*****                                                               ***GKS20780
C*****   Draw four rectangles on the right side of the frame to      ***GKS20790
C*****   show each of the interior styles for fill areas. Label      ***GKS20800
C*****   each style appropriately on the left hand side.             ***GKS20810
C*****                                                               ***GKS20820
C***********************************************************************GKS20830
                                                                        GKS20840
        STARTX = 0.450                                                  GKS20850
        CALL GSCHH(0.050)                                               GKS20860
        CALL GSTXAL(GAHNOR,GAVNOR)                                      GKS20870
        CALL GSTXFP(GFONT1,GSTRKP)                                      GKS20880
        DO 830 I=1,4                                                    GKS20890
          STARTY = 0.950 - I * 0.220                                    GKS20900
          DO 820 J=1,4                                                  GKS20910
            PX(J) = BLOCK(J) + STARTX                                   GKS20920
            PY(J) = BLOCK(J+4) + STARTY                                 GKS20930
  820     CONTINUE                                                      GKS20940
          STARTY = STARTY + 0.050                                       GKS20950
          CALL GTXS(0.100,STARTY,10,TEXT(I))                            GKS20960
          IF (I.EQ.4) THEN                                              GKS20970
            CALL GSFASI(59)                                             GKS20980
          ENDIF                                                         GKS20990
          CALL GSFAIS(I-1)                                              GKS21000
          CALL GSFACI(I)                                                GKS21010
          CALL GFA(4,PX,PY)                                             GKS21020
  830   CONTINUE                                                        GKS21030
        CALL GSFASI(1)                                                  GKS21040
                                                                        GKS21050
C***********************************************************************GKS21060
C*****                                                               ***GKS21070
C*****   Call RMENU to use pack data record to indicate two prompts  ***GKS21080
C*****   for the lpfks (next and return). RMENU will call RCHOI      ***GKS21090
C*****   to wait for a valid choice selection and will pass that     ***GKS21100
C*****   selection back in CHNR.                                     ***GKS21110
C*****                                                               ***GKS21120
C***********************************************************************GKS21130
                                                                        GKS21140
	CONT = .TRUE.
	CALL SLEEP(4)
                                                                        GKS21160
C***********************************************************************GKS21170
C*****                                                               ***GKS21180
C*****   If user chose to continue viewing the cell array panels,    ***GKS21190
C*****   continue on. Otherwise, fall out the bottom and return      ***GKS21200
C*****   to the output primitive screen.                             ***GKS21210
C*****                                                               ***GKS21220
C***********************************************************************GKS21230
                                                                        GKS21240
        IF (CONT) THEN                                                  GKS21250
                                                                        GKS21260
C***********************************************************************GKS21270
C*****                                                               ***GKS21280
C*****   Call SETUPF and FINSHF to draw the border and title of      ***GKS21290
C*****   the demo frame.                                             ***GKS21300
C*****                                                               ***GKS21310
C***********************************************************************GKS21320
                                                                        GKS21330
          CALL SETUPF(.FALSE.,.FALSE.)                                  GKS21340
          CALL GTXS(TXTX,TXTY,15,'TRANSFORMATIONS')                     GKS21350
          CALL FINSHF(.FALSE.)                                          GKS21360
                                                                        GKS21370
C***********************************************************************GKS21380
C*****                                                               ***GKS21390
C*****   Show a picture of fill area transformations. Draw a         ***GKS21400
C*****   triangle and rotate it. Draw a hexagon and zoom it.         ***GKS21410
C*****                                                               ***GKS21420
C***********************************************************************GKS21430
                                                                        GKS21440
          CALL GSCHH(0.050)                                             GKS21450
          CALL GSTXFP(GFONT1,GSTRKP)                                    GKS21460
          CALL GSFACI(GLGREN)                                           GKS21470
          CALL GCRSG(1)                                                 GKS21480
          CALL GSFAIS(GPATTR)                                           GKS21490
          CALL GFA(3,EXOR3(1),EXOR3(4))                                 GKS21500
          CALL GCLSG                                                    GKS21510
          CALL GSFAIS(GSOLID)                                           GKS21520
          CALL GFA(3,RPLC3(1),RPLC3(4))                                 GKS21530
          CALL GSFACI(GLGREN)                                           GKS21540
          CALL GFA(5,EXOR5(1),EXOR5(6))                                 GKS21550
          CALL GSFAIS(GPATTR)                                           GKS21560
          CALL GCRSG(2)                                                 GKS21570
          CALL GFA(5,RPLC5(1),RPLC5(6))                                 GKS21580
          CALL GCLSG                                                    GKS21590
          CALL GSVP(1,0.015,0.985,0.015,0.985)                          GKS21600
	  CALL GCRSG(5)
          CALL GTXS(0.050,0.825,8,'ROTATION')                           GKS21610
          CALL GTXS(0.550,0.825,7,'ZOOMING')                            GKS21640
	  CALL GCLSG
          CALL SLEEP(2)
          CALL GEVTM(0.250,0.450,0.000,0.000,PI/4.0,1.0,1.0,0,TM)       GKS21620
          CALL GSSGT(1,TM)                                              GKS21630
          CALL GEVTM(0.750,0.400,0.000,0.000,0.0,0.5,0.5,0,TM)          GKS21650
          CALL GSSGT(2,TM)                                              GKS21660
          CALL GSVP(1,0.000,1.000,0.000,1.000)                          GKS21670
          CALL GRSGWK(WKID)                                             GKS21680
C***********************************************************************GKS21690
C*****                                                               ***GKS21700
C*****   Call RMENU to use pack data record to indicate to prompts   ***GKS21710
C*****   for the lpfks (next and return). RMENU will call RCHOI      ***GKS21720
C*****   to wait for a valid choice selection and will pass that     ***GKS21730
C*****   selection back in CHNR.                                     ***GKS21740
C*****                                                               ***GKS21750
C***********************************************************************GKS21760
                                                                        GKS21770
	  CONT = .TRUE.
	CALL SLEEP(4)
	  CALL GDSG(1259)
                                                                        GKS21790
C***********************************************************************GKS21800
C*****                                                               ***GKS21810
C*****   If user chose to continue viewing the text panels,          ***GKS21820
C*****   continue on. Otherwise, fall out the bottom and return      ***GKS21830
C*****   to the output primitive screen.                             ***GKS21840
C*****                                                               ***GKS21850
C***********************************************************************GKS21860
                                                                        GKS21870
          IF (CONT) THEN                                                GKS21880
                                                                        GKS21890
C***********************************************************************GKS21900
C*****                                                               ***GKS21910
C*****   Call SETUPF and FINSHF to draw the border and title of      ***GKS21920
C*****   the demo frame.                                             ***GKS21930
C*****                                                               ***GKS21940
C***********************************************************************GKS21950
                                                                        GKS21960
            CALL SETUPF(.FALSE.,.FALSE.)                                GKS21970
            CALL GTXS(TXTX,TXTY,12,'HATCH STYLES')                      GKS21980
            CALL FINSHF(.FALSE.)                                        GKS21990
                                                                        GKS22000
C***********************************************************************GKS22010
C*****                                                               ***GKS22020
C*****   Draw a screen full of all the different hatch styles that   ***GKS22030
C*****   are supported by GKS-CO.                                    ***GKS22040
C*****                                                               ***GKS22050
C***********************************************************************GKS22060
                                                                        GKS22070
            CALL GSFAIS(GHATCH)                                         GKS22080
            CALL GSFACI (2)                                             GKS22090
            II = 0                                                      GKS22100
            DO 850 K=1,5                                                GKS22110
              DO 840 J=1,4                                              GKS22120
                II = II + 1                                             GKS22140
                CALL GSFASI(-(II))                                      GKS22130
                PX(1) = 0.09 + (J-1) * 0.21                             GKS22180
                PY(1) = 0.05 + (K-1) * 0.17                             GKS22190
                PX(2) = PX(1) + 0.19                                    GKS22200
                PY(2) = PY(1)                                           GKS22210
                PX(3) = PX(2)                                           GKS22220
                PY(3) = PY(1) + 0.15                                    GKS22230
                PX(4) = PX(1)                                           GKS22240
                PY(4) = PY(3)                                           GKS22250
                CALL GFA(4,PX,PY)                                       GKS22260
  840         CONTINUE                                                  GKS22270
  850       CONTINUE                                                    GKS22280
                                                                        GKS22290
C***********************************************************************GKS22300
C*****                                                               ***GKS22310
C*****   Call RMENU to use pack data record to indicate to prompt    ***GKS22320
C*****   for the lpfk (return). RMENU will call RCHOI to wait for    ***GKS22330
C*****   a valid choice selection and will pass that selection back  ***GKS22340
C*****   in CHNR. But in this case, the user could only select       ***GKS22350
C*****   return as a valid choice, so we will fall down to the       ***GKS22360
C*****   return statement and go back to the output primitves.       ***GKS22370
C*****                                                               ***GKS22380
C***********************************************************************GKS22390
                                                                        GKS22400
	    CONT = .TRUE.
	CALL SLEEP(4)
          ENDIF                                                         GKS22420
        ENDIF                                                           GKS22430
      ENDIF                                                             GKS22440
                                                                        GKS22450
      RETURN                                                            GKS22460
      END                                                               GKS22470
                                                                        GKS22480
                                                                        GKS22490
                                                                        GKS22500
C***********************************************************************GKS22510
C***********************************************************************GKS22520
C*****                                                               ***GKS22530
C*****   Subroutine Name: DEMOGD                                     ***GKS22540
C*****                                                               ***GKS22550
C*****   Subroutine Function: As there are no Generalized Drawing    ***GKS22560
C*****                        primitives yet available in GKS-CO,    ***GKS22570
C*****                        this routine simply fills the screen   ***GKS22580
C*****                        with 'not available' and waits for     ***GKS22590
C*****                        the user to chose to go on.            ***GKS22600
C*****                                                               ***GKS22610
C*****   Calls Subroutines:   SETUPF,FINSHF,RMENU                    ***GKS22620
C*****                                                               ***GKS22630
C***********************************************************************GKS22640
C***********************************************************************GKS22650
                                                                        GKS22660
      SUBROUTINE DEMOGD                                                 GKS22670
                                                                        GKS22680
      REAL*4       TXTX,TXTY                                            GKS22690
      LOGICAL      CONT                                                 GKS22700
      COMMON       /TEXT/ TXTX,TXTY                                     GKS22710
                                                                        GKS22720
      CALL SETUPF(.TRUE.,.FALSE.)                                       GKS22730
      CALL GTXS(TXTX,TXTY,5,'G D P')                                    GKS22740
      CALL FINSHF(.TRUE.)                                               GKS22750
      CALL GTXS(0.10,0.350,13,'NOT AVAILABLE')                          GKS22760
      CONT = .TRUE.
	CALL SLEEP(4)
      RETURN                                                            GKS22780
      END                                                               GKS22790
                                                                        GKS22800
                                                                        GKS22810
                                                                        GKS22820
C***********************************************************************GKS22830
C***********************************************************************GKS22840
C*****                                                               ***GKS22850
C*****   Subroutine Name: DEMOPL                                     ***GKS22860
C*****                                                               ***GKS22870
C*****   Subroutine Function: Draw a car to demonstrate the          ***GKS22880
C*****                        polyline output primitive. Draw the    ***GKS22890
C*****                        different linetypes. Show the various  ***GKS22900
C*****                        line attributes on a demo frame. And   ***GKS22910
C*****                        draw two different pictures of the     ***GKS22920
C*****                        sunflower star by using the same       ***GKS22930
C*****                        x,y data but varying the viewport.     ***GKS22940
C*****                                                               ***GKS22950
C*****   Calls Subroutines:   SETUPF,FINSHF,DRWHLS,RMENU             ***GKS22960
C*****                                                               ***GKS22970
C***********************************************************************GKS22980
C***********************************************************************GKS22990
                                                                        GKS23000
      SUBROUTINE DEMOPL                                                 GKS23010
      INCLUDE 'gkspar.inc'
                                                                        GKS23030
      INTEGER*4    WKID,WTYPE,N,I,ERRIND,NLT,LT,NLW,NPPLI,K             GKS23040
                                                                        GKS23050
      REAL*4       NOMLW,RLWMIN,RLWMAX                                  GKS23060
      REAL*4       OX(28),OY(28),KFX(26),KFY(26),UX(6),UY(6)            GKS23070
      REAL*4       DTX(11),DTY(11),MITX,MITY,R,ALPHA,RX(21),RY(21)      GKS23080
      REAL*4       T1X(2),T1Y(2),T2X(2),T3X(2),T3Y(2),T4X(4),T4Y(4)     GKS23090
      REAL*4       GR1X(2),GR2X(2),GR1Y(2),VDX(3),VDY(3)                GKS23100
      REAL*4       K1X(3),K1Y(3),K2X(3),K2Y(3),K3X(2),K3Y(2)            GKS23110
      REAL*4       G1X(2),G2X(2),G3X(2),G4X(2),G5X(2),G6X(2)            GKS23120
      REAL*4       G7X(2),G8X(2),G1Y(2),G2Y(2),G3Y(2),G4Y(2)            GKS23130
      REAL*4       G5Y(2),G6Y(2),G7Y(2),G8Y(2),XMIN,XMAX,YMIN,YMAX      GKS23140
      REAL*4       KOX(11),KOY(11),FIX(9),FIY(9)                        GKS23150
      REAL*4       PLX(2),PLY(2),RLX(20),RLY(20)                        GKS23160
                                                                        GKS23170
      LOGICAL      CONT                                                 GKS23180
                                                                        GKS23190
      COMMON       /WINF/   WKID,WTYPE                                  GKS23200
      COMMON       /TEXT/   TXTX,TXTY                                   GKS23210
      COMMON       /LIMITS/ XMIN,XMAX,YMIN,YMAX                         GKS23220
                                                                        GKS23230
C  CAR DATA                                                             GKS23240
                                                                        GKS23250
      DATA    OX   /0.264,0.272,0.770,0.776,0.774,0.608,0.594,0.588,    GKS23260
     *              0.564,0.330,0.320,0.310,0.300,0.290,0.280,0.268,    GKS23270
     *              0.264,0.258,0.250,0.240,0.234,0.228,0.224,0.222,    GKS23280
     *              0.224,0.280,0.288,0.290/                            GKS23290
      DATA    OY   /0.396,0.398,0.400,0.406,0.416,0.418,0.464,0.466,    GKS23300
     *              0.468,0.468,0.466,0.462,0.456,0.445,0.430,0.400,    GKS23310
     *              0.396,0.394,0.390,0.380,0.370,0.348,0.320,0.296,    GKS23320
     *              0.286,0.284,0.286,0.304/                            GKS23330
      DATA    KFX  /0.222,0.230,0.286,0.300,0.310,0.321,0.340,0.358,    GKS23340
     *              0.370,0.378,0.388,0.394,0.398,0.400,0.574,0.584,    GKS23350
     *              0.726,0.736,0.746,0.760,0.770,0.780,0.792,0.802,    GKS23360
     *              0.808,0.812/                                        GKS23370
      DATA    KFY  /0.296,0.300,0.350,0.360,0.366,0.370,0.372,0.370,    GKS23380
     *              0.364,0.358,0.346,0.332,0.316,0.294,0.294,0.297,    GKS23390
     *              0.371,0.373,0.374,0.372,0.370,0.366,0.358,0.346,    GKS23400
     *              0.332,0.310/                                        GKS23410
      DATA    UX   /0.382,0.384,0.390,0.690,0.698,0.702/                GKS23420
      DATA    UY   /0.302,0.286,0.282,0.282,0.290,0.302/                GKS23430
      DATA    DTX  /0.584,0.584,0.580,0.576,0.572,0.564,0.550,0.320,    GKS23440
     *              0.308,0.295,0.280/                                  GKS23450
      DATA    DTY  /0.297,0.399,0.424,0.440,0.448,0.452,0.454,0.454,    GKS23460
     *              0.452,0.448,0.430/                                  GKS23470
      DATA    T1X  /0.480,0.484/                                        GKS23480
      DATA    T1Y  /0.454,0.294/                                        GKS23490
      DATA    T2X  /0.476,0.480/                                        GKS23500
      DATA    T3X  /0.378,0.378/                                        GKS23510
      DATA    T3Y  /0.454,0.358/                                        GKS23520
      DATA    T4X  /0.584,0.590,0.598,0.608/                            GKS23530
      DATA    T4Y  /0.399,0.406,0.414,0.418/                            GKS23540
      DATA    GR1X /0.462,0.472/                                        GKS23550
      DATA    GR1Y /0.386,0.386/                                        GKS23560
      DATA    GR2X /0.488,0.498/                                        GKS23570
      DATA    VDX  /0.572,0.576,0.594/                                  GKS23580
      DATA    VDY  /0.448,0.454,0.464/                                  GKS23590
      DATA    K1X  /0.764,0.760,0.760/                                  GKS23600
      DATA    K1Y  /0.416,0.400,0.372/                                  GKS23610
      DATA    K2X  /0.774,0.770,0.770/                                  GKS23620
      DATA    K2Y  /0.416,0.400,0.369/                                  GKS23630
      DATA    K3X  /0.776,0.776/                                        GKS23640
      DATA    K3Y  /0.406,0.368/                                        GKS23650
      DATA    KOX  /0.793,0.795,0.812,0.816,0.824,0.822,0.816,0.806,    GKS23660
     *              0.796,0.786,0.776/                                  GKS23670
      DATA    KOY  /0.314,0.306,0.310,0.312,0.324,0.336,0.350,0.368,    GKS23680
     *              0.376,0.380,0.382/                                  GKS23690
      DATA    G1X  /0.670,0.672/                                        GKS23700
      DATA    G1Y  /0.390,0.378/                                        GKS23710
      DATA    G2X  /0.680,0.686/                                        GKS23720
      DATA    G2Y  /0.390,0.352/                                        GKS23730
      DATA    G3X  /0.690,0.696/                                        GKS23740
      DATA    G3Y  /0.390,0.357/                                        GKS23750
      DATA    G4X  /0.700,0.706/                                        GKS23760
      DATA    G4Y  /0.390,0.363/                                        GKS23770
      DATA    G5X  /0.710,0.715/                                        GKS23780
      DATA    G5Y  /0.390,0.367/                                        GKS23790
      DATA    G6X  /0.720,0.725/                                        GKS23800
      DATA    G6Y  /0.390,0.371/                                        GKS23810
      DATA    G7X  /0.730,0.735/                                        GKS23820
      DATA    G7Y  /0.390,0.373/                                        GKS23830
      DATA    G8X  /0.740,0.745/                                        GKS23840
      DATA    G8Y  /0.390,0.374/                                        GKS23850
      DATA  FIX /0.766,0.766,0.768,0.760,0.776,0.768,0.772,0.774,0.774/ GKS23860
      DATA  FIY /0.416,0.420,0.420,0.434,0.428,0.428,0.420,0.420,0.416/ GKS23870
                                                                        GKS23880
C***********************************************************************GKS23890
C*****                                                               ***GKS23900
C*****   Inquire information about the polyline facilities provided  ***GKS23910
C*****   by GKS-CO. Call subroutines SETUPF and FINSHF to draw the   ***GKS23920
C*****   border and title of the demo frame. Then draw the car       ***GKS23930
C*****   using all the data provided. The car will be red, the       ***GKS23940
C*****   hood ornament yellow, and the wheels white.                 ***GKS23950
C*****                                                               ***GKS23960
C***********************************************************************GKS23970
                                                                        GKS23980
                                                                        GKS23990
      CALL GQPLF(WTYPE,1,ERRIND,NLT,LT,NLW,NOMLW,RLWMIN,RLWMAX,NPPLI)   GKS24000
                                                                        GKS24010
      CALL SETUPF(.TRUE.,.FALSE.)                                       GKS24020
      CALL GTXS(TXTX,TXTY,8,'POLYLINE')                                 GKS24030
      CALL FINSHF(.TRUE.)                                               GKS24040
                                                                        GKS24050
      CALL GSWN(1,0.18,0.88,0.10,0.80)                                  GKS24060
      CALL GSPLCI(GRED)                                                 GKS24070
      CALL GPL(28,OX,OY)                                                GKS24080
      CALL GPL(26,KFX,KFY)                                              GKS24090
      CALL GPL(6,UX,UY)                                                 GKS24100
      CALL GPL(11,DTX,DTY)                                              GKS24110
      CALL GPL(11,KOX,KOY)                                              GKS24120
      CALL GPL(2,T1X,T1Y)                                               GKS24130
      CALL GPL(2,T2X,T1Y)                                               GKS24140
      CALL GPL(2,T3X,T3Y)                                               GKS24150
      CALL GPL(4,T4X,T4Y)                                               GKS24160
      CALL GPL(2,GR1X,GR1Y)                                             GKS24170
      CALL GPL(2,GR2X,GR1Y)                                             GKS24180
      CALL GPL(3,VDX,VDY)                                               GKS24190
      CALL GPL(3,K1X,K1Y)                                               GKS24200
      CALL GPL(3,K2X,K1Y)                                               GKS24210
      CALL GPL(2,K3X,K3Y)                                               GKS24220
      CALL GPL(2,G1X,G1Y)                                               GKS24230
      CALL GPL(2,G2X,G2Y)                                               GKS24240
      CALL GPL(2,G3X,G3Y)                                               GKS24250
      CALL GPL(2,G4X,G4Y)                                               GKS24260
      CALL GPL(2,G5X,G5Y)                                               GKS24270
      CALL GPL(2,G6X,G6Y)                                               GKS24280
      CALL GPL(2,G7X,G7Y)                                               GKS24290
      CALL GPL(2,G8X,G8Y)                                               GKS24300
      CALL GSPLCI(GYELOW)                                               GKS24310
      CALL GPL(9,FIX,FIY)                                               GKS24320
                                                                        GKS24330
C***********************************************************************GKS24340
C*****                                                               ***GKS24350
C*****   Change the color to grayish white and call subroutine       ***GKS24360
C*****   DRWHLS to draw each of the car wheels - inside and outside  ***GKS24370
C*****   borders.                                                    ***GKS24380
C*****                                                               ***GKS24390
C***********************************************************************GKS24400
                                                                        GKS24410
      CALL GSPLCI(GGRAY)                                                GKS24420
      MITX = 0.336                                                      GKS24430
      MITY = 0.302                                                      GKS24440
      N = 14                                                            GKS24450
      R = 0.020                                                         GKS24460
      CALL DRWHLS(MITX,MITY,N,R)                                        GKS24470
      N = 20                                                            GKS24480
      R = 0.044                                                         GKS24490
      CALL DRWHLS(MITX,MITY,N,R)                                        GKS24500
      MITX = 0.748                                                      GKS24510
      N = 14                                                            GKS24520
      R = 0.020                                                         GKS24530
      CALL DRWHLS(MITX,MITY,N,R)                                        GKS24540
      N = 20                                                            GKS24550
      R = 0.044                                                         GKS24560
      CALL DRWHLS(MITX,MITY,N,R)                                        GKS24570
      MITX = 0.640                                                      GKS24580
      MITY = 0.350                                                      GKS24590
      N = 14                                                            GKS24600
      R = 0.020                                                         GKS24610
      CALL DRWHLS(MITX,MITY,N,R)                                        GKS24620
      R = 0.044                                                         GKS24630
      ALPHA = 3.80                                                      GKS24640
      DO 900 I=1,14                                                     GKS24650
        RX(I) = MITX + R * SIN(ALPHA)                                   GKS24660
        RY(I) = MITY + R * COS(ALPHA)                                   GKS24670
        ALPHA = ALPHA + 0.310                                           GKS24680
  900 CONTINUE                                                          GKS24690
      CALL GPL(14,RX,RY)                                                GKS24700
      CALL GSWN(1,0.0,1.0,0.0,1.0)                                      GKS24710
                                                                        GKS24720
C***********************************************************************GKS24730
C*****                                                               ***GKS24740
C*****   Call RMENU to use pack data record to indicate two prompts  ***GKS24750
C*****   for the lpfks (next and return). RMENU will call RCHOI      ***GKS24760
C*****   to wait for a valid choice selection and will pass that     ***GKS24770
C*****   selection back in CHNR.                                     ***GKS24780
C*****                                                               ***GKS24790
C***********************************************************************GKS24800
                                                                        GKS24810
      CONT = .TRUE.
	CALL SLEEP(4)
                                                                        GKS24830
C***********************************************************************GKS24840
C*****                                                               ***GKS24850
C*****   If user chose to continue viewing the polyline panels,      ***GKS24860
C*****   continue on. Otherwise, fall out the bottom and return      ***GKS24870
C*****   to the output primitive screen.                             ***GKS24880
C*****                                                               ***GKS24890
C***********************************************************************GKS24900
                                                                        GKS24910
      IF (CONT) THEN                                                    GKS24920
                                                                        GKS24930
C***********************************************************************GKS24940
C*****                                                               ***GKS24950
C*****   Call SETUPF and FINSHF to draw the border and title of      ***GKS24960
C*****   the demo frame.                                             ***GKS24970
C*****                                                               ***GKS24980
C***********************************************************************GKS24990
                                                                        GKS25000
        CALL SETUPF(.FALSE.,.FALSE.)                                    GKS25010
        CALL GTXS(TXTX,TXTY,9,'LINETYPES')                              GKS25020
        CALL FINSHF(.FALSE.)                                            GKS25030
                                                                        GKS25040
C***********************************************************************GKS25050
C*****                                                               ***GKS25060
C*****   Draw a horizontal line in the linetype specified on the     ***GKS25070
C*****   left half of the demo frame for each linetype supported     ***GKS25080
C*****   by GKS-CO.                                                  ***GKS25090
C*****                                                               ***GKS25100
C***********************************************************************GKS25110
                                                                        GKS25120
        RLX(1) = 0.60                                                   GKS25130
        PLX(1) = 0.05                                                   GKS25140
        PLX(2) = 0.45                                                   GKS25150
        CALL GSPLCI(GWHITE)                                             GKS25160
        DO 920 I=1,NLT                                                  GKS25170
          CALL GSLN(I)                                                  GKS25180
          PLY(1) = (0.90 - (0.98 * REAL(I) / REAL(NLT+1)))              GKS25190
          PLY(2) = PLY(1)                                               GKS25200
          CALL GPL(2,PLX,PLY)                                           GKS25210
          ALPHA = 0.0                                                   GKS25220
          RLY(1) = PLY(1)                                               GKS25230
                                                                        GKS25240
C***********************************************************************GKS25250
C*****                                                               ***GKS25260
C*****   Draw a fancy set of diagonal lines in each linetype.        ***GKS25270
C*****                                                               ***GKS25280
C***********************************************************************GKS25290
                                                                        GKS25300
          DO 910 J=1,5                                                  GKS25310
            RLX(2) = RLX(1) + 0.3 * SIN(ALPHA)                          GKS25320
            RLY(2) = RLY(1) + (0.8 / REAL(NLT+1)) * COS(ALPHA)          GKS25330
            ALPHA = ALPHA + 3.141593 / 8.0                              GKS25340
            CALL GPL(2,RLX,RLY)                                         GKS25350
  910     CONTINUE                                                      GKS25360
  920   CONTINUE                                                        GKS25370
                                                                        GKS25380
C***********************************************************************GKS25390
C*****                                                               ***GKS25400
C*****   Call RMENU to use pack data record to indicate two prompts  ***GKS25410
C*****   for the lpfks (next and return). RMENU will call RCHOI      ***GKS25420
C*****   to wait for a valid choice selection and will pass that     ***GKS25430
C*****   selection back in CHNR.                                     ***GKS25440
C*****                                                               ***GKS25450
C***********************************************************************GKS25460
                                                                        GKS25470
	CONT = .TRUE.
	CALL SLEEP(4)
                                                                        GKS25490
C***********************************************************************GKS25500
C*****                                                               ***GKS25510
C*****   If user chose to continue viewing the polyline panels,      ***GKS25520
C*****   continue on. Otherwise, fall out the bottom and return      ***GKS25530
C*****   to the output primitive screen.                             ***GKS25540
C*****                                                               ***GKS25550
C***********************************************************************GKS25560
                                                                        GKS25570
        IF (CONT) THEN                                                  GKS25580
                                                                        GKS25590
C***********************************************************************GKS25600
C*****                                                               ***GKS25610
C*****   Call SETUPF and FINSHF to draw the border and title of      ***GKS25620
C*****   the demo frame.                                             ***GKS25630
C*****                                                               ***GKS25640
C***********************************************************************GKS25650
                                                                        GKS25660
          CALL SETUPF(.FALSE.,.FALSE.)                                  GKS25670
          CALL GTXS(TXTX,TXTY,10,'ATTRIBUTES')                          GKS25680
          CALL FINSHF(.FALSE.)                                          GKS25690
                                                                        GKS25700
C***********************************************************************GKS25710
C*****                                                               ***GKS25720
C*****   Draw on the left half of the demo frame, a line of each     ***GKS25730
C*****   linetype and linewidth supported.                           ***GKS25740
C*****                                                               ***GKS25750
C***********************************************************************GKS25760
                                                                        GKS25770
          RLX(1) = 0.60                                                 GKS25780
          PLX(1) = 0.05                                                 GKS25790
          RLX(1) = 0.45                                                 GKS25800
          CALL GSPLCI(GWHITE)                                           GKS25810
          DO 930 I = 1,NPPLI                                            GKS25820
            CALL GSLN(I)                                                GKS25830
            CALL GSLWSC(REAL(I))                                        GKS25840
            PLY(1) = (0.92 - (0.92 * REAL(I) / REAL(NPPLI+1)))          GKS25850
            PLY(2) = PLY(1)                                             GKS25860
            CALL GPL(2,PLX,PLY)                                         GKS25870
  930     CONTINUE                                                      GKS25880
                                                                        GKS25890
C***********************************************************************GKS25900
C*****                                                               ***GKS25910
C*****   Draw on the right half of the demo frame, a star like       ***GKS25920
C*****   figure containing a line of each color supported by the     ***GKS25930
C*****   currently defined color table.                              ***GKS25940
C*****                                                               ***GKS25950
C***********************************************************************GKS25960
                                                                        GKS25970
          RLX(17) = 0.75                                                GKS25980
          RLY(17) = 0.50                                                GKS25990
          ALPHA = 0.0                                                   GKS26000
          DO 940 I=1,16                                                 GKS26010
            RLX(I) = RLX(17) + 0.2 * SIN(ALPHA)                         GKS26020
            RLY(I) = RLY(17) + 0.2 * COS(ALPHA)                         GKS26030
            ALPHA = ALPHA + 3.141593 / 8.0                              GKS26040
  940     CONTINUE                                                      GKS26050
          PLX(1) = RLX(17)                                              GKS26060
          PLY(1) = RLY(17)                                              GKS26070
          DO 950 I=1,16                                                 GKS26080
            PLX(2) = RLX(I)                                             GKS26090
            PLY(2) = RLY(I)                                             GKS26100
            CALL GSLN(GLSOLI)                                           GKS26110
            CALL GSLWSC(1.0)                                            GKS26120
            IF (I.EQ.16) THEN                                           GKS26130
              CALL GSPLCI(GWHITE)                                       GKS26140
            ELSE                                                        GKS26150
              CALL GSPLCI(I)                                            GKS26160
            ENDIF                                                       GKS26170
            CALL GPL(2,PLX,PLY)                                         GKS26180
  950     CONTINUE                                                      GKS26190
                                                                        GKS26200
C***********************************************************************GKS26210
C*****                                                               ***GKS26220
C*****   Call RMENU to use pack data record to indicate two prompts  ***GKS26230
C*****   for the lpfks (next and return). RMENU will call RCHOI      ***GKS26240
C*****   to wait for a valid choice selection and will pass that     ***GKS26250
C*****   selection back in CHNR.                                     ***GKS26260
C*****                                                               ***GKS26270
C***********************************************************************GKS26280
                                                                        GKS26290
	CONT = .TRUE.
	CALL SLEEP(4)
                                                                        GKS26310
C***********************************************************************GKS26320
C*****                                                               ***GKS26330
C*****   If user chose to continue viewing the polyline panels,      ***GKS26340
C*****   continue on. Otherwise, fall out the bottom and return      ***GKS26350
C*****   to the output primitive screen.                             ***GKS26360
C*****                                                               ***GKS26370
C***********************************************************************GKS26380
                                                                        GKS26390
          IF (CONT) THEN                                                GKS26400
                                                                        GKS26410
C***********************************************************************GKS26420
C*****                                                               ***GKS26430
C*****   Call SETUPF and FINSHF to draw the border and title of      ***GKS26440
C*****   the demo frame.                                             ***GKS26450
C*****                                                               ***GKS26460
C***********************************************************************GKS26470
                                                                        GKS26480
            CALL SETUPF(.FALSE.,.FALSE.)                                GKS26490
            CALL GTXS(TXTX,TXTY,4,'STAR')                               GKS26500
            CALL FINSHF(.FALSE.)                                        GKS26510
                                                                        GKS26520
C***********************************************************************GKS26530
C*****                                                               ***GKS26540
C*****   Compute the coordinates of the sunflower star.              ***GKS26550
C*****                                                               ***GKS26560
C***********************************************************************GKS26570
                                                                        GKS26580
            N = 13                                                      GKS26590
            MITX = 0.50                                                 GKS26600
            MITY = 0.45                                                 GKS26610
            ALPHA = 0.0                                                 GKS26620
            DO 960 I=1,N                                                GKS26630
              RX(I) = MITX + 0.4 * SIN(ALPHA)                           GKS26640
              RY(I) = MITY + 0.4 * COS(ALPHA)                           GKS26650
              ALPHA = ALPHA + 2.0 * 3.141593 / REAL(N)                  GKS26660
  960       CONTINUE                                                    GKS26670
            CALL GSPLCI(GLBLUE)                                         GKS26680
            DO 980 I=1,(N-1)/2                                          GKS26690
              DO 970 J=0,N                                              GKS26700
                K = MOD(I*J,N)                                          GKS26710
                RLX(J+1) = RX(K+1)                                      GKS26720
                RLY(J+1) = RY(K+1)                                      GKS26730
  970         CONTINUE                                                  GKS26740
                                                                        GKS26750
C***********************************************************************GKS26760
C*****                                                               ***GKS26770
C*****   Draw two stars on the demo frame, using the same x,y data   ***GKS26780
C*****   and varying the viewport.                                   ***GKS26790
C*****                                                               ***GKS26800
C***********************************************************************GKS26810
                                                                        GKS26820
            CALL GSVP(1,0.00,0.60,0.35,0.95)                            GKS26830
            CALL GPL(N+1,RLX,RLY)                                       GKS26840
            CALL GSVP(1,0.40,1.00,0.03,0.63)                            GKS26850
            CALL GPL(N+1,RLX,RLY)                                       GKS26860
  980       CONTINUE                                                    GKS26870
                                                                        GKS26880
            CALL GSVP(1,0.0,1.0,0.0,1.0)                                GKS26890
                                                                        GKS26900
C***********************************************************************GKS26910
C*****                                                               ***GKS26920
C*****   Call RMENU to use pack data record to indicate one prompt   ***GKS26930
C*****   for the lpfk (return). RMENU will call RCHOI to wait for    ***GKS26940
C*****   a valid choice selection and will pass that selection back  ***GKS26950
C*****   in CHNR. But in this case, the user could only select       ***GKS26960
C*****   return as a valid choice, so we will fall down to the       ***GKS26970
C*****   return statement and go back to the output primitves.       ***GKS26980
C*****                                                               ***GKS26990
C***********************************************************************GKS27000
                                                                        GKS27010
	  CONT = .TRUE.
	CALL SLEEP(4)
          ENDIF                                                         GKS27030
        ENDIF                                                           GKS27040
      ENDIF                                                             GKS27050
c       define and select window transformation
      call gswn (1, -1.0, 1.0, -1.0, 1.0)
      call gselnt (1)
      call lines 
      call plot
      CALL GSWN(1,0.0,1.0,0.0,1.0)                                      GKS01860
      CALL GSVP(1,0.0,1.0,0.0,1.0)                                      GKS01870
                                                                        GKS27060
      RETURN                                                            GKS27070
      END                                                               GKS27080
                                                                        GKS27090
                                                                        GKS27100
                                                                        GKS27110
C***********************************************************************GKS27120
C***********************************************************************GKS27130
C*****                                                               ***GKS27140
C*****   Subroutine Name: DEMOPM                                     ***GKS27150
C*****                                                               ***GKS27160
C*****   Subroutine Function: This subroutine demonstrates the       ***GKS27170
C*****                        polymarker output primitives and       ***GKS27180
C*****                        its attributes: color and scale        ***GKS27190
C*****                        factor.                                ***GKS27200
C*****                                                               ***GKS27210
C*****   Calls Subroutines:   SETUPF,FINSHF,RMENU                    ***GKS27220
C*****                                                               ***GKS27230
C***********************************************************************GKS27240
C***********************************************************************GKS27250
                                                                        GKS27260
      SUBROUTINE DEMOPM                                                 GKS27270
      INCLUDE 'gkspar.inc'
                                                                        GKS27290
      INTEGER*4   I                                                     GKS27300
      REAL*4      PMX(1),PMY(1),TXTX,TXTY                               GKS27310
      LOGICAL     CONT                                                  GKS27320
      COMMON      /TEXT/   TXTX,TXTY                                    GKS27330
                                                                        GKS27340
C***********************************************************************GKS27350
C*****                                                               ***GKS27360
C*****   Call SETUPF and FINSHF to draw the border and title of      ***GKS27370
C*****   of the demo menu. Then use standard polymarker calls        ***GKS27380
C*****   to draw 3 rows of the various polymarker types in           ***GKS27390
C*****   different colors and sizes.                                 ***GKS27400
C*****                                                               ***GKS27410
C***********************************************************************GKS27420
                                                                        GKS27430
      CALL SETUPF(.TRUE.,.FALSE.)                                       GKS27440
      CALL GTXS(TXTX,TXTY,10,'POLYMARKER')                              GKS27450
      CALL FINSHF(.TRUE.)                                               GKS27460
      CALL GSCHH(0.03)                                                  GKS27470
      CALL GTXS(0.1,0.68,15,'STANDARD SIZE :')                          GKS27480
      CALL GTXS(0.1,0.43,19,'5 X STANDARD SIZE :')                      GKS27490
      CALL GTXS(0.1,0.18,14,'MINIMAL SIZE :')                           GKS27500
                                                                        GKS27510
C***********************************************************************GKS27520
C*****                                                               ***GKS27530
C*****   Loop 5 times (once for each valid polymarker type) and      ***GKS27540
C*****   draw the polymarkers in varying colors and sizes.           ***GKS27550
C*****                                                               ***GKS27560
C***********************************************************************GKS27570
                                                                        GKS27580
      DO 1000 I=1,5                                                     GKS27590
        CALL GSMK(I)                                                    GKS27600
        CALL GSMKSC(1.0)                                                GKS27610
        CALL GSPMCI(GWHITE)                                             GKS27620
        PMX(1) = (REAL(I) / 6.0)                                        GKS27630
        PMY(1) = 0.61                                                   GKS27640
        CALL GPM(1,PMX,PMY)                                             GKS27650
        CALL GSMKSC(5.0)                                                GKS27660
        CALL GSPMCI(GLBLUE)                                             GKS27670
        PMY(1) = 0.32                                                   GKS27680
        CALL GPM(1,PMX,PMY)                                             GKS27690
        CALL GSMKSC(1.0)                                                GKS27700
        CALL GSPMCI(GORNGE)                                             GKS27710
        PMY(1) = 0.13                                                   GKS27720
        CALL GPM(1,PMX,PMY)                                             GKS27730
 1000 CONTINUE                                                          GKS27740
                                                                        GKS27750
C***********************************************************************GKS27760
C*****                                                               ***GKS27770
C*****   Call RMENU to put up a choice prompt indicating that lpfk   ***GKS27780
C*****   1 is return. RMENU will then call RCHOI to wait for choice  ***GKS27790
C*****   input and return when the 'return' choice is selected.      ***GKS27800
C*****                                                               ***GKS27810
C***********************************************************************GKS27820
                                                                        GKS27830
      CONT = .TRUE.
	CALL SLEEP(4)
      RETURN                                                            GKS27850
      END                                                               GKS27860
                                                                        GKS27870
                                                                        GKS27880
                                                                        GKS27890
C***********************************************************************GKS27900
C***********************************************************************GKS27910
C*****                                                               ***GKS27920
C*****   Subroutine Name: DEMOTX                                     ***GKS27930
C*****                                                               ***GKS27940
C*****   Subroutine Function: This subroutne demonstrates the        ***GKS27950
C*****                        text output primitive and its various  ***GKS27960
C*****                        attributes: character expansion,       ***GKS27970
C*****                        character height, character spacing,   ***GKS27980
C*****                        character up vector, text color,       ***GKS27990
C*****                        text font and precision, text path,    ***GKS28000
C*****                        and text alignment.                    ***GKS28010
C*****                                                               ***GKS28020
C*****   Calls Subroutines:   SETUPF,FINSHF,RMENU                    ***GKS28030
C*****                                                               ***GKS28040
C***********************************************************************GKS28050
C***********************************************************************GKS28060
                                                                        GKS28070
      SUBROUTINE DEMOTX                                                 GKS28080
      INCLUDE 'gkspar.inc'
                                                                        GKS28100
      INTEGER*4    WKID,WTYPE,ERRIND,I                                  GKS28110
      REAL*4       PX,PY,PAX(1),PAY(1),XCHU,YCHU                        GKS28120
      REAL*4       XMIN,XMAX,YMIN,YMAX,CPX,CPY,TXEXPX(4)                GKS28130
      REAL*4       TXEXPY(4),RH,MINCHH                                  GKS28140
      LOGICAL      CONT                                                 GKS28150
      CHARACTER    HEIGHT(16)*1                                         GKS28160
      CHARACTER*7  FONTNA(11)                                           GKS28170
                                                                        GKS28180
      COMMON       /WINF/   WKID,WTYPE                                  GKS28190
      COMMON       /TEXT/   TXTX,TXTY                                   GKS28200
      COMMON       /LIMITS/ XMIN,XMAX,YMIN,YMAX                         GKS28210
                                                                        GKS28220
      DATA  FONTNA /'FONT 1 ','FONT 2 ','FONT 3 ','FONT 4 ',            GKS28230
     *              'FONT 5 ','FONT 6 ','FONT 7 ','FONT 8 ',            GKS28240
     *              'FONT 9 ','FONT 10','FONT 11'/                      GKS28250
      DATA  HEIGHT /'C','H','A','R','A','C','T','E','R',                GKS28260
     *              ' ','H','E','I','G','H','T'/                        GKS28270
                                                                        GKS28280
C***********************************************************************GKS28290
C*****                                                               ***GKS28300
C*****   Call subroutines SETUPF and FINSHF to draw the border and   ***GKS28310
C*****   border and title of the demo frame. Then draw the first     ***GKS28320
C*****   panel which says 'Select Text Attributes' in three          ***GKS28330
C*****   diagonal lines of different colors.                         ***GKS28340
C*****                                                               ***GKS28350
C***********************************************************************GKS28360
                                                                        GKS28370
      MINCHH = 0.002                                                    GKS28380
                                                                        GKS28390
      CALL SETUPF(.TRUE.,.FALSE.)                                       GKS28400
      CALL GTXS(TXTX,TXTY,4,'TEXT')                                     GKS28410
      CALL FINSHF(.TRUE.)                                               GKS28420
                                                                        GKS28430
      CALL GSCHH(0.08)                                                  GKS28440
      CALL GSTXFP(GFONT1,GSTRKP)                                        GKS28450
      CALL GSTXCI(GORNGE)                                               GKS28460
      CALL GSCHUP(-1.0,2.0)                                             GKS28470
      CALL GTXS(0.20,0.40,6,'SELECT')                                   GKS28480
      CALL GSCHXP(2.0)                                                  GKS28490
      CALL GSTXCI(GLBLUE)                                               GKS28500
      CALL GTXS(0.08,0.16,4,'TEXT')                                     GKS28510
      CALL GSCHXP(1.0)                                                  GKS28520
      CALL GSTXCI(GMGREN)                                               GKS28530
      CALL GTXS(0.32,0.10,10,'ATTRIBUTES')                              GKS28540
      CALL GSCHUP(0.0,1.0)                                              GKS28550
                                                                        GKS28560
C***********************************************************************GKS28570
C*****                                                               ***GKS28580
C*****   Call RMENU to use pack data record to indicate two prompts  ***GKS28590
C*****   for the lpfks (next and return). RMENU will call RCHOI      ***GKS28600
C*****   to wait for a valid choice selection and will pass that     ***GKS28610
C*****   selection back in CHNR.                                     ***GKS28620
C*****                                                               ***GKS28630
C***********************************************************************GKS28640
                                                                        GKS28650
      CONT = .TRUE.
	CALL SLEEP(4)
                                                                        GKS28670
C***********************************************************************GKS28680
C*****                                                               ***GKS28690
C*****   If user chose to continue viewing the text panels,          ***GKS28700
C*****   continue on. Otherwise, fall out the bottom and return      ***GKS28710
C*****   to the output primitive screen.                             ***GKS28720
C*****                                                               ***GKS28730
C***********************************************************************GKS28740
                                                                        GKS28750
      IF (CONT) THEN                                                    GKS28760
                                                                        GKS28770
C***********************************************************************GKS28780
C*****                                                               ***GKS28790
C*****   Call SETUPF and FINSHF to draw the border and title of      ***GKS28800
C*****   the demo frame.                                             ***GKS28810
C*****                                                               ***GKS28820
C***********************************************************************GKS28830
                                                                        GKS28840
        CALL SETUPF(.FALSE.,.FALSE.)                                    GKS28850
        CALL GTXS(TXTX,TXTY,18,'FONT AND PRECISION')                    GKS28860
        CALL FINSHF(.FALSE.)                                            GKS28870
                                                                        GKS28880
C***********************************************************************GKS28890
C*****                                                               ***GKS28900
C*****   Draw a series of demo panels which show 11 of the text      ***GKS28910
C*****   fonts supported by GKS-CO. The panels will show font one    ***GKS28920
C*****   and two in the top half of the panel and then via segments, ***GKS28930
C*****   change the bottom font each time the user selects the       ***GKS28940
C*****   continue choice key.                                        ***GKS28950
C*****                                                               ***GKS28960
C***********************************************************************GKS28970
                                                                        GKS28980
        CALL GSCHH(0.1)                                                 GKS28990
        CALL GSTXAL(GAHNOR,GAHALF)                                      GKS29000
        CALL GSTXCI(GRED)                                               GKS29010
        DO 1100 I=1,3                                                   GKS29060
          CALL GCRSG(1001)                                              GKS29070
          CALL GSTXFP(I*3-2,GSTRKP)                                     GKS29020
          CALL GTXS(0.15,0.25,6,FONTNA(I*3-2))                          GKS29030
          CALL GSTXFP(I*3-1,GSTRKP)                                     GKS29040
          CALL GTXS(0.15,0.50,6,FONTNA(I*3-1))                          GKS29050
          CALL GSTXFP(I*3,GSTRKP)                                       GKS29080
          CALL GTXS(0.15,0.75,7,FONTNA(I*3))                            GKS29090
          CALL GCLSG                                                    GKS29100
	  CONT = .TRUE.
	CALL SLEEP(4)
          CALL GDSG(1001)                                               GKS29120
	  CALL GUWK(WKID,1)
          IF (.NOT.(CONT)) GOTO 1110                                    GKS29130
 1100   CONTINUE                                                        GKS29140
 1110   CONTINUE                                                        GKS29150
	CALL GDSG(1259)
                                                                        GKS29160
C***********************************************************************GKS29170
C*****                                                               ***GKS29180
C*****   If user chose to continue viewing the text panels,          ***GKS29190
C*****   continue on. Otherwise, fall out the bottom and return      ***GKS29200
C*****   to the output primitive screen.                             ***GKS29210
C*****                                                               ***GKS29220
C***********************************************************************GKS29230
                                                                        GKS29240
        IF (CONT) THEN                                                  GKS29250
                                                                        GKS29260
C***********************************************************************GKS29270
C*****                                                               ***GKS29280
C*****   Reset the text alignment, font and precision.               ***GKS29290
C*****                                                               ***GKS29300
C***********************************************************************GKS29310
                                                                        GKS29320
          CALL GSTXAL(GAHNOR,GAVNOR)                                    GKS29330
          CALL GSTXFP(GFONT1,GSTRKP)                                    GKS29340
                                                                        GKS29350
C***********************************************************************GKS29360
C*****                                                               ***GKS29370
C*****   Call SETUPF and FINSHF to draw the border and title of      ***GKS29380
C*****   the demo frame.                                             ***GKS29390
C*****                                                               ***GKS29400
C***********************************************************************GKS29410
                                                                        GKS29420
          CALL SETUPF(.FALSE.,.FALSE.)                                  GKS29430
          CALL GTXS(TXTX,TXTY,6,'HEIGHT')                               GKS29440
          CALL FINSHF(.FALSE.)                                          GKS29450
                                                                        GKS29460
C***********************************************************************GKS29470
C*****                                                               ***GKS29480
C*****   Show how the character height can be varied by drawing      ***GKS29490
C*****   the letters 'character height', each in a different height. ***GKS29500
C*****                                                               ***GKS29510
C***********************************************************************GKS29520
                                                                        GKS29530
          PX = 0.025                                                    GKS29540
          PY = 0.30                                                     GKS29550
          RH = 3.0 * MINCHH                                             GKS29560
          CALL GSTXCI(GGREEN)                                           GKS29570
          CALL GSTXFP(GFONT1,GSTRKP)                                    GKS29580
          DO 1120 I=1,16                                                GKS29590
            CALL GSCHH(RH)                                              GKS29600
            CALL GTXS(PX,PY,1,HEIGHT(I))                                GKS29610
            CALL GQTXXS(WKID,PX,PY,1,HEIGHT(I),ERRIND,CPX,CPY,          GKS29620
     *                  TXEXPX,TXEXPY)                                  GKS29630
            PX = CPX                                                    GKS29640
            RH = RH + 0.0085                                            GKS29650
 1120     CONTINUE                                                      GKS29660
                                                                        GKS29670
C***********************************************************************GKS29680
C*****                                                               ***GKS29690
C*****   Call RMENU to use pack data record to indicate to prompts   ***GKS29700
C*****   for the lpfks (next and return). RMENU will call RCHOI      ***GKS29710
C*****   to wait for a valid choice selection and will pass that     ***GKS29720
C*****   selection back in CHNR.                                     ***GKS29730
C*****                                                               ***GKS29740
C***********************************************************************GKS29750
                                                                        GKS29760
	  CONT = .TRUE.
	CALL SLEEP(4)
                                                                        GKS29780
C***********************************************************************GKS29790
C*****                                                               ***GKS29800
C*****   If user chose to continue viewing the text panels,          ***GKS29810
C*****   continue on. Otherwise, fall out the bottom and return      ***GKS29820
C*****   to the output primitive screen.                             ***GKS29830
C*****                                                               ***GKS29840
C***********************************************************************GKS29850
                                                                        GKS29860
          IF (CONT) THEN                                                GKS29870
                                                                        GKS29880
C***********************************************************************GKS29890
C*****                                                               ***GKS29900
C*****   Call SETUPF and FINSHF to draw the border and title of      ***GKS29910
C*****   the demo frame.                                             ***GKS29920
C*****                                                               ***GKS29930
C***********************************************************************GKS29940
                                                                        GKS29950
            CALL SETUPF(.FALSE.,.FALSE.)                                GKS29960
            CALL GTXS(TXTX,TXTY,9,'UP-VECTOR')                          GKS29970
            CALL FINSHF(.FALSE.)                                        GKS29980
                                                                        GKS29990
C***********************************************************************GKS30000
C*****                                                               ***GKS30010
C*****   Show how the character up-vector can be varied by drawing   ***GKS30020
C*****   the word 'GKS' in a circle with each word a different       ***GKS30030
C*****   color and in a different 'up' position.                     ***GKS30040
C*****                                                               ***GKS30050
C***********************************************************************GKS30060
                                                                        GKS30070
            CALL GSWN(1,-0.050,1.050,-0.020,1.080)                      GKS30080
            PX = 0.58                                                   GKS30090
            PY = 0.05                                                   GKS30100
            XCHU = -0.5                                                 GKS30110
            YCHU = 1.0                                                  GKS30120
            CALL GSCHH(0.04)                                            GKS30130
            CALL GSTXFP(GFONT1,GSTRKP)                                  GKS30140
            DO 1130 I=1,16                                              GKS30150
              IF (I.EQ.16) THEN                                         GKS30160
                CALL GSTXCI(I-1)                                        GKS30170
              ELSE                                                      GKS30180
                CALL GSTXCI(I)                                          GKS30190
              ENDIF                                                     GKS30200
              CALL GSCHUP(XCHU,YCHU)                                    GKS30210
              CALL GTXS(PX,PY,5,' GKS ')                                GKS30220
              CALL GQTXXS(WKID,PX,PY,5,' GKS ',ERRIND,CPX,CPY,          GKS30230
     *                    TXEXPX,TXEXPY)                                GKS30240
              PX = CPX                                                  GKS30250
              PY = CPY                                                  GKS30260
              IF ((I.GE.2).AND.(I.LE.5)) THEN                           GKS30270
                YCHU = YCHU - 0.5                                       GKS30280
              ELSE                                                      GKS30290
                IF ((I.GE.6).AND.(I.LE.9)) THEN                         GKS30300
                  XCHU = XCHU + 0.5                                     GKS30310
                ELSE                                                    GKS30320
                  IF ((I.GE.10).AND.(I.LE.13)) THEN                     GKS30330
                    YCHU = YCHU + 0.5                                   GKS30340
                  ELSE                                                  GKS30350
                    XCHU = XCHU - 0.5                                   GKS30360
                  ENDIF                                                 GKS30370
                ENDIF                                                   GKS30380
              ENDIF                                                     GKS30390
 1130       CONTINUE                                                    GKS30400
            CALL GSCHUP(0.0,1.0)                                        GKS30410
            CALL GSWN(1,0.00,1.00,0.00,1.00)                            GKS30420
                                                                        GKS30430
C***********************************************************************GKS30440
C*****                                                               ***GKS30450
C*****   Call RMENU to use pack data record to indicate to prompts   ***GKS30460
C*****   for the lpfks (next and return). RMENU will call RCHOI      ***GKS30470
C*****   to wait for a valid choice selection and will pass that     ***GKS30480
C*****   selection back in CHNR.                                     ***GKS30490
C*****                                                               ***GKS30500
C***********************************************************************GKS30510
                                                                        GKS30520
	    CONT = .TRUE.
	CALL SLEEP(4)
                                                                        GKS30540
C***********************************************************************GKS30550
C*****                                                               ***GKS30560
C*****   If user chose to continue viewing the text panels,          ***GKS30570
C*****   continue on. Otherwise, fall out the bottom and return      ***GKS30580
C*****   to the output primitive screen.                             ***GKS30590
C*****                                                               ***GKS30600
C***********************************************************************GKS30610
                                                                        GKS30620
            IF (CONT) THEN                                              GKS30630
                                                                        GKS30640
C***********************************************************************GKS30650
C*****                                                               ***GKS30660
C*****   Call SETUPF and FINSHF to draw the border and title of      ***GKS30670
C*****   the demo frame.                                             ***GKS30680
C*****                                                               ***GKS30690
C***********************************************************************GKS30700
                                                                        GKS30710
              CALL SETUPF(.FALSE.,.FALSE.)                              GKS30720
              CALL GTXS(TXTX,TXTY,9,'EXPANSION')                        GKS30730
              CALL FINSHF(.FALSE.)                                      GKS30740
                                                                        GKS30750
C***********************************************************************GKS30760
C*****                                                               ***GKS30770
C*****   Show the effects of the character expansion attribute by    ***GKS30780
C*****   varying the expansion factor and writing out the word       ***GKS30790
C*****   'expansion' four times.                                     ***GKS30800
C*****                                                               ***GKS30810
C***********************************************************************GKS30820
                                                                        GKS30830
              PX = 0.03                                                 GKS30840
              PY = 0.75                                                 GKS30850
              CALL GSCHH(0.1)                                           GKS30860
              CALL GSTXCI(GORNGE)                                       GKS30870
              CALL GSTXFP(GFONT1,GSTRKP)                                GKS30880
              CALL GSVP(1,0.015,0.985,0.015,0.900)                      GKS30890
              DO 1140 I=1,4                                             GKS30900
                CALL GSCHXP(0.2 + REAL(I) * 0.30)                       GKS30910
                CALL GTXS(PX,PY,9,'EXPANSION')                          GKS30920
                PY = PY - 0.2                                           GKS30930
 1140         CONTINUE                                                  GKS30940
              CALL GSCHXP(1.0)                                          GKS30950
              CALL GSVP(1,0.0,1.0,0.0,1.0)                              GKS30960
                                                                        GKS30970
C***********************************************************************GKS30980
C*****                                                               ***GKS30990
C*****   Call RMENU to use pack data record to indicate to prompts   ***GKS31000
C*****   for the lpfks (next and return). RMENU will call RCHOI      ***GKS31010
C*****   to wait for a valid choice selection and will pass that     ***GKS31020
C*****   selection back in CHNR.                                     ***GKS31030
C*****                                                               ***GKS31040
C***********************************************************************GKS31050
                                                                        GKS31060
      CONT = .TRUE.
	CALL SLEEP(4)
                                                                        GKS31080
C***********************************************************************GKS31090
C*****                                                               ***GKS31100
C*****   If user chose to continue viewing the text panels,          ***GKS31110
C*****   continue on. Otherwise, fall out the bottom and return      ***GKS31120
C*****   to the output primitive screen.                             ***GKS31130
C*****                                                               ***GKS31140
C***********************************************************************GKS31150
                                                                        GKS31160
              IF (CONT) THEN                                            GKS31170
                                                                        GKS31180
C***********************************************************************GKS31190
C*****                                                               ***GKS31200
C*****   Call SETUPF and FINSHF to draw the border and title of      ***GKS31210
C*****   the demo frame.                                             ***GKS31220
C*****                                                               ***GKS31230
C***********************************************************************GKS31240
                                                                        GKS31250
                CALL SETUPF(.FALSE.,.FALSE.)                            GKS31260
                CALL GTXS(TXTX,TXTY,4,'PATH')                           GKS31270
                CALL FINSHF(.FALSE.)                                    GKS31280
                                                                        GKS31290
C***********************************************************************GKS31300
C*****                                                               ***GKS31310
C*****   Show the effects of the character path attribute by         ***GKS31320
C*****   drawing the words 'right', 'left', 'up', and 'down'         ***GKS31330
C*****   with their corresponding paths set.                         ***GKS31340
C*****                                                               ***GKS31350
C***********************************************************************GKS31360
                                                                        GKS31370
                PX = 0.48                                               GKS31380
                PY = 0.50                                               GKS31390
                CALL GSTXCI(GWHITE)                                     GKS31400
                CALL GSTXFP(GFONT1,GSTRKP)                              GKS31410
                CALL GSCHH(0.08)                                        GKS31420
                CALL GTXS(PX,PY,6,' RIGHT')                             GKS31430
                CALL GSTXP(GLEFT)                                       GKS31440
                CALL GTXS(PX,PY,5,' LEFT')                              GKS31450
                CALL GSTXP(GUP)                                         GKS31460
                CALL GTXS(PX,PY,3,' UP')                                GKS31470
                CALL GSTXP(GDOWN)                                       GKS31480
                CALL GTXS(PX,PY,4,'DOWN')                               GKS31490
                CALL GSTXP(GRIGHT)                                      GKS31500
                                                                        GKS31510
C***********************************************************************GKS31520
C*****                                                               ***GKS31530
C*****   Call RMENU to use pack data record to indicate to prompts   ***GKS31540
C*****   for the lpfks (next and return). RMENU will call RCHOI      ***GKS31550
C*****   to wait for a valid choice selection and will pass that     ***GKS31560
C*****   selection back in CHNR.                                     ***GKS31570
C*****                                                               ***GKS31580
C***********************************************************************GKS31590
                                                                        GKS31600
	 	CONT = .TRUE.
	CALL SLEEP(4)
                                                                        GKS31620
C***********************************************************************GKS31630
C*****                                                               ***GKS31640
C*****   If user chose to continue viewing the text panels,          ***GKS31650
C*****   continue on. Otherwise, fall out the bottom and return      ***GKS31660
C*****   to the output primitive screen.                             ***GKS31670
C*****                                                               ***GKS31680
C***********************************************************************GKS31690
                                                                        GKS31700
                IF (CONT) THEN                                          GKS31710
                                                                        GKS31720
C***********************************************************************GKS31730
C*****                                                               ***GKS31740
C*****   Call SETUPF and FINSHF to draw the border and title of      ***GKS31750
C*****   the demo frame.                                             ***GKS31760
C*****                                                               ***GKS31770
C***********************************************************************GKS31780
                                                                        GKS31790
                  CALL SETUPF(.FALSE.,.FALSE.)                          GKS31800
                  CALL GTXS(TXTX,TXTY,9,'ALIGNMENT')                    GKS31810
                  CALL FINSHF(.FALSE.)                                  GKS31820
                                                                        GKS31830
C***********************************************************************GKS31840
C*****                                                               ***GKS31850
C*****   Show the effects of the text alignment attribute by         ***GKS31860
C*****   displaying words that correspond with the various           ***GKS31870
C*****   alignment settings.                                         ***GKS31880
C*****                                                               ***GKS31890
C***********************************************************************GKS31900
                                                                        GKS31910
                  CALL GSWN(1,-0.020,1.080,-0.050,1.050)                GKS31920
                  PAX(1) = 0.02                                         GKS31930
                  PAY(1) = 0.720                                        GKS31940
                  CALL GSCHH(0.05)                                      GKS31950
                  CALL GSTXFP(GFONT1,GSTRKP)                            GKS31960
                  CALL GSTXCI(GMAGNT)                                   GKS31970
                  CALL GSPMCI(GWHITE)                                   GKS31980
                  DO 1150 I=1,5                                         GKS31990
                    CALL GPM(1,PAX,PAY)                                 GKS32000
                    CALL GSTXAL(GAHNOR,I)                               GKS32010
                    IF (I.EQ.1) THEN                                    GKS32020
                      CALL GTXS(PAX,PAY,4,'TOP ')                       GKS32030
                      CALL GQTXXS(WKID,PAX,PAY,4,'TOP ',ERRIND,         GKS32040
     *                            CPX,CPY,TXEXPX,TXEXPY)                GKS32050
                    ELSE                                                GKS32060
                      IF (I.EQ.2) THEN                                  GKS32070
                        CALL GTXS(PAX,PAY,4,'CAP ')                     GKS32080
                        CALL GQTXXS(WKID,PAX,PAY,4,'CAP ',ERRIND,       GKS32090
     *                              CPX,CPY,TXEXPX,TXEXPY)              GKS32100
                      ELSE                                              GKS32110
                        IF (I.EQ.3) THEN                                GKS32120
                          CALL GTXS(PAX,PAY,5,'HALF ')                  GKS32130
                          CALL GQTXXS(WKID,PAX,PAY,5,'HALF ',ERRIND,    GKS32140
     *                                CPX,CPY,TXEXPX,TXEXPY)            GKS32150
                        ELSE                                            GKS32160
                          IF (I.EQ.4) THEN                              GKS32170
                            CALL GTXS(PAX,PAY,5,'BASE ')                GKS32180
                            CALL GQTXXS(WKID,PAX,PAY,5,'HALF ',ERRIND,  GKS32190
     *                                  CPX,CPY,TXEXPX,TXEXPY)          GKS32200
                          ELSE                                          GKS32210
                            IF (I.EQ.5) THEN                            GKS32220
                              CALL GTXS(PAX,PAY,6,'BOTTOM')             GKS32230
                            ENDIF                                       GKS32240
                          ENDIF                                         GKS32250
                        ENDIF                                           GKS32260
                      ENDIF                                             GKS32270
                    ENDIF                                               GKS32280
                    PAX(1) = CPX                                        GKS32290
 1150             CONTINUE                                              GKS32300
                  PAX(1) = 0.50                                         GKS32310
                  PAY(1) = 0.40                                         GKS32320
                  CALL GSCHH(0.1)                                       GKS32330
                  CALL GSTXAL(GACENT,GAVNOR)                            GKS32340
                  CALL GPM(1,PAX,PAY)                                   GKS32350
                  CALL GTXS(PAX,PAY,6,'CENTER')                         GKS32360
                  PAY(1) = 0.25                                         GKS32370
                  CALL GSTXAL(GARITE,GAVNOR)                            GKS32380
                  CALL GPM(1,PAX,PAY)                                   GKS32390
                  CALL GTXS(PAX,PAY,5,'RIGHT')                          GKS32400
                  PAY(1) = 0.10                                         GKS32410
                  CALL GSCHH(0.1)                                       GKS32420
                  CALL GSTXAL(GALEFT,GAVNOR)                            GKS32430
                  CALL GPM(1,PAX,PAY)                                   GKS32440
                  CALL GTXS(PAX,PAY,4,'LEFT')                           GKS32450
                  CALL GSTXAL(GAHNOR,GAVNOR)                            GKS32460
                  CALL GSWN(1,0.00,1.00,0.00,1.00)                      GKS32470
                                                                        GKS32480
C***********************************************************************GKS32490
C*****                                                               ***GKS32500
C*****   Call RMENU to use pack data record to indicate to prompts   ***GKS32510
C*****   for the lpfks (next and return). RMENU will call RCHOI      ***GKS32520
C*****   to wait for a valid choice selection and will pass that     ***GKS32530
C*****   selection back in CHNR.                                     ***GKS32540
C*****                                                               ***GKS32550
C***********************************************************************GKS32560
                                                                        GKS32570
	CONT = .TRUE.
	CALL SLEEP(4)
                                                                        GKS32590
C***********************************************************************GKS32600
C*****                                                               ***GKS32610
C*****   If user chose to continue viewing the text panels,          ***GKS32620
C*****   continue on. Otherwise, fall out the bottom and return      ***GKS32630
C*****   to the output primitive screen.                             ***GKS32640
C*****                                                               ***GKS32650
C***********************************************************************GKS32660
                                                                        GKS32670
                  IF (CONT) THEN                                        GKS32680
                                                                        GKS32690
C***********************************************************************GKS32700
C*****                                                               ***GKS32710
C*****   Call SETUPF and FINSHF to draw the border and title of      ***GKS32720
C*****   the demo frame.                                             ***GKS32730
C*****                                                               ***GKS32740
C***********************************************************************GKS32750
                                                                        GKS32760
                    CALL SETUPF(.FALSE.,.FALSE.)                        GKS32770
                    CALL GTXS(TXTX,TXTY,7,'SPACING')                    GKS32780
                    CALL FINSHF(.FALSE.)                                GKS32790
                                                                        GKS32800
C***********************************************************************GKS32810
C*****                                                               ***GKS32820
C*****   Show the effects of the character spacing attribute by      ***GKS32830
C*****   vaying the spacing factor and writing out the sentence      ***GKS32840
C*****   'GKS - OH WHAT A MESS'.                                     ***GKS32850
C*****                                                               ***GKS32860
C***********************************************************************GKS32870
                                                                        GKS32880
                    PX = 0.03                                           GKS32890
                    PY = 0.75                                           GKS32900
                    CALL GSTXFP(GFONT1,GSTRKP)                          GKS32910
                    CALL GSCHH(0.07)                                    GKS32920
                    CALL GSTXCI(GLGREN)                                 GKS32930
                    CALL GSVP(1,0.015,0.985,0.015,0.900)                GKS32940
                    DO 1160 I=1,4                                       GKS32950
                      CALL GSCHSP(-0.8 + REAL(I) * 0.3)                 GKS32960
                      CALL GTXS(PX,PY,20,'GKS - SPACED OUT')            GKS32970
                      PY = PY - 0.2                                     GKS32980
 1160               CONTINUE                                            GKS32990
                    CALL GSCHSP(0.1)                                    GKS33000
                    CALL GSVP(1,0.0,1.0,0.0,1.0)                        GKS33010
                                                                        GKS33020
C***********************************************************************GKS33030
C*****                                                               ***GKS33040
C*****   Call RMENU to use pack data record to indicate to prompt    ***GKS33050
C*****   for the lpfk (return). RMENU will call RCHOI to wait for    ***GKS33060
C*****   a valid choice selection and will pass that selection back  ***GKS33070
C*****   in CHNR. But in this case, the user could only select       ***GKS33080
C*****   return as a valid choice, so we will fall down to the       ***GKS33090
C*****   return statement and go back to the output primitves.       ***GKS33100
C*****                                                               ***GKS33110
C***********************************************************************GKS33120
                                                                        GKS33130
	            CONT = .TRUE.
	CALL SLEEP(4)
                  ENDIF                                                 GKS33150
                ENDIF                                                   GKS33160
              ENDIF                                                     GKS33170
            ENDIF                                                       GKS33180
          ENDIF                                                         GKS33190
        ENDIF                                                           GKS33200
      ENDIF                                                             GKS33210
                                                                        GKS33220
      RETURN                                                            GKS33230
      END                                                               GKS33240
                                                                        GKS33250
                                                                        GKS33260
                                                                        GKS33270
C***********************************************************************GKS33280
C***********************************************************************GKS33290
C*****                                                               ***GKS33300
C*****   Subroutine Name: DRWHLS                                     ***GKS33310
C*****                                                               ***GKS33320
C*****   Subroutine Function: Draw a the wheels for the car picture  ***GKS33330
C*****                        through this common subroutine.        ***GKS33340
C*****                                                               ***GKS33350
C***********************************************************************GKS33360
C***********************************************************************GKS33370
                                                                        GKS33380
      SUBROUTINE DRWHLS(MITX,MITY,N,R)                                  GKS33390
                                                                        GKS33400
      INTEGER*4    N,I                                                  GKS33410
      REAL*4       MITX,MITY,R,ALPHA,RX(21),RY(21)                      GKS33420
                                                                        GKS33430
      ALPHA = 0.0                                                       GKS33440
      DO 1200 I=1,N                                                     GKS33450
        RX(I) = MITX + R * SIN(ALPHA)                                   GKS33460
        RY(I) = MITY + R * COS(ALPHA)                                   GKS33470
        ALPHA = ALPHA + 2.0 * 3.141593 / REAL(N)                        GKS33480
 1200 CONTINUE                                                          GKS33490
      RX(N+1) = RX(1)                                                   GKS33500
      RY(N+1) = RY(1)                                                   GKS33510
      CALL GPL(N+1,RX,RY)                                               GKS33520
                                                                        GKS33530
      RETURN                                                            GKS33540
      END                                                               GKS33550
                                                                        GKS33560
                                                                        GKS33570
                                                                        GKS33580
C***********************************************************************GKS33590
C***********************************************************************GKS33600
C*****                                                               ***GKS33610
C*****   Subroutine Name: FINSHF                                     ***GKS33620
C*****                                                               ***GKS33630
C*****   Subroutine Function: Finish the border border for each      ***GKS33640
C*****                        screen of the demo.                    ***GKS33650
C*****                                                               ***GKS33660
C***********************************************************************GKS33670
C***********************************************************************GKS33680
                                                                        GKS33690
      SUBROUTINE FINSHF(MFRAME)                                         GKS33700
      INCLUDE 'gkspar.inc'
                                                                        GKS33720
      REAL*4     PX2(5),PY3(5),PY5(5)                                   GKS33730
      LOGICAL    MFRAME                                                 GKS33740
                                                                        GKS33750
      DATA PX2 /0.015,0.015,0.985,0.985,0.015/                          GKS33760
      DATA PY3 /0.015,0.750,0.750,0.015,0.015/                          GKS33770
      DATA PY5 /0.015,0.900,0.900,0.015,0.015/                          GKS33780
                                                                        GKS33790
      IF (MFRAME) THEN                                                  GKS33800
        CALL GPL (5,PX2,PY3)                                            GKS33810
      ELSE                                                              GKS33820
        CALL GPL(5,PX2,PY5)                                             GKS33830
      ENDIF                                                             GKS33840
      CALL GSTXFP(GFONT1,GSTRKP)                                        GKS33850
      CALL GSTXAL(GAHNOR,GAVNOR)                                        GKS33860
      CALL GCLSG
      RETURN                                                            GKS33870
      END                                                               GKS33880
                                                                        GKS33890
                                                                        GKS33900
                                                                        GKS33910
C***********************************************************************GKS33920
C***********************************************************************GKS33930
C*****                                                               ***GKS33940
C*****   Subroutine Name: INTCRE                                     ***GKS33950
C*****                                                               ***GKS33960
C*****   Subroutine Function: This subroutine will create a new      ***GKS33970
C*****                        segment. The user can choose to        ***GKS33980
C*****                        create a segment with a polyline,      ***GKS33990
C*****                        polymarker, fill area or text in it.   ***GKS34000
C*****                                                               ***GKS34010
C*****   Calls Subroutines:   RCHOI,CREPLN,CREPMK,CREFA,CRETXT       ***GKS34020
C*****                                                               ***GKS34030
C***********************************************************************GKS34040
C***********************************************************************GKS34050
                                                                        GKS34060
      SUBROUTINE INTCRE                                                 GKS34070
      INCLUDE 'gkspar.inc'
                                                                        GKS34090
      INTEGER*4    WKID,WTYPE,CHNR,ERRIND                               GKS34100
      INTEGER*4    NELVAL,LCHVAL(1),LDRVAL                              GKS34110
      INTEGER*4    NELPRM,LCHPRM(5),LDRPRM                              GKS34120
                                                                        GKS34130
      REAL*4       XMIN,XMAX,YMIN,YMAX                                  GKS34140
                                                                        GKS34150
      CHARACTER*1  MENVAL                                               GKS34160
      CHARACTER*80 DTRVAL(1)                                            GKS34170
      CHARACTER*37 MENPRM                                               GKS34180
      CHARACTER*80 DTRPRM(5)                                            GKS34190
                                                                        GKS34200
      COMMON      /WINF/   WKID,WTYPE                                   GKS34210
      COMMON      /LIMITS/ XMIN,XMAX,YMIN,YMAX                          GKS34220
                                                                        GKS34230
      DATA MENVAL /' '/                                                 GKS34240
      DATA LCHVAL /1/                                                   GKS34250
      DATA NELVAL /1/                                                   GKS34260
                                                                        GKS34270
      DATA MENPRM /'POLYLINEPOLYMARKERFILL AREATEXTRETURN'/             GKS34280
      DATA LCHPRM /8,10,9,4,6/                                          GKS34290
      DATA NELPRM /5/                                                   GKS34300
                                                                        GKS34310
C***********************************************************************GKS34320
C*****                                                               ***GKS34330
C*****   Initialize the valuator for scale factors requested         ***GKS34340
C*****   later. Initialize the choice device and call RCHOI to       ***GKS34350
C*****   wait for the user to select which type of segment it        ***GKS34360
C*****   wishes to create: polyline, polymarker, fill area or        ***GKS34370
C*****   text. Call the appropriate subroutine based on that         ***GKS34380
C*****   selection or exit (choice number = 5).                      ***GKS34390
C*****                                                               ***GKS34400
C***********************************************************************GKS34410
                                                                        GKS34420
      CALL GPREC(0,0,0,0.0,NELVAL,LCHVAL,MENVAL,NELVAL,ERRIND,          GKS34430
     *           LDRVAL,DTRVAL)                                         GKS34440
      CALL GINVL(WKID,1,1.0,1,((XMAX-XMIN) * 0.66 + XMIN),XMAX,         GKS34450
     *           YMIN,YMAX,0.0,10.0,LDRVAL,DTRVAL)                      GKS34460
                                                                        GKS34470
      CALL GPREC(0,0,0,0.0,NELPRM,LCHPRM,MENPRM,NELPRM,                 GKS34480
     *           ERRIND,LDRPRM,DTRPRM)                                  GKS34490
 1300 CALL GINCH(WKID,1,1,1,3,XMIN,XMAX,YMIN,YMAX,LDRPRM,DTRPRM)        GKS34500
      CALL GMSGS(WKID,23,'SELECT OUTPUT PRIMITIVE')                     GKS34510
      CALL RCHOI(WKID,5,CHNR)                                           GKS34520
                                                                        GKS34530
C***********************************************************************GKS34540
C*****                                                               ***GKS34550
C*****   Call the appropriate subroutine based on the choice         ***GKS34560
C*****   returned from the user. If choice was RETURN (lpfk 5),      ***GKS34570
C*****   exit back to main interactive menu panel.                   ***GKS34580
C*****                                                               ***GKS34590
C***********************************************************************GKS34600
                                                                        GKS34610
      IF (CHNR.EQ.1) THEN                                               GKS34620
        CALL CREPLN                                                     GKS34630
      ELSE                                                              GKS34640
        IF (CHNR.EQ.2) THEN                                             GKS34650
          CALL CREPMK                                                   GKS34660
        ELSE                                                            GKS34670
          IF (CHNR.EQ.3) THEN                                           GKS34680
            CALL CREFA                                                  GKS34690
          ELSE                                                          GKS34700
            IF (CHNR.EQ.4) THEN                                         GKS34710
              CALL CRETXT                                               GKS34720
            ELSE                                                        GKS34730
              GOTO 1310                                                 GKS34740
            ENDIF                                                       GKS34750
          ENDIF                                                         GKS34760
        ENDIF                                                           GKS34770
      ENDIF                                                             GKS34780
      GOTO 1300                                                         GKS34790
 1310 RETURN                                                            GKS34800
      END                                                               GKS34810
                                                                        GKS34820
                                                                        GKS34830
                                                                        GKS34840
C***********************************************************************GKS34850
C***********************************************************************GKS34860
C*****                                                               ***GKS34870
C*****   Subroutine Name: INTDEL                                     ***GKS34880
C*****                                                               ***GKS34890
C*****   Subroutine Function:  Deletes a segment which is picked     ***GKS34900
C*****                         by the user with the locator          ***GKS34910
C*****                         device.                               ***GKS34920
C*****                                                               ***GKS34930
C*****   Calls Subroutine:     CRSEGM                                ***GKS34940
C*****                                                               ***GKS34950
C***********************************************************************GKS34960
C***********************************************************************GKS34970
                                                                        GKS34980
      SUBROUTINE INTDEL                                                 GKS34990
      INCLUDE 'gkspar.inc'
                                                                        GKS35010
      INTEGER*4    WKID,WTYPE,STAT,SGNA,PCID                            GKS35020
                                                                        GKS35030
      LOGICAL      SGNAME(50)                                           GKS35040
                                                                        GKS35050
      COMMON      /WINF/   WKID,WTYPE                                   GKS35060
      COMMON      /SEGM/   SGNAME                                       GKS35070
                                                                        GKS35080
C***********************************************************************GKS35090
C*****                                                               ***GKS35100
C*****   Call subroutine CRSEGM to create a segment and draw         ***GKS35110
C*****   a red box with 'RETURN' in it in the upper left hand        ***GKS35120
C*****   corner of the screen. This segment can be picked by         ***GKS35130
C*****   the user when it wants to return to the previous menu       ***GKS35140
C*****   frame (segment number = 6). If a valid segment was          ***GKS35150
C*****   picked, either delete the segment and wait for the          ***GKS35160
C*****   next input, or return.                                      ***GKS35170
C*****                                                               ***GKS35180
C***********************************************************************GKS35190
                                                                        GKS35200
      CALL CRSEGM                                                       GKS35210
 1400 CALL GMSGS(WKID,32,'PICK SEGMENT TO DELETE OR RETURN')            GKS35220
 1410 CALL GRQPK(WKID,1,STAT,SGNA,PCID)                                 GKS35230
      IF (STAT.EQ.1) THEN                                               GKS35240
        IF (SGNA.GE.7) THEN                                             GKS35250
          SGNAME(SGNA) = .FALSE.                                        GKS35260
          CALL GDSG(SGNA)                                               GKS35270
          GOTO 1400                                                     GKS35280
        ELSE                                                            GKS35290
          CALL GDSG(6)                                                  GKS35300
        ENDIF                                                           GKS35310
      ELSE                                                              GKS35320
        GOTO 1410                                                       GKS35330
      ENDIF                                                             GKS35340
      RETURN                                                            GKS35350
      END                                                               GKS35360
                                                                        GKS35370
                                                                        GKS35380
                                                                        GKS35390
C***********************************************************************GKS35400
C***********************************************************************GKS35410
C*****                                                               ***GKS35420
C*****   Subroutine Name: INTER                                      ***GKS35430
C*****                                                               ***GKS35440
C*****   Subroutine Function: This subroutine will demonstrate the   ***GKS35450
C*****                        use of GKS in doing interactive        ***GKS35460
C*****                        graphics functions. It will also use   ***GKS35470
C*****                        the valuator and stroke input devices  ***GKS35480
C*****                        for the first time.                    ***GKS35490
C*****                                                               ***GKS35500
C*****   Calls Subroutines:   RCHOI,INTCRE,INTINS,INTTRA,INTDEL      ***GKS35510
C*****                                                               ***GKS35520
C***********************************************************************GKS35530
C***********************************************************************GKS35540
                                                                        GKS35550
      SUBROUTINE INTER                                                  GKS35560
      INCLUDE 'gkspar.inc'
                                                                        GKS35580
      INTEGER*4    WKID,WTYPE,WISS,CHNR,NUMELA,LCHSTR(5),LENGTH,ERRIND  GKS35590
                                                                        GKS35600
      REAL*4       FRAME(10),FA3(6),FA4(8),FA5(10),XMIN,XMAX,YMIN,YMAX  GKS35610
                                                                        GKS35620
      CHARACTER*33 MENU                                                 GKS35630
      CHARACTER*80 DTREC(5)                                             GKS35640
                                                                        GKS35650
      LOGICAL      SGNAME(50)                                           GKS35660
                                                                        GKS35670
      COMMON      /WINF/   WKID,WTYPE                                   GKS35680
      COMMON      /SEGM/   SGNAME                                       GKS35690
      COMMON      /LIMITS/ XMIN,XMAX,YMIN,YMAX                          GKS35700
                                                                        GKS35710
      DATA MENU   /'CREATEINSERTTRANSFORMDELETERETURN'/                 GKS35720
      DATA LCHSTR /6,6,9,6,6/                                           GKS35730
      DATA NUMELA /5/                                                   GKS35740
                                                                        GKS35750
      DATA FRAME  /0.000,0.000,1.000,1.000,0.000,                       GKS35760
     *             0.000,1.000,1.000,0.000,0.000/                       GKS35770
      DATA FA3    /0.400,0.600,0.600,0.400,0.400,0.800/                 GKS35780
      DATA FA4    /0.100,0.200,0.300,0.200,0.200,0.100,0.200,0.300/     GKS35790
      DATA FA5    /0.300,0.400,0.350,0.250,0.200,                       GKS35800
     *             0.600,0.500,0.400,0.400,0.500/                       GKS35810
                                                                        GKS35820
C***********************************************************************GKS35830
C*****                                                               ***GKS35840
C*****   Open the WISS workstation. Initialize the list of segments  ***GKS35850
C*****   available (SGNAME) to available (false). The first six are  ***GKS35860
C*****   used by GKSDEMO and therefore never checked for their       ***GKS35870
C*****   availibility.                                               ***GKS35880
C*****                                                               ***GKS35890
C***********************************************************************GKS35900
                                                                        GKS35910
      WISS = WKID + 1                                                   GKS35920
      CALL GOPWK(WISS,1,3)                                              GKS35930
      CALL GACWK(WISS)                                                  GKS35940
      DO 1500 I=7,50                                                    GKS35950
        SGNAME(I) = .FALSE.                                             GKS35960
 1500 CONTINUE                                                          GKS35970
                                                                        GKS35980
C***********************************************************************GKS35990
C*****                                                               ***GKS36000
C*****   Clear the workstation and draw the initial screen for the   ***GKS36010
C*****   interactive portion of the demo. This screen consists of    ***GKS36020
C*****   three segments: one containing a red, hollow triangle. A    ***GKS36030
C*****   second segment containing a blue, hatched pentagon. And a   ***GKS36040
C*****   third segment containing a green, solid square.             ***GKS36050
C*****                                                               ***GKS36060
C***********************************************************************GKS36070
                                                                        GKS36080
      CALL GCLRWK(WKID,1)                                               GKS36090
      CALL GSLN(GLSOLI)                                                 GKS36100
      CALL GSLWSC(1.0)                                                  GKS36110
      CALL GSPLCI(GYELOW)                                               GKS36120
      CALL GPL(5,FRAME(1),FRAME(6))                                     GKS36130
      CALL GCRSG(7)                                                     GKS36140
      CALL GSDTEC(7,GDETEC)                                             GKS36150
      CALL GSFAIS(GHOLLO)                                               GKS36160
      CALL GSFACI(GRED)                                                 GKS36170
      CALL GFA(3,FA3(1),FA3(4))                                         GKS36180
      CALL GCLSG                                                        GKS36190
      SGNAME(7) = .TRUE.                                                GKS36200
      CALL GCRSG(8)                                                     GKS36210
      CALL GSDTEC(8,GDETEC)                                             GKS36220
      CALL GSFAIS(GHATCH)                                               GKS36230
      CALL GSFACI(GBLUE)                                                GKS36240
      CALL GFA(5,FA5(1),FA5(6))                                         GKS36250
      CALL GCLSG                                                        GKS36260
      SGNAME(8) = .TRUE.                                                GKS36270
      CALL GCRSG(9)                                                     GKS36280
      CALL GSDTEC(9,GDETEC)                                             GKS36290
      CALL GSFAIS(GSOLID)                                               GKS36300
      CALL GSFACI(GGREEN)                                               GKS36310
      CALL GFA(4,FA4(1),FA4(5))                                         GKS36320
      CALL GCLSG                                                        GKS36330
      SGNAME(9) = .TRUE.                                                GKS36340
                                                                        GKS36350
C***********************************************************************GKS36360
C*****                                                               ***GKS36370
C*****   Initialize the choice device and call subroutine RCHOI to   ***GKS36380
C*****   wait for one of the following choices:                      ***GKS36390
C*****        Choice 1: Create Segment                               ***GKS36400
C*****               2: Insert Segment                               ***GKS36410
C*****               3: Transform Segment                            ***GKS36420
C*****               4: Delete Segment                               ***GKS36430
C*****               5: Return                                       ***GKS36440
C*****   Call the appropriate function depending on the input        ***GKS36450
C*****   received from the choice device, or exit.                   ***GKS36460
C*****                                                               ***GKS36470
C***********************************************************************GKS36480
                                                                        GKS36490
      CALL GPREC(0,0,0,0.0,NUMELA,LCHSTR,MENU,NUMELA,                   GKS36500
     *           ERRIND,LENGTH,DTREC)                                   GKS36510
 1510 CALL GMSGS(WKID,1,' ')                                            GKS36520
      CALL GINCH(WKID,1,1,1,3,XMIN,XMAX,YMIN,YMAX,LENGTH,DTREC)         GKS36530
      CALL RCHOI(WKID,5,CHNR)                                           GKS36540
      IF (CHNR.EQ.1) THEN                                               GKS36550
        CALL INTCRE                                                     GKS36560
      ELSE                                                              GKS36570
        IF (CHNR.EQ.2) THEN                                             GKS36580
          CALL INTINS                                                   GKS36590
        ELSE                                                            GKS36600
          IF (CHNR.EQ.3) THEN                                           GKS36610
            CALL INTTRA                                                 GKS36620
          ELSE                                                          GKS36630
            IF (CHNR.EQ.4) THEN                                         GKS36640
              CALL INTDEL                                               GKS36650
            ELSE                                                        GKS36660
              GOTO 1520                                                 GKS36670
            ENDIF                                                       GKS36680
          ENDIF                                                         GKS36690
        ENDIF                                                           GKS36700
      ENDIF                                                             GKS36710
      GOTO 1510                                                         GKS36720
1520  CALL GDAWK(WISS)                                                  GKS36730
      CALL GCLWK(WISS)                                                  GKS36740
      RETURN                                                            GKS36750
      END                                                               GKS36760
                                                                        GKS36770
                                                                        GKS36780
                                                                        GKS36790
C***********************************************************************GKS36800
C***********************************************************************GKS36810
C*****                                                               ***GKS36820
C*****   Subroutine Name: INTINS                                     ***GKS36830
C*****                                                               ***GKS36840
C*****   Subroutine Function:  This subroutine will insert chosen    ***GKS36850
C*****                         segments into a new segment and       ***GKS36860
C*****                         delete the old segment(s).            ***GKS36870
C*****                                                               ***GKS36880
C*****   Calls Subroutines:    CRSEGM                                ***GKS36890
C*****                                                               ***GKS36900
C***********************************************************************GKS36910
C***********************************************************************GKS36920
                                                                        GKS36930
      SUBROUTINE INTINS                                                 GKS36940
      INCLUDE 'gkspar.inc'
                                                                        GKS36960
      INTEGER*4    WKID,WTYPE,I,STAT,SGNA,PCID                          GKS36970
                                                                        GKS36980
      REAL*4       TM(2,3)                                              GKS36990
                                                                        GKS37000
      LOGICAL      SGNAME(50)                                           GKS37010
                                                                        GKS37020
      COMMON      /WINF/   WKID,WTYPE                                   GKS37030
      COMMON      /SEGM/   SGNAME                                       GKS37040
                                                                        GKS37050
C***********************************************************************GKS37060
C*****                                                               ***GKS37070
C*****   Set up a transformation matrix to hold new segment data.    ***GKS37080
C*****   Loop through list of segment numbers (segments 1 - 6 are    ***GKS37090
C*****   reserved for GKSDEMO use) and find the first free segment.  ***GKS37100
C*****                                                               ***GKS37110
C***********************************************************************GKS37120
                                                                        GKS37130
      CALL GEVTM(0.0,0.0,0.0,0.0,0.0,1.0,1.0,1,TM)                      GKS37140
      DO 1600 I=7,50                                                    GKS37150
        IF (.NOT.(SGNAME(I))) THEN                                      GKS37160
          GOTO 1610                                                     GKS37170
        ENDIF                                                           GKS37180
 1600 CONTINUE                                                          GKS37190
      CALL GMSGS(WKID,32,'NO MORE THAN 50 SEGMENTS ALLOWED')            GKS37200
      GOTO 1640                                                         GKS37210
 1610 CALL CRSEGM                                                       GKS37220
                                                                        GKS37230
C***********************************************************************GKS37240
C*****                                                               ***GKS37250
C*****   Create a new segment and wait for the user to choose all    ***GKS37260
C*****   existing segments it wants added to the new segment.        ***GKS37270
C*****                                                               ***GKS37280
C***********************************************************************GKS37290
                                                                        GKS37300
      SGNAME(I) = .TRUE.                                                GKS37310
      CALL GCRSG(I)                                                     GKS37320
      CALL GSDTEC(I,1)                                                  GKS37330
      CALL GSVIS(I,1)                                                   GKS37340
 1620 CALL GMSGS(WKID,32,'PICK SEGMENT TO INSERT OR RETURN')            GKS37350
 1630 CALL GRQPK(WKID,1,STAT,SGNA,PCID)                                 GKS37360
      IF (STAT.EQ.1) THEN                                               GKS37370
                                                                        GKS37380
C***********************************************************************GKS37390
C*****                                                               ***GKS37400
C*****   If user picked an existing segment for inserting, insert    ***GKS37410
C*****   the segment in the new segment and delete the old one.      ***GKS37420
C*****   Continue loop until user chooses 'return' (segment 6).      ***GKS37430
C*****                                                               ***GKS37440
C***********************************************************************GKS37450
                                                                        GKS37460
        IF (SGNA.GE.7) THEN                                             GKS37470
          CALL GINSG(SGNA,TM)                                           GKS37480
          CALL GDSG(SGNA)                                               GKS37490
          SGNAME(SGNA) = .FALSE.                                        GKS37500
          GOTO 1620                                                     GKS37510
        ELSE                                                            GKS37520
          CALL GCLSG                                                    GKS37530
          CALL GDSG(6)                                                  GKS37540
          GOTO 1640                                                     GKS37550
        ENDIF                                                           GKS37560
      ELSE                                                              GKS37570
        GOTO 1630                                                       GKS37580
      ENDIF                                                             GKS37590
 1640 RETURN                                                            GKS37600
      END                                                               GKS37610
                                                                        GKS37620
                                                                        GKS37630
                                                                        GKS37640
C***********************************************************************GKS37650
C***********************************************************************GKS37660
C*****                                                               ***GKS37670
C*****   Subroutine Name: INTTRA                                     ***GKS37680
C*****                                                               ***GKS37690
C*****   Subroutine Function:  Perform transformations on selected   ***GKS37700
C*****                         segments, allowing the user to        ***GKS37710
C*****                         input the various transformation      ***GKS37720
C*****                         variables: fixed point, shift         ***GKS37730
C*****                         factor, rotation angle and x,y        ***GKS37740
C*****                         scaling factors.                      ***GKS37750
C*****                                                               ***GKS37760
C*****   Calls Subroutine:    RCHOI                                  ***GKS37770
C*****                                                               ***GKS37780
C***********************************************************************GKS37790
C***********************************************************************GKS37800
                                                                        GKS37810
      SUBROUTINE INTTRA                                                 GKS37820
      INCLUDE 'gkspar.inc'
                                                                        GKS37840
      INTEGER*4    WKID,WTYPE,CHNR1,ERRIND,SW                           GKS37850
      INTEGER*4    STAT,SGNA,PCID,NUMEL1,LCHST1(6),LENGT1,STATVL        GKS37860
      INTEGER*4    VIS,HIGHL,DET,NUMEL2,LCHST2(1),LENGT2,STATLC,TNR     GKS37870
                                                                        GKS37880
      REAL*4       XMIN,XMAX,YMIN,YMAX                                  GKS37890
      REAL*4       SEGTM(2,3),SGPR,X0,Y0,DX,DY,PHI,FX,FY                GKS37900
      REAL*4       MOUT(2,3),PX1(1),PY1(1),SCFACT,PX2(1),PY2(1)         GKS37910
                                                                        GKS37920
      CHARACTER*42 MENU1                                                GKS37930
      CHARACTER*1  MENU2                                                GKS37940
      CHARACTER*80 DTREC1(6)                                            GKS37950
      CHARACTER*80 DTREC2(1)                                            GKS37960
                                                                        GKS37970
      COMMON      /WINF/   WKID,WTYPE                                   GKS37980
      COMMON      /LIMITS/ XMIN,XMAX,YMIN,YMAX                          GKS37990
                                                                        GKS38000
      DATA MENU1  /'TRANS-SGMTFIXPOINTSHIFTROTATESCALINGRETURN'/        GKS38010
      DATA LCHST1 /10,8,5,6,7,6/                                        GKS38020
      DATA NUMEL1 /6/                                                   GKS38030
                                                                        GKS38040
      DATA MENU2  /' '/                                                 GKS38050
      DATA LCHST2 /1/                                                   GKS38060
      DATA NUMEL2 /1/                                                   GKS38070
                                                                        GKS38080
C***********************************************************************GKS38090
C*****                                                               ***GKS38100
C*****   Set up the transformation default values. Use pack data     ***GKS38110
C*****   record to set up the prompts for the choice device. The     ***GKS38120
C*****   choices are: transformation segment, fixed point, shift,    ***GKS38130
C*****   rotate and scaling factors, and return.                     ***GKS38140
C*****                                                               ***GKS38150
C***********************************************************************GKS38160
                                                                        GKS38170
      X0 = 0.5                                                          GKS38180
      Y0 = 0.5                                                          GKS38190
      DX = 0.0                                                          GKS38200
      DY = 0.0                                                          GKS38210
      PHI = 0.0                                                         GKS38220
      FX = 1.0                                                          GKS38230
      FY = 1.0                                                          GKS38240
      SW = 0                                                            GKS38250
      CALL GSMK(GAST)                                                   GKS38260
      CALL GSMKSC(1.0)                                                  GKS38270
      CALL GSPMCI(GBLUE)                                                GKS38280
      CALL GPREC(0,0,0,0.0,NUMEL1,LCHST1,MENU1,NUMEL1,                  GKS38290
     *           ERRIND,LENGT1,DTREC1)                                  GKS38300
      CALL GINCH(WKID,1,1,1,3,XMIN,XMAX,YMIN,YMAX,LENGT1,DTREC1)        GKS38310
 1700 CALL GMSGS(WKID,26,' SELECT FOR TRANSFORMATION')                  GKS38320
      CALL RCHOI(WKID,6,CHNR1)                                          GKS38330
                                                                        GKS38340
C***********************************************************************GKS38350
C*****                                                               ***GKS38360
C*****   If the user chose to pick the transformation segment,       ***GKS38370
C*****   then call subroutine to add a pickable segment for          ***GKS38380
C*****   'RETURN' (segment 6). Read the segment number that was      ***GKS38390
C*****   picked. If not the return segment, then perform the         ***GKS38400
C*****   transformation with the current values in each of the       ***GKS38410
C*****   transformation parameters. Otherwise, delete segment 6      ***GKS38420
C*****   (return segment) and exit the transformation routine.       ***GKS38430
C*****                                                               ***GKS38440
C***********************************************************************GKS38450
                                                                        GKS38460
      IF (CHNR1.EQ.1) THEN                                              GKS38470
        CALL CRSEGM                                                     GKS38480
        CALL GMSGS(WKID,22,'PICK SEGMENT OR RETURN')                    GKS38490
 1710   CALL GRQPK(WKID,1,STAT,SGNA,PCID)                               GKS38500
        IF (STAT.EQ.1) THEN                                             GKS38510
          IF (SGNA.GE.7) THEN                                           GKS38520
            CALL GQSGA(SGNA,ERRIND,SEGTM,VIS,HIGHL,SGPR,DET)            GKS38530
            CALL GACTM(SEGTM,X0,Y0,DX,DY,PHI,FX,FY,SW,MOUT)             GKS38540
            CALL GSSGT(SGNA,MOUT)                                       GKS38550
            CALL GMSGS(WKID,27,'PICK NEXT SEGMENT OR RETURN')           GKS38560
            GOTO 1710                                                   GKS38570
          ELSE                                                          GKS38580
            CALL GDSG(6)                                                GKS38590
          ENDIF                                                         GKS38600
        ENDIF                                                           GKS38610
      ELSE                                                              GKS38620
                                                                        GKS38630
C***********************************************************************GKS38640
C*****                                                               ***GKS38650
C*****   Set up the pick device so that the user can enter a         ***GKS38660
C*****   fixed point using the locator device. Once a fixed point    ***GKS38670
C*****   has been returned, exit back to the main choice panel.      ***GKS38680
C*****                                                               ***GKS38690
C***********************************************************************GKS38700
                                                                        GKS38710
        IF (CHNR1.EQ.2) THEN                                            GKS38720
          CALL GPREC(0,0,0,0.0,NUMEL2,LCHST2,MENU2,NUMEL2,              GKS38730
     *               ERRIND,LENGT2,DTREC2)                              GKS38740
          CALL GINLC(WKID,1,1,0.1,0.1,2,XMIN,XMAX,YMIN,YMAX,            GKS38750
     *               LENGT2,DTREC2)                                     GKS38760
          CALL GMSGS(WKID,17,'LOCATE FIX POINT')                        GKS38770
 1720     CALL GRQLC(WKID,1,STATLC,TNR,PX1(1),PY1(1))                   GKS38780
          IF (STATLC.EQ.1) THEN                                         GKS38790
            X0 = PX1(1)                                                 GKS38800
            Y0 = PY1(1)                                                 GKS38810
          ELSE                                                          GKS38820
            GOTO 1720                                                   GKS38830
          ENDIF                                                         GKS38840
        ELSE                                                            GKS38850
                                                                        GKS38860
C***********************************************************************GKS38870
C*****                                                               ***GKS38880
C*****   Set up the pick device so that the user can enter two       ***GKS38890
C*****   point from which the shift factor can be determined.        ***GKS38900
C*****   Once both points have been entered, calculate the shift     ***GKS38910
C*****   factors and return to the main choice panel.                ***GKS38920
C*****                                                               ***GKS38930
C***********************************************************************GKS38940
                                                                        GKS38950
          IF (CHNR1.EQ.3) THEN                                          GKS38960
            CALL GINLC(WKID,1,1,0.1,0.1,3,XMIN,XMAX,YMIN,YMAX,          GKS38970
     *                 LENGT2,DTREC2)                                   GKS38980
            CALL GMSGS(WKID,18,'LOCATE FIRST POINT')                    GKS38990
 1730       CALL GRQLC(WKID,1,STATLC,TNR,PX1(1),PY1(1))                 GKS39000
            IF (STATLC.EQ.1) THEN                                       GKS39010
              CALL GINLC(WKID,1,1,PX1(1),PY1(1),4,XMIN,XMAX,            GKS39020
     *                   YMIN,YMAX,LENGT2,DTREC2)                       GKS39030
              CALL GMSGS(WKID,19,'LOCATE SECOND POINT')                 GKS39040
 1740         CALL GRQLC(WKID,1,STATLC,TNR,PX2(1),PY2(1))               GKS39050
              IF (STATLC.EQ.1) THEN                                     GKS39060
                DX = PX2(1) - PX1(1)                                    GKS39070
                DY = PY2(1) - PY1(1)                                    GKS39080
              ELSE                                                      GKS39090
                GOTO 1740                                               GKS39100
              ENDIF                                                     GKS39110
            ELSE                                                        GKS39120
              GOTO 1730                                                 GKS39130
            ENDIF                                                       GKS39140
          ELSE                                                          GKS39150
                                                                        GKS39160
C***********************************************************************GKS39170
C*****                                                               ***GKS39180
C*****   Use the valuator input to determine the rotation angle in   ***GKS39190
C*****   radians. Once input from the valuator has been received     ***GKS39200
C*****   (user turns valuator and then hits ENTER), return back to   ***GKS39210
C*****   the main transformation choice panel.                       ***GKS39220
C*****                                                               ***GKS39230
C***********************************************************************GKS39240
                                                                        GKS39250
            IF (CHNR1.EQ.4) THEN                                        GKS39260
              CALL GINVL(WKID,1,1.0,1,((XMAX-XMIN) * 0.66 + XMIN),      GKS39270
     *                   XMAX,YMIN,YMAX,0.0,6.2832,LENGT2,DTREC2)       GKS39280
              CALL GMSGS(WKID,35,                                       GKS39290
     *                   'EVALUATE ROTATION ANGLE IN RADIANS')          GKS39300
 1750         CALL GRQVL(WKID,1,STATVL,SCFACT)                          GKS39310
              IF (STATVL.EQ.1) THEN                                     GKS39320
                PHI = SCFACT                                            GKS39330
              ELSE                                                      GKS39340
                GOTO 1750                                               GKS39350
              ENDIF                                                     GKS39360
            ELSE                                                        GKS39370
                                                                        GKS39380
C***********************************************************************GKS39390
C*****                                                               ***GKS39400
C*****   Use the valuator input to determine the x and y scaling     ***GKS39410
C*****   factors. Once both values have been received (user turns    ***GKS39420
C*****   valuator and then hits ENTER), return back to the main      ***GKS39430
C*****   transformation choice panel.                                ***GKS39440
C*****                                                               ***GKS39450
C***********************************************************************GKS39460
                                                                        GKS39470
              IF (CHNR1.EQ.5) THEN                                      GKS39480
                CALL GINVL(WKID,1,1.0,1,                                GKS39490
     *                     ((XMAX-XMIN) * 0.66 + XMIN),XMAX,            GKS39500
     *                     YMIN,YMAX,0.0,5.0,LENGT2,DTREC2)             GKS39510
                CALL GMSGS(WKID,25,'EVALUATE X - SCALE FACTOR')         GKS39520
 1760           CALL GRQVL(WKID,1,STATVL,SCFACT)                        GKS39530
                IF (STATVL.EQ.1) THEN                                   GKS39540
                  FX = SCFACT                                           GKS39550
                  CALL GMSGS(WKID,25,'EVALUATE Y - SCALE FACTOR')       GKS39560
 1770             CALL GRQVL(WKID,1,STATVL,SCFACT)                      GKS39570
                  IF (STATVL.EQ.1) THEN                                 GKS39580
                    FY = SCFACT                                         GKS39590
                  ELSE                                                  GKS39600
                    GOTO 1770                                           GKS39610
                  ENDIF                                                 GKS39620
                ELSE                                                    GKS39630
                  GOTO 1760                                             GKS39640
                ENDIF                                                   GKS39650
                                                                        GKS39660
C***********************************************************************GKS39670
C*****                                                               ***GKS39680
C*****   User selected choice key 6 - RETURN. Exit out of the        ***GKS39690
C*****   transformation panel and back to the Interactive main       ***GKS39700
C*****   choice panel.                                               ***GKS39710
C*****                                                               ***GKS39720
C***********************************************************************GKS39730
                                                                        GKS39740
              ELSE                                                      GKS39750
                GOTO 1780                                               GKS39760
              ENDIF                                                     GKS39770
            ENDIF                                                       GKS39780
          ENDIF                                                         GKS39790
        ENDIF                                                           GKS39800
      ENDIF                                                             GKS39810
      GOTO 1700                                                         GKS39820
 1780 RETURN                                                            GKS39830
      END                                                               GKS39840
                                                                        GKS39850
                                                                        GKS39860
                                                                        GKS39870
C***********************************************************************GKS39880
C***********************************************************************GKS39890
C*****                                                               ***GKS39900
C*****   Subroutine Name: MAPDEM                                     ***GKS39910
C*****                                                               ***GKS39920
C*****   Subroutine Function: High level picture of how the GKS-CO   ***GKS39930
C*****                        mapper fits into the graPHIGS systems  ***GKS39940
C*****                        picture.                               ***GKS39950
C*****                                                               ***GKS39960
C*****   Calls Subroutines:   SETUPF,FINSHF,RMENU                    ***GKS39970
C*****                                                               ***GKS39980
C***********************************************************************GKS39990
C***********************************************************************GKS40000
                                                                        GKS40010
      SUBROUTINE MAPDEM                                                 GKS40020
      INCLUDE 'gkspar.inc'
                                                                        GKS40040
      INTEGER*4 WKID,WTYPE                                              GKS40050
      REAL*4    GKSX(4),PAPPX(4),GAPPX(4),PHIGSX(4),PWKX(4),            GKS40060
     *          GKSY(4),PAPPY(4),GAPPY(4),PHIGSY(4),PWKY(4)             GKS40070
      REAL*4    PLX(27),PLY(27),ALPHA                                   GKS40080
      REAL*4    XMIN,XMAX,YMIN,YMAX,TXTX,TXTY                           GKS40090
      REAL*4    PAPX(10),GAGX(10),GPX(10),PWX(10),                      GKS40100
     *          PAPY(10),GAGY(10),GPY(10),PWY(10),                      GKS40110
     *          PLATTX(10),PLATTY(10),X(10),WKMX(3),WKMY(3)             GKS40120
      LOGICAL   CONT                                                    GKS40130
                                                                        GKS40140
      COMMON    /WINF/   WKID,WTYPE                                     GKS40150
      COMMON    /LIMITS/ XMIN,XMAX,YMIN,YMAX                            GKS40160
      COMMON    /TEXT/ TXTX,TXTY                                        GKS40170
                                                                        GKS40180
      DATA    EMPTRP /0.0/                                              GKS40190
      DATA    GKSX   /08.3,08.3,14.5,14.5/,                             GKS40200
     *        GKSY   /09.5,13.5,13.5,09.5/                              GKS40210
      DATA    PAPPX  /01.5,01.5,07.7,07.7/,                             GKS40220
     *        PAPPY  /14.5,15.8,15.8,14.5/                              GKS40230
      DATA    GAPPX  /08.3,08.3,14.5,14.5/,                             GKS40240
     *        GAPPY  /14.5,15.8,15.8,14.5/                              GKS40250
      DATA    PHIGSX /01.5,01.5,14.5,14.5/,                             GKS40260
     *        PHIGSY /04.3,08.8,08.8,04.3/                              GKS40270
      DATA    PWKX   /01.5,01.5,03.5,03.5/,                             GKS40280
     *        PWKY   /01.2,03.5,03.5,01.2/                              GKS40290
      DATA    PAPX   /04.6,04.5,04.6,04.7,04.6,04.6,04.5,04.6,04.7,04.6/GKS40300
     *        PAPY   /14.3,14.3,14.5,14.3,14.3,09.0,09.0,08.8,09.0,09.0/GKS40310
      DATA    GAGX   /11.4,11.3,11.4,11.5,11.4,11.4,11.3,11.4,11.5,11.4/GKS40320
     *        GAGY   /14.3,14.3,14.5,14.3,14.3,13.7,13.7,13.5,13.7,13.7/GKS40330
      DATA    GPX    /11.4,11.3,11.4,11.5,11.4,11.4,11.3,11.4,11.5,11.4/GKS40340
     *        GPY    /09.3,09.3,09.5,09.3,09.3,09.0,09.0,08.8,09.0,09.0/GKS40350
      DATA    PWX    /02.5,02.4,02.5,02.6,02.5,02.5,02.4,02.5,02.6,02.5/GKS40360
     *        PWY    /04.1,04.1,04.3,04.1,04.1,03.7,03.7,03.5,03.7,03.7/GKS40370
      DATA    PLATTX /14.7,14.7,14.5,14.7,14.7,15.1,15.1,15.3,15.1,15.1/GKS40380
     *        PLATTY /12.5,12.6,12.5,12.4,12.5,12.5,12.6,12.5,12.4,12.5/GKS40390
      DATA    WKMX   /08.0,09.5,11.0/,                                  GKS40400
     *        WKMY   /2.35,2.35,2.35/                                   GKS40410
                                                                        GKS40420
C***********************************************************************GKS40430
C*****                                                               ***GKS40440
C*****   Call the setup frame and finish frame subroutines to        ***GKS40450
C*****   create the border and title of the display frame for        ***GKS40460
C*****   the gks-co diagram of the demo. This frame of the           ***GKS40470
C*****   demo shows a flowchart type picture of how gks-co fits      ***GKS40480
C*****   in with graPHIGS and both gks and phigs applications.       ***GKS40490
C*****                                                               ***GKS40500
C***********************************************************************GKS40510
                                                                        GKS40520
      CALL SETUPF(.FALSE.,.FALSE.)                                      GKS40530
      CALL GTXS(TXTX,TXTY,20,'        XGKS        ')                    GKS40540
      CALL FINSHF(.FALSE.)                                              GKS40550
                                                                        GKS40560
C***********************************************************************GKS40570
C*****                                                               ***GKS40580
C*****   Set the window, view and some text and fill area            ***GKS40590
C*****   parameters.                                                 ***GKS40600
C*****                                                               ***GKS40610
C***********************************************************************GKS40620
                                                                        GKS40630
      CALL GSCHXP(0.9)                                                  GKS40640
      CALL GSTXFP(GFONT1,GSTRKP)                                        GKS40650
      CALL GSWN(2,0.0,18.0,0.0,18.0)                                    GKS40660
      CALL GSVP(2,0.0,1.0,0.0,1.0)                                      GKS40670
      CALL GSELNT(2)                                                    GKS40680
      CALL GSFAIS(1)                                                    GKS40690
                                                                        GKS40700
C***********************************************************************GKS40710
C*****                                                               ***GKS40720
C*****   Create a blue rectangle to hold the text which will         ***GKS40730
C*****   denote the graPHIGS API layer in the flowchart picture.     ***GKS40740
C*****                                                               ***GKS40750
C***********************************************************************GKS40760
                                                                        GKS40770
      CALL GSFACI(GBLUE)                                                GKS40780
      CALL GFA(4,PHIGSX,PHIGSY)                                         GKS40790
      CALL GSCHH(1.0)                                                   GKS40800
      CALL GSTXAL(GACENT,GAHALF)                                        GKS40810
      CALL GSTXCI(GWHITE)                                               GKS40820
      CALL GSTXFP(GFONT2,GSTRKP)                                        GKS40830
      CALL GSCHSP(0.1)                                                  GKS40840
      CALL GTXS(8.0,6.55,17,'graPHIGS (TM) API')                        GKS40850
                                                                        GKS40860
C***********************************************************************GKS40870
C*****                                                               ***GKS40880
C*****   Create three small light blue rectangles at the bottom      ***GKS40890
C*****   of the flowchart to denote the workstations connected       ***GKS40900
C*****   via graPHIGS.                                               ***GKS40910
C*****                                                               ***GKS40920
C***********************************************************************GKS40930
                                                                        GKS40940
      CALL GSFACI(GLBLUE)                                               GKS40950
      CALL GSTXCI(GBLUE)                                                GKS40960
      CALL GSCHH(1.0)                                                   GKS40970
      CALL GSCHSP(0.1)                                                  GKS40980
      CALL GFA(4,PWKX,PWKY)                                             GKS40990
      CALL GTXS(2.5,2.35,1,'1')                                         GKS41000
      DO 1800 I=1,4                                                     GKS41010
        X(I) = PWKX(I) + 3.0                                            GKS41020
 1800 CONTINUE                                                          GKS41030
      CALL GFA(4,X,PWKY)                                                GKS41040
      CALL GTXS(5.5,2.35,1,'2')                                         GKS41050
      DO 1810 I=1,4                                                     GKS41060
        X(I) = X(I) + 8.0                                               GKS41070
 1810 CONTINUE                                                          GKS41080
      CALL GFA(4,X,PWKY)                                                GKS41090
      CALL GTXS(13.5,2.35,1,'N')                                        GKS41100
      CALL GSTXCI(GWHITE)                                               GKS41110
      CALL GSCHH(0.6)                                                   GKS41120
      CALL GSCHSP(0.5)                                                  GKS41130
      CALL GSCHXP(1.2)                                                  GKS41140
      CALL GSTXAL(GACENT,GAVNOR)                                        GKS41150
      CALL GTXS(8.0,0.35,12,'WORKSTATIONS')                             GKS41160
                                                                        GKS41170
C***********************************************************************GKS41180
C*****                                                               ***GKS41190
C*****   Create an orange rectangle above the graPHIGS API layer     ***GKS41200
C*****   to denote the GKS-CO interpreter layer.                     ***GKS41210
C*****                                                               ***GKS41220
C***********************************************************************GKS41230
                                                                        GKS41240
      CALL GSCHSP(0.1)                                                  GKS41250
      CALL GSCHXP(0.9)                                                  GKS41260
      CALL GSFACI(GORNGE)                                               GKS41270
      CALL GFA(4,GKSX,GKSY)                                             GKS41280
      CALL GSCHH(0.9)                                                   GKS41290
      CALL GSTXAL(GACENT,GAHALF)                                        GKS41300
      CALL GTXS(11.4,11.5,6,'GKS-CO')                                   GKS41310
                                                                        GKS41320
C***********************************************************************GKS41330
C*****                                                               ***GKS41340
C*****   Write Metafile Input Output directly under the picture of   ***GKS41350
C*****   a disk storage device (this will be drawn later in the      ***GKS41360
C*****   program).                                                   ***GKS41370
C*****                                                               ***GKS41380
C***********************************************************************GKS41390
                                                                        GKS41400
      CALL GSCHH(0.2)                                                   GKS41410
      CALL GSCHSP(0.2)                                                  GKS41420
      CALL GSTXFP(GFONT1,GSTRKP)                                        GKS41430
      CALL GTXS(16.3,11.5,8,'METAFILE')                                 GKS41440
      CALL GTXS(16.3,11.2,5,'INPUT')                                    GKS41450
      CALL GTXS(16.3,10.9,6,'OUTPUT')                                   GKS41460
                                                                        GKS41470
C***********************************************************************GKS41480
C*****                                                               ***GKS41490
C*****   Create two green rectangles at the top of the flowchart,    ***GKS41500
C*****   one denoting a phigs application and one denoting a gks     ***GKS41510
C*****   application.                                                ***GKS41520
C*****                                                               ***GKS41530
C***********************************************************************GKS41540
                                                                        GKS41550
      CALL GSCHSP(0.1)                                                  GKS41560
      CALL GSFACI(GMGREN)                                               GKS41570
      CALL GSTXCI(GWHITE)                                               GKS41580
      CALL GSCHSP(0.2)                                                  GKS41590
      CALL GFA(4,GAPPX,GAPPY)                                           GKS41600
      CALL GSCHH(0.3)                                                   GKS41610
      CALL GTXS(11.4,15.15,15,'GKS APPLICATION')                        GKS41620
      CALL GSCHH(0.25)                                                  GKS41630
      CALL GFA(4,PAPPX,PAPPY)                                           GKS41640
      CALL GTXS(4.6,15.15,25,'graPHIGS (TM) APPLICATION')               GKS41650
                                                                        GKS41660
C***********************************************************************GKS41670
C*****                                                               ***GKS41680
C*****   Calculate the points in order to draw a grey-blue disk      ***GKS41690
C*****   storage device off to the side of the GKS-CO layer that     ***GKS41700
C*****   will denote the GKS-CO metafile capability.                 ***GKS41710
C*****                                                               ***GKS41720
C***********************************************************************GKS41730
                                                                        GKS41740
      CALL GSTXCI(GBLUE)                                                GKS41750
      CALL GSCHSP(0.1)                                                  GKS41760
      PLX(27) = 16.3                                                    GKS41770
      PLY(27) = 13.0                                                    GKS41780
      ALPHA = -3.141593 / 2.0                                           GKS41790
      DO 1820 I = 1,13                                                  GKS41800
        PLX(I) = PLX(27) + 1.00 * SIN(ALPHA)                            GKS41810
        PLY(I) = PLY(27) + 0.25 * COS(ALPHA)                            GKS41820
        ALPHA  = ALPHA + 3.141593 / 12.0                                GKS41830
 1820 CONTINUE                                                          GKS41840
      ALPHA = 3.141593 / 2.0                                            GKS41850
      PLY(27) = 12.0                                                    GKS41860
      DO 1830 I=14,26                                                   GKS41870
        PLX(I) = PLX(27) + 1.00 * SIN(ALPHA)                            GKS41880
        PLY(I) = PLY(27) + 0.25 * COS(ALPHA)                            GKS41890
        ALPHA  = ALPHA + 3.141593 / 12.0                                GKS41900
 1830 CONTINUE                                                          GKS41910
      CALL GSFACI(GGRAYB)                                               GKS41920
      CALL GFA(26,PLX,PLY)                                              GKS41930
      PLX(27) = PLX(1)                                                  GKS41940
      PLY(27) = PLY(1)                                                  GKS41950
                                                                        GKS41960
C***********************************************************************GKS41970
C*****                                                               ***GKS41980
C*****   Outline the disk storage icon with a white polyline.        ***GKS41990
C*****                                                               ***GKS42000
C***********************************************************************GKS42010
                                                                        GKS42020
      CALL GSPLCI(GWHITE)                                               GKS42030
      CALL GPL(27,PLX,PLY)                                              GKS42040
      PLX(27) = 16.3                                                    GKS42050
      PLY(27) = 13.0                                                    GKS42060
      ALPHA = 3.141593 / 2.0                                            GKS42070
      DO 1840 I=1,13                                                    GKS42080
        PLX(I) = PLX(27) + 1.00 * SIN(ALPHA)                            GKS42090
        PLY(I) = PLY(27) + 0.25 * COS(ALPHA)                            GKS42100
        ALPHA  = ALPHA + 3.141593 / 12.0                                GKS42110
 1840 CONTINUE                                                          GKS42120
      CALL GPL(13,PLX,PLY)                                              GKS42130
                                                                        GKS42140
C***********************************************************************GKS42150
C*****                                                               ***GKS42160
C*****   Draw directional arrows between all the layers in the       ***GKS42170
C*****   flowchart.                                                  ***GKS42180
C*****                                                               ***GKS42190
C***********************************************************************GKS42200
                                                                        GKS42210
      CALL GSPLCI(GLBLUE)                                               GKS42220
      CALL GPL(10,PLATTX,PLATTY)                                        GKS42230
      CALL GPL(10,PAPX,PAPY)                                            GKS42240
      CALL GPL(10,GAGX,GAGY)                                            GKS42250
      CALL GPL(10,GPX,GPY)                                              GKS42260
      CALL GPL(10,PWX,PWY)                                              GKS42270
      DO 1850 I=1,10                                                    GKS42280
        X(I) = PWX(I) + 3.0                                             GKS42290
 1850 CONTINUE                                                          GKS42300
      CALL GPL(10,X,PWY)                                                GKS42310
      DO 1860 I=1,10                                                    GKS42320
        X(I) = X(I) + 8.0                                               GKS42330
 1860 CONTINUE                                                          GKS42340
      CALL GPL(10,X,PWY)                                                GKS42350
                                                                        GKS42360
C***********************************************************************GKS42370
C*****                                                               ***GKS42380
C*****   Draw three light blue polymarkers inbetween the connected   ***GKS42390
C*****   workstations to denote that there are up to 'n'             ***GKS42400
C*****   workstations connected. Return the transformation view      ***GKS42410
C*****   back to view one.                                           ***GKS42420
C*****                                                               ***GKS42430
C***********************************************************************GKS42440
                                                                        GKS42450
      CALL GSPMCI(GLBLUE)                                               GKS42460
      CALL GPM(3,WKMX,WKMY)                                             GKS42470
      CALL GSELNT(1)                                                    GKS42480
      CONT = .TRUE.
	CALL SLEEP(4)
      RETURN                                                            GKS42500
      END                                                               GKS42510
                                                                        GKS42520
                                                                        GKS42530
                                                                        GKS42540
C***********************************************************************GKS42550
C***********************************************************************GKS42560
C*****                                                               ***GKS42570
C*****   Subroutine Name: PRIMIT                                     ***GKS42580
C*****                                                               ***GKS42590
C*****   Subroutine Function: This subroutine demonstrates all the   ***GKS42600
C*****                        output primitives and their            ***GKS42610
C*****                        attributes.                            ***GKS42620
C*****                                                               ***GKS42630
C*****   Calls Subroutines:   SETUPF,FINSHF,RCHOI,DEMOPM,DEMOPL      ***GKS42640
C*****                        DEMOTX,DEMOFA,DEMOCA,DEMOGD            ***GKS42650
C*****                                                               ***GKS42660
C***********************************************************************GKS42670
C***********************************************************************GKS42680
                                                                        GKS42690
      SUBROUTINE PRIMIT                                                 GKS42700
      INCLUDE 'gkspar.inc'
                                                                        GKS42720
      INTEGER*4    WKID,WTYPE,LCHSTR(7),LENGTH,CHNR,ERRIND,NUMELA       GKS42730
      INTEGER*4    NUMELA,I,J,PIXEL(9,9)                                GKS42740
                                                                        GKS42750
      REAL*4       FRAME(10),XMIN,XMAX,YMIN,YMAX,TXTX,TXTY              GKS42760
      REAL*4       PMX(13),PMY(13),ALPHA,PI,PX(5),PY(5)                 GKS42770
      REAL*4       ZX(6),ZY(6),FA3(6),FA4(8),FA5(10)                    GKS42780
                                                                        GKS42790
      CHARACTER*50 MENU                                                 GKS42800
      CHARACTER*80 DTREC(7)                                             GKS42810
                                                                        GKS42820
      COMMON       /WINF/ WKID,WTYPE                                    GKS42830
      COMMON       /TEXT/ TXTX,TXTY                                     GKS42840
      COMMON       /LIMITS/ XMIN,XMAX,YMIN,YMAX                         GKS42850
                                                                        GKS42860
      DATA FRAME /0.000,0.000,0.995,0.995,0.000,                        GKS42870
     *            0.000,0.995,0.995,0.000,0.000/                        GKS42880
      DATA PIXEL /08,08,13,13,13,13,13,08,08,                           GKS42890
     *            08,08,08,13,13,13,08,08,08,                           GKS42900
     *            08,08,08,13,13,13,08,08,08,                           GKS42910
     *            08,12,08,13,13,12,08,12,08,                           GKS42920
     *            12,12,12,12,12,12,12,12,12,                           GKS42930
     *            12,12,12,12,12,12,12,12,12,                           GKS42940
     *            12,12,12,12,12,12,12,12,12,                           GKS42950
     *            08,12,12,12,12,12,12,12,08,                           GKS42960
     *            08,08,12,12,12,12,12,08,08/                           GKS42970
      DATA FA3 /0.350,0.750,0.750,0.250,0.250,0.850/                    GKS42980
      DATA FA4 /0.250,0.650,0.650,0.250,0.100,0.100,0.450,0.450/        GKS42990
      DATA FA5 /0.450,0.650,0.750,0.550,0.350,                          GKS43000
     *          0.350,0.350,0.550,0.650,0.550/                          GKS43010
      DATA MENU  /'POLYMARKERPOLYLINETEXTFILL AREACELL ARRAYGDPRETURN'/ GKS43020
      DATA LCHSTR /10,8,4,9,10,3,6/                                     GKS43030
      DATA NUMELA /7/                                                   GKS43040
                                                                        GKS43050
C***********************************************************************GKS43060
C*****                                                               ***GKS43070
C*****   Use pack data record to set up the prompt array for the     ***GKS43080
C*****   choice input device.                                        ***GKS43090
C*****                                                               ***GKS43100
C***********************************************************************GKS43110
                                                                        GKS43120
      CALL GPREC(0,0,0,0.0,NUMELA,LCHSTR,MENU,NUMELA,                   GKS43130
     *           ERRIND,LENGTH,DTREC)                                   GKS43140
                                                                        GKS43150
C***********************************************************************GKS43160
C*****                                                               ***GKS43170
C*****   One big loop to keep drawing main output primitives menu    ***GKS43180
C*****   until the user selects the 'return' choice (lpfk 7).        ***GKS43190
C*****   Draw the outside border around the output primitives menu.  ***GKS43200
C*****                                                               ***GKS43210
C***********************************************************************GKS43220
                                                                        GKS43230
 1900 CALL SETUPF(.TRUE.,.FALSE.)                                       GKS43240
      CALL GTXS(TXTX,0.886,6,'OUTPUT')                                  GKS43250
      CALL GTXS(TXTX,0.788,10,'PRIMITIVES')                             GKS43260
      CALL FINSHF(.TRUE.)                                               GKS43270
                                                                        GKS43280
C***********************************************************************GKS43290
C*****                                                               ***GKS43300
C*****   Draw six boxes which indicate the six choice alternatives:  ***GKS43310
C*****     Choice 1: Polymarker                                      ***GKS43320
C*****            2: Polyline                                        ***GKS43330
C*****            3: Text                                            ***GKS43340
C*****            4: Fill Area                                       ***GKS43350
C*****            5: Cell Array                                      ***GKS43360
C*****            6: GDPs                                            ***GKS43370
C*****                                                               ***GKS43380
C***********************************************************************GKS43390
                                                                        GKS43400
      CALL GSCHH(0.060)                                                 GKS43410
      CALL GSWN(2,0.0,1.0,0.0,1.0)                                      GKS43420
      CALL GSVP(2,0.075,0.325,0.395,0.695)                              GKS43430
      CALL GSELNT(2)                                                    GKS43440
                                                                        GKS43450
C******************************************************************     GKS43460
C*****                                                        *****     GKS43470
C*****   Draw the polymarker choice box, place the text       *****     GKS43480
C*****   'polymarker' in the box, and draw a circle of        *****     GKS43490
C*****   polymarkers in the box.                              *****     GKS43500
C*****                                                        *****     GKS43510
C******************************************************************     GKS43520
                                                                        GKS43530
      CALL GPL(5,FRAME(1),FRAME(6))                                     GKS43540
      CALL GSCHSP(0.2)                                                  GKS43550
      CALL GTXS(0.100,0.900,12,'POLYMARKER  ')                          GKS43560
      PMX(1) = 0.50                                                     GKS43570
      PMY(1) = 0.45                                                     GKS43580
      PI     = 3.141593                                                 GKS43590
      ALPHA  = 0.0                                                      GKS43600
      DO 1910 I=2,13                                                    GKS43610
        PMX(I) = PMX(1) + 0.3 * SIN(ALPHA)                              GKS43620
        PMY(I) = PMY(1) + 0.3 * COS(ALPHA)                              GKS43630
        ALPHA  = ALPHA + 2.0 * PI / 12.0                                GKS43640
 1910 CONTINUE                                                          GKS43650
      CALL GSMK(GAST)                                                   GKS43660
      CALL GSMKSC(1.0)                                                  GKS43670
      CALL GSPMCI(GGREEN)                                               GKS43680
      CALL GPM(13,PMX,PMY)                                              GKS43690
                                                                        GKS43700
C******************************************************************     GKS43710
C*****                                                        *****     GKS43720
C*****   Draw the polyline choice box, place the text         *****     GKS43730
C*****   'polyline' in the box, and draw a star with          *****     GKS43740
C*****   blue polylines in the box.                           *****     GKS43750
C*****                                                        *****     GKS43760
C******************************************************************     GKS43770
                                                                        GKS43780
      CALL GSVP(2,0.375,0.625,0.395,0.695)                              GKS43790
      CALL GPL(5,FRAME(1),FRAME(6))                                     GKS43800
      CALL GTXS(0.100,0.900,12,'POLYLINE    ')                          GKS43810
      ALPHA  = 0.0                                                      GKS43820
      DO 1920 I=1,5                                                     GKS43830
        PX(I) = 0.5 +0.35 * SIN(ALPHA)                                  GKS43840
        PY(I) = 0.4 +0.35 * COS(ALPHA)                                  GKS43850
        ALPHA  = ALPHA + 2.0 * PI / 5.0                                 GKS43860
 1920 CONTINUE                                                          GKS43870
      CALL GSLN(GSOLID)                                                 GKS43880
      CALL GSLWSC(1.0)                                                  GKS43890
      CALL GSPLCI(GLBLUE)                                               GKS43900
      DO 1940 I=1,2                                                     GKS43910
        DO 1930 J=0,5                                                   GKS43920
          K = MOD(I*J,5)                                                GKS43930
          ZX(J+1) = PX(K+1)                                             GKS43940
          ZY(J+1) = PY(K+1)                                             GKS43950
 1930   CONTINUE                                                        GKS43960
        CALL GPL(6,ZX,ZY)                                               GKS43970
 1940 CONTINUE                                                          GKS43980
      CALL GSPLCI(GYELOW)                                               GKS43990
                                                                        GKS44000
C******************************************************************     GKS44010
C*****                                                        *****     GKS44020
C*****   Draw the text choice box, place the text 'text'      *****     GKS44030
C*****   in the box, and draw 'GKS TEXT OUTPUT' in green,     *****     GKS44040
C*****   red, and blue in the box.                            *****     GKS44050
C*****                                                        *****     GKS44060
C******************************************************************     GKS44070
                                                                        GKS44080
      CALL GSVP(2,0.675,0.925,0.395,0.695)                              GKS44090
      CALL GPL(5,FRAME(1),FRAME(6))                                     GKS44100
      CALL GTXS(0.100,0.900,12,'TEXT        ')                          GKS44110
      CALL GSTXFP(1,GCHARP)                                             GKS44120
      CALL GSCHH(0.2)                                                   GKS44130
      CALL GSTXCI(GGREEN)                                               GKS44140
      CALL GTXS(0.22,0.60,3,'GKS')                                      GKS44150
      CALL GSTXFP(2,GCHARP)                                             GKS44160
      CALL GSCHH(0.1)                                                   GKS44170
      CALL GSTXCI(GRED)                                                 GKS44180
      CALL GTXS(0.27,0.45,4,'TEXT')                                     GKS44190
      CALL GSTXFP(1,GSTRKP)                                             GKS44200
      CALL GSCHH(0.1)                                                   GKS44210
      CALL GSTXCI(GLBLUE)                                               GKS44220
      CALL GSCHUP(1.0,1.0)                                              GKS44230
      CALL GTXS(0.18,0.33,1,'O')                                        GKS44240
      CALL GSCHUP(1.0,2.0)                                              GKS44250
      CALL GTXS(0.30,0.25,1,'U')                                        GKS44260
      CALL GSCHUP(1.0,4.0)                                              GKS44270
      CALL GTXS(0.42,0.20,1,'T')                                        GKS44280
      CALL GSCHUP(-1.0,4.0)                                             GKS44290
      CALL GTXS(0.57,0.19,1,'P')                                        GKS44300
      CALL GSCHUP(-1.0,2.0)                                             GKS44310
      CALL GTXS(0.68,0.23,1,'U')                                        GKS44320
      CALL GSCHUP(-1.0,1.0)                                             GKS44330
      CALL GTXS(0.79,0.30,1,'T')                                        GKS44340
      CALL GSCHUP(0.0,1.0)                                              GKS44350
      CALL GSTXFP(1,GSTRKP)                                             GKS44360
      CALL GSTXCI(GYELOW)                                               GKS44370
      CALL GSCHH(0.06)                                                  GKS44380
      CALL GSCHSP(0.2)                                                  GKS44390
                                                                        GKS44400
C******************************************************************     GKS44410
C*****                                                        *****     GKS44420
C*****   Draw the fill area box, place the text 'fill         *****     GKS44430
C*****   area' in the box, and draw some different solid      *****     GKS44440
C*****   color shapes in the box.                             *****     GKS44450
C*****                                                        *****     GKS44460
C******************************************************************     GKS44470
                                                                        GKS44480
      CALL GSVP(2,0.075,0.325,0.045,0.345)                              GKS44490
      CALL GPL(5,FRAME(1),FRAME(6))                                     GKS44500
      CALL GTXS(0.100,0.900,12,'FILL AREA   ')                          GKS44510
      CALL GSFASI(1)                                                    GKS44520
      CALL GSFAIS(GSOLID)                                               GKS44530
      CALL GSFACI(GMAGNT)                                               GKS44540
      CALL GFA(3,FA3(1),FA3(4))                                         GKS44550
      CALL GSFACI(GMRED)                                                GKS44560
      CALL GFA(4,FA4(1),FA4(5))                                         GKS44570
      CALL GSFACI(GMGREN)                                               GKS44580
      CALL GFA(5,FA5(1),FA5(6))                                         GKS44590
                                                                        GKS44600
C******************************************************************     GKS44610
C*****                                                        *****     GKS44620
C*****   Draw the cell array box, place the text 'cell        *****     GKS44630
C*****   array' in the box, and draw a picture of a tree      *****     GKS44640
C*****   using the cell array primitive.                      *****     GKS44650
C*****                                                        *****     GKS44660
C******************************************************************     GKS44670
                                                                        GKS44680
      CALL GSVP(2,0.375,0.625,0.045,0.345)                              GKS44690
      CALL GPL(5,FRAME(1),FRAME(6))                                     GKS44700
      CALL GTXS(0.100,0.900,12,'CELL ARRAY  ')                          GKS44710
      CALL GCA(0.250,0.250,0.750,0.750,9,9,1,1,9,9,PIXEL)               GKS44720
                                                                        GKS44730
C******************************************************************     GKS44740
C*****                                                        *****     GKS44750
C*****   Draw the GDPs box, place the text 'GDPs' in the      *****     GKS44760
C*****   box, and display 'not available' in the box also     *****     GKS44770
C*****   (as there is no GDP support in GKS-CO as yet).       *****     GKS44780
C*****                                                        *****     GKS44790
C******************************************************************     GKS44800
                                                                        GKS44810
      CALL GSVP(2,0.675,0.925,0.045,0.345)                              GKS44820
      CALL GPL(5,FRAME(1),FRAME(6))                                     GKS44830
      CALL GTXS(0.100,0.900,12,'GDPs        ')                          GKS44840
      CALL GTXS(0.05,0.4,13,'NOT AVAILABLE')                            GKS44850
                                                                        GKS44860
C******************************************************************     GKS44870
C*****                                                        *****     GKS44880
C*****   Initialize the choice device and call subroutine     *****     GKS44890
C*****   RCHOI to wait for valid choice input. Analyze the    *****     GKS44900
C*****   input and call the appropriate routine.              *****     GKS44910
C*****                                                        *****     GKS44920
C******************************************************************     GKS44930
                                                                        GKS44940
      CALL GSCHSP(0.1)                                                  GKS44950
      CALL GSELNT(1)                                                    GKS44960
      CALL GINCH(WKID,1,1,1,3,XMIN,XMAX,YMIN,YMAX,LENGTH,DTREC)         GKS44970
      CALL SLEEP(5)
      DO 1949 CHNR=1,5
      IF (CHNR.EQ.1) THEN                                               GKS44990
        CALL DEMOPM                                                     GKS45000
      ELSE                                                              GKS45010
        IF (CHNR.EQ.2) THEN                                             GKS45020
          CALL DEMOPL                                                   GKS45030
        ELSE                                                            GKS45040
          IF (CHNR.EQ.3) THEN                                           GKS45050
            CALL DEMOTX                                                 GKS45060
          ELSE                                                          GKS45070
            IF (CHNR.EQ.4) THEN                                         GKS45080
              CALL DEMOFA                                               GKS45090
            ELSE                                                        GKS45100
              IF (CHNR.EQ.5) THEN                                       GKS45110
                CALL DEMOCA                                             GKS45120
              ELSE                                                      GKS45130
                IF (CHNR.EQ.6) THEN                                     GKS45140
                  CALL DEMOGD                                           GKS45150
                ELSE                                                    GKS45160
                  GOTO 1950                                             GKS45170
                ENDIF                                                   GKS45180
              ENDIF                                                     GKS45190
            ENDIF                                                       GKS45200
          ENDIF                                                         GKS45210
        ENDIF                                                           GKS45220
      ENDIF                                                             GKS45230
1949  CONTINUE
                                                                        GKS45240
C***********************************************************************GKS45250
C*****                                                               ***GKS45260
C*****   Continue the big loop to draw the output primitive menu.    ***GKS45270
C*****                                                               ***GKS45280
C***********************************************************************GKS45290
                                                                        GKS45300
      GOTO 1900                                                         GKS45310
                                                                        GKS45320
C***********************************************************************GKS45330
C*****                                                               ***GKS45340
C*****   The user has setected the 'return' choice.                  ***GKS45350
C*****                                                               ***GKS45360
C***********************************************************************GKS45370
                                                                        GKS45380
 1950 RETURN                                                            GKS45390
      END                                                               GKS45400
                                                                        GKS45410
                                                                        GKS45420
                                                                        GKS45430
C***********************************************************************GKS45440
C***********************************************************************GKS45450
C*****                                                               ***GKS45460
C*****   Subroutine Name: RCHOI                                      ***GKS45470
C*****                                                               ***GKS45480
C*****   Subroutine Function: This subroutine will wait on valid     ***GKS45490
C*****                        input from the choice device (lpfks).  ***GKS45500
C*****                        If invalid input is received, put out  ***GKS45510
C*****                        an appropriate message and wait for    ***GKS45520
C*****                        another input choice.                  ***GKS45530
C*****                                                               ***GKS45540
C***********************************************************************GKS45550
C***********************************************************************GKS45560
                                                                        GKS45570
      SUBROUTINE RCHOI(WKID,MXCHNR,CHNR)                                GKS45580
                                                                        GKS45590
      INTEGER    WKID,MXCHNR,CHNR,STAT                                  GKS45600
                                                                        GKS45610
 2000 CALL GRQCH(WKID,1,STAT,CHNR)                                      GKS45620
      IF (STAT.EQ.1) THEN                                               GKS45630
        IF ((CHNR.GT.0).AND.(CHNR.LE.MXCHNR)) THEN                      GKS45640
          CALL GMSG(WKID,' ')                                           GKS45650
          GOTO 2010                                                     GKS45660
        ELSE                                                            GKS45670
          CALL GMSGS(WKID,22,' INVALID CHOICE NUMBER')                  GKS45680
          STAT=0                                                        GKS45690
        ENDIF                                                           GKS45700
      ELSE                                                              GKS45710
        CALL GMSGS(WKID,22,' CHOICE NOT SUCCESSFUL')                    GKS45720
      ENDIF                                                             GKS45730
      GOTO 2000                                                         GKS45740
 2010 CONTINUE                                                          GKS45750
      RETURN                                                            GKS45760
      END                                                               GKS45770
                                                                        GKS45780
C***********************************************************************GKS45790
C***********************************************************************GKS45800
C*****                                                               ***GKS45810
C*****   Subroutine Name: RMENU                                      ***GKS45820
C*****                                                               ***GKS45830
C*****   Subroutine Function: This subroutine will use pack data     ***GKS45840
C*****                        record to get the choice prompts       ***GKS45850
C*****                        set up and then it will initialize     ***GKS45860
C*****                        the choice device (lpfks) and call     ***GKS45870
C*****                        subroutine RCHOI to wait for valid     ***GKS45880
C*****                        choice input.                          ***GKS45890
C*****                                                               ***GKS45900
C*****   Calls Subroutines:   RCHOI                                  ***GKS45910
C*****                                                               ***GKS45920
C*****   Input:   prompt - true: one prompt (return)                 ***GKS45930
C*****                     false: two prompts (continue and return)  ***GKS45940
C*****                                                               ***GKS45950
C*****   Output:  cont   - true:  user selected the continue choice  ***GKS45960
C*****                   - false: user selected the return choice    ***GKS45970
C*****                                                               ***GKS45980
C***********************************************************************GKS45990
C***********************************************************************GKS46000
                                                                        GKS46010
      SUBROUTINE RMENU(PROMPT,CONT)                                     GKS46020
      INCLUDE 'gkspar.inc'
                                                                        GKS46040
      INTEGER*4    WKID,LCHSTR(1),LCHST2(2),LENG1,ERRIND,CHNR           GKS46050
      REAL*4       XMIN,XMAX,YMIN,YMAX                                  GKS46060
      LOGICAL      PROMPT,CONT                                          GKS46070
      CHARACTER*6  MENU                                                 GKS46080
      CHARACTER*14 MENU2                                                GKS46090
      CHARACTER*80 DTREC1(1)                                            GKS46100
                                                                        GKS46110
      COMMON       /WINF/   WKID,WTYPE                                  GKS46120
      COMMON       /LIMITS/ XMIN,XMAX,YMIN,YMAX                         GKS46130
                                                                        GKS46140
      DATA         MENU   /'RETURN'/                                    GKS46150
      DATA         MENU2  /'CONTINUERETURN'/                            GKS46160
      DATA         LCHSTR /6/                                           GKS46170
      DATA         LCHST2 /8,6/                                         GKS46180
                                                                        GKS46190
      IF (PROMPT) THEN                                                  GKS46200
        CALL GPREC(0,0,0,0.0,1,LCHSTR,MENU,LCHSTR,ERRIND,LENG1,DTREC1)  GKS46210
        NUMELA = 1                                                      GKS46220
      ELSE                                                              GKS46230
        CALL GPREC(0,0,0,0.0,2,LCHST2,MENU2,LCHST2,ERRIND,LENG1,DTREC1) GKS46240
        NUMELA = 2                                                      GKS46250
      ENDIF                                                             GKS46260
      CALL GINCH(WKID,1,GOK,1,3,XMIN,XMAX,YMIN,YMAX,LENG1,DTREC1)       GKS46270
      CALL RCHOI(WKID,NUMELA,CHNR)                                      GKS46280
      IF ((CHNR.EQ.1).AND.(NUMELA.EQ.2)) THEN                           GKS46290
        CONT = .TRUE.                                                   GKS46300
      ELSE                                                              GKS46310
        CONT = .FALSE.                                                  GKS46320
      ENDIF                                                             GKS46330
      RETURN                                                            GKS46340
      END                                                               GKS46350
                                                                        GKS46360
                                                                        GKS46370
                                                                        GKS46380
C***********************************************************************GKS46390
C***********************************************************************GKS46400
C*****                                                               ***GKS46410
C*****   Subroutine Name: SETCOL                                     ***GKS46420
C*****                                                               ***GKS46430
C*****   Subroutine Function: To set up the color table for the      ***GKS46440
C*****                        workstation indicated in WKID.         ***GKS46450
C*****                                                               ***GKS46460
C***********************************************************************GKS46470
C***********************************************************************GKS46480
                                                                        GKS46490
      SUBROUTINE SETCOL                                                 GKS46500
                                                                        GKS46510
      INTEGER*4  WKID,I,WTYPE                                           GKS46520
      REAL*4     RED(15),GREEN(15),BLUE(15)                             GKS46530
      COMMON     /WINF/ WKID,WTYPE                                      GKS46540
                                                                        GKS46550
      DATA RED   /1.00,0.00,0.00,1.00,1.00,1.00,0.66,0.00,0.00,0.33,    GKS46560
     *            0.33,0.00,0.66,0.66,1.00/                             GKS46570
      DATA GREEN /0.00,1.00,0.00,1.00,0.33,0.00,0.00,1.00,0.66,0.33,    GKS46580
     *            1.00,0.66,0.00,0.66,1.00/                             GKS46590
      DATA BLUE  /0.00,0.00,1.00,0.00,0.00,1.00,0.66,1.00,1.00,0.66,    GKS46600
     *            0.33,0.00,0.00,0.66,1.00/                             GKS46610
                                                                        GKS46620
      DO 2100 I=1,15                                                    GKS46630
         CALL GSCR(WKID,I,RED(I),GREEN(I),BLUE(I))                      GKS46640
 2100 CONTINUE                                                          GKS46650
      RETURN                                                            GKS46660
      END                                                               GKS46670
                                                                        GKS46680
                                                                        GKS46690
                                                                        GKS46700
C***********************************************************************GKS46710
C***********************************************************************GKS46720
C*****                                                               ***GKS46730
C*****   Subroutine Name: SETUPF                                     ***GKS46740
C*****                                                               ***GKS46750
C*****   Subroutine Function: To draw the frame border for each      ***GKS46760
C*****                        screen of the demo.                    ***GKS46770
C*****                                                               ***GKS46780
C***********************************************************************GKS46790
C***********************************************************************GKS46800
                                                                        GKS46810
      SUBROUTINE SETUPF(MFRAME,FRAME3)                                  GKS46820
      INCLUDE 'gkspar.inc'
                                                                        GKS46840
      INTEGER*4  WKID,WTYPE                                             GKS46850
      REAL*4     PX1(5),PX2(5),PY1(5),PY2(5),PY4(5)                     GKS46860
      REAL*4     TXTX,TXTY                                              GKS46870
      LOGICAL    MFRAME,FRAME3                                          GKS46880
      COMMON     /WINF/ WKID,WTYPE                                      GKS46890
      COMMON     /TEXT/ TXTX,TXTY                                       GKS46900
                                                                        GKS46910
      DATA PX1 /0.000,0.000,1.000,1.000,0.000/                          GKS46920
      DATA PX2 /0.015,0.015,0.985,0.985,0.015/                          GKS46930
      DATA PY1 /0.000,1.000,1.000,0.000,0.000/                          GKS46940
      DATA PY2 /0.765,0.985,0.985,0.765,0.765/                          GKS46950
      DATA PY4 /0.915,0.985,0.985,0.915,0.915/                          GKS46960
                                                                        GKS46970
C***********************************************************************GKS46980
C*****                                                               ***GKS46990
C*****   Clear the workstation whether it is empty or not.           ***GKS47000
C*****                                                               ***GKS47010
C***********************************************************************GKS47020
                                                                        GKS47030
      CALL GCLRWK(WKID,GALWAY)                                          GKS47040
                                                                        GKS47050
C***********************************************************************GKS47060
C*****                                                               ***GKS47070
C*****   Draw the outside frame border.                              ***GKS47080
C*****                                                               ***GKS47090
C***********************************************************************GKS47100
                                                                        GKS47110
      CALL GCRSG (1259)
      CALL GSLN(GLSOLI)                                                 GKS47120
      CALL GSLWSC(1.0)                                                  GKS47130
                                                                        GKS47140
C***********************************************************************GKS47150
C*****                                                               ***GKS47160
C*****   If drawing menu 3, the color indices will be invalid,       ***GKS47170
C*****   so a special index must be set up for the color wanted.     ***GKS47180
C*****   If not, set the color to yellow from the current color      ***GKS47190
C*****   table. Draw the top line.                                   ***GKS47200
C*****                                                               ***GKS47210
C***********************************************************************GKS47220
                                                                        GKS47230
      IF (FRAME3) THEN                                                  GKS47240
       CALL GSCR(WKID,15,1.0,1.0,0.0)                                   GKS47250
       CALL GSPLCI(15)                                                  GKS47260
      ELSE                                                              GKS47270
       CALL GSPLCI(GYELOW)                                              GKS47280
      ENDIF                                                             GKS47290
      CALL GPL(5,PX1,PY1)                                               GKS47300
                                                                        GKS47310
C***********************************************************************GKS47320
C*****                                                               ***GKS47330
C*****   Draw the frame that goes around the text at the top of      ***GKS47340
C*****   the mainframe.                                              ***GKS47350
C*****                                                               ***GKS47360
C***********************************************************************GKS47370
                                                                        GKS47380
      TXTX=0.5                                                          GKS47390
      IF (MFRAME) THEN                                                  GKS47400
       CALL GPL(5,PX2,PY2)                                              GKS47410
       CALL GSCHH(0.072)                                                GKS47420
       TXTY=0.835                                                       GKS47430
      ELSE                                                              GKS47440
       CALL GPL(5,PX2,PY4)                                              GKS47450
       CALL GSCHH(0.036)                                                GKS47460
       TXTY=0.931                                                       GKS47470
      ENDIF                                                             GKS47480
                                                                        GKS47490
C***********************************************************************GKS47500
C*****                                                               ***GKS47510
C*****   Set up the text attributes and go back to the routine       ***GKS47520
C*****   from which this subroutine was called.                      ***GKS47530
C*****                                                               ***GKS47540
C***********************************************************************GKS47550
                                                                        GKS47560
      CALL GSTXFP(GFONT2,GCHARP)                                        GKS47570
      CALL GSTXAL(GACENT,GAVNOR)                                        GKS47580
      CALL GSTXCI(GYELOW)                                               GKS47590
      CALL GSCHXP(1.0)                                                  GKS47600
      RETURN                                                            GKS47610
      END                                                               GKS47620
                                                                        GKS47630
                                                                        GKS47640
                                                                        GKS47650
        subroutine lines 

        integer wkid
        integer lintyp
        integer i
        integer npnts
        integer color
        integer nxtcol

        real x1(2)
        real y1(2)
        real x2(2)
        real y2(2)
        real x(1404)
        real y(1404)

        data wkid   /1/


        lintyp = 1

           call gsln (lintyp)

           call gclrwk (wkid, 1)
           x1(1) = -1.0
           x1(2) =  1.0
           x2(1) = -1.0
           x2(2) =  1.0
           do 10 i = 100, 0, -1
              y1(1) = float (i) / 100.0
              y1(2) = y1(1)
              y2(1) = -y1(1)
              y2(2) = y2(1)
              color = nxtcol ('LINE')
              call gpl (2, x1, y1)
              call gpl (2, y1, x1)
              call gpl (2, x2, y2)
              call gpl (2, y2, x2)
  10       continue

           call gclrwk (wkid, 1)
           x1(2) = -1
           x2(2) =  1
           y1(1) =  1
           y2(1) = -1
           do 20 i = -100, 100
              x1(1) = float (i) / 100.0
              x2(1) = -x1(1)
              y1(2) =  x2(1)
              y2(2) =  x1(1)
              color = nxtcol ('LINE')
              call gpl (2, x1, y1)
              call gpl (2, x1, y2)
              call gpl (2, x2, y1)
              call gpl (2, x2, y2)
  20       continue

           do 30 npnts = 1400, 1403
              call getpts (npnts, 400, -0.5, 0.0, 0.25, 0.5, x, y)
              call gclrwk (wkid, 1)
              color = nxtcol ('LINE')
              call gpl (npnts+1, x, y)
  30       continue

           do 40 npnts = 900, 903
              call getpts (npnts, 300, 0.25, 1.0, 0.5, 0.0, x, y)
              call gclrwk (wkid, 1)
              color = nxtcol ('LINE')
              call gpl (npnts+1, x, y)
  40       continue

           do 50 npnts = 1301, 1303
              call getpts (npnts, 650, -0.25, 0.1, 0.15, 0.0, x, y)
              call gclrwk (wkid, 1)
              color = nxtcol ('LINE')
              call gpl (npnts+1, x, y)
  50       continue

           do 60 npnts = 1200, 1203
              call getpts (npnts, 300, 0.1, 0.0, 0.1, 0.2, x, y)
              call gclrwk (wkid, 1)
              color = nxtcol ('LINE')
              call gpl (npnts+1, x, y)
  60       continue


        call gsln (1)
	call sleep(2)

        return

        end

        subroutine getpts (npnts, nfreq, hole, xrot, yrot, zrot, x, y)
        integer npnts
        integer nfreq
        real hole
        real xrot
        real yrot
        real zrot
        real x(*)
        real y(*)

        integer ndx
        real diam
        real radius
        real t
        real sint
        real z
        real dist


        diam = 1.0 - hole
        radius = (1.0 + hole) / 2.0

        do 10 ndx = 1, npnts
           t = float (ndx) / float (npnts) * 2.0 * 3.1415927
           sint = sin (float (nfreq) * t)
           if (abs (sint) .le. 0.5) then
              z = diam * sint
              dist = -diam * sqrt (0.25 - sint*sint)
           elseif (sint .le. 0.0) then
              z = -diam * (1.0+sint)
              dist = diam * sqrt (0.25 - (1.0+sint)*(1.0+sint))
           else
              z = diam * (1.0-sint)
              dist = diam * sqrt (0.25 - (1.0-sint)*(1.0-sint))
           endif

           dist = radius + dist
           x(ndx) = dist * sin (t)
           y(ndx) = dist * cos (t)

           call rotate (x(ndx), y(ndx), z, xrot)
           call rotate (y(ndx), z, x(ndx), yrot)
           call rotate (z, x(ndx), y(ndx), zrot)

  10    continue

        x(npnts+1) = x(1)
        y(npnts+1) = y(1)

        return
        end

        subroutine rotate (x, y, z, angle)
        real x
        real y
        real z
        real angle

        real r
        real theta


        r = sqrt (y * y + z * z)
        if ((y .eq. 0.0) .and. (z .eq. 0.0)) then
           theta = 0.0
        else
           theta = atan2 (y, z)
        endif

        y = r * sin (theta + angle)
        z = r * cos (theta + angle)

        return
        end

        integer function nxtcol (type)
        character*4 type

        integer ncols, color
        common /color/ ncols, color


           color = color + 10
           if (color .ge. 256) color = 1
           if (type(1:1) .eq. 'L') then
              call gsplci (color)
           elseif (type(1:1) .eq. 'F') then
              call gsfaci (color)
           elseif (type(1:1) .eq. 'T') then
              call gstxci (color)
           endif

        nxtcol = color

        return
        end

        subroutine plot ()

        integer level
        common /level/ level

        character*4 funcnm(3)
        integer wkid
        integer grdfrq
        integer if
        integer func
        integer i
        integer errind
        integer col1, col2
        real x(100)
        real y(100)
        real txexpx(4)
        real txexpy(4)
        real cpx
        real cpy

        data funcnm /' sin', ' cos', 'frac'/
        data wkid   /1/



        do 40 grdfrq = 1, 1

           do 30 if = 0, 3 

              func = mod (if, 3) + 1

              call gclrwk (wkid, 1)

              call grid (grdfrq, col1, col2)

              call gschh (0.06)
              call gstxal (1, 3)
              if (col1 .ne. 0) call gstxci (col1)
              call gtx (0.65, 0.95, funcnm(func))

              call gsln (1)
              do 10 i = 1, 100
                 x(i) = -0.8 + float(i-1) * 1.7 / 99.0
                 if (func .eq. 1) then
                    y(i) =  0.8 * sin (float (i-1) / 9.9)
                 elseif (func .eq. 2) then
                    y(i) =  0.8 * cos (float (i-1) / 9.9)
                 elseif (func .eq. 3) then
                    y(i) =  0.8 * (float (i-1) / 9.9 -
     +                             float (int (float (i-1) / 9.9)))
                 endif
  10          continue

              if (col1 .ne. 0) call gsplci (col1)
              call gpl (100, x, y)

              call gschh (0.06)
              call gstxal (1, 3)
              if (col2 .ne. 0) call gstxci (col2)
              call gtx (0.65, 0.87, funcnm(func))
              call gqtxx (wkid, 0.65, 0.87, funcnm(func), errind, cpx,
     +                    cpy, txexpx, txexpy)
              call gschh (0.04)
              call gstxal (1, 4)
              call gtx (cpx, 0.87, '2')

              call gsln (2)
              do 20 i = 1, 100
                 if (func .eq. 1) then
                    y(i) = sin (float (i-1) / 9.9)
                 elseif (func .eq. 2) then
                    y(i) = cos (float (i-1) / 9.9)
                 elseif (func .eq. 3) then
                    y(i) =  float (i-1) / 9.9 -
     +                      float (int (float (i-1) / 9.9))
                 endif
                 y(i) = 0.8 * y(i) * y(i)
  20          continue
              if (col2 .ne. 0) call gsplci (col2)
              call gpl (100, x, y)
	      call sleep(3)

  30       continue

  40    continue
        call gsln (1)
        call gstxal (0, 0)
        if (level .ge. 0) call gslwsc (1.0)


        return
 100    format (1X, 'Plot started at ', A8, '.')
 101    format (1X, 'Plot finished at ', A8, '.  Elapsed time:', F8.2,
     +              ' seconds.', /)
        end

        subroutine grid (grdfrq, col1, col2)
        integer grdfrq
        integer col1
        integer col2

        integer level
        common /level/ level

        character*3 xlabel
        character*5 ylabel
        integer grdlns
        integer i
        integer color
        integer nxtcol
        real x(2)
        real y(2)
        real offset
        real lwidth


        color = nxtcol ('LINE')
        call gsln (1)
        if (level .ge. 0) call gslwsc (1.0)
        grdlns = grdfrq * 8
        x(1) = -0.8
        x(2) =  0.9
        offset = 0.0
        if (grdlns .ne. 0) offset = 1.6 / float (grdlns)
        do 10 i = 0, grdlns
           if ((grdfrq .gt. 1) .and. (mod (i, grdfrq) .eq. 0) .and.
     +         (level .ge. 0)) call gslwsc (3.0)
           y(1) = -0.8 + float(i) * offset
           y(2) = y(1)
           call gpl (2, x, y)
           if ((grdfrq .gt. 1) .and. (mod (i, grdfrq) .eq. 0) .and.
     +         (level .ge. 0)) call gslwsc (1.0)
  10    continue

        if (level .ge. 0) call gstxfp (1, 2)
        call gschh (0.04)
        call gstxal (3, 3)
        do 20 i = -4, 4
           call gtx (-0.83, -0.8 + float (i+4) / 8.0 * 1.6, ylabel)
  20    continue

        grdlns = grdfrq * 9
        y(1) = -0.8
        y(2) =  0.8
        offset = 0.0
        if (grdlns .ne. 0) offset = 1.7 / float (grdlns)
        do 30 i = 0, grdlns
           if ((grdfrq .gt. 1) .and. (mod (i, grdfrq) .eq. 0) .and.
     +         (level .ge. 0)) call gslwsc (3.0)
           x(1) = -0.8 + float(i) * offset
           x(2) =  x(1)
           call gpl (2, x, y)

           if ((grdfrq .gt. 1) .and. (mod (i, grdfrq) .eq. 0) .and.
     +         (level .ge. 0)) call gslwsc (1.0)
  30    continue

        call gstxal (2, 1)
        do 40 i = 0, 9
           call gtx (-0.8 + float (i) * 1.7 / 9.0, -0.83, xlabel)
  40    continue

        call gschh (0.10)
        call gstxal (0, 0)
        color = nxtcol ('TEXT')
        call gtx (-0.75, 0.85, 'Plot Test')

        lwidth = 3.0
        if (grdfrq .gt. 1) lwidth = 5.0
        if (level .ge. 0) call gslwsc (lwidth)
        x(1) = 0.20
        x(2) = 0.60
        y(1) = 0.95
        y(2) = 0.95
        call gsln (1)
        call gpl (2, x, y)

        y(1) = 0.87
        y(2) = 0.87
        call gsln (2)
        call gpl (2, x, y)
        call gsln (1)

        return
 100    format (F5.2)
 101    format (F3.1)
        end

