/* So what is this? Toneloc.c right before I try to add busy
   redial garbage MT thinks is important enough to stall the release,
   but can't be bothered to write. */

/*
** ToneLoc - Dialtone & Carrier locator.
** Copyright (c) 1994 by Minor Threat & Mucho Maas.
**
** This program dials numbers looking for a dialtone
** or a carrier.  It is useful for finding PBX's, loops,
** anything that gives a constant tone sound, and other
** modems.
**
*/

/* Response codes are stored in oldones[] as well as being logged.
 *
 * Key:
 *
 * (x = number of rings, more than 9 recorded as 9.)
 *
 *  00 = Undialed
 *  10 = Busy
 *  2x = Voice
 *  30 = No Dialtone
 *  40 = Noted
 *    41 = Fax
 *    42 = Girl
 *    43 = VMB
 *    44 = Yelling Asshole
 *    49 = Person that sounds like Mucho
 *  5x = Aborted
 *  6x = Ringout
 *  7x = Timeout
 *  8x = Tone
 *  9x = Carrier
 * 100 = Excluded
 * 110 = Omitted
 * 120 = Dialed
 * 130 = Blacklisted
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <dos.h>
#include <conio.h>
#include <ctype.h>
#include <stdarg.h>
#include <mem.h>
#include <alloc.h>
#include <string.h>
#include <dir.h>
#include <time.h>
#include "cxlvid.h"
#include "cxldef.h"
#include "cxlkey.h"
#include "cxlwin.h"
#include "toneloc.h"
#include "tlcfg.h"
#include "serial.h"
#include "fossil.h"
#include "getopt.h"
//#include "ctlib.h"        // Library for CTEK

#define VERSION "1.00"
#define VERSIONID 0x0100
#define METERLENGTH 30
//#define _isspace(c) ((c >= 0x09 && c <= 0x0D) || c == ' ')
#define _isspace isspace
#define LINEFEED 0x0d
#define BIGSTRING 100          // Length of our long string.
#define HUGESTRING 300        //  Length of our mega string

// define this if this is a beta version
//#define BETA
//#define DEBUG

#ifdef BETA
char     betatester[] = "Narq\0!";   // embedded beta tester name!
#endif
struct   time m_start, m_end,      /* clock ticks used to time */
         m_hours, right_now;
CONFIG   cfg;                      // config file struct
byte     DESQview;                 // Is DESQview present?
long     starttime;                /* Time started scanning */
int      current_tried=0;          /* tried this session */
struct   _scan scan;               /* data file header */
struct   _scan95 stats;            /* statistics */
int      speaker;                  /* Speaker status. 0=off, 1=on. */
int      save;                     // save seconds or rings in dat file
int      maxdials;                 // maximum # of numbers to dial
WINDOW   w_activity;               /* status window handle for CXL */
WINDOW   w_modem;                  /* modem window handle */
WINDOW   w_stats;                  /* activity window handle */
WINDOW   w_found;                  /* "Found" window handle */
WINDOW   w_carrier;                /* carrier logging window */
WINDOW   w_nudge;                  /* nudge string window */
char     configfilename[13];       /* configuration file name (.cfg) */
char     datfilename[13];          /* data file name (.dat) */
char     textfilename[13];         /* text file to read xxxx's from */
char     maskstring[40];           /* Who knows what they'll scan? */
char     rangestring[20];          /* String that holds the range */
char     exrangestring[10][20];    /* Array of strings that holds the excluded range */
int      expoint;                  /* pointer to exrangestring[][] */
word     old_point;                /* pointer to current # in oldones */
byte     oldones[10000];           /* already dialed numbers */
word     maxcodes;                 /* maximum # of possible numbers */
char     neg_mask[10][20];         /* Exclude masks : /X */
int      negpoint;                 /* Index to exclude masks */
int      check_flag;               /* whether to check for a carrier this dial */
// WHAT THE FUCK IS ^THIS M00CH0!????????
FILE     *logfile;                 /* logfile ptr */
FILE     *numberfile;              /* file of text file numbers (textfilename) */
int      speak_flag;               /* Flag for speaker toggle */
int      vol_flag;                 /* Flag for volume change */
int      NoTone=0;                 // notone counter
long     AutoTime;                 /* mytime() for next autosave */
long     startdial;                /* dial start. Needed to be global. */
word     CurrentNum;               /* Current mask number for above */

struct   black_number * black_top; // top of blacklist linked list (ooh!)
word     black_count;              // how many blacklisted #'s exist

int      smode_override;           /* Flag sets whether scan mode set
                                      from command line. Kludge. */
// ^ can't we just set scantype in commandline() and yank this variable?

int      foundnum;                 /* Number found in this run */
// ^ this _could_ be local ...
int      quiet;                    /* command line flag for silence */
struct   _scan95 ex;               /* excluded stuff compensation shit */
                                   /* Number of tried to subtract from */
                                   /* stats.tried since they are excluded */
long    unixstart;

/* Functions */

   void   main(int, char *[]);
   void   mainloop(void);             // Main dialing loop - blood n' guts
   void   init_serial(void);          // initialized serial routines
   void   init_fossil(void);          // initiali{es fossil driver routines
   void   deinit(void);               /* deinits FOSSIL */
   void   dial(char *);               /* dials telephone */
   void   hang_up(int);               /* Drop & raise DTR */
   void   quit(int);                  /* exits gracefully, saves datafile */
   int    initmodem(char *);          /* sends modem init string(s) */
   char * process(char *);            /* replaces 'X's with #'s */
   char * scanstr(int);               /* returns verbose string from scan_type */
   void   drawscreen(void);           /* draws the windowed screen */
   void   message(char *, ...);       /* updates message window */
   void   modem(char *, ...);         /* updates modem window */
   void   found(char *, char *,int);  /* updates found window */
   void   foundlog(char *, ...);      // writes to found log file
   void   log(char *, ...);           /* writes to log file */
   void   count_tried(void);          /* counts the number of tried */
   void   toggle_speaker(void);       /* initializes speaker */
   void   initvol(int);               /* Initializes volume thingy */
   void   checkdone(void);            /* Check to see if end time has been reached */
   void   waitstart(void);            /* Wait until start time if needed */
   void   meter(word, word, int, int); /* updates status meter */
   void   undometer(int, word);       /* for slick erase of status meter */
   void   exclude(char *);            /* Excludes a mask string from present dial */
   void   include(void);              /* re-includes excluded numbers into mask */
   void   dvdelay(word);              /* delay() with DESQview pauses */
   void   helpscreen(void);           /* help screen for lamers */
   void   updatestats(void);          /* update statistics window */
   void   shell(void);                /* saves screen, shells to DOS */
   int    memavail(void);             /* returns K available */
   void   updateclock(void);          /* updates clock on screen */
   void   loadconfig(char *);         /* loads in config file */
   void   setdefaults(void);          // set defaults variables
   void   loadblacklist(void);        /* loads in blacklist */
   void   loaddata(char *);           /* loads in data file */
   void   commandline(int, char *[]); /* process command line */
   int    check_response(char *);     /* check modem response */
   int    checkdupe(char *);          /* check for dupes  */
   int    checkblack(char *);         /* check for blacklisting */
   char * DateString(void);           /* 05-Mar-95 type date string */
   char * TimeString(void);           /* 10:33:03 type time string */
   char * mgetsx(char *);             // excellent function by MT (of course)
   char * GetSystemOutput(char *);    // MM's improved alternative to mgetsx
                                      // for carrier logging
   long   mytime(void);               // returns time as an absolute longint
   word   dialhour(void);             /* figures dials per hour */
   void   toggle_spk_flag(void);      /* Toggle the flag for the speaker */
   int    chopten(int);               /* if greater than 9 --> 9 */
   void   NextAuto(void);             /* Sets up next AutoSave Interval */
   void   CheckAuto(void);            /* Check for the Auto save */
   void   DatBack(char *);            /* Saves .DAT file, flushes logfile */
   void   blip(int);                  /* 50ms blip of sound */
   void   range(char *);              /* excludes all but a range */
   void   exrange(char *);            /* excludes a range */
   word   eta(word,word);             /* calculates estimated time left */
   void   customnote(char *);         /* custom note */
   void   parsetime(char *,struct time *); /* interprets '11:30p' */
   void   blank(int);                 /* screen blank */
   char * switchbrief(int);           /* For found() */
   char * switchverbose(int);         /* For found() */
   void   resultcolor(int);           /* picks the right result color */
   void   replace(char * s, char * s1, char * s2); // replaces s1 with s2
   char * trim(char *);               /* trims spaces, " lame " -> "lame" */
   char * StripParity(char * s );      /* strips parity bit from a string */
   char * StripChar(char *s, char);    /* strips a certain character from a string */
   void   OpenCarrier(void);          /* Initializes the carrier window */
   void   OpenNudge(void);		/* Initializes the nudge window */
   void   printn(int,int,int,char,int); /* prints a char n times at x,y */
   void   modemch(char);              /* output char to modem window */
   void   carrierch(char);            /* output char to carrier window */
   void   carrier_string(char *);     /* output string to carrier window */
   char   Charin();                   /* get char from FOSSIL, send to window */
   void   ModemCommand(char *);       /* send command to modem, then delay */
   int    ModemCommandChar(char);     /* a single modem command character */
   void	  NudgeChar(char);            /* Put a character in the nudge window */
   int    Carrier(void);              // is there a carrier in the house?
   void   strrep(char *,char,char);
   void   LogSystem(void);            /* Logs what comes after a carrier */

// funky-ass function pointers!  for FOSSIL/N0FOSSIL shit

   int    (* Comhit)(void);          /* comhita or com_waiting */
   void   (* Charout)(byte);         /* send char to FOSSIL & modem window */
   int    (* charin)(void);          /* charin function pointer */
   void   (* SetDTR)(int);           /* setdtr function pointer */
   int    (* GetCTS)(void);          // returns CTS state
   void   (* FlushIn)(void);         /* flushina function Pointer */

