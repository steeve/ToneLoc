/*
   ÚÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ¿
   ³                                                                          ³
   ³  CXLVID.H  -  CXL is Copyright (c) 1987-1990 by Mike Smedley.            ³
   ³                                                                          ³
   ³  This header file contains function prototypes and definitions for       ³
   ³  screen/video functions.  Windowing functions are defined in CXLWIN.H    ³
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


#elif defined(__ZTC__)                      // Zortech C++
    #define _Cdecl
    #define _Near
#elif defined(M_I86) && !defined(__ZTC__)   // Microsoft C/QuickC
    #if !defined(NO_EXT_KEYS)
        #define _Cdecl  cdecl
        #define _Near   near
    #else
        #define _Cdecl
        #define _Near
    #endif
#endif


/*---------------------------[ function prototypes ]-------------------------*/

void     _Cdecl box_(int srow,int scol,int erow,int ecol,int btype,int attr);
void     _Cdecl cclrscrn(int attr);
int      _Cdecl cgardbyte(char far *src);
int      _Cdecl cgardword(int far *src);
void     _Cdecl cgareadn(int far *src,int *dest,unsigned n);
void     _Cdecl cgawrbyte(char far *dest,int chr);
void     _Cdecl cgawriten(int *src,int far *dest,unsigned n);
void     _Cdecl cgawrstr(char far *dest,char *string,int attr);
void     _Cdecl cgawrword(int far *dest,int chratr);
void     _Cdecl clreol_(void);
void     _Cdecl clrscrn(void);
int      _Cdecl disktoscrn(char *fname);
int      _Cdecl disktowin(char *fname);
void     _Cdecl fill_(int srow,int scol,int erow,int ecol,int ch,int atr);
void     _Cdecl getcursz(int *sline,int *eline);
void     _Cdecl gotoxy_(int row,int col);
void     _Cdecl hidecur(void);
void     _Cdecl lgcursor(void);
int      _Cdecl mapattr(int attr);
void     _Cdecl mode(int mode_code);
void     _Cdecl printc(int row,int col,int attr,int ch);
void     _Cdecl prints(int row,int col,int attr,char *str);
void     _Cdecl putchat(int ch,int attr);
unsigned _Cdecl readchat(void);
void     _Cdecl readcur(int *row,int *col);
void     _Cdecl revattr(int count);
int      _Cdecl revsattr(int attr);
int      _Cdecl scrntodisk(char *fname);
void     _Cdecl setattr(int attr,int count);
void     _Cdecl setcursz(int sline,int eline);
int      _Cdecl setlines(int numlines);
int      _Cdecl setvparam(int setting);
void     _Cdecl showcur(void);
void     _Cdecl smcursor(void);
void     _Cdecl spc(int num);
void     _Cdecl srestore(int *sbuf);
int     *_Cdecl ssave(void);
int      _Cdecl videoinit(void);
int      _Cdecl vidmode(void);
int      _Cdecl vidtype(void);
int      _Cdecl wintodisk(int srow,int scol,int erow,int ecol,char *fname);

/*-------------[ display adapter types returned from vidtype() ]-------------*/

#define V_NONE      0
#define V_MDA       1
#define V_EGAMONO   2
#define V_MCGAMONO  3
#define V_VGAMONO   4
#define V_HGC       5
#define V_HGCPLUS   6
#define V_INCOLOR   7
#define V_CGA       8
#define V_EGA       9
#define V_MCGA      10
#define V_VGA       11

/*------------------------[ video parameter settings ]-----------------------*/

#define VP_DMA    0     /* direct screen writes                     */
#define VP_CGA    1     /* direct screen writes, eliminate CGA snow */
#define VP_BIOS   2     /* BIOS screen writes                       */
#define VP_MONO   3     /* monochrome attribute translation on      */
#define VP_COLOR  4     /* monochrome attribute translation off     */

/*------------------------[ video information record ]-----------------------*/

struct _vinfo_t {
    unsigned int    videoseg;   /* video buffer segment address */
    unsigned char   adapter;    /* video adapter type           */
    unsigned char   numrows;    /* number of displayed rows     */
    unsigned char   numcols;    /* number of displayed columns  */
    unsigned char   cheight;    /* character height in pixels   */
    unsigned char   cwidth;     /* character width  in pixels   */
    unsigned char   mono;       /* is it a monochrome adapter?  */
    unsigned char   mapattr;    /* map color attribs to mono?   */
    unsigned char   cgasnow;    /* is CGA snow present?         */
    unsigned char   usebios;    /* use BIOS for video writes?   */
    unsigned char   dvcheck;    /* check for DESQview?          */
    unsigned char   dvexist;    /* is DESQview present?         */
};

extern struct _vinfo_t _Near _Cdecl _vinfo;

/*--------------[ attribute codes for functions that use them ]--------------*/

#if     !defined(BLACK)     /* foreground colors */
#define BLACK       0
#define BLUE        1
#define GREEN       2
#define CYAN        3
#define RED         4
#define MAGENTA     5
#define BROWN       6
#define YELLOW      14
#define WHITE       15
#endif
#if     !defined(LGREY)
#define LGREY       7       /* CXL abbreviates LIGHT as L    */
#define DGREY       8       /* and DARK as D for ease of use */
#define LBLUE       9
#define LGREEN      10
#define LCYAN       11
#define LRED        12
#define LMAGENTA    13

#define _BLACK      0       /* background colors */
#define _BLUE       16
#define _GREEN      32
#define _CYAN       48
#define _RED        64
#define _MAGENTA    80
#define _BROWN      96
#define _LGREY      112
#endif

#if     !defined(BLINK)
#define BLINK       128     /* blink attribute */
#endif

/*-----------------------[ macro-function definitions ]----------------------*/

#if !defined(MK_FP)
#define MK_FP(seg,ofs)      ((void far *) (((unsigned long)(seg) << 16) | \
                            (unsigned)(ofs)))
#endif
#define attrib(f,b,i,bl)    ((b<<4)|(f)|(i<<3)|(bl<<7))
#define clrwin(a,b,c,d)     gotoxy_(a,b);fill_(a,b,c,d,' ',(readchat()>>8))
