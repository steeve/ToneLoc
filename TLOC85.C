/* 
 * ToneLoc - Dial Tone locator by Minor Threat
 * This program dials numbers looking for a dialtone
 * Useful for finding PBX's, Loops, or anything that 
 * gives a constant tone sound.  
 */

#include <stdlib.h>
#include <conio.h>
#include <stdio.h>
#include <stdarg.h>
#include <dos.h>
#include <time.h>
#include "toneloc.h"
#define VERSION "v0.85"
#define METERLENGTH 30
#define DELAYTIME 200

struct   time start, end;          /* clock ticks used to time */
int      port;                     /* which comport to use */
int      tried=0;                  /* numbers tried */
int      tones, busys, voices, rings; /* to keep track of stuff */
byte     oldones[10000];           /* already used numbers */
word     maxcodes;                 /* maximum # of possible numbers */
char     initstring[50];           /* modem initialization string */
word     baudrate;                 /* baud rate for modem, duh */
FILE     *logfile, *datafile;      /* logfile and used numbers file */
char     DESQview;                 /* global for DESQview? */
byte     mesposx, statposx, modemposx; /* current x positions in windows */
byte     mesposy, statposy, modemposy; /* current y positions in windows */
byte     foundposx, foundposy;         /* ditto */

void main(int argc, char *argv[])
{
    
   int    mprintf(char *, ...);       /* printf to FOSSIL */
   void   mputs(char *);              /* puts() to modem */
   void   charout(char);              /* sends a char to modem */
   int    initfoss(int);              /* inits FOSSIL */
   void   deinit(void);               /* deinits FOSSIL */
   void   dial(char *);               /* dials telephone (1-800...) */
   int    comhit(void);               /* kbhit(), for comport & local kb */
   char   getkey(void);               /* gets key */
   char   charinn(void);              /* gets char, returns 0 if none */
   char * mgets(char *);              /* gets from comport */
   char * clearbuffer(word);          /* clear FOSSIL input buffer */
   void   quit(char * filename);      /* exits gracefully, saves datafile */
   int    initmodem(char *);          /* sends modem init string(s) */
   char * getresponse(word);          /* get modem response */
   int    rnd(word);                  /* generate random number (string) */
   int    parse(int, char *[], char *); /* parse command line */
   char * process(char *);            /* replaces 'X's with #'s */
   void   drawscreen(void);           /* draws the windowed screen */
   void   box(int,int,int,int,int, char *);   /* draws a box neato */
   void   message(char *, ...);       /* updates message window */
   void   stats(char *, ...);         /* updates stats window */
   void   modem(char *, ...);         /* updates modem window */
   void   found(char *);              /* updates found window */
   void   log(char *, ...);           /* writes to log file */
   void   hline(int,int,int,int);     /* draw smart horizontal line */
   void   vline(int,int,int,int);     /* draw smart vertical line */
   void   meter(word, word);          /* updates status meter */
   void   dvdelay(word);              /* delay() with DESQview pauses */
   void   timeslice(void);            /* releases remainder of timeslice */
   void   helpscreen(void);           /* help screen for lamers */
   void   updatestats(void);          /* update statistics window */
   void   doresponse(char *, int *, char *); /* processes modem response */
   void   shell(void);                /* saves screen, shells to DOS */
   int    memavail(void);             /* returns K available */
   void   updateclock(void);          /* updates clock on screen */

   int    index;
   char   dialstring[50];
   char   response[30];
   char   num[20];
   char   filename[13];
   char   ch, again;

   if ( (argc==1) || (strchr(argv[1],'?')) ) helpscreen();  

   randomize();

   port=1;         /* 0=COM1, 1=COM2, etc */
   if (argc>2) port=(atoi(argv[2])-1);

   baudrate=9600;  /* 2400, 4800, 9600, 19200, 38400 */
   maxcodes=1;

   strupr(argv[1]);
   for (ch=0; ch < strlen(argv[1]); ch++) 
      if (argv[1][ch]=='X') maxcodes *= 10;
      
   ch=0; again=0;
   mesposx=1; statposx=1; modemposx=1; foundposx=1;
   mesposy=1; statposy=1; modemposy=1; foundposy=1;

   for (index=0; index<10000; oldones[index++]=0);

   strcpy(filename,argv[1]);
   filename[8]=0;             /* no more than 8 chars */
   strcat(filename,".dat");
   if (datafile=fopen(filename,"rb")) {
      fread(&tones,sizeof(tones),1,datafile);
      fread(&rings,sizeof(rings),1,datafile);
      fread(&busys,sizeof(busys),1,datafile);
      fread(&voices,sizeof(voices),1,datafile);
      fread(&tried,sizeof(tried),1,datafile);
      fread(&oldones,sizeof(oldones),1,datafile);
      fclose(datafile);
   }

   if (initfoss(baudrate)==-1)
   {
      textattr(12);
      cprintf("No FOSSIL driver found!  Load X00 or BNU or something!\n\n\r");
      fcloseall();
      helpscreen();
   }

   drawscreen();
   logfile=fopen("tone.log","at");

   log("¯\n");
   log("ToneLoc started on %s\n",__DATE__);
   log("Mask used: %s\n",argv[1]);

   strcpy(dialstring,argv[1]);
         
   initmodem("ATS11=40E1");
   modem(clearbuffer(800));
   gettime(&start);
   window(1,1,80,25);

   while ((ch != 27) && (tried != maxcodes))
   {
      if (!again) strcpy(num,process(dialstring));
      again=0;
      gotoxy(49,22);
      cprintf("°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°");    
      updatestats();

      log("%s ... ",num);

      dial(num);       
      strcpy(response,getresponse(15000));

      doresponse(response,&again, num);
      modem(response);

      if (kbhit()) switch(ch=getch()) {
         case  27 : log("!Aborted\n"); ungetch(27); break;
         case 'N' : log("!* Noted *\n");  break;
         case 'n' : log("!* Noted *\n");  break;
         case 'j' : shell();  again=1;    break;
         case 'J' : shell();  again=1;    break;
         default  : log("!Aborted\n");    break;                     
      } else if (!strlen(response)) log("!Timeout\n");


      /* if (!strlen(response)) log("!Timeout\n"); */

      charout(13);
      delay(300);
      mprintf("ATH0%c",13);
      strcpy(response,clearbuffer(800));
      modem(response);

      delay(DELAYTIME);
      if (kbhit()) ch=getch();

      if (argc>3) delay(atoi(argv[3]));
   }

   if (tried==maxcodes) log("All %d codes exhausted\n",maxcodes);
   quit(filename);
}