void main(int argc, char *argv[])
{
   int    cnt;
   int    loop=0;
   struct date startdate;
   int    dv_version;

   printf("ToneLoc version %s (%s)\n",VERSION,__DATE__);
//   printf("ToneLoc version %s\n",VERSION);
   printf("by Minor Threat and Mucho Maas\n\n");
   printf("Initializing System, please wait ...\n\n");

   videoinit();                         /* CXL video initialization */
   DESQview = ((dv_version=dv_there())!=0);  /* DESQview check */
   sound(2600);
   nosound();
   setdefaults();
   randomize();
   memset(&oldones,0,sizeof(oldones));
   getdate(&startdate);
   gettime(&right_now);
   unixstart = dostounix(&startdate,&right_now);
   startdial = mytime();


   if ((argc==1) || (strchr(argv[1],'?')))
      helpscreen();

#ifdef BETA
   if (!strcmp(argv[1],"!Lamer"))
   {
      printf("(%s)\n",betatester);
      exit(0);
   }
#endif

   commandline(argc,argv);
// loadconfig(configfilename);
   loadblacklist();
   if (smode_override) cfg.scan_type = smode_override-1;
   count_tried();

   speak_flag=speaker;
   vol_flag = -1;
   if (cfg.DialMethod == -1) CurrentNum=10000;
   if (cfg.DialMethod ==  1) CurrentNum=-1;

   if (cfg.DialMethod ==  2) {
      CurrentNum = -1;                // This will be irrelevant, right?
      numberfile = fopen(textfilename,"rt");
      if (numberfile == NULL)
      {
         printf("Error opening number file: %s\n",textfilename);
         exit(1);
      }
      else
         printf("Reading from number file: %s\n",textfilename);
    }


   if (cfg.Fossil)
      init_fossil();
   else
      init_serial();

   strupr(cfg.logname);
   if (!cfg.logging)
      strcpy(cfg.logname,"<None>");
   else
   {
      struct dfree d;
      long avail;

      getdfree(0,&d);
      avail = (long) d.df_avail * (long) d.df_bsec * (long) d.df_sclus;
      if (avail < 10240L) {
         printf("This drive has less than 10K free\n");
         printf("Press any key to continue without logging, or ESCAPE to exit...");
         if (getch()==27)
            exit(1);
         cfg.logging=0;
      } else {
         logfile=fopen(cfg.logname,"at");
         if (logfile == NULL)
         {
            cfg.logging = 0;
            printf("Error writing to logfile '%s' -- exiting\n",cfg.logname);
            exit(1);
         }
      }
   }

   strupr(cfg.logname);
   if (!cfg.logging)
      strcpy(cfg.logname,"<None>");

   drawscreen();
//   OpenCarrier();

   log("¯\n");
   log("ToneLoc V%s (%s)\n",VERSION,__DATE__);
   log("ToneLoc started on %s\n",DateString());

   /* this part lost in disk lame */
// if (cfg.Fossil)

   if (!cfg.irq)       // If IRQ is not set manually, we are using a standard com port
      log("Using COM%u (%s UART)\n",cfg.port,cfg.Fossil?"FOSSIL":uart_id());
   else                // Otherwise, it is custom com port:
      {
      log("Custom port: Base %03Xh, IRQ %d\n",cfg.portaddx,cfg.irq);
      log("             (%s UART)\n",uart_id());
      }
// else
//    log("Using%s UART detected\n",uart_id());
   if (DESQview)
      log("DESQview v%d.%02d detected\n",(dv_version>>8),(dv_version & 0x00FF));

   log("Data file:      %s\n",datfilename);
   log("Config file:    %s\n",configfilename);
   log("Log file:       %s\n",cfg.logname);
   if (cfg.DialMethod==2)
      log("NumberFile:  %s\n",textfilename);
   log("Mask used:      %s\n",maskstring);
   if (rangestring[0])
      log("Range used:     %s\n",rangestring);

   for (cnt=0; cnt < expoint; cnt++)
      log("Ex Range %d:     %s\n",cnt+1,exrangestring[cnt]);

   for (cnt=0; cnt < negpoint; cnt++)
      log("Exclude Mask %d: %s\n",cnt+1,neg_mask[cnt]);

   log("Scanning for:   %s\n",scanstr(cfg.scan_type));

   waitstart();
   /* end of disk lame part */

   while ((loop++ < 3) && (initmodem(cfg.initstring) == -1))
      ;
   if (loop==4)
   {
      log("Initialization Failure, exiting\n");
      quit(1);
   }

   if (m_hours.ti_hour <= 24) // was != 99
   {
      gettime(&right_now);
      m_end.ti_hour = m_hours.ti_hour + right_now.ti_hour;
      m_end.ti_min  = m_hours.ti_min  + right_now.ti_min;
      m_end.ti_sec  = 0;
      while (m_end.ti_min > 60) {
         m_end.ti_hour++;          // add an hour, but
         m_end.ti_min -= 60;       // subtract 60 minutes
      }
      while (m_end.ti_hour >= 24)
         m_end.ti_hour -= 24;      //  27:00 -> 3:00
   }
   if (m_end.ti_hour != 99)
      log("Dialing until %02d:%02d\n",m_end.ti_hour,m_end.ti_min);

   if (maxdials)
      log("Dialing %d numbers maximum\n",maxdials);

   starttime=mytime();
   NextAuto();

   if (quiet)
   {
      log("Quiet mode\n");
      cfg.sfx = 0;
      speak_flag = 0;
      toggle_speaker();
   }

   mainloop();

   quit(0);
}

