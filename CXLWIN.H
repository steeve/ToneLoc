/*
   ÚÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ¿
   ³                                                                          ³
   ³  CXLWIN.H  -  CXL is Copyright (c) 1987-1990 by Mike Smedley.            ³
   ³                                                                          ³
   ³  This header file contains function prototypes and definitions for       ³
   ³  windowing functions.                                                    ³
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


/*------------------------------[ WINDOW type ]------------------------------*/

typedef int     WINDOW;

/*---------------------------[ function prototypes ]-------------------------*/

int      _Cdecl wactiv(WINDOW whandle);
int      _Cdecl wborder(int btype);
int      _Cdecl wbox(int wsrow,int wscol,int werow,int wecol,int btype
                ,int attr);
int      _Cdecl wbprintc(int bord,int offs,int attr,int ch);
int      _Cdecl wcclear(int attr);
int      _Cdecl wcenters(int wrow,int attr,char *str);
int      _Cdecl wchgattr(int battr,int wattr);
int      _Cdecl wchkbox(int wsrow,int wscol,int werow,int wecol);
int      _Cdecl wchkcol(int wcol);
int      _Cdecl wchkcoord(int wrow,int wcol);
int      _Cdecl wchkrow(int wrow);
int      _Cdecl wclose(void);
int      _Cdecl wcloseall(void);
int      _Cdecl wclreol(void);
int      _Cdecl wclreos(void);
WINDOW   _Cdecl wcopy(int nsrow,int nscol);
int      _Cdecl wdelline(int wrow,int direc);
int      _Cdecl wdrag(int direction);
int      _Cdecl wdump(void);
int      _Cdecl wdupc(int ch,int count);
int      _Cdecl wdups(char *str,int count);
char    *_Cdecl werrmsg(void);
int      _Cdecl wfill(int wsrow,int wscol,int werow,int wecol,int ch,int attr);
struct   _wrec_t *_Cdecl wfindrec(WINDOW whandle);
int      _Cdecl wgetc(void);
int      _Cdecl wgetchf(char *valid,int defchar);
int      _Cdecl wgetns(char *str,int maxchars);
int      _Cdecl wgets(char *str);
int      _Cdecl wgetyn(int cdefault);
int      _Cdecl wgotoxy(int wrow,int wcol);
WINDOW   _Cdecl whandle(void);
int      _Cdecl whelpcat(int cat);
int      _Cdecl whelpclr(void);
int      _Cdecl whelpdef(char *file,unsigned key,int winattr,int textattr,
                         int selattr,int barattr,void (_Cdecl *open)(void));
int      _Cdecl whelpop(void);
int      _Cdecl whelpopc(void);
int      _Cdecl whelppcat(int cat);
int      _Cdecl whelpush(void);
int      _Cdecl whelpushc(int cat);
int      _Cdecl whelpwin(int srow,int scol,int erow,int ecol,int btype,
                         int title);
int      _Cdecl whide(void);
int      _Cdecl whline(int wsrow,int wscol,int count,int btype,int attr);
WINDOW   _Cdecl windowat(int row,int col);
int      _Cdecl winpbeg(int fieldattr,int textattr);
int      _Cdecl winpdef(int wrow,int wcol,char *str,char *format,int fconv,
                        int mode,int (_Cdecl *validate) (char *),int help);
int      _Cdecl winpfba(void (_Cdecl *before)(void),
                        void (_Cdecl *after)(void));
struct _field_t *_Cdecl winpffind(int wrow,int wcol);
int      _Cdecl winpkey(unsigned (_Cdecl *getkey)(int *),unsigned *termkey);
int      _Cdecl winpread(void);
int      _Cdecl winputsf(char *str,char *fmt);
int      _Cdecl winsline(int wrow,int direc);
int      _Cdecl wmenubeg(int srow,int scol,int erow,int ecol,int btype,
                         int battr,int wattr,void (_Cdecl *open)(void));
int      _Cdecl wmenubegc(void);
int      _Cdecl wmenuend(int taginit,int menutype,int barwidth,int textpos,
                         int textattr,int scharattr,int noselattr,int barattr);
int      _Cdecl wmenuget(void);
int      _Cdecl wmenuiba(void (_Cdecl *before)(void),
                         void (_Cdecl *after)(void));
int      _Cdecl wmenuidsab(int tagid);
int      _Cdecl wmenuienab(int tagid);
struct   _item_t *_Cdecl wmenuifind(int tagid);
int      _Cdecl wmenuinext(int tagid);
int      _Cdecl wmenuitem(int wrow,int wcol,char *str,int schar,int tagid,
                          int fmask,void (_Cdecl *select)(void),
                          unsigned hotkey,int help);
