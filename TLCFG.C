/*
 * TLCFG.C - ToneLoc Configuration Program
 *           by Minor Threat 1993
 *           (with some help from Mucho Maas)
 */

/* The help system numbering explained:
 *
 * Each of the top menu bar options has 100 sub-menu item
 * numbers set aside for it. (Except files, which has 99 because
 * 0 is reserved by CXL.) See:
 *
 * 1-99      = Files
 * 100-199   = ModemStrings
 * 200-299   = ModemOptions
 * 300-399   = ScanOptions
 * 400-499   = Colors
 * 500-599   = Quit
 *
 * This is way more than will ever be needed, but hey.
 *
 * The first number of each group (1, 100, 200, 300, 400, 500) is
 * the help screen for the option as a whole.
 *
 * When there is nesting of menu levels, the numbers are further
 * subdivided.
 *
 * 100-199   = ModemStrings
 * 101-125   =   Modem Commands
 * 126-150   =   Modem Responses
 * 151-175   =   Volume Commands
 *
 * Again, the first number in each group is for a description of the
 * option as a whole. Exception is Modem Commands here, which is 101
 * since 100 is already reserved for ModemStrings.
 *
 * Numbers above 1000 are ad-hoc help screens, for example Special
 * Characters.
 *
 */

#define __TURBOC__ 1

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <conio.h>
#include <dos.h>
#include <ctype.h>
#include "tlcfg.h"
#include "cxlvid.h"
#include "cxldef.h"
#include "cxlkey.h"
#include "cxlwin.h"
#include "cxlmou.h"

#define VERSION "1.10"
#define TLVERSION  0x0110
#define DATVERSION 0x0100
#define CFGVERSION 0x0110

byte     DESQview;
CONFIG   cfg;
WINDOW   tlwin=0;
int    * buffer;
int      xpos,ypos;
int      _CURSORBACK;
int      CURSORFRONT;
int      mono=0;
unsigned terminate_key;
char     configfile[13];

   void   mainloop(void);
   void   save_changes(void);
   void   shell(void);
   void   abandon(void);
   void   quit(void);
   void   files_menu(void);
   void   mcommands(void);
   void   mresponses(void);
   void   vcommands(void);
   void   moptions(void);
   void   soptions(void);
   void   colors(void);
   WINDOW sc_win(void);
   void   loadcfg(char *);
   void   writecfg(char *);
   void   shadow(void);
   void   setdefaults(void);
   void   do_nothing(void);
   void   mopts1(void);
   void   mopts2(void);
   void   mopts3(void);
   void   mopts4(void);
   void   mopts5(void);
   void   mopts6(void);
   void   mopts7(void);
   void   mopts8(void);
   void   mopts9(void);
   void   mopts10(void);
   void   mopts11(void);
   void   mopts12(void);
   void   mopts13(void);
   void   mopts14(void);
   void   mopts15(void);
   void   mopts16(void);

   void   scopts1(void);
   void   scopts2(void);
   void   scopts3(void);
   void   scopts4(void);
   void   scopts5(void);
   void   scopts6(void);
   void   scopts7(void);
   void   scopts8(void);
   void   scopts9(void);
   void   scopts10(void);
   void   scopts11(void);
   void   scopts12(void);
   void   scopts13(void);
   void   scopts14(void);
   void   scopts15(void);
   void   scopts16(void);
   void   scopts17(void);
   void   scopts18(void);

   void   mrsps1(void);
   void   mrsps2(void);
   void   mrsps3(void);
   void   mrsps4(void);
   void   mrsps5(void);
   void   mrsps6(void);
   void   mrsps7(void);
   void   mrsps8(void);
   void   mcmds1(void);
   void   mcmds2(void);
   void   mcmds3(void);
   void   mcmds4(void);
   void   mcmds5(void);
   void   mcmds6(void);
   void   mcmds7(void);
   void   mcmds8(void);
   void   mcmds9(void);
   void   mcmds10(void);
   void   mcmds11(void);
   void   mcmds12(void);

   void   color1(void);
   void   color2(void);
   void   color3(void);
   void   color4(void);
   void   color5(void);
   void   color6(void);
   void   color7(void);
   void   color8(void);
   void   color9(void);
   void   color10(void);
   void   color11(void);
   void   color12(void);
   void   color13(void);
   void   color14(void);

   void   files1(void);
   void   files2(void);
   void   files3(void);
   void   files4(void);
   void   files5(void);
   void   clear_help(void);
   void   tlwindow(void);
   char * trim(char * s);
   unsigned get_key(int *done);



void main(int argc, char *argv[])
{

   _CURSORBACK = _BLUE;
   CURSORFRONT = WHITE;
   if (argc > 1)
   {
     if (strcmp(argv[1],"/b") == 0)             // Black & White
      {
       _CURSORBACK = _BLUE;
       CURSORFRONT  = BLACK;
       _vinfo.mapattr = 1 ;
       setvparam(VP_MONO) ;
       strcpy(configfile,"TL.CFG");
      }
     else
      strcpy(configfile,argv[1]);
   }
   else
    strcpy(configfile,"TL.CFG");
   xpos = wherex();
   ypos = wherey();
   videoinit();

   setkbloop(do_nothing);
   loadcfg(configfile);

   whelpdef("TLCFG.HLP",0x3B00,WHITE|_GREEN,BLACK|_GREEN,YELLOW|_GREEN,
      CURSORFRONT|_CURSORBACK,shadow);
   whelpwin(5,14,19,65,1,1);

   msinit();

   mainloop();

   whelpundef();

}