void mainloop(void)
{
   char   ch=0, again=0, qflag=0;
   int    AbortLoop, RingCount;
   char   response[BIGSTRING];
   char   num[30];       // 30
   char   str[40];       // 40
   word   orig_wait_delay = cfg.wait_delay;

   if ((stats.tried-ex.tried) >= maxcodes)
      qflag=1;

   while (qflag==0)
   {
      long now;
      int integer;

      RingCount = 0;
      AbortLoop = 0;
      cfg.wait_delay = orig_wait_delay;
      checkdone();

      sprintf(str,"%2d/%2d",0,cfg.MaxRing);
      prints(_vinfo.numrows-15,73,cfg.stats_text,str);

      if (!again)
        strcpy(num,process(maskstring));
      again = 0;

      printn(_vinfo.numrows-4,48,cfg.meter_back,cfg.meterback,METERLENGTH);

      updatestats();

      while (Comhit())
         mgetsx(response);
      // clear out extra BS (like when BUSY...)

      if (!cfg.ignore_cd && Carrier())
         hang_up(1);

      log("%s - ",num);

      if ((integer=checkblack(num)) != 0) {   // blacklisted number
         AbortLoop = A_BLACKLISTED;
         log("* Blacklist #%u *\n",integer);
         oldones[old_point]=130;
      }
      else
         dial(num);

      startdial=mytime();

      while (!AbortLoop)
      {
         response[0]=0;
         now=mytime();
         updateclock();

         meter((word) (now-startdial),cfg.wait_delay,0,cfg.meter_fore);

	 if (Comhit())
	 {
        mgetsx(response);
	    switch (check_response(response))
	    {
	       case M_OK      : AbortLoop = A_TONE;
                                if (cfg.scan_type != ALLBUT_T)
                                {
                                   log("** TONE **\n");
                                   found(num,response,TONES);
                                }
                                else
                                   log("Tone\n");
                                stats.tones++;
                                oldones[old_point] = 80+chopten(RingCount);
                                break;
	       case M_VOICE   : AbortLoop = A_VOICE;
                                log("Voice   (%d)\n",RingCount);
                                if ((cfg.scan_type==ALLBUT_C) || (cfg.scan_type==ALLBUT_T))
                                    found(num,response,ALLBUT_T);
                                stats.voices++;
                                oldones[old_point]=20+chopten(RingCount);
                                break;
  	       case M_BUSY    : AbortLoop = A_BUSY;
                                log("Busy\n");
                                if ((cfg.scan_type==ALLBUT_C) || (cfg.scan_type==ALLBUT_T))
                                   found(num,response,ALLBUT_T);
                                stats.busys++;
                                oldones[old_point]=10+chopten(RingCount);
                                break;
	       case M_RINGING : sprintf(str,"%2d/%2d",++RingCount,cfg.MaxRing);
                                prints(_vinfo.numrows-15,73,cfg.stats_text,str);
                                if ((cfg.MaxRing) && (RingCount >= cfg.MaxRing))
                                {
                                   AbortLoop = A_RINGABORT;
                                   log("Ringout (%d)\n",RingCount);
                                   stats.rings++;
                                   oldones[old_point] = 60+chopten(RingCount);
                                   if ((cfg.scan_type == ALLBUT_C) || (cfg.scan_type == ALLBUT_T))
                                      found(num,response,ALLBUT_T);
                                   // make sure hang_up() isn't necessary
                                }
			        break;
	       case M_CONNECT : AbortLoop = A_CONNECT;
                                if (cfg.scan_type != ALLBUT_C)
                                {
                                   log("* CARRIER *\n");
                                   found(num,response,CARRIERS);
                                }
                                else
                                   log("Carrier\n");
                                stats.carriers++;
                                oldones[old_point]=90+chopten(RingCount);
                                break;
               case M_FAX     : AbortLoop = A_FAX; // can we test this?
                                log("Fax\n");
                                oldones[old_point]=41;
                                break;
	       case M_NO_TONE : AbortLoop = A_NO_TONE; again=1;
                                log("No Dialtone #%d\n",++NoTone);
                                oldones[old_point]=30+chopten(RingCount);
                                // put notone code below up here
                                break;
	       case M_NO_CARRIER : AbortLoop = A_NO_CARRIER;
                                   log("No Carrier\n");
                                   oldones[old_point]=70+chopten(RingCount);
                                   break;
	       case M_NOTHING : break;          // ignore blanks
               case M_UNKNOWN : // fallthrough
               default        : if (!cfg.ignore_unknown) {
                                   AbortLoop = A_UNKNOWN_RESP;
                                   log("Unknown: %s\n",response);
                                   again=1;
                                }
            } // switch
	 } // if comhit

	 if (kbhita())
	 {
            switch (ch=toupper(getch()))
            {
               case  0  : if (kbhit()) getch(); break;
                          // do f-keys here!
               case 'A' : blank(0); break;
               case 'B' : blank(1); break;
               case 'Q' : blip(1000); qflag=3;     // jazz this up a bit
                          prints(_vinfo.numrows-1,40,LRED|BLINK,"Quitting after this number ...");
                          break;
               case 'S' : toggle_spk_flag(); break;
               case 'X' : if (cfg.wait_delay > 60000U) {
                             blip(2600);
                             cfg.wait_delay=65000U;
                          } else cfg.wait_delay += 5000U;
                          break;
               case '!' : DatBack("SNAPSHOT.DAT"); break;
               default  : if (strchr("CFGIJKNPRTVYM \x1B",ch) && ch)
                             AbortLoop = A_KEYPRESSED;
                          if (strchr("0123456789",ch) && ch)  {
                             vol_flag = ch-48;
                             blip(1600+(vol_flag*100));
                             sprintf(str,"Volume%d: %-11s",vol_flag,cfg.vol[vol_flag]);
                             prints(_vinfo.numrows-1,0,LGREEN|BLINK,str);
                             // Volume1: ATL1|
                          }
            }
	 }
	 if ((now-startdial) >= cfg.wait_delay)  AbortLoop = A_TIMEOUT;
	 if (!AbortLoop && !cfg.ignore_cd && Carrier()) {
            AbortLoop = A_CARRIER;
            if (cfg.scan_type != ALLBUT_C)
            {
               log("* CARRIER *\n");
	       if (!(cfg.CarrierLogging))  // This might interfere w/ carrier logging
	       {
	       do
		{
	         trim(mgetsx(response));
	       	}
	        while (response[0]==0 && Comhit());
	       }
               found(num,response,CARRIERS);
            }
            else
               log("Carrier\n");
            stats.carriers++;
            oldones[old_point]=90+chopten(RingCount);
//          hang_up(1);
         }
      }

      //       " Please, lemme sleep! "    hahahahah

      switch (AbortLoop) {
         case A_FAX : // treat as carrier
         case A_CONNECT :
         case A_CARRIER : if (cfg.CarrierLogging)
                            LogSystem();
                          hang_up(1);
                          if (cfg.Fossil) {
                             FlushIn();
                          }
                          break;
         case A_TONE    : hang_up(2); break;
         default        : hang_up(0);
      }

      if (AbortLoop == A_KEYPRESSED)
      {
         switch (ch)
         {
            case  27 : log("Escaped\n");    oldones[old_point]=0;
                       quit(1);
                       break;
            case 'N' : log("* Noted *\n");  oldones[old_point]=40;
                       foundlog("%s - Noted",num);
                       break;
            case 'C' : log("Carrier\n");    oldones[old_point]=90+RingCount;
		       stats.carriers++;
                       foundlog("%s - Carrier (Noted)",num);
                       break;
            case 'F' : log("Fax\n");        oldones[old_point]=41;
                       foundlog("%s - Fax",num);
                       break;
            case 'G' : log("Girl\n");       oldones[old_point]=42;
                       foundlog("%s - Girl",num);
                       break;
            case 'I' : log("Ignored\n");    oldones[old_point] = 0;
                       stats.tried--;
                       current_tried--;
                       break;
//          case 'I' :  /* re-init modem.. */
            case 'K' : customnote(str);     oldones[old_point]=40;
                       foundlog("%s - %s",num,str);
                       break;
            case 'T' : log("Tone\n");      oldones[old_point]=80+RingCount;
                       foundlog("%s - Tone (Noted)",num);
                       break;
            case 'V' : log("VMB\n");       oldones[old_point]=43;
                       foundlog("%s - VMB",num);
                       break;
            case 'Y' : log("Yelling asshole\n"); oldones[old_point]=44;
                       foundlog("%s - Yelling Asshole",num);
                       break;
            case 'M' : log("Mucho sound-alike\n"); oldones[old_point]=49;
                       foundlog("%s - Mucho!",num);
                       break;
            case 'P' : log("Paused\n"); again=1;
                       prints(_vinfo.numrows-1,24,LRED|BLINK,"Paused - Press a key to continue");
                       while (!kbhita()); getch();
                       prints(_vinfo.numrows-1,24,LGREY,"                                ");
                       break;
            case 'R' : again = 1; log("Redialing\n");         break;
            case 'J' : again = 1; shell();                    break;
            case ' ' : log("Aborted (%d)\n",RingCount);
                       oldones[old_point]=50+chopten(RingCount);
                       break;
         }
      }

      if ((AbortLoop == A_NO_TONE) && (NoTone >= cfg.NoToneAbort))
      {
         log("Too many 'No Dialtones'\n");
         quit(1);
      }
      if (AbortLoop != A_NO_TONE)
         NoTone=0;

      if (AbortLoop == A_TIMEOUT)
      {
         log("Timeout (%d)\n",RingCount);
         if ((cfg.scan_type == ALLBUT_C) || (cfg.scan_type == ALLBUT_T))
            found(num,response,ALLBUT_T);
         oldones[old_point] = 70+chopten(RingCount);
      }

      while (Comhit()) Charin(0);         // test
      if (speak_flag != speaker) toggle_speaker();
      if (vol_flag != -1) initvol(vol_flag);
      CheckAuto();                  /* Do a .dat file backup if time */

      undometer(cfg.between_wipe,cfg.between_delay);

      if (kbhita())
         if ((ch=getch())==27) {
            log("Escape Pressed\n");
            qflag=1;
         }

      if ((stats.tried-ex.tried) >= maxcodes)
         qflag=1;

      if (maxdials && (current_tried >= maxdials))
         qflag=2;
   }

   switch (qflag) {       // reason for quitting
      case 1 : log("All %d numbers dialed\n",maxcodes); break;
      case 2 : log("%d numbers dialed\n",maxdials);     break;
      case 3 : log("Q pressed, quitting\n"); quit(1);
      default: log("Unknown exit reason\n"); quit(1);
   }
}

void init_serial(void)
{
   // Set function pointers to serial routines
   FlushIn = purge_in;
   SetDTR = set_dtr;
   GetCTS = serial_cts;
   charin = get_serial;
   Charout = put_serial;
   Comhit = in_ready;

   if (!cfg.irq)
     printf("COM%u: ",cfg.port);
   else
     printf("Custom port ");
   open_port(cfg.port,512);              /* Invisible Shit is being passed in cfg.*, etc! bad practice! */
   set_port(cfg.baudrate,8,NO_PARITY,1);
   set_rx_rts(1);    // receiver rts/cts flow control on
   set_tx_rts(1);    // xmitter  rts/cts flow control on

   if (!cfg.irq)   // Standard com port
      printf("Initialized: %ld baud, rx buffer = %d (%s UART)\n\n",
      get_baud(),ilen,uart_id());
   else            // Non-standard com port
      printf("Initialized: %ld baud, rx buffer = %d (%03Xh, IRQ %d) (%s UART)\n\n",
      get_baud(),ilen,cfg.portaddx,cfg.irq,uart_id());
}

void init_fossil(void)
{

   // Set function pointers to FOSSIL routines
   FlushIn = flushina;
   SetDTR = setdtr;
   GetCTS = fossil_cts;
   charin = charina;
   Charout = charouta;
   Comhit = comhita;

   printf("COM%u: ",cfg.port);
   if (!open_fossil(cfg.port)) {
      printf("\aError initializing!  Load a FOSSIL driver (and read the docs).\n");
      exit(1);
   }

   set_fossil_baud(cfg.baudrate);
   set_fossil_flow(0x02);       // bit 2 only = CTS/RTS flow control

   printf("Initialized at %ld baud\n\n",cfg.baudrate);
}

long mytime(void)      /* converts time into units of milliseconds */
{
   static struct time t;
   static struct date d;
   static long mama;

   getdate(&d);
   gettime(&t);

   mama = ((((dostounix(&d,&t)-unixstart)*100)+t.ti_hund)*10);

   return(mama);
}

char * mgetsx(char * s)
{
   char c;
   int count = 0;
   char *p = s;
   long start;

   start=mytime();

   // should we use GetDelay here (we don't use it anywhere else!)
   //                                         .. 250 ..

   while ((count < 50) && ((mytime()-start) < cfg.getdelay))
   {
      if (Comhit())
      {
         c = Charin();
         *p++ = c;
         count++;             // Always count characters, even CR/LF!

         if (c == '\r' || c == '\n')
         {
            if (count > 1)   // If it's more than just a CR/LF (i.e. non-blank)
               break;        // then
            else             // count == 1 and it's a CR/LF
               continue;     // next iteration of while() loop
         }
      }
   }
   *p = 0;                   // Null terminate the string
   return (s);               // and return it
}

void toggle_spk_flag(void)
{
   switch (speak_flag)
   {
      case 0 : speak_flag=1;
	       blip(2600);
	       break;
      default: speak_flag=0;
	       blip(440);
   }
   if (speak_flag != speaker)
      prints(_vinfo.numrows-1,73,LRED|BLINK,"Speaker");
   else
      prints(_vinfo.numrows-1,73,LGREY,"       ");
}

void toggle_speaker(void)
{
   char s[BIGSTRING];

   prints(_vinfo.numrows-1,73,LRED,"Speaker");

   switch (speaker) {
      case 0 : speaker = 1;
               log("Speaker ON\n");
	       ModemCommand(cfg.speakon);
	       break;
      case 1 : speaker = 0;
               log("Speaker OFF\n");
	       ModemCommand(cfg.speakoff);
	       break;
   }

   while (Comhit())
      mgetsx(s);

   prints(_vinfo.numrows-1,73,LGREY,"       ");
}

void initvol(int volnumb)
{
   char s[BIGSTRING];

   sprintf(s,"Volume%d: %s ",vol_flag,cfg.vol[vol_flag]);
   prints(_vinfo.numrows-1,0,LGREEN,s);

   hang_up(0);
   log("Volume set to %d\n",volnumb);
   ModemCommand(cfg.vol[volnumb]);

   while (Comhit())
      mgetsx(s);

   prints(_vinfo.numrows-1,0,LGREY,"                          ");

   vol_flag = -1;        /* clear the flag */
}

