/*
   ÚÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ¿
   ³                                                                          ³
   ³  CXLMOU.H  -  CXL is Copyright (c) 1987-1990 by Mike Smedley.            ³
   ³                                                                          ³
   ³  This header file contains function prototypes and definitions for       ³
   ³  Microsoft compatible mouse functions.                                   ³
   ³                                                                          ³
   ÀÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÙ
*/

#if defined(__TURBOC__)                         /* Turbo C */
    #if __STDC__
        #define _Cdecl
    #else
        #define _Cdecl  cdecl
    #endif
    #define _Near
#elif defined(__ZTC__)                          /* Zortech C++ */
    #define _Cdecl
    #define _Near
#elif defined(M_I86) && !defined(__ZTC__)       /* Microsoft C/QuickC */
    #if !defined(NO_EXT_KEYS)
        #define _Cdecl  cdecl
        #define _Near   near
    #else
        #define _Cdecl
        #define _Near
    #endif
#endif

/*---------------------------[ function prototypes ]-------------------------*/

void    _Cdecl msbclear(void);
void    _Cdecl msbpress(int button,int *bstat,int *bcount,int *row,int *col);
void    _Cdecl msbreles(int button,int *bstat,int *bcount,int *row,int *col);
void    _Cdecl mscondoff(int srow,int scol,int erow,int ecol);
void    _Cdecl mscursor(unsigned curtype,unsigned smask,unsigned cmask);
void    _Cdecl msgotoxy(int row,int col);
void    _Cdecl mshbounds(int leftcol,int rightcol);
void    _Cdecl mshidecur(void);
int     _Cdecl msinit(void);
void    _Cdecl msmotion(int *rowcount,int *colcount);
void    _Cdecl msshowcur(void);
void    _Cdecl msspeed(int xratio,int yratio);
void    _Cdecl msstatus(int *bstat,int *row,int *col);
void    _Cdecl mssupport(int type);
void    _Cdecl msvbounds(int toprow,int botrow);

extern int _Near _Cdecl _mouse;  /* mouse support level */

/*----------------------[ mouse support definitions ]------------------------*/

#define MS_NONE     0   /* no mouse support                         */
#define MS_KEYS     1   /* mouse movement emulates arrow keys       */
#define MS_CURS     2   /* free-moving mouse cursor where supported */
#define MS_FULL     3   /* full mouse support (MS_KEYS | MS_CURS)   */