int      _Cdecl wmenuitxt(int wrow,int wcol,int attr,char *str);
int      _Cdecl wmessage(char *str,int border,int leftofs,int attr);
int      _Cdecl wmove(int nsrow,int nscol);
WINDOW   _Cdecl wopen(int srow,int scol,int erow,int ecol,int btype,int battr,
                      int wattr);
int      _Cdecl wperror(char *message);
int      _Cdecl wpgotoxy(int wrow,int wcol);
char    *_Cdecl wpickfile(int srow,int scol,int erow,int ecol,int btype,
                          int bordattr,int winattr,int barattr,int title,
                          char *filespec,void (_Cdecl *open)(void));
int      _Cdecl wpickstr(int srow,int scol,int erow,int ecol,int btype,
                         int bordattr,int winattr,int barattr,char *strarr[],
                         int initelem,void (_Cdecl *open)(void));
int      _Cdecl wprintc(int wrow,int wcol,int attr,int ch);
int      _Cdecl wprintf(const char *format,...);
int      _Cdecl wprints(int wrow,int wcol,int attr,char *str);
int      _Cdecl wprintsf(int wrow,int wcol,int attr,char *format,char *str);
int      _Cdecl wputc(int ch);
int      _Cdecl wputns(char *str,int width);
int      _Cdecl wputs(char *str);
int      _Cdecl wputsw(char *str);
int      _Cdecl wreadcur(int *wrow,int *wcol);
void     _Cdecl wrestore(int *wbuf);
int      _Cdecl wrjusts(int wrow,int wjcol,int attr,char *str);
int     *_Cdecl wsave(int srow,int scol,int erow,int ecol);
int      _Cdecl wscanf(const char *format,...);
int      _Cdecl wscroll(int count,int direc);
int      _Cdecl wscrollbox(int wsrow,int wscol,int werow,int wecol,int count,
                           int direction);
int      _Cdecl wselstr(int wrow,int wcol,int attr,char *strarr[],
                        int initelem);
int      _Cdecl wshadoff(void);
int      _Cdecl wshadow(int attr);
int      _Cdecl wsize(int nerow,int necol);
int      _Cdecl wslide(int nsrow,int nscol);
int      _Cdecl wtextattr(int attr);
int      _Cdecl wtitle(char *str,int tpos,int tattr);
int      _Cdecl wunhide(WINDOW whandle);
int      _Cdecl wunlink(WINDOW w);
int      _Cdecl wvline(int wsrow,int wscol,int count,int btype,int attr);
int      _Cdecl wwprints(WINDOW whandle,int wrow,int wcol,int attr,char *str);

/*-------------[ error codes returned from windowing functions ]-------------*/

#define W_NOERROR   0       /* no error                          */
#define W_ESCPRESS  1       /* Escape key was pressed            */
#define W_ALLOCERR  2       /* memory allocation error           */
#define W_NOTFOUND  3       /* record not found                  */
#define W_NOACTIVE  4       /* no active window                  */
#define W_INVCOORD  5       /* invalid coordinates               */
#define W_INVFORMT  6       /* invalid format string             */
#define W_NOINPDEF  7       /* no input fields defined           */
#define W_STRLONG   8       /* string too long for window        */
#define W_INVBTYPE  9       /* invalid box type                  */
#define W_NOBORDER  10      /* no window border                  */
#define W_NOHIDDEN  11      /* no hidden windows                 */
#define W_NOTHIDD   12      /* window is not hidden              */
#define W_NOSELECT  13      /* no selectable menu items          */
#define W_NOITMDEF  14      /* no menu items defined             */
#define W_NOMNUEND  15      /* no end of menu specified          */
#define W_NOMNUDEF  16      /* no menu defined                   */
#define W_NOMNUBEG  17      /* no begin of menu specified        */
#define W_NOFRMDEF  18      /* no form defined                   */
#define W_NOFRMBEG  19      /* no begin of form specified        */
#define W_NOHLPDEF  20      /* no help record defined            */
#define W_HLPSTKOV  21      /* help stack overflow               */
#define W_HLPSTKUN  22      /* help stack underflow              */
#define W_DOSERROR  23      /* DOS error                         */
#define W_NOMATCH   24      /* no files matched input filespec   */
#define W_INVTAGID  25      /* invalid tag identifier            */
                                           
/*-----------------------[ window border identifiers ]----------------------*/

#define TP_BORD  0      /* top border    */
#define BT_BORD  1      /* bottom border */
#define LT_BORD  2      /* left border   */
#define RT_BORD  3      /* right border  */

/*--------------------------[ direction codes ]------------------------------*/

#define D_DOWN    0
#define D_UP      1
#define D_LEFT    2
#define D_RIGHT   3
#define SDOWN     D_DOWN    /* scroll down */
#define SUP       D_UP      /* scroll up   */

