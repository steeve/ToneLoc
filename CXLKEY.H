/*
   ÚÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ¿
   ³                                                                          ³
   ³  CXLKEY.H  -  CXL is Copyright (c) 1987-1990 by Mike Smedley.            ³
   ³                                                                          ³
   ³  This header file contains function prototypes and definitions for       ³
   ³  keyboard functions.  Keyboard functions for windowing functions are     ³
   ³  defined in CXLWIN.H                                                     ³
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

void     _Cdecl capsoff(void);
void     _Cdecl capson(void);
struct _onkey_t *_Cdecl chgonkey(struct _onkey_t *kblist);
void     _Cdecl freonkey(void);
int      _Cdecl getchf(char *valid,int defchar);
int      _Cdecl getns(char *str,int maxchars);
unsigned _Cdecl getxch(void);
int      _Cdecl inputsf(char *str,char *fmt);
void     _Cdecl kbclear(void);
int      _Cdecl kbmhit(void);
int      _Cdecl kbput(unsigned xch);
int      _Cdecl kbputs(char *str);
unsigned _Cdecl kbstat(void);
void     _Cdecl numoff(void);
void     _Cdecl numon(void);
int      _Cdecl scancode(int ch);
int      _Cdecl setonkey(unsigned keycode,void (_Cdecl *func) (void),
                         unsigned pass);
int      _Cdecl waitkey(void);
int      _Cdecl waitkeyt(int duration);

/*------------------------[ definition of kbuf record ]----------------------*/

struct _kbuf_t {
    struct _kbuf_t *prev;       /* previous record */
    struct _kbuf_t *next;       /* next record     */
    unsigned xch;               /* keypress        */
};

/*-----------------------[ definition of onkey record ]----------------------*/

struct _onkey_t {
    struct _onkey_t *prev;      /* pointer to previous record      */
    struct _onkey_t *next;      /* pointer to next record          */
    unsigned int keycode;       /* Scan/ASCII code of trap key     */
    void (*func) (void);        /* address of onkey function       */
    unsigned int pass;          /* key to pass back, 0=don't pass  */
};

/*-------------------[ definition of keyboard info record ]------------------*/

struct _kbinfo_t {
    struct _kbuf_t *kbuf;       /* pointer to head record in key buffer      */
    struct _onkey_t *onkey;     /* pointer to head record in onkey list      */
    void (*kbloop) (void);      /* pointer to function to call while waiting */
    unsigned char inmenu;       /* inmenu flag used by menuing functions     */
    unsigned char source;       /* source of keypress 0=kb, 1=kbuf, 2=mouse  */
};

extern struct _kbinfo_t _Near _Cdecl _kbinfo;

/*-------------[ keyboard status codes returned from kbstat() ]--------------*/

#define RSHIFT      1       /* right shift pressed   */
#define LSHIFT      2       /* left shift pressed    */
#define CTRL        4       /* [Ctrl] pressed        */
#define ALT         8       /* [Alt] pressed         */
#define SCRLOCK     16      /* [Scroll Lock] toggled */
#define NUMLOCK     32      /* [Num Lock] toggled    */
#define CAPSLOCK    64      /* [Caps Lock] toggled   */
#define INS         128     /* [Ins] toggled         */

/*-----------------------[ macro-function definitions ]-----------------------*/

#if !defined(MK_FP)
#define MK_FP(seg,ofs)      ((void far *) (((unsigned long)(seg) << 16) | \
                            (unsigned)(ofs)))
#endif
#define clearkeys()         while(kbhit()) getch()
#define setkbloop(a)        (_kbinfo.kbloop=a)