void dial(char * number)
{
   char string[50];

   mprintf("ATDT %s W;%c",number,13);
   strcpy(string,clearbuffer(500));
   if (string != "") modem(string);
   modem("\n");
}

void helpscreen(void)
{
   textattr(14);
   if (initfoss(2400) != -1) clrscr();
   cprintf("\n\rToneLoc %s by Minor Threat\n\n\r",VERSION);
   textattr(3);
   cprintf("ToneLoc is a tone locator.  It will dial phone numbers using a mask\n\r"
           "that you give it.  You must have a FOSSIL driver loaded also.  ToneLoc\n\r"
           "is useful for finding: PBX's, Loops, LD carriers, & anything that gives\n\r"
           "a constant tone upon answering.  It works with the USR HST/DS.\n\n\r"

           "Usage:\n\r"
           "ToneLoc [Mask] [Port]\n\n\r"
           "   [Mask] - To use for phone numbers - (474-XX99)\n\r"
           "   [Port] - COM port to use (1,2,3, etc)\n\n\r"

           "ToneLoc is written in Turbo C++ using only the Turbo C libraries.\n\r");

   exit(0);

            

}

void charout(char character)
{
   union REGS regs;

   delay(10);
   regs.h.al=character;
   regs.x.dx=port;
   regs.h.ah=0x01;
   int86(FOS,&regs,&regs);
}