/*----------------------[ menu item record definition ]----------------------*/

struct _item_t {
    struct _item_t *prev;   /* pointer to previous record      */
    struct _item_t *next;   /* pointer to next record          */
    void *child;            /* pointer to submenu's record     */
    char *str;              /* address of selection string     */
    char *desc;             /* text description of menu option */
    void (*select) (void);  /* address of selection function   */
    void (*before) (void);  /* address of "before" function    */
    void (*after)  (void);  /* address of "after"  function    */
    unsigned hotkey;        /* hot key to select function      */
    int tagid;              /* tag identifier                  */
    int help;               /* help category number            */
    int wrow;               /* start of text - window row      */
    unsigned char wcol;     /* start of text - window column   */
    unsigned char schar;    /* quick selection character       */
    unsigned char fmask;    /* special feature mask            */
    unsigned char dwrow;    /* description window row          */
    unsigned char dwcol;    /* description window column       */
    unsigned char dattr;    /* description attribute           */
    unsigned char redisp;   /* redisplay flag                  */
};

/*--------------------[ window menu record definition ]----------------------*/

struct _menu_t {
    struct _menu_t *prev;       /* pointer to prev menu structure   */
    struct _menu_t *next;       /* pointer to next menu structure   */
    struct _menu_t *parent;     /* pointer to parent menu           */
    struct _item_t *item;       /* pointer to head menu item        */
    struct _item_t *citem;      /* pointer to current menu item     */
    void (*open) (void);        /* address of post-opening function */
    int tagcurr;                /* tag ID of item selection bar on  */
    unsigned char usecurr;      /* will menu use current window?    */
    unsigned char srow;         /* starting row of menu window      */
    unsigned char scol;         /* starting column of menu window   */
    unsigned char erow;         /* ending row of menu window        */
    unsigned char ecol;         /* ending column of menu window     */
    unsigned char btype;        /* menu window border type          */
    unsigned char battr;        /* menu window border attribute     */
    unsigned char wattr;        /* menu window attribute            */
    unsigned char menutype;     /* menu type mask                   */
    unsigned char barwidth;     /* width of menu bar or zero        */
    unsigned char textpos;      /* offset of text from start of bar */
    unsigned char textattr;     /* attribute of menu text           */
    unsigned char scharattr;    /* attribute of selection character */
    unsigned char noselattr;    /* non-selectable text attribute    */
    unsigned char barattr;      /* attribute of selection bar       */
};

/*--------------------[ window help information record ]---------------------*/

struct _help_t {
    int help[20];               /* help stack                 */
    char *file;                 /* help file name             */
    int helpptr;                /* help stack pointer         */
    unsigned int  key;          /* help hot key               */
    unsigned char winattr;      /* help window attribute      */
    unsigned char textattr;     /* help window text attribute */
    unsigned char selattr;      /* selection text attribute   */
    unsigned char barattr;      /* selection bar attribute    */
    unsigned char srow;         /* help window start row      */
    unsigned char scol;         /* help window start column   */
    unsigned char erow;         /* help window end row        */
    unsigned char ecol;         /* help window end column     */
    unsigned char btype;        /* help window box type       */
    unsigned char title;        /* display "Help" title?      */
    void (*open) (void);        /* pointer to open function   */
};

/*-----------------------[ window information record ]-----------------------*/

struct _winfo_t {
    struct _wrec_t  *active;    /* pointer to active window         */
    struct _wrec_t  *hidden;    /* pointer to head hidden window    */
    struct _menu_t  *menu;      /* pointer to head menu record      */
    struct _menu_t  *cmenu;     /* pointer to current menu record   */
    struct _help_t  *helptr;    /* pointer to help info record      */
    WINDOW handle;              /* last handle given to a window    */
    int maxfiles;               /* max files allowed in wpickfile() */
    int help;                   /* current help category            */
    int errno;                  /* error num from last window func  */
    int total;                  /* total number of open windows     */
    int mlevel,ilevel;          /* system variables used in menus   */
    unsigned char esc;          /* check for Esc in input funcions? */
    unsigned char tabwidth;     /* window TTY output tab width      */
    unsigned char fillch;       /* character to fill windows with   */
};

extern struct _winfo_t _Near _Cdecl _winfo;

/*-------------------[ fmask definitions for wmenuitem() ]-------------------*/

#define M_HASPD     1   /* has pull-down menu attached   */
#define M_NOSEL     2   /* is not selectable             */
#define M_CLOSE     4   /* close menu after select func  */
#define M_CLALL     8   /* close all menus when selected */
#define M_CLOSB    16   /* close menu before select func */

/*------------------[ menutype definitions for wmenuend() ]------------------*/