void mainloop(void)
{
   struct text_info ti;

   gettextinfo(&ti);
   buffer=ssave();
   if (!wopen(0,0,ti.screenheight-1,79,5,BLUE,YELLOW)) {
      printf("Error opening window!\n");
      exit(1);
   }

   wbox(0,0,2,79,3,BLUE);
   prints(0,27,BLUE,"µ ToneLoc Configuration Æ");
   fill_(3,0,ti.screenheight-3,79,'±',BLUE);
   whline(ti.screenheight-2,0,80,1,BLUE);

   if (_argc > 1) mssupport(atoi(_argv[1]));

   /* define the menu */

   wmenubeg(1,2,1,78,5,BLUE,CYAN,NULL);
      wmenuitem(0,1,"  Files  ",'F',0,0,files_menu,0,1);
      wmenuitem(0,10,"  ModemStrings  ",'M',10,0,NULL,0,100);
         wmenubeg(2,12,6,32,3,BLUE,CYAN,shadow);
         wmenuitem(0,0," Modem Commands  ",'C',11,0,mcommands,0,101);
         wmenuitem(1,0," Modem Responses ",'R',12,0,mresponses,0,126);
         wmenuitem(2,0," Volume Commands ",'V',13,0,vcommands,0,151);
         wmenuend(11,M_PD|M_SAVE,23,1,CYAN,YELLOW,DGREY,CURSORFRONT|_CURSORBACK);
      wmenuitem(0,26,"  ModemOptions  ",'O',20,0,moptions,0,200);
      wmenuitem(0,42,"  ScanOptions  ",'S',30,0,soptions,0,300);
      wmenuitem(0,57,"  Colors  ",'C',40,0,colors,0,400);
      wmenuitem(0,67,"  Quit  ",'Q',50,0,   NULL,0,500);
         wmenubeg(2,59,6,77,3,BLUE,CYAN,shadow);
         wmenuitem(0,0," Save Changes  ",'S',151,0,save_changes,0,501);
         wmenuitem(1,0," DOS Shell     ",'D',152,0,shell,0,526);
         wmenuitem(2,0," Abort Changes ",'A',153,0,abandon,0,551);
         wmenuend(151,M_PD|M_SAVE,23,1,CYAN,YELLOW,DGREY,CURSORFRONT|_CURSORBACK);
   wmenuend(0,M_HORZ,0,0,CYAN,YELLOW,DGREY,CURSORFRONT|_CURSORBACK);

   hidecur();
   wmenuget();
   abandon();
}

void mcommands(void)
{
   sc_win();    // special characters window

//   wopen(4,1,17,79,3,BLUE,CYAN);
   wopen(4,1,17,78,3,BLUE,CYAN);
//   shadow();

   // print menu options
   textattr(CYAN);
   wprintf(" Init String\n");
   wprintf(" Init Response\n");
   wprintf(" Dial Prefix\n");
   wprintf(" Dial Suffix\n");
   wprintf(" Speaker ON\n");
   wprintf(" Speaker OFF\n");
   wprintf(" Normal Hangup\n");
   wprintf(" Carrier Hangup\n");
   wprintf(" Tone Hangup\n");
   wprintf(" Exit String\n");
   wprintf(" Shell String\n");
   wprintf(" Shell Return");

   winpbeg(WHITE,CURSORFRONT|_CURSORBACK);
   winpdef(0,17,cfg.initstring,"***********************************************************",0,1,0,102);
   winpfba(mcmds1,0);
   winpdef(1,17,cfg.initresp,  "***********************************************************",0,1,0,103);
   winpfba(mcmds2,0);
   winpdef(2,17,cfg.prefix,    "***********************************************************",0,1,0,104);
   winpfba(mcmds3,0);
   winpdef(3,17,cfg.suffix,    "***********************************************************",0,1,0,105);
   winpfba(mcmds4,0);
   winpdef(4,17,cfg.speakon,   "***********************************************************",0,1,0,106);
   winpfba(mcmds5,0);
   winpdef(5,17,cfg.speakoff,  "***********************************************************",0,1,0,107);
   winpfba(mcmds6,0);
   winpdef(6,17,cfg.hangup,    "***********************************************************",0,1,0,108);
   winpfba(mcmds7,0);
   winpdef(7,17,cfg.c_hangup,  "***********************************************************",0,1,0,109);
   winpfba(mcmds8,0);
   winpdef(8,17,cfg.t_hangup,  "***********************************************************",0,1,0,110);
   winpfba(mcmds9,0);
   winpdef(9,17,cfg.exitstring,"***********************************************************",0,1,0,111);
   winpfba(mcmds10,0);
   winpdef(10,17,cfg.toshell,  "***********************************************************",0,1,0,112);
   winpfba(mcmds11,0);
   winpdef(11,17,cfg.fromshell,"***********************************************************",0,1,0,113);
   winpfba(mcmds12,0);

   winpkey(get_key,&terminate_key);
   winpread();
   hidecur();
   clear_help();
   wclose();
   trim(cfg.initstring);
   trim(cfg.initresp);
   trim(cfg.prefix);
   trim(cfg.suffix);
   trim(cfg.speakon);
   trim(cfg.speakoff);
   trim(cfg.hangup);
   trim(cfg.c_hangup);
   trim(cfg.t_hangup);
   wclose();
}

void mresponses(void)
{
   wopen(5,1,14,48,3,BLUE,CYAN);
   shadow();

   // print menu options
   textattr(CYAN);
   wprintf(" Found Carrier String\n");
   wprintf(" Found Tone String\n");
   wprintf(" Found Fax String\n");
   wprintf(" Ringing String\n");
   wprintf(" Busy String\n");
   wprintf(" Voice String\n");
   wprintf(" No Dialtone String\n");
   wprintf(" No Carrier String");

   winpbeg(WHITE,CURSORFRONT|_CURSORBACK);
   winpdef(0,24,cfg.connect_string,"*******************",0,1,0,127);
   winpfba(mrsps1,0);
   winpdef(1,24,cfg.tone_string,   "*******************",0,1,0,128);
   winpfba(mrsps2,0);
   winpdef(2,24,cfg.fax_string,    "*******************",0,1,0,129);
   winpfba(mrsps3,0);
   winpdef(3,24,cfg.ringing_string,"*******************",0,1,0,130);
   winpfba(mrsps4,0);
   winpdef(4,24,cfg.busy_string,   "*******************",0,1,0,131);
   winpfba(mrsps5,0);
   winpdef(5,24,cfg.voice_string,  "*******************",0,1,0,132);
   winpfba(mrsps6,0);
   winpdef(6,24,cfg.notone_string, "*******************",0,1,0,133);
   winpfba(mrsps7,0);
   winpdef(7,24,cfg.nocarrier_string,"*******************",0,1,0,134);
   winpfba(mrsps8,0);
	 winpkey(get_key,&terminate_key);

	 winpread();
   hidecur();
   clear_help();
   wclose();
   trim(cfg.connect_string);
   trim(cfg.tone_string);
   trim(cfg.fax_string);
   trim(cfg.ringing_string);
   trim(cfg.busy_string);
   trim(cfg.voice_string);
   trim(cfg.notone_string);
   trim(cfg.nocarrier_string);

}