void mputs(char * str)
{
   int i;
   for (i=0; i<strlen(str); charout(str[i++]));
}

int mprintf(char *format, ...)
{
   char string[128];
   int cnt;
   va_list argptr;

   va_start(argptr, format);
   cnt=vsprintf(string, format, argptr);
   va_end(argptr);

   mputs(string);
   delay(DELAYTIME);
   return(cnt);
}

int initfoss(int baud)
{
   union REGS regs;

   regs.h.ah=0x04;
   regs.x.dx=port;
   regs.h.al=0;

   switch (baud)
   {
      case   300: regs.h.al +=  67; break;
      case  1200: regs.h.al +=  35; break;
      case  2400: regs.h.al += 163; break;
      case  4800: regs.h.al +=  99; break;
      case  9600: regs.h.al += 227; break;
      case 38400: regs.h.al += 131; break;
   }     

   int86(FOS,&regs,&regs);

   if (regs.x.ax==0x1954) return(0);
   else
      return(-1);          /* Error with FOSSIL */
}

void deinit(void)
{
   union REGS regs;
   regs.h.ah=0x05;
   regs.x.dx=port;
   int86(FOS,&regs,&regs);
}
 
char charinn(void)    /* returns char in buffer, or 0 if none */
{
   if (comhit()) return(getkey()); else return(0);
}

int comhit(void)
{
   union REGS regs;

   regs.h.ah=0x0C;
   regs.x.dx=port;
   int86(FOS,&regs,&regs);

   if ((regs.x.ax==0xFFFF) || (regs.h.al==0)) return(0);
   return(regs.h.al);
}

char getkey(void)    /* waits for keypress from comport */
{
   union REGS regs;

   do
      {
         regs.h.ah=0x02;
         regs.x.dx=port;
         int86(FOS,&regs,&regs);
      } while(regs.h.al==0);
   return(regs.h.al);
}

char * clearbuffer(word milliseconds)
                                    /* waits seconds seconds for input */
{                                   /* if no input after X seconds, */
   word x=0;                        /* returns NULL, else returns */
   char string[51];                 /* string returned. */
   byte percent;

   for (x=0; x<51; string[x++]=0);   /* zero out string */

   do {
      delay(100);
      x += 100;  
   } while ( (x<milliseconds) and (!comhit()) );

   if (!comhit()) return(NULL);

   x=0;
   while (comhit()) {
      string[x++]=getkey();

   }

   return(string);

}

char * getresponse(word milliseconds) 
                                    /* waits seconds seconds for input */
{                                   /* if no input after X seconds, */
   word x=0;                        /* returns NULL, else returns */
   char string[50];                 /* string returned. */

   for (x=0; x<50; string[x++]=0);  /* zero out string */

   x=0;

   do {
      meter(x,milliseconds);

      if (kbhit()) x=milliseconds;
      delay(95);
      updateclock();

      x += 100;

   } while ( (x<=milliseconds) and (!comhit()) );

   if (!comhit()) return(NULL);

   x=0;
   mgets(string); /* clear out the crap */
   if (comhit()=='\n') getkey(); /* more crap */
   mgets(string);

   return(string);

}

char * mgets(char *strng)
{
   char ch;
   int cnt=0;  

   do {               
      ch=getkey();
      strng[cnt++]=ch;
   } while (ch != '\r');

   strng[cnt]=0;
   return(strng);
}