void dial(char * number)
{
   char s[HUGESTRING];

   switch (cfg.scan_type)
   {
      case CARRIERS :
      case ALLBUT_C : sprintf(s,"%s %s%s|",cfg.prefix,number,cfg.suffix);
                      break;
      case TONES    :
      case ALLBUT_T : sprintf(s,"%s %s%s W;|",cfg.prefix,number,cfg.suffix);
                      break;
   }
   ModemCommand(s);
   do
   {
     mgetsx(s);
   }
   while (Comhit());
}

char * scanstr(int scan_type)
{
/* rewrite this MOTHERFUCKER as:
   char * scanstr[] = { "Carriers","Tones"... ,NULL };
  */
    switch (scan_type) {
	case CARRIERS :  return("Carriers");
	case TONES    :  return("Tones");
	case ALLBUT_T :  return("All but tones");
	case ALLBUT_C :  return("All but carriers");
    }
    return(NULL);
}

void helpscreen(void)
{
   cclrscrn(7);
   wopen(0,0,24,79,5,3,3);

   wtextattr(14);
   wprintf("       ToneLoc %s by Minor Threat & Mucho Maas (%s)\n\n",VERSION,__DATE__);
//   wprintf("       ToneLoc %s by Minor Threat & Mucho Maas\n\n",VERSION);
   wtextattr(15);
   wprintf("ToneLoc is a dual purpose wardialer.  It dials phone numbers using a mask that\n");
   wprintf("you give it.  It can look for either dialtones or modem carriers.  It is useful\n");
   wprintf("for finding PBX's, Loops, LD carriers, and other modems.  It works well with\n");
   wprintf("the USRobotics series of modems, and most hayes-compatible modems.\n\n");

   wtextattr(13);
   wprintf("USAGE:\n");
   wtextattr(10);
   wprintf("ToneLoc  \033A%c[DataFile]\033A%c  \033I/M\033I:[Mask] \033I/R\033I:[Range] \033I/X\033I:[ExMask] \033I/D\033I:[ExRange] \033I/C\033I:[Config]\n"
       "                     \033I/#\033I:[Number] \033I/S\033I:[StartTime] \033I/E\033I:[EndTime] \033I/H\033I:[Hours] \033I/T /K\n\n\r",LRED,GREEN);
   wtextattr(11);
   wprintf("   [DataFile]  - File to store data in, may also be a mask        \033A%cRequired\n",YELLOW);
   wtextattr(3);
   wprintf("   [Mask]      - To use for phone numbers    Format: 555-XXXX     Optional\n");
   wprintf("   [Range]     - Range of numbers to dial    Format: 5000-6999    Optional\n");
   wprintf("   [ExMask]    - Mask to exclude from scan   Format: 1XXX         Optional\n");
   wprintf("   [ExRange]   - Range to exclude from scan  Format: 2500-2699    Optional\n");
   wprintf("   [Config]    - Configuration file to use                        Optional\n");
   wprintf("   [Number]    - Number of dials to make     Format: 250          Optional\n");
   wprintf("   [StartTime] - Time to begin scanning      Format: 9:30p        Optional\n");
   wprintf("   [EndTime]   - Time to end scanning        Format: 6:45a        Optional\n");
   wprintf("   [Hours]     - Max # of hours to scan      Format: 5:30         Optional\n");
   wprintf("                 Overrides [EndTime]\n");
   wprintf("   /T = Tones, /K = Carriers (Override config file, '-' inverts)  Optional\n");
   exit(0);
}

void deinit(void)
{
   if (cfg.Fossil)
      close_fossil();
   else
      close_port();
}

void quit(int errorlevel)
{
   char scrap[BIGSTRING];

   trim(cfg.exitstring);
   if (strcmp(cfg.exitstring,"") != 0)
    {
     log("Sending exit string ...");
     ModemCommand(cfg.exitstring);
     do
      {
       mgetsx(scrap);
      }
     while (Comhit());
     log(" Done\n");
    }
   deinit();

   include();
   DatBack(datfilename);

   log("Dials = %d, Dials/hour = %u\n",current_tried,dialhour());
   if (foundnum)
      log("%ss found: %d\n",switchverbose(cfg.scan_type),foundnum);
   log("%d:%02d spent current scan\n",scan.Minutes/60,scan.Minutes%60);
   log("Exit with errorlevel %d\n",errorlevel);

   fcloseall();

   gotoxy_(_vinfo.numrows-2,0);
   showcur();
   exit(errorlevel);
}

int initmodem(char * initstring)
{
   char string[51];
   register int x;

   if (!cfg.ignore_cd && Carrier())
      hang_up(1);     /* get rid of a possible connection */

   dvdelay(250);
   FlushIn();               /* flush any awaiting bullshit */

   log("Initializing Modem ... ");

   ModemCommand(initstring);

   x=0;

   while ((Comhit())  &&  (x < 51))
      string[x++]=Charin();
   string[x]=0;

   if (!strstr(string,cfg.initresp))
   {
      log("Failed!\n");
      return(-1);
   } else
      log("Done\n");

   mgetsx(string);

   if (!quiet)
      (speaker)?ModemCommand(cfg.speakon):ModemCommand(cfg.speakoff);

   while (Comhit())
      mgetsx(string);

   return(0);
}

char * process(char * mask) /* replaces X's with random #'s */
{
   byte pos, xpos;
   char aftermask[50] = "";
   char xstring[10];
   char xstr[10];
   char quickc[BIGSTRING]; //81
   char xcount=0;

   strupr(mask);        /* convert to uppercase */
   for (pos=0; pos<strlen(mask); pos++)
      if (mask[pos]=='X')
         xcount++;

   do {
      if (cfg.DialMethod != 2)
      {
         CurrentNum += (cfg.DialMethod);
	 if (cfg.DialMethod) switch (xcount)
         {
	    case 4 : sprintf(xstr,"%04d",CurrentNum); break;
	    case 3 : sprintf(xstr,"%03d",CurrentNum); break;
	    case 2 : sprintf(xstr,"%02d",CurrentNum); break;
	    case 1 : sprintf(xstr,"%01d",CurrentNum); break;
	    case 0 : sprintf(xstr,"%d",CurrentNum+(cfg.DialMethod)); break;
            // test all this shit
	    default    : cprintf("\aERROR!");
	 }
      }
      switch (cfg.DialMethod)
      {
	 case  0 : // generate random numbers
                   for (pos=0, xpos=0; pos<strlen(mask); pos++)
		   {
		      if (mask[pos]=='X')
		      {
			 aftermask[pos]=((char) random(10)+48);
			 xstring[xpos++]=aftermask[pos];
		      }
		      else
			 aftermask[pos]=mask[pos];
		   }
                   break;
	 case -1 : // backward sequential
                   for (pos=0, xpos=0; pos<strlen(mask); pos++)
		   {
		      if (mask[pos]=='X')
		      {
			 aftermask[pos]=xstr[xpos];
			 xstring[xpos++]=aftermask[pos];
		      }
		      else
			 aftermask[pos]=mask[pos];
		   }
		   break;

	 case  1 : // forward sequential
                   for (pos=0, xpos=0; pos<strlen(mask); pos++)
		   {
		      if (mask[pos]=='X')
		      {
			 aftermask[pos]=xstr[xpos];
			 xstring[xpos++]=aftermask[pos];
		      }
		      else
			 aftermask[pos]=mask[pos];
		   }
		   break;
         case 2 : // read numbers from text file
                  fgets(quickc, 80, numberfile);
		  if (feof(numberfile))
                  {
	  	     log("End of numberfile %s\n",textfilename);
		     fclose(numberfile);
		     quit(0);
		  }
		  for (pos=0, xpos=0; pos<strlen(mask); pos++)
		  {
		     if (mask[pos]=='X')
		     {
			aftermask[pos]=quickc[xpos];
			xstring[xpos++]=aftermask[pos];
		     }
		     else
		        aftermask[pos]=mask[pos];
		  }
		  break;
      }
      aftermask[pos]=0;
      xstring[xpos]=0;
   } while (checkdupe(xstring));

   return(aftermask);

}

void range(char * rangestr)
{
   register word x;
   int cnt, low, high, roundval;
   char s[25];

   strcpy(s,rangestr);

   high = -1;      /* Illegal values indicate bad input */
   low  = -1;

   cnt = (strchr(s,'-') - s);
   high = atoi(&s[cnt+1]);
   s[cnt] = 0;
   low = atoi(s);

   if (low > high)                               /* Swap if needed */
      SWAP (low,high);                       /* my macro comes in! */

   cnt = maxcodes;                  // save original maxcodes
   for (x=0; x < cnt; x++)          // maxcodes was 10000
   {
      if ((x > high) || (x < low))
      {
         maxcodes--;                        /* this is always down */
	 roundval = (oldones[x]/10) * 10;
         if (oldones[x] != 0)
            ex.tried++;
         switch (roundval)
	 {
            case  0 : oldones[x] = 100; break;
	    case 10 : ex.busys++;    break;
	    case 20 : ex.voices++;   break;
	    case 60 : ex.rings++;    break;
            case 80 : ex.tones++;    break;
	    case 90 : ex.carriers++; break;
         }
      }
   }
}

void exrange(char * exrangestr)
{
   register word x;
   int cnt, low, high, roundval;
   char s[25];

   strcpy(s,exrangestr);

   high = -1;      /* Illegal values indicate bad input */
   low  = -1;

   cnt = (strchr(s,'-') - s);
   high = atoi(&s[cnt+1]);
   s[cnt] = 0;
   low = atoi(s);

   if (low > high)                               /* Swap if needed */
      SWAP (low,high);                       /* my macro comes in! */

   for (x=0; x < 10000; x++)
   {
      if ((x >= low) && (x <= high))
      {
         maxcodes--;
	 roundval = (oldones[x]/10) * 10;
         if (oldones[x] != 0)
	    ex.tried++;
	 switch (roundval)
         {
            case  0 : oldones[x] = 100; break;
	    case 10 : ex.busys++;    break;
	    case 20 : ex.voices++;   break;
	    case 60 : ex.rings++;    break;
            case 80 : ex.tones++;    break;
	    case 90 : ex.carriers++; break;
         }
      }
   }
}