void vcommands(void)
{
   register int x;
   sc_win();    // special characters window

   wopen(6,5,17,39,3,BLUE,CYAN);
   shadow();

   // print menu options
   textattr(CYAN);
   wprintf(" Volume 0\n");
   wprintf(" Volume 1\n");
   wprintf(" Volume 2\n");
   wprintf(" Volume 3\n");
   wprintf(" Volume 4\n");
   wprintf(" Volume 5\n");
   wprintf(" Volume 6\n");
   wprintf(" Volume 7\n");
   wprintf(" Volume 8\n");
   wprintf(" Volume 9");

   winpbeg(WHITE,CURSORFRONT|_CURSORBACK);
   winpdef(0,12,cfg.vol[0],"*******************",0,1,0,152);
   winpdef(1,12,cfg.vol[1],"*******************",0,1,0,152);
   winpdef(2,12,cfg.vol[2],"*******************",0,1,0,152);
   winpdef(3,12,cfg.vol[3],"*******************",0,1,0,152);
   winpdef(4,12,cfg.vol[4],"*******************",0,1,0,152);
   winpdef(5,12,cfg.vol[5],"*******************",0,1,0,152);
   winpdef(6,12,cfg.vol[6],"*******************",0,1,0,152);
   winpdef(7,12,cfg.vol[7],"*******************",0,1,0,152);
   winpdef(8,12,cfg.vol[8],"*******************",0,1,0,152);
   winpdef(9,12,cfg.vol[9],"*******************",0,1,0,152);

   prints(24,2,WHITE,"Strings to send modem when 0-9 pressed");
	 winpkey(get_key,&terminate_key);

	 winpread();
   hidecur();
   clear_help();
   wclose();
   wclose();
   for (x=0; x<10; x++)
      trim(cfg.vol[x]);
}

void moptions(void)
{
   char port[3], baud[7], fossil[2], cmddelay[5],
        pacing[4], getdelay[5], ignore_cd[2], ignore_cts[2],
        ignore_unknown[2], portaddx[5], portirq[3],
        device_type[3], ouresn[10], ournum[12], tumbl_esn[2],
        tumbl_min[2];

   // convert cfg variables to temp strings
   sprintf(device_type,"%u",cfg.device_type);

   sprintf(ouresn,"%-8X",cfg.ouresn);
   strcpy(ournum,cfg.ournum);
   sprintf(tumbl_esn,"%c",cfg.tumbl_esn?'Y':'N');
   sprintf(tumbl_min,"%c",cfg.tumbl_min?'Y':'N');

   sprintf(port,"%u",cfg.port);
   sprintf(portaddx,"%-3X",cfg.portaddx);
   sprintf(portirq,"%d",cfg.irq);
   sprintf(baud,"%ld",cfg.baudrate);
   sprintf(fossil,"%c",cfg.Fossil?'Y':'N');
   sprintf(cmddelay,"%d",cfg.command_delay);
   sprintf(pacing,"%d",cfg.pacing);
   sprintf(getdelay,"%d",cfg.getdelay);
   sprintf(ignore_cd,"%c",cfg.ignore_cd?'Y':'N');
   sprintf(ignore_cts,"%c",cfg.ignore_cts?'Y':'N');
   sprintf(ignore_unknown,"%c",cfg.ignore_unknown?'Y':'N');

   if (cfg.ctek_enabled == 0)
    wopen(2,27,14,60,3,BLUE,CYAN);
   else
    wopen(2,27,19,65,3,BLUE,CYAN);

   shadow();

   // print menu options
   textattr(CYAN);

   wprintf(" Serial Port\n");
   wprintf(" Port Address\n");
   wprintf(" Port IRQ\n");
   wprintf(" Baud Rate\n");
   wprintf(" Use FOSSIL\n");
   wprintf(" Command Delay\n");
   wprintf(" Character Delay\n");
   wprintf(" Response Wait\n");
   wprintf(" Ignore CD line\n");
   wprintf(" Ignore CTS line\n");
   wprintf(" Ignore Unknowns");
   if (cfg.ctek_enabled == 1) {
    wprintf("\n Device Type\n");
    wprintf(" ESN\n");
    wprintf(" MIN\n");
    wprintf(" Tumble ESN\n");
    wprintf(" Tumble MIN");
   }

   winpbeg(WHITE,CURSORFRONT|_CURSORBACK);
   winpdef(0,23,port,"%%",0,1,0,201);            // %%
   winpfba(mopts1,0);
   winpdef(1,23,portaddx,"???",0,1,0,202);
   winpfba(mopts2,0);
   winpdef(2,23,portirq,"%",0,1,0,203);
   winpfba(mopts3,0);
   winpdef(3,23,baud,"%%%%%%",0,1,0,204);
   winpfba(mopts4,0);
   winpdef(4,23,fossil,"Y",'U',1,0,205);
   winpfba(mopts5,0);
   winpdef(5,23,cmddelay,"%%%%",0,1,0,206);
   winpfba(mopts6,0);
   winpdef(6,23,pacing,"%%%",0,1,0,207);
   winpfba(mopts7,0);
   winpdef(7,23,getdelay,"%%%%",0,1,0,208);
   winpfba(mopts8,0);
   winpdef(8,23,ignore_cd,"Y",'U',1,0,209);
   winpfba(mopts9,0);
   winpdef(9,23,ignore_cts,"Y",'U',1,0,210);
   winpfba(mopts10,0);
   winpdef(10,23,ignore_unknown,"Y",'U',1,0,211);
   winpfba(mopts11,0);

   if (cfg.ctek_enabled == 1) {

    winpdef(11,23,device_type,"%%",0,1,0,212);
    winpfba(mopts12,0);

    winpdef(12,23,ouresn,"?????????",0,1,0,213);
    winpfba(mopts13,0);

    winpdef(13,23,ournum,"%%%%%%%%%%%",0,1,0,214);
    winpfba(mopts14,0);

    winpdef(14,23,tumbl_esn,"Y",'U',1,0,215);
    winpfba(mopts15,0);

    winpdef(15,23,tumbl_min,"Y",'U',1,0,216);
    winpfba(mopts16,0);

     }

	 winpkey(get_key,&terminate_key);
   winpread();
   hidecur();
   clear_help();

   // transfer temp strings back to cfg variables
   cfg.port     = atow(port);
   if (atoi(port) == 84)                       // They have proved eliteness!
     cfg.ctek_enabled = 1;                     // Dub them elite
   if (atoi(port) == 94)
     cfg.ctek_enabled = 0;                     // They give up eliteness
   sscanf(portaddx,"%x",&cfg.portaddx);
   cfg.irq      = atoi(portirq);
   cfg.baudrate = atol(baud);
   cfg.Fossil   = fossil[0]=='Y'?1:0;
   cfg.command_delay = atoi(cmddelay);
   cfg.pacing   = atoi(pacing);
   cfg.getdelay = atoi(getdelay);
   cfg.ignore_cd= ignore_cd[0]=='Y'?1:0;
   cfg.ignore_cts=ignore_cts[0]=='Y'?1:0;
   cfg.ignore_unknown = ignore_unknown[0]=='Y'?1:0;
   cfg.device_type = atoi(device_type);
   strcpy(cfg.ournum,ournum);
   sscanf(ouresn,"%08lx",&cfg.ouresn);
//   cfg.ouresn = atol(ouresn);
   cfg.tumbl_esn = tumbl_esn[0]=='Y'?1:0;
   cfg.tumbl_min = tumbl_min[0]=='Y'?1:0;

   wclose();
}