void quit(char * filename)
{
   log("ToneLoc exiting ...\n");

   mprintf("ATH0%c",13);
   clearbuffer(DELAYTIME);

   deinit();

   fflush(logfile);
   
   datafile=fopen(filename,"wb");
      fwrite(&tones,sizeof(tones),1,datafile);
      fwrite(&rings,sizeof(rings),1,datafile);
      fwrite(&busys,sizeof(busys),1,datafile);
      fwrite(&voices,sizeof(voices),1,datafile);
      fwrite(&tried,sizeof(tried),1,datafile);
      fwrite(&oldones,sizeof(oldones),1,datafile);
      fflush(datafile);
         
   fcloseall();

   window(1,1,80,25);
   gotoxy(1,24);
   exit(0);   
}

int initmodem(char * initstring)
{
   char string[50];

   charout(13);
   clearbuffer(DELAYTIME);

   mprintf("ATZ%c",13);
   delay(900);


   strcpy(string,clearbuffer(2000));
   if (!string) return(-1);
      else modem("%s",string);

   mprintf("%s%c",initstring,13);

   do {
      strcpy(string,clearbuffer(500));
      if (strlen(string)) modem("%s",string);
   } while (strlen(string));

   return(0);
}

int rnd(word maxvalue)
{
   char string[12];
   word randomnumber;
   word x, y, numbertried;  

   randomnumber=(rand() % maxvalue);
   for (y=0; y<tried; y++)
   {
      if (randomnumber==oldones[y]) return(NULL);
   }

   oldones[tried]=randomnumber;
   numbertried++;
   return(randomnumber);
}

int parsecommandline(char * commandline, int argc, char *argv[])
{
   byte x;
   for (x=1; x<argc; x++) 
   {
      switch (argv[x][1])
      {
      }
   }
   return(0);
}

char * process(char * mask) /* replaces X's with random #'s */
{
   byte pos, xpos;
   char aftermask[50];
   char xstring[10];

   strupr(mask);        /* convert to uppercase */

   do {
      for (pos=0, xpos=0; pos<strlen(mask); pos++) {
         if (mask[pos]=='X') {
            aftermask[pos]=((char) random(10)+48);
            xstring[xpos++]=aftermask[pos];
         } else aftermask[pos]=mask[pos];
      } 
      aftermask[pos]=NULL;
      xstring[xpos]=NULL;
   } while (!checkdupe(xstring));
   
   return(aftermask);
}

checkdupe(char * string)
{
   word wordvar;
   int x;

   wordvar=atoi(string);
   if (oldones[wordvar]==1) return(0); /* number already dialed */

   oldones[wordvar]=1;
   tried++;
   return(1);
}

void drawscreen(void)
{
   char copyright[70];
   clrscr();
   box( 1, 1,45,23,12,"´ Status Log Ã");
   box(47,10,80,23,15,"´ Activity Window Ã");
   box(47, 1,80, 9, 9,"´ Modem Ã");
   gettime(&end);
   stats("Started: %02d:%02d:%02d\n\r",end.ti_hour, end.ti_min, end.ti_sec);
   stats("Current: %02d:%02d:%02d\n\r",end.ti_hour, end.ti_min, end.ti_sec);
   stats("Possible Codes:  %5d\n\r",maxcodes);
   stats("Exhausted Codes:     0\n\n\r");
   stats("Tones :     0\n\r");
   stats("Voice :     0\n\r");
   stats("Busy  :     0\n\r");
   stats("Ring  :     0\n\r");
   stats("Try # :     0");

   hline(47,80,15,15);
   hline(47,80,21,15);
   vline(15,21,63,15);
   gotoxy(67,15);
   textattr(15);
   cprintf("´ Found Ã");
   /*
   window(1,1,80,25);
   gotoxy(49,22);
   textattr(11);
   cprintf("°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°");
   */
   textattr(14);
   sprintf(copyright,"ToneLocator %s (%s) by Minor Threat",VERSION,__DATE__);
   gotoxy(40-( (strlen(copyright) )/2),24);
   cprintf(copyright);
   textattr(11);
}