void exclude(char * mask)                /* excludes a mask */
{
   int len;
   int roundval;
   register int x, y;
   char s[25], fmt[10];

   len=strlen(mask);
   sprintf(fmt,"%%0%dd",len);     /* 4 = "%04d", etc */

   for (x=0; x<maxcodes; x++)
   {
      sprintf(s,fmt,x);
      for (y=0; y<len; y++)
	 if (mask[y]=='X')
            s[y]='X';

      if (!strcmp(mask,s))
      {                                   /* if # is within mask */
	 maxcodes--;
	 roundval = (oldones[x]/10) * 10;
	 if (oldones[x] != 0)
	    ex.tried++;
         switch (roundval)
	 {
            case  0 : oldones[x] = 100; break;
	    case 10 : ex.busys++;    break;
            case 20 : ex.voices++;   break;
            case 60 : ex.rings++;    break;
            case 80 : ex.tones++;    break;
	    case 90 : ex.carriers++; break;
	 }
      }
   }
}

void include(void)
{
   register int cnt;

   for (cnt=0; cnt < 10000; cnt++)
      if (oldones[cnt] == 100 || oldones[cnt] == 110)
         oldones[cnt] = 0;
}

int checkdupe(char * string)
{
    register word wordvar;

    wordvar = atow(string);
    if (oldones[wordvar])
       return(1);    /* number already dialed */

    oldones[wordvar]=70;
    old_point = wordvar;
    stats.tried++;
    current_tried++;

    return(0);
}

int checkblack(char * number)
{
   register int x;
   struct black_number * blacklist;

   blacklist = black_top;     // start at top of linked list
   x = black_count;

   while (blacklist != NULL)
   {
      if (strstr(number,blacklist->number) != NULL)
         return(x);         // found
      blacklist = blacklist->next;
      x--;
   }

   return(0);
}

void drawscreen(void)
{
   char copyright[79];
   cclrscrn(7);

   wopen(0,0,_vinfo.numrows-3,44,0,cfg.act_win,cfg.act_text);
   wtitle("´ Activity Log Ã",TCENTER,cfg.act_win);
   w_activity=wopen(1,2,_vinfo.numrows-4,43,5,cfg.act_win,cfg.act_text);

   w_found=wopen(_vinfo.numrows-10,63,_vinfo.numrows-6,78,5,cfg.stats_win,cfg.stats_win);

   wopen(0,46,_vinfo.numrows-17,79,0,cfg.mod_win,cfg.mod_text);
   wtitle("´ Modem Ã",TCENTER,cfg.mod_win);
   w_modem=wopen(1,48,_vinfo.numrows-18,78,5,cfg.mod_win,cfg.mod_text);

   wopen(_vinfo.numrows-16,46,_vinfo.numrows-3,79,0,cfg.stats_win,cfg.stats_win);
   wtitle("´ Statistics Ã",TCENTER,cfg.stats_win);
   whline( 4,0,33,0,cfg.stats_win);
   whline(10,0,33,0,cfg.stats_win);
   wvline(4,15,7,0,cfg.stats_win);
   wprints(4,19,cfg.stats_win,"´ Found Ã");

   gettime(&right_now);

   wtextattr(cfg.stats_items);
   wprintf("\033A%c Started:\033A%c %02d:%02d:%02d  \33A%cRing:\33A%c\n",
            cfg.stats_items,cfg.stats_text,right_now.ti_hour,right_now.ti_min,
        right_now.ti_sec,cfg.stats_items,cfg.stats_text);
   wprintf("\033A%c Current:\033A%c %02d:%02d:%02d  \33A%cSecs:\n",
        cfg.stats_items,cfg.stats_text,right_now.ti_hour, right_now.ti_min,
        right_now.ti_sec,cfg.stats_items);
   wprintf("\033A%c Max Dials: \033A%c %5d\n",cfg.stats_items,cfg.stats_text,maxcodes);
   wprintf("\033A%c Dials/Hour: \033A%c%5d  \033A%cETA:\n",
            cfg.stats_items,cfg.stats_text,0,cfg.stats_items);

   w_stats=wopen(_vinfo.numrows-10,48,_vinfo.numrows-6,61,5,cfg.stats_win,cfg.stats_items);

   cfg.scan_type==TONES?wprintf("Tones :      \n"):wprintf("CD's  :      \n");
   // haha, there's mud in yer eye!

   wprintf("Voice :      \n");
   wprintf("Busy  :      \n");
   wprintf("Rings :      \n");
   wprintf("Try # :      ");

   printn(_vinfo.numrows-4,48,cfg.meter_back,cfg.meterback,METERLENGTH);
//   sprintf(copyright,"ToneLoc %s (%s) by Minor Threat & Mucho Maas",VERSION,__DATE__);
   sprintf(copyright,"ToneLoc %s by Minor Threat & Mucho Maas",VERSION);
   prints(_vinfo.numrows-2,40-((strlen(copyright))/2),random(7)+9,copyright);
   hidecur();
}

void message(char * format, ...)     /* Status window message */
{
   char string[BIGSTRING];
   va_list argptr;

   wactiv(w_activity);

   va_start(argptr, format);
   vsprintf(string, format, argptr);
   va_end(argptr);

   wprintf("%s",string);
}

void modem(char * format, ...)
{
   char string[BIGSTRING];
   va_list argptr;

   wactiv(w_modem);

   va_start(argptr, format);
   vsprintf(string, format, argptr);
   va_end(argptr);

   if (string[0])
      wprintf("%s",string);
}

void found(char * number, char * response, int result)
{
    /*
      Key to FlogString:

      %t = Time
      %d = Date
      %n = Number
      %b = Brief result (T, C, NT, NC)
      %l = Long result (Tone, Carrier, No Tone, No Carrier)
      %r = Response from modem (CONNECT 9600/ARQ)
      %x = "Lamer!"
    */

   register int x;
   char tempstr[HUGESTRING];

   strcpy(tempstr,cfg.flogstring);

   replace(tempstr,"%d",DateString());
   replace(tempstr,"%t",TimeString());
   replace(tempstr,"%n",number);
   replace(tempstr,"%l",switchverbose(result));
   replace(tempstr,"%b",switchbrief(result));
   replace(tempstr,"%r",response);
   replace(tempstr,"%x","Lamer!");

   wactiv(w_found);                    // write to found window
   resultcolor(result);
   foundnum++==0?wprintf(" %s",number):wprintf("\n\r %s",number);

   foundlog("%s",tempstr);
   for (x=1000; x<2000; x += 50)       // make a noise
      blip(x);
}

void foundlog(char * format, ...)
{
   FILE *foundfile;
   char s[BIGSTRING];
   va_list argptr;

   va_start(argptr, format);
   vsprintf(s, format, argptr);
   va_end(argptr);

   foundfile=fopen(cfg.foundname,"at");
   if (!foundfile)
      log("Error writing to '%s'\n",cfg.foundname);
   else {
      fprintf(foundfile,"%s\n",s);
      fclose(foundfile);
   }
}

void replace(char * s, char * s1, char * s2)
{
   register int x,y;
   int l, i;
   char * p;
   char t[BIGSTRING];

   p = strstr(s,s1);    // p = pointer to s1 in s
   if (!p) return;

   i = (p - s);         // i = length of s before s1
   l = strlen(s2);      // l = length of s2
   strncpy(t,s,i);      // t = part of s before s1
   t[i] = 0;

   for (x=i, y=0; y<l; y++, x++)
      t[x] = s2[y];     // append s2 to t

   y = i + strlen(s1);  // y = new length of t

   while (s[y])
      t[x++] = s[y++];  // copy rest of s to t

   t[x] = 0;            // terminate t
   strcpy(s,t);         // s = t
}

void resultcolor(int result)
{
    switch (result) // scan_type
    {
       case CARRIERS :
       case ALLBUT_C : wtextattr(cfg.carrier_text);  break;
       case TONES    :
       case ALLBUT_T : wtextattr(cfg.tone_text);  break;
       default       : wtextattr(cfg.tone_text);
   }
}

char * switchbrief(int result)    /* Brief result switch */
{
   switch (result)   // scan_type
   {
      case CARRIERS : return("C");
      case TONES    : return("T");
      case ALLBUT_T : return("NT");
      case ALLBUT_C : return("NC");
      default       : break;
   }
   return("?");
}

char * switchverbose(int result)   /* Verbose result switch */
{
   switch (result)  // scan_type
   {
      case CARRIERS : return("Carrier");
      case TONES    : return("Tone");
      case ALLBUT_T : return("No Tone");
      case ALLBUT_C : return("No Carrier");
      default       : break;
   }
   return("?");
}

void log(char * format, ...)
{
   struct time now;
   va_list argptr;
   static int showdate=1;
   char string[BIGSTRING];
   char s[BIGSTRING];

   va_start(argptr, format);
   vsprintf(string, format, argptr);
   va_end(argptr);

   gettime(&now);

// if (string[0] != '!')
//    sprintf(s,"%s %s",TimeString(),string);
// else
//    strcpy(&s[0],&string[1]);
   if (showdate)
      sprintf(s,"%s %s",TimeString(),string);
   else
      strcpy(s,string);

   if (s[strlen(s)-1]=='\n')
      showdate=1;
   else
      showdate=0;

   message("%s",s);

   if (cfg.logging)
   {
      fprintf(logfile,"%s",s);
      if ( ferror(logfile))
      {
	  message("Log file error, aborting!");
	  cfg.logging = 0;
	  quit(1);
      }
   }
}