void soptions(void)
{
   char sfx[2], foundsfx[2], between[6], wait[6], wipe[2], nodupes[2],
        maxring[2], scan_type[2], autoint[4], dialmethod[2], logging[2],
        notoneabort[4], flogstring[90], carrierlogging[2], nudgestring[90],
        postnudgedelay[6], autoe71[2], lfstrip[2];

   // convert cfg variables to temp strings
   sprintf(sfx,"%c",cfg.sfx?'Y':'N');
   sprintf(foundsfx,"%c",cfg.foundsfx?'Y':'N');
   sprintf(between,"%u",cfg.between_delay);
   sprintf(wait,"%u",cfg.wait_delay);
   sprintf(wipe,"%d",cfg.between_wipe);
   sprintf(nodupes,"%c",cfg.NoDupes?'Y':'N');
   sprintf(maxring,"%d",cfg.MaxRing);
   sprintf(scan_type,"%d",cfg.scan_type);
   sprintf(autoint,"%d",cfg.AutoInt);
   sprintf(dialmethod,"%d",cfg.DialMethod);
   sprintf(logging,"%c",cfg.logging?'Y':'N');
   sprintf(notoneabort,"%d",cfg.NoToneAbort);
   strcpy(flogstring,cfg.flogstring);
   sprintf(carrierlogging,"%c",cfg.CarrierLogging?'Y':'N');
   strcpy(nudgestring,cfg.NudgeString);
   sprintf(postnudgedelay,"%u",cfg.PostNudgeDelay);
   sprintf(autoe71,"%d",cfg.AutoE71);
   sprintf(lfstrip,"%c",cfg.LFstrip?'Y':'N');

   wopen(2,3,21,77,3,BLUE,CYAN);
   //shadow();

   // print menu options
   textattr(CYAN);
   wprintf(" Sound Effects\n");
   wprintf(" Found Chime\n");
   wprintf(" Between-call Delay\n");
   wprintf(" Wait Delay\n");
   wprintf(" Between Wipe\n");
   wprintf(" Save .DAT Files\n");
   wprintf(" Maximum Rings\n");
   wprintf(" Scan For\n");
   wprintf(" Auto-Save Interval\n");
   wprintf(" Scanning Method\n");
   wprintf(" Logging to Disk\n");
   wprintf(" No Dialtone Limit\n");
   wprintf(" Found Log String\n");
   wprintf(" Carrier Logging\n");
   wprintf(" Nudge String\n");
   wprintf(" Post-nudge Delay\n");
   wprintf(" Parity Stripping\n");
   wprintf(" Linefeed Stripping");

   winpbeg(WHITE,CURSORFRONT|_CURSORBACK);
   winpdef(0,22,sfx,"Y",'U',1,0,301);
   winpfba(scopts1,0);
   winpdef(1,22,foundsfx,"Y",'U',1,0,302);
   winpfba(scopts2,0);
   winpdef(2,22,between,"%%%%%",0,1,0,303);
   winpfba(scopts3,0);
   winpdef(3,22,wait,"%%%%%",0,1,0,304);
   winpfba(scopts4,0);
   winpdef(4,22,wipe,"%",0,1,0,305);
   winpfba(scopts5,0);
   winpdef(5,22,nodupes,"Y",'U',1,0,306);
   winpfba(scopts6,0);
   winpdef(6,22,maxring,"%",0,1,0,307);
   winpfba(scopts7,0);
   winpdef(7,22,scan_type,"%",0,1,0,308);
   winpfba(scopts8,0);
   winpdef(8,22,autoint,"%%%",0,1,0,309);
   winpfba(scopts9,0);
   winpdef(9,22,dialmethod,"%",0,1,0,310);
   winpfba(scopts10,0);
   winpdef(10,22,logging,"Y",'U',1,0,311);
   winpfba(scopts11,0);
   winpdef(11,22,notoneabort,"%%%",0,1,0,312);
   winpfba(scopts12,0);
   winpdef(12,22,flogstring,"**********************************************************************",0,1,0,313);
   winpfba(scopts13,0);
   winpdef(13,22,carrierlogging,"Y",'U',1,0,314);
   winpfba(scopts14,0);
   winpdef(14,22,nudgestring,"*********************************************************************",0,1,0,315);
   winpfba(scopts15,0);
   winpdef(15,22,postnudgedelay,"%%%%%",0,1,0,316);
   winpfba(scopts16,0);
   winpdef(16,22,autoe71,"%",0,1,0,317);
   winpfba(scopts17,0);
   winpdef(17,22,lfstrip,"Y",'U',1,0,318);
   winpfba(scopts18,0);

	 winpkey(get_key,&terminate_key);

   winpread();
   hidecur();
   clear_help();

   // transfer temp strings back to cfg variables
   cfg.sfx             = sfx[0]=='Y' ? 1 : 0;
   cfg.foundsfx        = foundsfx[0]=='Y' ? 1 : 0;
   cfg.between_delay   = atow(between);
   cfg.wait_delay      = atow(wait);
   cfg.between_wipe    = atoi(wipe);
   cfg.NoDupes         = nodupes[0]=='Y' ? 1 : 0;
   cfg.MaxRing         = atoi(maxring);
   cfg.scan_type       = atoi(scan_type);
   cfg.AutoInt         = atoi(autoint);
   cfg.DialMethod      = atoi(dialmethod);
   cfg.logging         = logging[0]=='Y' ? 1 : 0;
   cfg.NoToneAbort     = atoi(notoneabort);
   strcpy(cfg.flogstring,flogstring);
   cfg.CarrierLogging  = carrierlogging[0]=='Y' ? 1 : 0;
   strcpy(cfg.NudgeString,nudgestring);
   cfg.PostNudgeDelay  = atow(postnudgedelay);
   cfg.AutoE71         = atoi(autoe71);
   cfg.LFstrip         = lfstrip[0]=='Y' ? 1 : 0;

   trim(cfg.flogstring);
   trim(cfg.NudgeString);
   wclose();
}

