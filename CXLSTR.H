/*
   ÚÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ¿
   ³                                                                          ³
   ³  CXLSTR.H  -  CXL is Copyright (c) 1987-1990 by Mike Smedley.            ³
   ³                                                                          ³
   ³  This header file contains function prototypes and definitions for       ³
   ³  string manipulating functions.                                          ³
   ³                                                                          ³
   ÀÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÙ
*/

#if defined(__TURBOC__)                        !/* Turbo C */
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

int      _Cdecl cvaltype(int ch,int ctype);
double   _Cdecl cvtcf(char *field,int wholesize,int fracsize);
int      _Cdecl cvtci(char *field);
void     _Cdecl cvtfc(char *field,double value,int wholesize,int fracsize);
void     _Cdecl cvtic(char *field,int value,int size);
int      _Cdecl strblank(char *str);
char    *_Cdecl strbmatch(char *str,char *strarr[]);
int      _Cdecl strchg(char *str,int oldch,int newch);
unsigned long _Cdecl strchksum(char *str);
char    *_Cdecl strcode(char *str,char *key);
char    *_Cdecl strdel(char *substr,char *str);
char    *_Cdecl strdela(char *substr,char *str);
int      _Cdecl strichg(char *str,int oldch,int newch);
unsigned long _Cdecl strichksum(char *str);
char    *_Cdecl stridel(char *substr,char *str);
char    *_Cdecl stridela(char *substr,char *str);
char    *_Cdecl striinc(char *str1,char *str2);
char    *_Cdecl strinc(char *str1,char *str2);
char    *_Cdecl strins(char *instr,char *str,int st_pos);
int      _Cdecl striocc(char *str,int ch);
char    *_Cdecl strischg(char *str,char *find,char *replace);
int      _Cdecl strisocc(char *str1,char *str2);
char    *_Cdecl strisrep(char *str,char *search,char *replace);
char    *_Cdecl strljust(char *str);
char    *_Cdecl strltrim(char *str);
int      _Cdecl strmatch(char *str1,char *str2);
char    *_Cdecl strmid(char *str,int st_pos,int num_chars);
int      _Cdecl strocc(char *str,int ch);
char    *_Cdecl strrjust(char *str);
char    *_Cdecl strrol(char *str,int count);
char    *_Cdecl strror(char *str,int count);
char    *_Cdecl strschg(char *str,char *find,char *replace);
char    *_Cdecl strsetsz(char *str,int newsize);
char    *_Cdecl strshl(char *str,int count);
char    *_Cdecl strshr(char *str,int count);
int      _Cdecl strsocc(char *str1,char *str2);
char    *_Cdecl strsrep(char *str,char *search,char *replace);
char    *_Cdecl strtrim(char *str);
char    *_Cdecl struplow(char *str);
int      _Cdecl touplow(char *str,char *pos,int ch);

/*-----------------------[ macro-function definitions ]----------------------*/

#define strbtrim(st)        strtrim(strltrim(st))
#define strleft(st,nu)      strmid(st,0,nu)
#define strright(st,nu)     strmid(st,strlen(st)-nu,nu)
#define strrtrim(st)        strtrim(st)