void meter(word part, word total, int type, int frontcolor)
{
   register word x, percent;
   long ppart;
   int bottom = _vinfo.numrows - 4;

   ppart = (long) part * 1000;

   percent = (word) (((ppart / total) * METERLENGTH) / 1000);

   if (percent > METERLENGTH)
      percent = METERLENGTH;

   for (x=0; x < METERLENGTH; x++)
   {
      switch (type)
      {
         case 0 : if (percent >= x)                 // normal
                     printc(bottom,x+48,frontcolor,cfg.meterfront);
                  else
                     printc(bottom,x+48,cfg.meter_back,cfg.meterback);
                  break;
         case 1 : if ((percent / 2) > x) {          // edges -> middle
                     printc(bottom,48+x,cfg.stats_items,' ');
                     printc(bottom,47+METERLENGTH-x,cfg.stats_items,' ');
                  } break;
         case 2 : if ((percent / 2) > x) {          // middle -> edges
                     printc(bottom,63+x,cfg.stats_items,' ');
                     printc(bottom,62-x,cfg.stats_items,' ');
                  } break;
         case 3 : if (percent >= x)                 // right -> left
                     printc(bottom,47+METERLENGTH-x,cfg.stats_items,' ');
                  break;
         case 4 : if ((percent / 2) > x) {          // double -> right
                     printc(bottom,48+x,cfg.stats_items,' ');
                     printc(bottom,48+x+(METERLENGTH/2),cfg.stats_items,' ');
                  } break;
         case 5 : if ((percent / 3) > x) {          // triple -> right
                     printc(bottom,48+x,cfg.stats_items,' ');
                     printc(bottom,48+x+(METERLENGTH/3),cfg.stats_items,' ');
                     printc(bottom,48+x+(2*(METERLENGTH/3)),cfg.stats_items,' ');
                  } break;
         default : printn(bottom,48,cfg.stats_items,' ',METERLENGTH);
      } // switch
   } // for
}

void undometer(int type, word milliseconds)
{
   long now, start;

   start = mytime();

   do {
      updateclock();
      now=mytime();
      meter((word) (now - start),milliseconds,type,cfg.meter_fore);
   }
   while ((word) (now-start) < milliseconds);
}

void updatestats(void)
{
// updateclock();        // why?

   wactiv(w_stats);
   wgotoxy(0,0);

   if (cfg.scan_type==TONES)
      wprintf("\033C\x7\033A%c %5d\n",cfg.stats_text,(stats.tones-ex.tones));
   else
      wprintf("\033C\x7\033A%c %5d\n",cfg.stats_text,(stats.carriers-ex.carriers));
   wprintf("\033C\x7\033A%c %5d\n",cfg.stats_text,(stats.voices-ex.voices));
   wprintf("\033C\x7\033A%c %5d\n",cfg.stats_text,(stats.busys-ex.busys));
   wprintf("\033C\x7\033A%c %5d\n",cfg.stats_text,(stats.rings-ex.rings));
   wprintf("\033C\x7\033A%c %5d",  cfg.stats_text,(stats.tried-ex.tried));
}

void shell(void)
{
   int *screen_buffer;
   char drive, cwd[BIGSTRING], scrap[BIGSTRING];

   hang_up(0);
   log("OS Shell\n");

   trim(cfg.toshell);
   if (strcmp(cfg.toshell,"") != 0)
    {
     log("Sending shell string ...");
     ModemCommand(cfg.toshell);
     do
     {
      mgetsx(scrap);
     }
     while (Comhit());
     log(" Done\n");
    }

   DatBack(datfilename);             // save logfile & .datfile

   if ((screen_buffer=ssave())==NULL)
      printf("\a");

   gotoxy_(_vinfo.numrows-2,0);
   clreol_();
   printf("%uK available to OS ...",memavail());
   gotoxy_(_vinfo.numrows-3,0);
   showcur();

   drive = getdisk();
   getcwd(cwd,64);
   system("");
   setdisk(drive);
   chdir(cwd);

   hidecur();
   srestore(screen_buffer);
   log("Back from OS\n");

   trim(cfg.fromshell);
   if (strcmp(cfg.fromshell,"") != 0)
    {
     log("Sending shell return ...");
     ModemCommand(cfg.fromshell);
     do
     {
      mgetsx(scrap);
     }
     while (Comhit());
     log(" Done\n");
    }
}

int memavail(void)
{
   word segptr, stat;

   stat = (word) allocmem(0xFFFF, &segptr);
   return(stat/64);
}

void updateclock(void)
{
   static byte last_sec=255;
   char s[15];
   long elapsed;

   elapsed = ((mytime()-startdial) / (long) 1000);
// if ((elapsed < 100) && (elapsed > 0))
   {
      sprintf(s,"%2ld/%2u ",elapsed,cfg.wait_delay/1000);
      prints(_vinfo.numrows-14,73,cfg.stats_text,s);
   }

   gettime(&right_now);

   if (right_now.ti_sec != last_sec)
   {
      register word x;

      /* update current time */
      prints(_vinfo.numrows-14,57,cfg.stats_text,TimeString());

      /* update dials/hour */
      sprintf(s,"%5u",dialhour());
      prints(_vinfo.numrows-12,60,cfg.stats_text,s);

      /* update eta */
      x = eta((word) maxcodes-(stats.tried-ex.tried),dialhour());
      sprintf(s,"%3d:%02d",x/60,x%60);
      prints(_vinfo.numrows-12,72,cfg.stats_text,s);

      last_sec = right_now.ti_sec;
   }

   if (DESQview)
      dv_release();
}

void parsetime(char * instring, struct time * ti)
{
   char tstr[30];
   int cnt, temp;

   tstr[0]=0;
   for (cnt=0; cnt < strlen(instring); cnt++)
   {
      if (instring[cnt] != ':' && instring[cnt] != ' ')
      {
         temp = strlen(tstr);
         tstr[temp] = instring[cnt];
         tstr[temp+1] = 0;
      }
      else
         break;
   }
   ti->ti_hour = atoi(tstr);
   tstr[0] = 0;
   cnt++;
   for (cnt = cnt; cnt < strlen(instring); cnt++)
   {
      if (isdigit(instring[cnt]))
      {
         temp = strlen(tstr);
         tstr[temp] = instring[cnt];
         tstr[temp+1] = 0;
      }
      else
         break;
   }
   ti->ti_min = atoi(tstr);
   tstr[0] = 0;
   if ((instring[cnt] == 'P') && (ti->ti_hour != 12))
      ti->ti_hour += 12;
   ti->ti_sec  = 0;
   ti->ti_hund = 0;
}

void checkdone()
{
   gettime(&right_now);
   if (m_end.ti_hour <= 24)      // hour != 99
      if ((m_end.ti_hour == right_now.ti_hour) && (m_end.ti_min <= right_now.ti_min))
      {
         log("Stopping at %s\n",TimeString());
         quit(0);
      }
}

void waitstart(void)
{
   if (m_start.ti_hour <= 24)     // hour != 99
   {
      log("Waiting until %02d:%02d\n",m_start.ti_hour,m_start.ti_min);
      while ((m_start.ti_hour != right_now.ti_hour) || (m_start.ti_min != right_now.ti_min))
      {
         gettime(&right_now);
         if (kbhita())
         {
            getch();
            break;     // if key hit, start now
         }
         startdial = mytime();
         updateclock();
      }
   }
}

void loadconfig(char * configfilename)
{
   FILE *f;

   f=fopen(configfilename,"rt");
   if (!f)
   {
      printf("\aConfiguration file '%s' not found!\n",configfilename);
      printf("\Please run TLCFG.\n");
      exit(1);
   }
   else
   {
      printf("Loading config file %s ... ",configfilename);
      fread(&cfg,sizeof(cfg),1,f);
      fclose(f);
      printf("Done\n");
   }

   if (cfg.VersionID != VERSIONID)
   {
      printf("\n\rConfig file %s is from an old version of ToneLoc!\n",configfilename);
      printf("You'll need to delete %s and run TLCFG.\n",configfilename);
      exit(1);
   }

}

void loaddata(char * filename)
{
   FILE *datafile;
   register int x;
   register int round;

   if (strchr(filename,'.') == 0)
   {
      datfilename[8] = 0;
      strcat(datfilename,".DAT");
   }

   datafile=fopen(datfilename,"rb");
   if (!datafile)           /* Initialize new data file */
   {
      scan.ProductCode[0]='T';
      scan.ProductCode[1]='L';
      scan.VersionID = VERSIONID;
      scan.Minutes = 0;
      memset(&scan.Extra,0,sizeof(scan.Extra));
      memset(&oldones,0,sizeof(oldones));
      DatBack(datfilename);
   }
   else               /* Read existing data file */
   {
      fread(&scan,sizeof(scan),1,datafile);
      fread(&oldones,sizeof(oldones),1,datafile);
      if ((scan.ProductCode[0] != 'T') || (scan.ProductCode[1] != 'L') ||
         (scan.VersionID != VERSIONID))
      {
         printf("%s is not a current ToneLoc datafile!  If it is from a\n"
         "previous version of ToneLoc, you must run TCONVERT on it first.\n"
         "And you should probably TCONVERT all of your datafiles.\n",filename);
         exit(1);
      }

      for (x=0; x < 10000; x++)
      {
         round = (oldones[x]/10) * 10;
         if ((round) && (round != 100))
            stats.tried++;
         switch (round)
         {
            case 10 : stats.busys++;    break;
            case 20 : stats.voices++;   break;
            case 60 : stats.rings++;    break;
            case 80 : stats.tones++;    break;
            case 90 : stats.carriers++; break;
         }
      }
      fclose(datafile);
   }
}