void colors(void)
{
   char s[13][3];
   WINDOW colormenu;

   // load temp strings with config values
   sprintf(s[0],"%u",cfg.act_win);
   sprintf(s[1],"%u",cfg.act_text);
   sprintf(s[2],"%u",cfg.mod_win);
   sprintf(s[3],"%u",cfg.mod_text);
   sprintf(s[4],"%u",cfg.stats_win);
   sprintf(s[5],"%u",cfg.stats_text);
   sprintf(s[6],"%u",cfg.stats_items);
   sprintf(s[7],"%u",cfg.tone_text);
   sprintf(s[8],"%u",cfg.carrier_text);
   sprintf(s[9],"%u",cfg.meter_back);
   sprintf(s[10],"%u",cfg.meter_fore);
   sprintf(s[11],"%c",cfg.meterback);
   sprintf(s[12],"%c",cfg.meterfront);

   colormenu=wopen(2,48,16,75,3,BLUE,CYAN);
   shadow();


   tlwindow();
   wactiv(colormenu);

   // print menu options
   textattr(CYAN);
   wprintf(" Activity Window\n");
   wprintf(" Activity Text\n");
   wprintf(" Modem Window\n");
   wprintf(" Modem Text\n");
   wprintf(" Statistics Window\n");
   wprintf(" Statistics Text\n");
   wprintf(" Statistics Items\n");
   wprintf(" Found Tone Text\n");
   wprintf(" Found Carrier Text\n");
   wprintf(" Meter Background\n");
   wprintf(" Meter Foreground\n");
   wprintf(" Meter Back Char\n");
   wprintf(" Meter Front Char");

   winpbeg(WHITE,CURSORFRONT|_CURSORBACK);
   winpdef(0,22,s[0],"%%",0,1,0,0);
   winpfba(color1,0);
   winpdef(1,22,s[1],"%%",0,1,0,0);
   winpfba(color2,0);
   winpdef(2,22,s[2],"%%",0,1,0,0);
   winpfba(color3,0);
   winpdef(3,22,s[3],"%%",0,1,0,0);
   winpfba(color4,0);
   winpdef(4,22,s[4],"%%",0,1,0,0);
   winpfba(color5,0);
   winpdef(5,22,s[5],"%%",0,1,0,0);
   winpfba(color6,0);
   winpdef(6,22,s[6],"%%",0,1,0,0);
   winpfba(color7,0);
   winpdef(7,22,s[7],"%%",0,1,0,0);
   winpfba(color8,0);
   winpdef(8,22,s[8],"%%",0,1,0,0);
   winpfba(color9,0);
   winpdef(9,22,s[9],"%%",0,1,0,0);
   winpfba(color10,0);
   winpdef(10,22,s[10],"%%",0,1,0,11);
   winpfba(color11,0);
   winpdef(11,22,s[11],"?",0,1,0,0);
   winpfba(color12,0);
   winpdef(12,22,s[12],"?",0,1,0,0);
   winpfba(color13,0);
	 winpkey(get_key,&terminate_key);

	 winpread();
   hidecur();
   clear_help();

   // transfer temp strings back to cfg variables
   cfg.act_win   = atob(s[0]);
   cfg.act_text  = atob(s[1]);
   cfg.mod_win   = atob(s[2]);
   cfg.mod_text  = atob(s[3]);
   cfg.stats_win = atob(s[4]);
   cfg.stats_text= atob(s[5]);
   cfg.stats_items=atob(s[6]);
   cfg.tone_text = atob(s[7]);
   cfg.carrier_text=atob(s[8]);
   cfg.meter_back= atob(s[9]);
   cfg.meter_fore= atob(s[10]);
   cfg.meterback = s[11][0];
   cfg.meterfront= s[12][0];

   wclose();
   wactiv(tlwin);
   wclose();
   tlwin=0;

}

