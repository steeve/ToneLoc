/*
   ÚÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ¿
   ³                                                                          ³
   ³  CXLDEF.H  -  CXL is Copyright (c) 1987-1990 by Mike Smedley.            ³
   ³                                                                          ³
   ³  This header file contains miscellaneous function prototypes and         ³
   ³  definitions.                                                            ³
   ³                                                                          ³
   ÀÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÙ
*/

#if defined(__TURBOC__)                     /* Turbo C */
    #if __STDC__
        #define _Cdecl
    #else
        #define _Cdecl  cdecl
    #endif
    #define _Near
#elif defined(__ZTC__)                      /* Zortech C++ */
    #define _Cdecl
    #define _Near
#elif defined(M_I86) && !defined(__ZTC__)   /* Microsoft C/QuickC */
    #if !defined(NO_EXT_KEYS)
        #define _Cdecl  cdecl
        #define _Near   near
    #else
        #define _Cdecl
        #define _Near
    #endif
#endif

/*---------------------------[ function prototypes ]-------------------------*/

char           *_Cdecl biosver(void);
int             _Cdecl clockcal(void);
char           *_Cdecl cxlver(void);
void            _Cdecl delay_(unsigned duration);
unsigned        _Cdecl expmem(void);
unsigned        _Cdecl extmem(void);
int             _Cdecl fcrypt(char *file,char *key);
int             _Cdecl gameport(void);
int             _Cdecl machid(void);
int             _Cdecl mathchip(void);
int             _Cdecl numflop(void);
int             _Cdecl numpar(void);
int             _Cdecl numser(void);
char           *_Cdecl randfile(void);
void            _Cdecl sound_(unsigned pitch,unsigned duration);
void            _Cdecl _stdoutch(int ch);
char           *_Cdecl sysdate(int dtype);
char           *_Cdecl systime(int ttype);
int             _Cdecl tabstop(int col,int tabwidth);
unsigned long   _Cdecl timer(void);

/*-----------------[ definitions for common control codes ]------------------*/

#define NUL  0
#define BEL  7
#define BS   8
#define HT   9
#define LF   10
#define FF   12
#define CR   13
#define ESC  27

/*------------------------[ definition of NULL ]-----------------------------*/

#if !defined(NULL)
    #if defined(__TURBOC__)                     /* Turbo C */
        #if defined(__TINY__) || defined(__SMALL__) || defined(__MEDIUM__)
            #define NULL    0
        #else
            #define NULL    0L
        #endif
    #elif defined(__ZTC__)                      /* Zortech C++ */
        #ifdef LPTR
            #define NULL    0L
        #else
            #define NULL    0
        #endif
    #elif defined(M_I86) && !defined(__ZTC__)   /* Microsoft C/QuickC */
        #if defined(M_I86SM) || defined(M_I86MM)
            #define NULL    0
        #else
            #define NULL    0L
        #endif
    #endif
#endif

/*-----------------------[ Boolean logic conditions ]------------------------*/

#define NO      0
#define YES     1
#define FALSE   0
#define TRUE    1
#define OFF     0
#define ON      1

/*----------------[ machine ID codes returned from machid() ]----------------*/

#define IBMPC       255     /* IBM PC             */
#define IBMPCXT     254     /* IBM PC/XT          */
#define IBMPCJR     253     /* IBM PCjr           */
#define IBMPCAT     252     /* IBM PC/AT          */
#define IBMPCXT2    251     /* IBM PC/XT          */
#define IBMCONV     249     /* IBM PC Convertible */
#define SPERRYPC    48      /* Sperry PC          */

/*-----------------------[ macro-function definitions ]-----------------------*/

#if !defined(MK_FP)
#define MK_FP(seg,ofs)  ((void far *) (((unsigned long)(seg) << 16) | \
                        (unsigned)(ofs)))
#endif
#define beep() _stdoutch(BEL)