void box(int left, int top, int right, int bottom, int attribute, char * title) 
{
   int x;
   int length;
   byte character;

   textattr(attribute);
   window(left,top,right,bottom);
   clrscr();
   window(1,1,80,25);

   gotoxy(left,top);
   cprintf("Ú");
   for (x=left; x < right; x++)    
      cprintf("Ä");

   gotoxy(right,top);
   cprintf("¿");
   for (x=top+1; x < bottom; x++) {
      gotoxy(right,x);
      cprintf("³");
   }

   gotoxy(right,bottom);
   cprintf("Ù");
   for (x=right-1; x > left; x--) {
      gotoxy(x,bottom);
      cprintf("Ä");
   }

   gotoxy(left,bottom);
   cprintf("À");
   for (x=bottom-1; x > top; x--) {
      gotoxy(left,x);
      cprintf("³");
   }

   x=(right-left+1) / 2;
   length=strlen(title);
   x = x-(length/2);

   gotoxy(left+x,top);
   cprintf(title);
}

void message(char * format, ...)
{
   char string[45];
   va_list argptr;

   window(3,2,44,22);
   gotoxy(mesposx,mesposy);

   va_start(argptr, format);
   vsprintf(string, format, argptr);
   va_end(argptr);

   cprintf("%s",string);
   if (string[strlen(string)-1]=='\n') cprintf("\r");

   mesposx=wherex();
   mesposy=wherey();
   window(1,1,80,25);
}

void stats(char * format, ...)
{
   char string[45];
   va_list argptr;
   
   window(49,11,78,22);
   gotoxy(statposx,statposy);

   va_start(argptr, format);
   vsprintf(string, format, argptr);
   va_end(argptr);

   cputs(string);

   statposx=wherex();
   statposy=wherey();
   window(1,1,80,25);
}

void modem(char * format, ...)
{ 
   char string[45];
   va_list argptr;

   window(49, 2,78, 8);
   gotoxy(modemposx,modemposy);

   va_start(argptr, format);
   vsprintf(string, format, argptr);
   va_end(argptr);

   if (strlen(string)) cputs(string);

   modemposx=wherex();
   modemposy=wherey();
   window(1,1,80,25);
}

void found(char * string)
{   
   window(65,16,79,20);
   gotoxy(foundposx,foundposy);
   cprintf("%s\n\r",string);
   foundposx=wherex();
   foundposy=wherey();
   window(1,1,80,25);
}

void log(char * format, ...) 
{
   struct time now;
   va_list argptr;
   char string[50];

   va_start(argptr, format);
   vsprintf(string, format, argptr);
   va_end(argptr);

   gettime(&now);
   if (string[0] != '!') {   
      fprintf(logfile,"%02d:%02d:%02d ", now.ti_hour, now.ti_min, now.ti_sec);
      message("%02d:%02d:%02d ",now.ti_hour, now.ti_min, now.ti_sec);
   } else strcpy(&string[0],&string[1]);

   fprintf(logfile,"%s", string); 
   message("%s", string);
   fflush(logfile);
}

void hline(int left, int right, int row, int attribute)
{
   byte ch;
   char buffer[2];

   gotoxy(left,row);
   textattr(attribute);
   gettext(left,row,left,row,buffer);


   switch (buffer[0]) {
      case '³' : putch('Ã'); break;
      case '´' : putch('Å'); break;
      case '¿' : putch('Â'); break;
      case 'Ù' : putch('Á'); break;
   }

   gotoxy(right,row);
   gettext(right,row,right,row,buffer);

   switch(buffer[0]) {
      case '³' : putch('´'); break;
      case 'Ã' : putch('Å'); break;
      case 'Ú' : putch('Â'); break;
      case 'À' : putch('Á'); break;
   }

   gotoxy(left+1,row);
   for (ch=left+1; ch<right; ch++) 
      putch('Ä');
}