WINDOW sc_win(void)
{
   WINDOW help;
   help = wopen(8,53,16,75,3,YELLOW,WHITE);
   wtitle("µ Special Chars Æ",TCENTER,YELLOW);
   shadow();
   wprintf("\n ~ = « sec delay\n");
   wprintf(" | = [Enter]\n");
   wprintf(" < = Drop DTR\n");
   wprintf(" > = Raise DTR\n");
   wprintf(" ! = ¬ sec DTR drop");
   return(help);
}


void files_menu(void)
{            // 2,3,7,31,3  == original values
   wopen(2,3,8,31,3,BLUE,CYAN);
   shadow();
   textattr(CYAN);
   wprintf(" Log File\n");
   wprintf(" Found File\n");
   wprintf(" Black List\n");
   wprintf(" Alt Screen\n");
   wprintf(" Carrier Log File");

   winpbeg(WHITE,CURSORFRONT|_CURSORBACK);
   winpdef(0,13,cfg.logname ,"FFFFFFFFFFFF",'U',2,0,2);
   winpfba(files1,0);
   winpdef(1,13,cfg.foundname ,"FFFFFFFFFFFF",'U',2,0,3);
   winpfba(files2,0);
   winpdef(2,13,cfg.blackfilename ,"FFFFFFFFFFFF",'U',2,0,4);
   winpfba(files3,0);
   winpdef(3,13,cfg.AltScreen ,"FFFFFFFFFFFF",'U',2,0,5);
   winpfba(files4,0);

   winpdef(4,13,cfg.carrierlogname ,"FFFFFFFFFFFF",'U',2,0,6);
   winpfba(files5,0);

	 winpkey(get_key,&terminate_key);

   winpread();                 // Does the read
   hidecur();
   clear_help();
   wclose();

   trim(cfg.logname);
   trim(cfg.foundname);
   trim(cfg.blackfilename);
   trim(cfg.AltScreen);
   trim(cfg.carrierlogname);

}

void mopts1(void)
{
   prints(24,2,WHITE,"Serial port to use (1=COM1 ... 4=COM4)                     ");
}

void mopts2(void)
{
   prints(24,2,WHITE,"Hex Serial Port Address (0 for default)                    ");
}

void mopts3(void)
{
   prints(24,2,WHITE,"IRQ for serial port (0 for default)                        ");
}

void mopts4(void)
{
   prints(24,2,WHITE,"What baud rate to use                                      ");
}

void mopts5(void)
{
   prints(24,2,WHITE,"Use an external FOSSIL driver?                             ");
}

void mopts6(void)
{
   prints(24,2,WHITE,"Delay (in ms) to pause AFTER a modem cmd                   ");
}

void mopts7(void)
{
   prints(24,2,WHITE,"Delay (in ms) to pause between characters                  ");
}

void mopts8(void)
{
   prints(24,2,WHITE,"Time (in ms) to wait for modem responses                   ");
}

void mopts9(void)
{
   prints(24,2,WHITE,"Ignore the modem's Carrier Detect line?                    ");
}

void mopts10(void)
{
   prints(24,2,WHITE,"Ignore the modem's Clear To Send line?                     ");
}

void mopts11(void)
{
   prints(24,2,WHITE,"Ignore all unknown responses from modem?                   ");
}

void mopts12(void)
{
   prints(24,2,WHITE,"Type of device to use for dialing?                         ");
}

void mopts13(void)
{
   prints(24,2,WHITE,"Our ESN (Electronic Serial Number)?                        ");
}

void mopts14(void)
{
   prints(24,2,WHITE,"Our MIN (Mobile Identification Number, i.e. phone number)  ");
}

void mopts15(void)
{
   prints(24,2,WHITE,"Tumble ESN between calls?                                  ");
}

void mopts16(void)
{
   prints(24,2,WHITE,"Tumble MIN between calls?                                  ");
}


void scopts1(void)
{
   prints(24,2,WHITE,"Use sound effects?                                         ");
}

void scopts2(void)
{
   prints(24,2,WHITE,"Make a noise when a carrier is found?                      ");
}

void scopts3(void)
{
   prints(24,2,WHITE,"Millisecond delay between calls                            ");
}

void scopts4(void)
{
   prints(24,2,WHITE,"Millisecond delay to wait for Tone/Carrier                 ");
}

void scopts5(void)
{
   prints(24,2,WHITE,"Which timer bar wipe to use                                ");
}

void scopts6(void)
{
   prints(24,2,WHITE,"Save .DAT files to disk?                                   ");
}

void scopts7(void)
{
   prints(24,2,WHITE,"Maxium number of rings before dial aborted                 ");
}

void scopts8(void)
{
   prints(24,2,WHITE,"Scan for tones, carriers, all but tones, all but carriers  ");
}

void scopts9(void)
{
   prints(24,2,WHITE,"Autosave interval in minutes                               ");
}

void scopts10(void)
{
   prints(24,2,WHITE,"Scan random, sequential                                    ");
}

void scopts11(void)
{
   prints(24,2,WHITE,"Log modem window to disk?                                  ");
}

void scopts12(void)
{
   prints(24,2,WHITE,"Number of NO DIALTONE responses before abort               ");
}

void scopts13(void)
{
   prints(24,2,WHITE,"Structure of the found log entry                           ");
}

void scopts14(void)
{
   prints(24,2,WHITE,"Log results of carriers to disk?                           ");
}

void scopts15(void)
{
   prints(24,2,WHITE,"String to send to target system to prompt a response       ");
}

void scopts16(void)
{
   prints(24,2,WHITE,"Millisecond delay to wait for a response from target system");
}

void scopts17(void)
{
   prints(24,2,WHITE,"Parity Stripping (8N1 only, E71 only, Auto)                ");
}

void scopts18(void)
{
   prints(24,2,WHITE,"Linefeed Stripping                                         ");
}

void mrsps1(void)
{
   prints(24,2,WHITE,"How your modem responds to a Carrier                       ");
}

void mrsps2(void)
{
   prints(24,2,WHITE,"How your modem responds to a Tone                          ");
}