#define M_HORZ      1       /* horizontal menu         */
#define M_VERT      2       /* vertical menu           */
#define M_OMNI      7       /* omnidirectional menu    */
#define M_PD        8       /* pull-down menu          */
#define M_NOQS      16      /* disable quick selection */
#define M_SAVE      32      /* save last bar position  */

/*------------------[ special return codes from wmenuget() ]-----------------*/

#define M_EXIT      32764   /* exit menu               */
#define M_EXITALL   32765   /* exit all menus          */
#define M_PREVPD    32766   /* previous pull-down menu */
#define M_NEXTPD    32767   /* next pull-down menu     */

/*---------------------[ window input field definition ]---------------------*/

struct _field_t {
    struct _field_t *prev;      /* pointer to previous field       */
    struct _field_t *next;      /* pointer to next field           */
    char *str;                  /* address of receiving string     */
    char *buf;                  /* address of temp receive string  */
    char *format;               /* input field format string       */
    int (*validate) (char *);   /* address of validation function  */
    void (*before) (void);      /* address of "before" function    */
    void (*after)  (void);      /* address of "after"  function    */
    int help;                   /* help category number            */
    int lenbuf;                 /* length of buffer                */
    int lenfld;                 /* length of screen input field    */
    int lenfmt;                 /* length of format string         */
    unsigned char wrow;         /* start of input - window row     */
    unsigned char wcol;         /* start of input - window column  */
    unsigned char mode;         /* 0=init, 1=update, 2=cond update */
    unsigned char decpos;       /* decimal position (numeric)      */
    unsigned char redisp;       /* redisplay flag                  */
    char fconv;                 /* field conversion character      */
};
                                 
/*-------------------[ window data entry form definition ]-------------------*/

struct _form_t {
    unsigned (*getkey) (int *); /* pointer to alternate get func   */
    struct _form_t  *prev;      /* pointer to previous form record */
    struct _form_t  *next;      /* pointer to next form record     */
    struct _field_t *field;     /* pointer to head field record    */
    struct _field_t *cfield;    /* pointer to current field record */
    unsigned *termkey;          /* addr of int for terminating key */
    char *pformat;              /* format string pointer           */
    char *pbuf;                 /* buffer string pointer           */
    unsigned char cwrow;        /* current window row              */
    unsigned char cwcol;        /* current window column           */
    unsigned char decimal;      /* decimal field flag              */
    unsigned char insert;       /* insert mode flag                */
    unsigned char fieldattr;    /* field attribute                 */
    unsigned char textattr;     /* text attribute                  */
};

/*----------------------[ structure of window records ]----------------------*/

struct _wrec_t {
    struct _wrec_t *prev;   /* pointer to previous window record */
    struct _wrec_t *next;   /* pointer to next window record     */
    struct _form_t *form;   /* pointer to head form record       */
    int *wbuf;              /* address of window's buffer        */
    int *wsbuf;             /* address of window shadow's buffer */
    char *title;            /* address of window's title string  */
    WINDOW whandle;         /* window's handle                   */
    int help;               /* help category number              */
    unsigned char srow;     /* start row of window               */
    unsigned char scol;     /* start column of window            */
    unsigned char erow;     /* end row of window                 */
    unsigned char ecol;     /* end column of window              */
    unsigned char btype;    /* window's box type                 */
    unsigned char wattr;    /* window's initial text attribute   */
    unsigned char battr;    /* atrribute of window's border      */
    unsigned char border;   /* has border?  0 = no, 1 = yes      */
    unsigned char row;      /* window's current cursor row       */
    unsigned char column;   /* window's current cursor column    */
    unsigned char attr;     /* window's current text attribute   */
    unsigned char tpos;     /* position of window's title        */
    unsigned char tattr;    /* attribute of window's title       */
    unsigned char wsattr;   /* attribute of window's shadow      */
};                                 

/*------------[ window title position definitions for wtitle() ]-------------*/

#define TLEFT       1   /* left justified  */
#define TCENTER     2   /* centered        */
#define TRIGHT      3   /* right justified */

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

/*----------------------------[ macro functions ]----------------------------*/

#if !defined(MK_FP)
#define MK_FP(seg,ofs)      ((void far *) (((unsigned long)(seg) << 16) | \
                            (unsigned)(ofs)))
#endif
#define wclear()            wcclear(_winfo.active->wattr)
#define wfillch(a)          (_winfo.fillch=a)
#define whelpundef()        whelpdef(NULL,0,0,0,0,0,NULL)
#define winpfcurr()         (_winfo.active->form->cfield)
#define wisactiv(a)         (a==_winfo.active->whandle)
#define wmenumcurr()        (_winfo.cmenu)
#define wmenuicurr()        (wmenumcurr()->citem)
#define wsetesc(a)          (_winfo.esc=a)
#define wtabwidth(a)        (_winfo.tabwidth=((a==0)?1:a))