void vline(int top, int bottom, int column, int attribute)
{
   byte ch;
   char buffer[2];

   gotoxy(column,top);
   textattr(attribute);
   gettext(column,top,column,top,buffer);


   switch (buffer[0]) {
      case 'Ä' : putch('Â'); break;
      case 'Á' : putch('Å'); break;
      case 'Ù' : putch('´'); break;
      case 'À' : putch('Ã'); break;
   }

   gotoxy(column,bottom);
   gettext(column,bottom,column,bottom,buffer);

   switch(buffer[0]) {
      case 'Ä' : putch('Á'); break;
      case 'Â' : putch('Å'); break;
      case 'Ú' : putch('Ã'); break;
      case '¿' : putch('´'); break;
   }

   gotoxy(column,top+1);
   for (ch=top+1; ch<bottom; ch++) {
      gotoxy(column,ch);
      putch('³');
   }
}

void meter(word ms, word total)
{
   word percent;

   window(49,22,79,22);
   percent=ms / (total / METERLENGTH);
               
   gotoxy(percent,1);
   putch('²');
}

void dvdelay(word milliseconds)
{
   word slice;
   for (slice=0; slice<milliseconds; slice++) {
      timeslice();
      delay(1);
      slice++;
   }
}

void timeslice(void)
{
   union REGS regs;
   if (DESQview) {
      regs.x.ax=0x101A;
      regs.x.bx=0x1000;
      int86(DV,&regs,&regs);
      regs.x.ax=regs.x.bx;
      int86(DV,&regs,&regs);
      regs.x.ax=0x1025;
      int86(DV,&regs,&regs);
   }
}

void updatestats(void)
{
   gotoxy(58,12);
   gettime(&end);
   cprintf("%02d:%02d:%02d",end.ti_hour,end.ti_min,end.ti_sec);
   gotoxy(66,14);
   cprintf("%5d",tried-1);
   gotoxy(57,16);
   cprintf("%5d",tones);
   gotoxy(57,17);
   cprintf("%5d",voices);
   gotoxy(57,18);
   cprintf("%5d",busys);
   gotoxy(57,19);
   cprintf("%5d",rings);
   gotoxy(57,20);
   cprintf("%5d",tried);
}

void doresponse(char * response, int * againptr, char * num)
{
   if (strstr(response,"OK")) { 
      log("!*** TONE FOUND ***\n"); 
      found(num); 
      tones++;
   }

   if (strstr(response,"RINGING")) {
      log("!Ringing\n");
      rings++;
   }

   if (strstr(response,"BUSY")) {
      log("!Busy\n");
      busys++;
   }

   if (strstr(response,"VOICE")) {
      log("!Voice\n");
      voices++;
   }

   if (strstr(response,"NO DIAL")) {
      log("!No Fucking Dialtone\n");
      *againptr=1;
   }

   modem(response);
}

void shell(void)
{
   char buffer[4096];

   charout(13);
   delay(DELAYTIME);
   mprintf("ATH0%c",13);

   log("!Jumped to DOS\n");
   fflush(logfile);

   gettext(1,1,80,25,buffer);
   gotoxy(1,24);
   clreol();
   printf("%dK available to DOS ...\n",memavail());
   gotoxy(1,23);
   system("");

   clrscr();
   puttext(1,1,80,25,buffer);
   log("Back from DOS\n");
}

int memavail(void)
{
   unsigned segptr;
   int stat;

   stat = allocmem(65535, &segptr);

   /* (65535 x 16) = 1048560 bytes (1 Meg - 16 bytes) */

   return(stat/64);
}

void updateclock(void)
{
   struct text_info ti;

   gettextinfo(&ti);

   window(1,1,80,25);
   gettime(&end);
   gotoxy(58,12);
   cprintf("%02d:%02d:%02d",end.ti_hour,end.ti_min,end.ti_sec);
   window(ti.winleft,ti.wintop,ti.winright,ti.winbottom);
   gotoxy(ti.curx,ti.cury);
}