void mrsps3(void)
{
   prints(24,2,WHITE,"How your modem responds to a Fax                           ");
}

void mrsps4(void)
{
   prints(24,2,WHITE,"How your modem responds to a RINGING (X6)                  ");
}

void mrsps5(void)
{
   prints(24,2,WHITE,"How your modem responds to a BUSY                          ");
}

void mrsps6(void)
{
   prints(24,2,WHITE,"How your modem responds to a VOICE                         ");
}

void mrsps7(void)
{
   prints(24,2,WHITE,"How your modem responds to a NO DIALTONE                   ");
}

void mrsps8(void)
{
   prints(24,2,WHITE,"How your modem responds to a NO CARRIER                    ");
}

void mcmds1(void)
{
   prints(24,2,WHITE,"String to send to modem when ToneLoc is first run          ");
}
void mcmds2(void)
{
   prints(24,2,WHITE,"How your modem responds to the Init String                 ");
}
void mcmds3(void)
{
   prints(24,2,WHITE,"Command to dial the modem (including *70, etc)             ");
}
void mcmds4(void)
{
   prints(24,2,WHITE,"Suffix to Dial String                                      ");
}
void mcmds5(void)
{
   prints(24,2,WHITE,"Command to turn the modem speaker ON                       ");
}
void mcmds6(void)
{
   prints(24,2,WHITE,"Command to turn the modem speaker OFF                      ");
}
void mcmds7(void)
{
   prints(24,2,WHITE,"How to hang up the modem between dial attempts             ");
}
void mcmds8(void)
{
   prints(24,2,WHITE,"How to hang up the modem that is connected                 ");
}
void mcmds9(void)
{
   prints(24,2,WHITE,"How to hang up the modem that just found a tone            ");
}


void mcmds10(void)
{
   prints(24,2,WHITE,"String to send to modem when ToneLoc exits                 ");
}
void mcmds11(void)
{
   prints(24,2,WHITE,"String to send to modem before a dos shell                 ");
}
void mcmds12(void)
{
   prints(24,2,WHITE,"String to send to modem upon returning from a dos shell    ");
}


void color1(void)
{
   prints(24,2,WHITE,"Color of the Activity Window                               ");
}

void color2(void)
{
   prints(24,2,WHITE,"Color of the Activity Text                                 ");
}

void color3(void)
{
   prints(24,2,WHITE,"Color of the Modem Window                                  ");
}

void color4(void)
{
   prints(24,2,WHITE,"Color of the Modem Text                                    ");
}

void color5(void)
{
   prints(24,2,WHITE,"Color of the Statistics Window                             ");
}

void color6(void)
{
   prints(24,2,WHITE,"Color of the Statistics Text                               ");
}

void color7(void)
{
   prints(24,2,WHITE,"Color of the Statistics Items                              ");
}

void color8(void)
{
   prints(24,2,WHITE,"Color of the Found Tone Text                               ");
}

void color9(void)
{
   prints(24,2,WHITE,"Color of the Found Carrier Text                            ");
}

void color10(void)
{
   prints(24,2,WHITE,"Color of the Meter Background Character                    ");
}

void color11(void)
{
   prints(24,2,WHITE,"Color of the Meter Foreground Character                    ");
}

void color12(void)
{
   prints(24,2,WHITE,"The Meter Background Character                             ");
}

void color13(void)
{
   prints(24,2,WHITE,"The Meter Foreground Character                             ");
}


void files1(void)
{
   prints(24,2,WHITE,"Filename for the main ToneLoc log file                     ");
}

void files2(void)
{
   prints(24,2,WHITE,"Filename to log all Tones and Carriers found               ");
}

void files3(void)
{
   prints(24,2,WHITE,"File with list of phone numbers to NEVER dial              ");
}

void files4(void)
{
   prints(24,2,WHITE,"Filename of Alternate (Parent) Screen                      ");
}

void files5(void)
{
   prints(24,2,WHITE,"Filename of Carrier Log File                               ");
}


void clear_help(void)
{
   prints(24,2,WHITE,"                                                                   ");
}

void shadow(void)
{
// wshadow(DGREY|_BLACK);
   wshadow(BLACK|_BLACK);
}

void abandon(void)
{
   quit();
   puts("\nþ ToneLoc Configuration changes abandoned");
   exit(0);
}

void save_changes(void)
{
   writecfg(configfile);
   quit();
   puts("\nþ ToneLoc Configuration changes saved");
   exit(0);
}

void quit(void)
{
   srestore(buffer);
   gotoxy(xpos,ypos);
   showcur();
}

void shell(void)
{
   int * xbuffer;
   xbuffer=ssave();
   clrscr();
   showcur();
   system("");
   // get/restore directory (code in toneloc.c)
   hidecur();
   srestore(xbuffer);
}

void writecfg(char * fname)
{
   FILE *f;

   f=fopen(fname,"wb");
   if (!f)
      wperror(" Error writing config file ");
   else {
      fwrite(&cfg,sizeof(cfg),1,f);
      fclose(f);
   }
}

void loadcfg(char * fname)
{
   FILE *f;

   f=fopen(fname,"rb");
   if (!f) {
//    f=fopen(fname,"wb");
      setdefaults();
   } else {
      fread(&cfg,sizeof(cfg),1,f);
      fclose(f);
   }
  if (cfg.VersionID != CFGVERSION)
   {
      printf("\n\rThis config file (%s) is from an old version of ToneLoc!\n\rDelete it and run TLCFG again.\n\r",fname);
      exit(1);
   }

}

