/*
   ÚÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ¿
   ³                                                                          ³
   ³  CXLEMS.H  -  CXL is Copyright (c) 1987-1990 by Mike Smedley.            ³
   ³                                                                          ³
   ³  This header file contains prototypes of CXL's Expanded Memory           ³
   ³  Specification (EMS) functions.                                          ³
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

#if !defined(MK_FP)
#define MK_FP(seg,ofs)      ((void far *) (((unsigned long)(seg) << 16) | \
                            (unsigned)(ofs)))
#endif
int      _Cdecl emsalloc(int numpages);
int      _Cdecl emsdealloc(int emmhandle);
int      _Cdecl emsexist(void);
unsigned _Cdecl emsframe(void);
unsigned _Cdecl emsfree(void);
int      _Cdecl emsmap(int emmhandle,int logpage,int phypage);
int      _Cdecl emsread(char *dest,unsigned emsofs,unsigned numbytes);
unsigned _Cdecl emstotal(void);
char    *_Cdecl emsver(void);
int      _Cdecl emswrite(char *src,unsigned emsofs,unsigned numbytes);
