/*
   ÚÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ¿
   ³                                                                          ³
   ³  CXLPRN.H  -  CXL is Copyright (c) 1987-1990 by Mike Smedley.            ³
   ³                                                                          ³
   ³  This header file contains function prototypes and definitions for       ³
   ³  printer functions.                                                      ³
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

void     _Cdecl lprintc(int ch);
void     _Cdecl lprintf(const char *format,...);
void     _Cdecl lprintns(char *str,int count);
void     _Cdecl lprints(char *str);
void     _Cdecl lprintsb(char *str,int reps);
void     _Cdecl lprintsu(char *str);
void     _Cdecl scrndump(void);
void     _Cdecl windump(int srow,int scol,int erow,int ecol);

/*-----------[ printer Escape codes for Epson compatible printers ]----------*/

    /*  Example:  lprints(L_BFON"This is bold printing"L_BFOFF);    */

#define L_BFOFF     "\033F"     /* turns bold faced printing off  */
#define L_BFON      "\033E"     /* turns bold faced printing on   */
#define L_DWOFF     "\033W0"    /* turns double wide printing off */
#define L_DWON      "\033W1"    /* turns double wide printing on  */
#define L_ELITE     "\033M"     /* sets printer in 12 CPI mode    */
#define L_INIT      "\033@"     /* initializes printer            */
#define L_ITALOFF   "\0335"     /* turns italicized printing off  */
#define L_ITALON    "\0334"     /* turns italicized printing on   */
#define L_PICA      "\033P"     /* sets printer in 10 CPI mode    */
#define L_ULOFF     "\033-0"    /* turns underlined printing off  */
#define L_ULON      "\033-1"    /* turns underlined printing on   */

/*-----------------------[ macro-function definitions ]----------------------*/

#define lcrlf()     lprintc(10)