void setdefaults(void)
{
   memset(&cfg,0,sizeof(cfg));
   cfg.ProductCode[0]='T';
   cfg.ProductCode[1]='L';
   cfg.VersionID = CFGVERSION;      // 0x0110 = 1.10
   cfg.Fossil = 0;
   cfg.port = 1;                  // Default port = Com 1
   cfg.baudrate = 2400;
   cfg.sfx = 1;
   cfg.foundsfx = 1;

   cfg.ctek_enabled = 0;          // Not unless they are el1te!
   strcpy(cfg.ournum,"");
   cfg.ouresn = 0;
   cfg.tumbl_esn = 0;
   cfg.tumbl_min = 0;

   cfg.act_win    = 3;
   cfg.act_text   = 9;
   cfg.mod_win    = 15;
   cfg.mod_text   = 2;
   cfg.tone_text  = 10;
   cfg.carrier_text = 12;
   cfg.stats_win   = 11;
   cfg.stats_items = 3;
   cfg.stats_text  = 15;
   cfg.meter_back  = 1;
   cfg.meter_fore  = 14;
   cfg.meterfront = 'Û';
   cfg.meterback  = '±';

   strcpy(cfg.initstring,"ATZ|~~~ATX4S11=50|~~");
   strcpy(cfg.initresp,"OK");
   strcpy(cfg.prefix,"ATDT");
   strcpy(cfg.suffix,"");
   strcpy(cfg.connect_string,"CONNECT");
   strcpy(cfg.tone_string,"OK");
   strcpy(cfg.fax_string,"FAX");
   strcpy(cfg.ringing_string,"RINGING");
   strcpy(cfg.busy_string,"BUSY");
   strcpy(cfg.voice_string,"VOICE");
   strcpy(cfg.notone_string,"NO DIAL");
   strcpy(cfg.nocarrier_string,"NO CARRIER");
   strcpy(cfg.speakon,"ATM1|~");
   strcpy(cfg.speakoff,"ATM0|~");
   strcpy(cfg.vol[0],"ATM0|~");
   // ...
   strcpy(cfg.vol[9],"ATL5|~");

   strcpy(cfg.c_hangup,"<~>");
   strcpy(cfg.t_hangup,"ATH0|");
   strcpy(cfg.hangup,"!");
   strcpy(cfg.exitstring,"ATZ|");
   strcpy(cfg.toshell,"");
   strcpy(cfg.fromshell,"");
   strcpy(cfg.logname,"TONE.LOG");

   cfg.command_delay = 250;
   cfg.between_delay = 500;
   cfg.wait_delay    = 35000;

   cfg.between_wipe = 3;
   cfg.NoDupes      = 1;
   cfg.MaxRing      = 0;
   cfg.scan_type    = 1;
   cfg.ignore_cd    = 0;
   cfg.ignore_cts   = 0;
   cfg.ignore_unknown = 0;
   cfg.AutoInt      = 20;

   strcpy(cfg.foundname,"FOUND.LOG");
   strcpy(cfg.carrierlogname,"FOUND.LOG");
   strcpy(cfg.blackfilename,"BLACK.LST");
   cfg.DialMethod  = 0;
   cfg.logging     = 1;
   cfg.NoToneAbort = 30;
   cfg.getdelay    = 350;
   cfg.pacing      = 0;
   strcpy(cfg.AltScreen,"HELP.BIN");
   strcpy(cfg.flogstring,"%d %t %n %b: %r");
   cfg.CarrierLogging = 1;
   strcpy(cfg.NudgeString,"|||~~~|||~~~~~~~|||");
   cfg.PostNudgeDelay = 40000;
   cfg.AutoE71 = 1;
   cfg.LFstrip = 1;

}

unsigned get_key (int *done)       // don't need the done flag, but it is in the declaration.
{
   unsigned int key;

     key = getxch();

     if(key == 0x011b)     // Escape
		    key=0x1c0a;        // Ctrl-Enter

		 // Window Yes/No/Cancel

 /*
		switch (result)
		{
			case yes :  		key=0x1c0a;    // Ctrl-Enter: exit and save changes
											break;
			case no :		 		key=0x11b;     // Esc: exit and discard changes
											break;
			case cancel :		key=0x4900;    // PgDn: should be discarded.
		}
		*/

   return(key);
}



void do_nothing(void)
{
   char tmstr[80];
   struct time t;
   char ampm = 'a';

   gettime(&t);
   if (t.ti_hour > 12) {
      t.ti_hour -= 12;
      ampm='p';
   }
   if (t.ti_hour == 0)
      t.ti_hour = 12;
//   sprintf(tmstr,"F1 for help %2d%c%02d %cm",t.ti_hour,(t.ti_hund>50)?':':' ',t.ti_min,ampm);
   sprintf(tmstr,"F1 for help");

// prints(24,71,WHITE,tmstr);
   prints(24,69,WHITE,tmstr);

   if (_vinfo.dvexist) {
      union REGS r;
      r.x.ax=0x1000;
      int86(0x15,&r,&r);
   }
}

void tlwindow(void)
{

   if (tlwin==0) {     // draw window for
      tlwin= wopen(5,2,19,45,5,WHITE,WHITE);
//    shadow();
      wbox(0,1,14,42,0,BLACK);                   // Box around whole thing.
//    wopen(5,3,19,44,0,WHITE,WHITE);
//    wtitle("[ Color Sample ]",TCENTER,WHITE);
//    wopen(6,5,18,25,0,cfg.act_win,cfg.act_text);
      wbox(1,3,13,23,0,cfg.act_win);
//    wprintf(" 10:23 ¯\n 10:24 ToneLoc\n 10:24 555-XXXX\n");
//    wopen(6,27,10,42,0,cfg.mod_win,cfg.mod_text);
      wbox(1,25,5,40,0,cfg.mod_win);
//    wprintf("ATZ\nOK\n");
//    wopen(11,27,18,42,0,cfg.stats_win,cfg.stats_text);
      wbox(6,25,13,40,0,cfg.stats_win);
      whline(9,25,15,0,cfg.stats_win);
      whline(12,25,15,0,cfg.stats_win);
      wvline(9,33,4,0,cfg.stats_win);
   }

}

char * trim(char * s)
{
   /* trims leading/trailing spaces off of a string */
   register int x=0;

   if (s==NULL)
      return(NULL);

   while (isspace(s[x]))
      x++;
   if (x)
      strcpy(&s[0],&s[x]);

   x=strlen(s)-1;
   while (isspace(s[x]))
      x--;
   s[x+1]=0;

   return(s);
}