void hang_up(int c)
{
   char *p;
   char s[BIGSTRING];

   switch (c) {
      case 0  : p=cfg.hangup; break;
      case 1  : p=cfg.c_hangup; break;
      case 2  : p=cfg.t_hangup; break;
      default : p=cfg.hangup;
   }

   if (Comhit())      // possible infinite loop here
      mgetsx(s);

   ModemCommand(p);

   if (!cfg.ignore_cd && Carrier())
   {
      log("Trying long DTR carrier drop\n");
      ModemCommand("<~~~>");        // 1.5 second dtr drop
      while (Comhit()) mgetsx(s);
      if (Carrier())
      {
         log("Trying slow hangup command\n");
         ModemCommand("~~~+++~~~");  // Modem escape sequence
         while (Comhit()) mgetsx(s);
         ModemCommand("ATH0|~");     // Hang up command
         while (Comhit()) mgetsx(s);
         if (Carrier()) {
            log("Couldn't drop carrier, Exiting\n");
            quit(1);
         }
      }
      log("Carrier drop successful\n");
   }

   SetDTR(1);
   while (Comhit())
      mgetsx(s);
}

void count_tried(void)
{
   register int x;

   stats.tried = 0;

   for (x=0; x < 10000; x++)
      if ((oldones[x] != 0) && (oldones[x] != 100))
         stats.tried++;
}

void commandline(int argc, char *argv[])
{
   int x;
   char ch;
   char altlog[13] = "";
   extern  char  *optarg;
   extern   int   optind;

   for (x=1; x < argc; x++) {   /* Convert to upper case */
      strrep(argv[x],':',' ');
      strupr(argv[x]);
   }

   if (argv[1][0]=='/' || argv[1][0]=='-')
   {
      printf("You need to specify a dial mask or filename first!\n");
      exit(1);
   }

   strcpy(maskstring,argv[1]);
   maxcodes = 1;
   for (x=0; x < strlen(maskstring); x++)
      if (maskstring[x]=='X') maxcodes *= 10;

   if (argc == 2)
   {
      strcpy(datfilename,argv[1]);
      loaddata(datfilename);
   }

   if (argc > 2)
   {
      strcpy(datfilename,argv[1]);
      loaddata(datfilename);
      optind=2;

      while ((ch = getopt(argc,argv,"M:R:X:D:C:S:E:H:L:#:TKQ")) != EOF)
      {
         trim(optarg);
         switch (ch)
         {
            case 'M' : maxcodes = 1;                  /* Mask */
                       for (x=0; x < strlen(optarg); x++)
                          if (optarg[x]=='X')
                             maxcodes *= 10;
                       strcpy(maskstring,optarg);
                       break;
            case 'C' : strcpy(configfilename,optarg);  /* Cfg file */
                       if (strchr(configfilename,'.') == 0)
                       {
                          configfilename[8]=0;
                          strcat(configfilename,".CFG");
                       }
                       strupr(configfilename);
                       break;
            case 'X' : if (strlen(optarg) <= 4) {  /* Exclusion mask */
                          exclude(optarg);
                          strcpy(neg_mask[negpoint],strupr(optarg));
                          negpoint++;
                       }
                       break;
            case 'S' : parsetime(optarg,&m_start);  /* Start Time */
                       break;
            case 'E' : parsetime(optarg,&m_end);      /* End Time */
                       break;
            case 'R' : strcpy(rangestring,optarg);
                       range(rangestring);      /* Range */
                       break;
            case 'D' : exrange(optarg);
                       strcpy(exrangestring[expoint],optarg);
                       expoint++;     /* Exclude Range */
                       break;
            case 'F' : //strcpy(altfound,optarg);       // alternate found
                       break;
            case 'H' : parsetime(optarg,&m_hours); /* Hours to Scan */
                       break;
            case 'L' : strcpy(altlog,optarg);       // alternate log
                       break;
            case 'T' : smode_override = 1;    /* look for tones */
                       if ((optarg[1] == '-') || (optarg[2] == '-'))
                          smode_override = 3;
                       break;
            case 'K' : smode_override = 2;    /* look for carriers */
                       if ((optarg[1] == '-') || (optarg[2] == '-'))
                          smode_override = 4;
                       break;
            case 'Q' : quiet = 1;            /* total silence */
                       break;
            case '#' : maxdials = atoi(optarg);
                       break;
            case '?' : printf("Invalid command line argument: %s\n\n",argv[optind]);
                       exit(1);
                       break;
         }  // switch
      }  // while
   }  // if (argc > 2)

   // now load config file
   loadconfig(configfilename);

   // now override config file options with command line options
   if (altlog[0])
      strcpy(cfg.logname,altlog);
   // if (altfound[0])
      // strcpy(cfg.foundname,altfound);
}

void setdefaults(void)
{
   speaker = 1;                                       /* 0=off, 1=on */
   save = 0;                       /* save rings, not seconds in dat */
   strcpy(configfilename, "TL.CFG");
   strcpy(textfilename,"NUMBERS.TXT");
   negpoint = 0;
   expoint = 0;
   m_start.ti_hour = 99;      /* Illegal values indicate immediate start */
   m_start.ti_min = 99;
   m_start.ti_sec = 99;
   m_end = m_hours = m_start;
   speak_flag = 0;
   smode_override = 0;
   quiet = 0;
   foundnum = 0;

   ex.tried = 0;
   ex.voices = 0;
   ex.busys = 0;
   ex.rings = 0;
   ex.tones = 0;
   ex.carriers = 0;
}


char * DateString(void)
{
   static char s[12];
   static char * months[] = { "Jan", "Feb", "Mar", "Apr", "May", "Jun",
                              "Jul", "Aug", "Sep", "Oct", "Nov", "Dec" };
   struct date d;

   getdate(&d);
   sprintf(s,"%02d-%3s-%02d",d.da_day,months[d.da_mon-1],d.da_year-1900);
   return(s);
}

char * TimeString(void)
{
   static char s[10];

   gettime(&right_now);
   sprintf(s,"%02d:%02d:%02d",right_now.ti_hour,right_now.ti_min,right_now.ti_sec);
   return(s);
}

int check_response(char * s)
{
   trim(s);
   if (s[0]==0)                    return(M_NOTHING);
   if (strstr(s,cfg.tone_string))      return(M_OK);
   if (strstr(s,cfg.connect_string))   return(M_CONNECT);
   if (strstr(s,cfg.ringing_string))   return(M_RINGING);
   if (strstr(s,cfg.busy_string))      return(M_BUSY);
   if (strstr(s,cfg.voice_string))     return(M_VOICE);
   if (strstr(s,cfg.notone_string))    return(M_NO_TONE);
   if (strstr(s,cfg.nocarrier_string)) return(M_NO_CARRIER);
   if (strstr(s,cfg.fax_string))       return(M_FAX);

   return(M_UNKNOWN);
}

void dvdelay(word ms)
{
   long start;

   start=mytime();
   do
   {
      // updateclock();      // error before drawscreen() is called
      if (DESQview) dv_release();
   } while ((mytime()-start) < ms);
}

word dialhour(void)
{
   long differ;
   long current;
   float result;

   current = mytime();

   differ = (current - starttime);

   result = (float) (((differ / (float) 1000) / (float) 60) / (float) 60);

   if (result)
      return((word) (current_tried / result));
   else
      return(0);
}

int chopten(register int x)
{
   if (x > 9)
      x = 9;
   return(x);
}

void NextAuto(void)
{
   if (cfg.AutoInt)
      AutoTime = (mytime() + (cfg.AutoInt * (long) 60000L));
   else
      AutoTime = 0;
}

void CheckAuto(void)
{
   if (cfg.AutoInt)
      if (mytime() > AutoTime)
      {
         log("Autosaving\n");
         DatBack(datfilename);

         fclose(logfile);
         logfile=fopen(cfg.logname,"at");
         NextAuto();
      }
}

void DatBack(char * datfiln)
{
   FILE *datafile;
   static word lastspent=0;
   word spent;

   spent = (word) ((mytime()+30000U) / 60000U);
   scan.Minutes += (spent-lastspent);
   lastspent = spent;

   if (cfg.NoDupes)
   {
      datafile=fopen(datfiln,"wb");
      fwrite(&scan,sizeof(scan),1,datafile);
      fwrite(&oldones,sizeof(oldones),1,datafile);
      fclose(datafile);
   }
}

void blip(int frequency)
{
   if (cfg.sfx)
   {
      sound(frequency);
      delay(50);      /* dvdelay isn't accurate enough */
      nosound();
   }
}

void loadblacklist(void)
{
   char *p;
   char s[100];
   FILE *blackfile;

   black_top = NULL;
   blackfile = fopen(cfg.blackfilename,"rt");

   if (blackfile)
   {
      black_count=0;

      printf("Loading blacklist file %s ...",cfg.blackfilename);
      while (fgets(s,99,blackfile))
      {
         p = (char *) strchr(s,';');   // locate the semicolon, if any
         if (p) *p=0;                  // truncate at semicolon
         if (strlen(s) > 1)            // Make sure non-blank [MM]
         {
            struct black_number * blacklist;

            if (black_count++ > 1000) {
               printf("\a too big!, please limit blacklist to 1000 entries!\n");
               printf("Press a key to continue ...");
               getch();
               printf("\n\n");
               break; // exit while loop
            }
            blacklist=malloc(sizeof(struct black_number));
            if (blacklist==NULL)          // out of memory
            {
               printf("\aOut of memory loading blacklist number #%u (%s)\n",black_count,trim(s));
               printf("(Shorten your blacklist!)\n");
               exit(1);
            }
            s[13]=0;         // make sure it isn't too long
            sscanf(s,"%s",blacklist->number);
            blacklist->next = black_top;
            black_top = blacklist;
         }
      }
      fclose(blackfile);
      printf(" Done\n\n");
   }
}

word eta(word left, word dph)
{
   float dphf;
   word timeleft=0;

   dphf = (float) ((float) dph / (float) 60);

   if (dphf)
      timeleft = (word) ((float) left / (float) dphf);

   return(timeleft);
}

void customnote(char * s)
{
// char note[25];

   wopen(6,20,10,60,0,14,15);
   wshadow(8);
   wtitle("´ Type a note for this number Ã",TCENTER,14);
   wputs("\n         ");

   showcur();
   s[0]=0;
   if (wgetns(s,17)==W_ESCPRESS)
      strcpy(s,"Noted");

   hidecur();
   wclose();
   log("** %s\n",s);
}

