/****************************************************************************\
*                                                                            *
*  FASTGRAF.H                                                                *
*                                                                            *
*  This file contains the C and C++ function prototypes for Fastgraph V3.02. *
*                                                                            *
*  Copyright (c) 1991-1993 Ted Gruber Software.  All rights reserved.        *
*                                                                            *
\****************************************************************************/

#ifdef __cplusplus
extern "C" {
#endif

int    fg_allocate (int);
int    fg_alloccms (int);
int    fg_allocems (int);
int    fg_allocxms (int);
int    fg_automode (void);

int    fg_bestmode (int, int, int);
void   fg_box (int, int, int, int);
void   fg_boxdepth (int, int);
void   fg_boxw (double, double, double, double);
void   fg_boxx (int, int, int, int);
void   fg_boxxw (double, double, double, double);
int    fg_button (int);

int    fg_capslock (void);
void   fg_chgattr (int);
void   fg_chgtext (char *, int);
void   fg_circle (int);
void   fg_circlef (int);
void   fg_circlefw (double);
void   fg_circlew (double);
void   fg_clipmask (char *, int, int);
void   fg_clpimage (char *, int, int);
void   fg_clprect (int, int, int, int);
void   fg_clprectw (double, double, double, double);
void   fg_copypage (int, int);
void   fg_cursor (int);

void   fg_dash (int, int, int);
void   fg_dashrel (int, int, int);
void   fg_dashrw (double, double, int);
void   fg_dashw (double, double, int);
void   fg_defcolor (int, int);
void   fg_defpages (int, int);
void   fg_dispfile (char *, int, int);
void   fg_display (char *, int, int);
void   fg_displayp (char *, int, int);
void   fg_draw (int, int);
void   fg_drawmap (char *, int, int);
void   fg_drawmask (char *, int, int);
void   fg_drawrel (int, int);
void   fg_drawrelx (int, int);
void   fg_drawrw (double, double);
void   fg_drawrxw (double, double);
void   fg_draww (double, double);
void   fg_drawx (int, int);
void   fg_drawxw (double, double);
void   fg_drect (int, int, int, int, char *);
void   fg_drectw (double, double, double, double, char *);
void   fg_drwimage (char *, int, int);

int    fg_egacheck (void);
void   fg_ellipse (int, int);
void   fg_ellipsef (int, int);
void   fg_ellipsew (double, double);
void   fg_ellipsfw (double, double);
void   fg_erase (void);

void   fg_fadein (int);
void   fg_fadeout (int);
void   fg_fillpage (void);
void   fg_flipmask (char *, int, int);
void   fg_flood (int, int);
void   fg_floodw (double, double);
void   fg_flpimage (char *, int, int);
void   fg_fontsize (int);
int    fg_freepage (int);

int    fg_getaddr (void);
int    fg_getattr (int, int);
void   fg_getblock (char far *, int, int, int, int);
int    fg_getchar (int, int);
long   fg_getclock (void);
int    fg_getcolor (void);
void   fg_getdacs (int, int, char *);
void   fg_getentry (int, int *, int *);
int    fg_gethpage (void);
void   fg_getimage (char *, int, int);
int    fg_getindex (int);
void   fg_getkey (unsigned char *, unsigned char *);
int    fg_getlines (void);
void   fg_getmap (char *, int, int);
int    fg_getmaxx (void);
int    fg_getmaxy (void);
int    fg_getmode (void);
int    fg_getpage (void);
int    fg_getpixel (int, int);
void   fg_getrgb (int, int *, int *, int *);
int    fg_getvpage (void);
void   fg_getworld (double *, double *, double *, double *);
int    fg_getxjoy (int);
int    fg_getxpos (void);
int    fg_getyjoy (int);
int    fg_getypos (void);

void   fg_hush (void);
void   fg_hushnext (void);

void   fg_imagebuf (char far *, unsigned);
long   fg_imagesiz (int, int);
int    fg_initems (void);
int    fg_initjoy (int);
void   fg_initw (void);
int    fg_initxms (void);
int    fg_inside (int *, int, int, int);
void   fg_intjoy (int, char *, char *);
void   fg_intkey (unsigned char *, unsigned char *);

void   fg_justify (int, int);

void   fg_kbinit (int);
int    fg_kbtest (int);

void   fg_locate (int, int);

int    fg_makegif (int, int, int, int, char *);
int    fg_makepcx (int, int, int, int, char *);
int    fg_makeppr (int, int, int, int, char *);
int    fg_makespr (int, int, int, int, char *);
int    fg_maprgb (int, int, int);
int    fg_measure (void);
long   fg_memavail (void);
int    fg_memory (void);
void   fg_mousebut (int, int *, int *, int *);
void   fg_mousecur (int, int);
void   fg_mousefin (void);
int    fg_mouseini (void);
void   fg_mouselim (int, int, int, int);
void   fg_mousemov (int, int);
void   fg_mousepos (int *, int *, int *);
void   fg_mouseptr (int *, int, int);
void   fg_mousespd (int, int);
void   fg_mousevis (int);
void   fg_move (int, int);
void   fg_moverel (int, int);
void   fg_moverw (double, double);
void   fg_movew (double, double);
void   fg_music (char *);
void   fg_musicb (char *, int);

int    fg_numlock (void);

long   fg_pagesize (void);
void   fg_paint (int, int);
void   fg_paintw (double, double);
void   fg_palette (int, int);
void   fg_palettes (int *);
void   fg_pan (int, int);
void   fg_panw (double, double);
void   fg_pattern (int, int);
int    fg_pcxhead (char *, char *);
int    fg_pcxmode (char *);
int    fg_playing (void);
void   fg_point (int, int);
void   fg_pointw (double, double);
void   fg_pointx (int, int);
void   fg_pointxw (double, double);
void   fg_polyfill (int *, int *, int);
void   fg_polygon (int *, int *, int);
void   fg_polygonw (double *, double *, int);
void   fg_polyline (int *, int);
void   fg_polyoff (int, int);
void   fg_print (char *, int);
void   fg_putblock (char far *, int, int, int, int);
void   fg_putimage (char *, int, int);

void   fg_quiet (void);

void   fg_rect (int, int, int, int);
void   fg_rectw (double, double, double, double);
void   fg_reset (void);
void   fg_resize (int, int);
void   fg_restore (int, int, int, int);
void   fg_restorew (double, double, double, double);
void   fg_resume (void);
void   fg_revimage (char *, int, int);
void   fg_revmask (char *, int, int);

void   fg_save (int, int, int, int);
void   fg_savew (double, double, double, double);
int    fg_scrlock (void);
void   fg_scroll (int, int, int, int, int, int);
void   fg_setangle (double);
void   fg_setattr (int, int, int);
void   fg_setcaps (int);
void   fg_setclip (int, int, int, int);
void   fg_setclipw (double, double, double, double);
void   fg_setcolor (int);
void   fg_setdacs (int, int, char *);
void   fg_setentry (int, int, int);
void   fg_setfunc (int);
void   fg_sethpage (int);
void   fg_setlines (int);
void   fg_setmode (int);
void   fg_setnum (int);
void   fg_setpage (int);
void   fg_setratio (double);
void   fg_setrgb (int, int, int, int);
void   fg_setsize (int);
void   fg_setsizew (double);
void   fg_setvpage (int);
void   fg_setworld (double, double, double, double);
int    fg_showgif (char *, int);
int    fg_showpcx (char *, int);
int    fg_showppr (char *, int);
int    fg_showspr (char *, int);
void   fg_sound (int, int);
void   fg_sounds (int *, int);
void   fg_stall (int);
void   fg_suspend (void);
int    fg_svgainit (int);
int    fg_svgastat (void);
void   fg_svgaver (int *, int *);
void   fg_swchar (char *, int, int);
double fg_swlength (char *, int);
void   fg_swtext (char *, int, int);

void   fg_tcdefine (int, int);
void   fg_tcmask (int);
void   fg_tcxfer (int, int, int, int, int, int, int, int);
int    fg_testmode (int, int);
void   fg_text (char *, int);
void   fg_transfer (int, int, int, int, int, int, int, int);

void   fg_version (int *, int *);
void   fg_voice (int, int, int, int);
void   fg_voices (int *, int);

void   fg_waitfor (int);
void   fg_waitkey (void);
void   fg_waitvr  (int);
void   fg_where (int *, int *);

int    fg_xalpha (int);
int    fg_xconvert (int);
int    fg_xscreen (double);
double fg_xworld (int);

int    fg_yalpha (int);
int    fg_yconvert (int);
int    fg_yscreen (double);
double fg_yworld (int);

#ifdef __cplusplus
}
#endif