void blank(int showalt)
{
   static word  vidseg,    segp;
   static byte  blanked=0, loaded=0;
   FILE *alt;
   static int * screen;

   if (blanked==0)      /* blank screen */
   {
      int  stat;
      word size;

      size = (word) ((_vinfo.numrows*_vinfo.numcols*2) / 16);  /* = 256 */
      stat = allocmem(size,&segp);
      if (stat != -1)
         printf("\a");
      movedata(_vinfo.videoseg,0,segp,0, (size_t) (_vinfo.numrows*_vinfo.numcols*2));
      vidseg = _vinfo.videoseg;
      _vinfo.videoseg = segp;
      clrscrn();

      if (showalt)
      {
         if (!loaded)
         {
            alt=fopen(cfg.AltScreen,"rb");
            if (alt)
            {
               screen = (int *) malloc((size_t) (_vinfo.numrows*_vinfo.numcols*2));
               memset(screen,0,(size_t) (_vinfo.numrows*_vinfo.numcols*2));
               fread(screen,(size_t) (_vinfo.numrows*_vinfo.numcols*2),1,alt);
               fclose(alt);
               loaded = 1;
            }
            else
               loaded = 2;
         }

         if (loaded == 1)
         {
            int far * p;
            p = (int far *) screen;
            movedata(FP_SEG(p),FP_OFF(p),vidseg,0,(size_t) (_vinfo.numrows*_vinfo.numcols*2));
         }
      }
      blanked = 1;
   }
   else      // restore screen
   {
      _vinfo.videoseg = vidseg;
      movedata(segp,0,_vinfo.videoseg,0,(size_t) (_vinfo.numrows*_vinfo.numcols*2));
      freemem(segp);
      blanked = 0;
   }
}

char * trim(char * s)
{
   /* trims leading/trailing spaces off of a string */
   register int x=0;

   if (s==NULL)
      return(NULL);

   while (_isspace(s[x]))
      x++;
   if (x)
      strcpy(&s[0],&s[x]);

   x=strlen(s)-1;
   while (_isspace(s[x]))
      x--;
   s[x+1]=0;

   return(s);
}

void printn(int x, int y, int attr, char c, int n)
{
   /* prints 'c' n times at x,y with attr */
   register int i;

   for (i = 0; i < n; i++)
      printc(x,y+i,attr,c);
}

void modemch(char ch)
{
   wactiv(w_modem);                    // display in modem window

   if (cfg.AutoE71)
     ch &= 0x7F;

   if (ch == '\a')                      // if it's a BEEP
   {
      wtextattr(cfg.mod_text^0x08);     // reverse intensity bit (xor)
      wputc('^');                       // substitute it
      wputc('G');                       // with '^G'
      wtextattr(cfg.mod_text);          // restore attribute
   }
   else
      wputc(ch);
}

void carrierch(char ch)
{
   wactiv(w_carrier);                    // display in carrier window

//   if (cfg.AutoE71)
//     ch &= 0x7F;

   if (ch == '\a')                      // if it's a BEEP
   {
//      wtextattr(cfg.carrier_text^0x08);     // reverse intensity bit (xor)
      wputc('^');                       // substitute it
      wputc('G');                       // with '^G'
//     wtextattr(cfg.carrier_text);          // restore attribute
   }
   else
      wputc(ch);
}

void carrier_string(char * s)
{
  char * p = s;

  while (*p)
  {
   carrierch(*p);
   p++;
  }
}


char Charin(void)                           // Inputs character & displays in window
{                                           // This is the real function; ignore others.
   char ch;
   ch = (char) charin();
   modemch(ch);
   return(ch);
}

void ModemCommand(char * s)
{
   // Make sure the modem is still alive
   if (!cfg.ignore_cts && !GetCTS()) {    // CTS has disappeared!
      wopen(3,16,7,64,0,LRED,WHITE);
      wshadow(8);
      wtitle(" Modem Error ",TCENTER,LRED|BLINK);

      wprintf("  The CTS signal has disappeared.  Fix the\n"
              "  problem, press Escape to exit or any other\n"
              "  key to continue without CTS signal support");
      putchar(7); // beep
      while (!kbhit() && !GetCTS())
         ;
      if (!GetCTS()) {
         cfg.ignore_cts=1;
         set_rx_rts(0);
         set_tx_rts(0);
         if (getch()==27) {
            wclose();
            log("Exiting\n");
            quit(1);
         }
      } else dvdelay(500);
      wclose();
   }

   while (*s)
   {
      switch (*s)
      {
         case '!' : SetDTR(0);           // stop,
                    delay(250);          // drop,
                    SetDTR(1);           // and roll
                    break;
         case '<' : SetDTR(0);             break;
         case '>' : SetDTR(1);             break;
         case '|' : Charout('\r');         break;
         case '~' : delay(500);            break;
         case '^' : s++; Charout(*s-0x40); break;    // Control Character
         default  : Charout(*s);
                    if (cfg.pacing) delay(cfg.pacing);
      }
      s++;
   }
   delay(cfg.command_delay);
}

void NudgeChar(char ch)
{
 wactiv(w_nudge);
 wputc(ch);
}

int ModemCommandChar(char ch)
{
   int DelayFactor = 0;
   static NextCharControl = FALSE;
   static NextDTR = FALSE;

   if (ch)
   {
      if (NextCharControl == TRUE)
      {
	Charout(ch-0x40);
	NextCharControl = FALSE;
      }
      else
       if (NextDTR == TRUE)
	{
	  SetDTR(1);
	  NextDTR = FALSE;
	}
       else
       switch (ch)
       {
         case '!' : SetDTR(0);
		    DelayFactor=250;
		    NextDTR = TRUE;
                    break;
         case '<' : SetDTR(0);               break;
         case '>' : SetDTR(1);               break;
         case '|' : Charout('\r');           break;
         case '~' : DelayFactor=500;         break;
         case '^' : NextCharControl = TRUE;  break;
         default  : Charout(ch);
       }
   }
 return(DelayFactor);         // Returns the amount of time to delay, if any.
}

int Carrier(void)
{
   if (cfg.Fossil)
      return(carriera());
   return(carrier());
}

void strrep(char * s, char c1, char c2)
{
   // replace c1 with c2 in s
   while (*s)
   {
      if (*s == c1)
         *s = c2;
      s++;
   }
}

void LogSystem(void)
{
    int DelayFactor = 0;
    int EscPressed = 0;
    long start;
    long DelayStart = 0L;
    FILE *foundfile;
    char tmpstring[BIGSTRING];
    char ch;
    char * p = cfg.NudgeString;

    OpenCarrier();                          // open the carrier window
    OpenNudge();                            // open the Nudgestring window
    wactiv(w_carrier);                      // Display in carrier window
    start=mytime();                         // Mark the current time
    printn(_vinfo.numrows-4,48,cfg.meter_back,cfg.meterback,METERLENGTH);  // Update the meter; we've just started
    foundfile=fopen(cfg.carrierlogname,"at");          // Open the carrier log
    if (!foundfile)
     log("Error writing to '%s'\n",cfg.carrierlogname);    // Error opening the carrier log
    else
     {
     while (((mytime()-start) < cfg.PostNudgeDelay) &&
           (!cfg.ignore_cd && Carrier()) && (!EscPressed))
      {

       if (kbhita())
	{
	 ch = getch();
	 if (ch == 27)
	  {
	   EscPressed = 1;

	   sprintf(tmpstring,"\n<*LOG ABORTED (%1u sec.)*>\n",((mytime()-start)/(long)1000));
           carrier_string(tmpstring);
           fprintf(foundfile,tmpstring);

	   while (Comhit())               /* flush the buffer (works poorly) */
	    {
	     dvdelay(150);
	     ch = (char) charin();
	    }

	   continue;
	  }
	}

       if (DelayFactor > 0)
	if ((mytime()-DelayStart) >= DelayFactor)
	 {
	   DelayFactor = 0;
	   DelayStart  = 0L;       // Waited long enough, clear delays.
	 }

       if ((*p) && (DelayFactor == 0))        // There's another character, and no delay happening
       {
	 DelayFactor = ModemCommandChar(*p);
	 NudgeChar(*p);
	 wactiv(w_carrier);

	 if (DelayFactor)
	 {
	   DelayStart = mytime();
	 }
	 p++;
       }

       meter((word) (mytime()-start),cfg.PostNudgeDelay,0,12);   // Update meter in red
       if (Comhit())                    // If a character is waiting from the modem
        {
	 ch = (char) charin();       // Get the character..

	 if (cfg.AutoE71)            // Strip it if need be
	   ch &= 0x7F;

	 if (cfg.LFstrip)
	  if (ch == LINEFEED)       // Skip it if need be
	   continue;

	 carrierch(ch);              //  and display it in the carrier window
	 fprintf(foundfile,"%c",ch);
        }
     }

     fprintf(foundfile,"\n");
     if (!cfg.ignore_cd && !Carrier())  // If we lost carrier prematurely
     {
	sprintf(tmpstring,"<*CARRIER LOST (%1u sec.)*>\n",((mytime()-start)/(long)1000));
        carrier_string(tmpstring);
	fprintf(foundfile,tmpstring);
     }
    }
    fclose(foundfile);
    wclose();
    wactiv(w_nudge);
    wclose();
}

char * StripParity(char * s)
{
  int x;

  if (s==NULL)
     return(NULL);

  for (x=0;x<strlen(s);x++)
    s[x] &= 0x7F;

 return(s);
}

char * StripChar(char * s, char char_to_strip)
{
   register int x=0;

   if (s==NULL)
      return(NULL);

   for (x=0;x<strlen(s);x++)
    if (s[x] == char_to_strip)
      strcpy(&s[x],&s[x+1]);

   return(s);
}

void OpenCarrier(void)
{
   w_carrier=wopen(0,0,15,79,3,14,15);
   wtitle("´ Carrier Ã",TCENTER,14);
}

void OpenNudge(void)
{
   w_nudge=wopen(16,0,19,79,3,13,15);
   wtitle("´ NudgeString Ã",TCENTER,13);
}

