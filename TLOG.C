/*
 * TLog.C - ToneLoc .DAT -> .LOG file generator
 * by Minor Threat & Mucho Maas
 */

#include <stdlib.h>
#include <stdio.h>
#include "ToneLoc.h"

void   helpscreen(void);                /* helpscreen */
void   badparms(void);                 /* command line help screen */
void   mainloop(void);
void   ParseCmdLine(int, char *[]);

byte   rcount=0;
byte   responses[15];
byte   oldones[10000];
struct _scan scan;
FILE   *datfile, *logfile;
char   datfilename[13], logfilename[13], string[25];
char   nxx[40];

void main(int argc, char * argv[])
{

   puts("TLog;  Creates log file from ToneLoc Data file      V1.0");
   puts("       by Minor Threat and Mucho Maas\n");

   if (strchr(argv[1],'?')) helpscreen();
   if (argc < 4) badparms();

   printf("\nPrefix (NXX) : ");
   gets(nxx);

   ParseCmdLine(argc, argv);
   mainloop();

}

void mainloop(void)
{
   register int x;
   int y;
   int RingCount;
   register int z;

   if ((datfile=fopen(datfilename,"rb")) == NULL) {
     printf("\a\nError opening %s!\n",datfilename);
     exit(1);
   }

   fread(&scan,sizeof(scan),1,datfile);
   fread(&oldones,sizeof(oldones),1,datfile);
   fclose(datfile);

   logfile=fopen(logfilename,"wt");

   for (x=0; x<10000; x++) {
      y=fix(oldones[x]);
      if (y != 0)
         y = ((y/10) * 10);             /* filter RingCount */
      for (z=0; z<rcount; z++)
         if ((responses[z] == y) || (responses[z] == 230) || ((responses[z] == 120) && (y != 0)))
         {
            fprintf(logfile,"%s-%04d - ",nxx,x);
            RingCount = oldones[x]-y;              /* RingCount */
            switch (y) {
               case   0 : fprintf(logfile,"Undialed\n");  break;
               case  10 : fprintf(logfile,"Busy\n",RingCount); break;
               case  20 : fprintf(logfile,"Voice   (%d)\n",RingCount); break;
               case  40 : switch (oldones[x]) {
                  case 40 : fprintf(logfile,"Noted\n");           break;
                  case 41 : fprintf(logfile,"Fax\n");             break;
                  case 42 : fprintf(logfile,"Girl\n");            break;
                  case 43 : fprintf(logfile,"VMB\n");             break;
                  case 44 : fprintf(logfile,"Yelling Asshole\n"); break;
                  default : fprintf(logfile,"* Noted *\n");       break;
               } break;
               case  50 : fprintf(logfile,"Aborted (%d)\n",RingCount); break;
               case  60 : fprintf(logfile,"Ringout (%d)\n",RingCount); break;
               case  70 : fprintf(logfile,"Timeout (%d)\n",RingCount); break;
               case  80 : fprintf(logfile,"Tone (%d)\n",RingCount); break;
               case  90 : fprintf(logfile,"Carrier (%d)\n",RingCount); break;
               case 130 : fprintf(logfile,"* Blacklist *\n"); break;
            }
         }
      }
   fclose(logfile);
}

int fix(register int num)
{
   while (num % 10)
      num--;
   return(num);
}

void ParseCmdLine(int argc, char *argv[])
{
   register int x;

   strcpy(datfilename,argv[1]);
   datfilename[8]=0;
   strcat(datfilename,".DAT");
   strupr(datfilename);
   printf("Using Data File: %s\n",datfilename);

   strcpy(logfilename,argv[argc-1]);
   strupr(logfilename);
   printf("Writing to file: %s\n",logfilename);

   rcount=0;

   for (x=2; x<argc-1; x++) {
      strupr(argv[x]);
      responses[rcount] = 0xFF;
      if (strstr("UNDIALED",argv[x]))  responses[rcount]=00;
      if (strstr("BUSY",argv[x]))      responses[rcount]=10;
      if (strstr("VOICE",argv[x]))     responses[rcount]=20;
      if (strstr("NOTE",argv[x]))      responses[rcount]=40;
      if (strstr("ABORT",argv[x]))     responses[rcount]=50;
      if (strstr("RING",argv[x]))      responses[rcount]=60;
      if (strstr("TIME",argv[x]))      responses[rcount]=70;
      if (strstr("TONE",argv[x]))      responses[rcount]=80;
      if (strstr("CARRIER",argv[x]))   responses[rcount]=90;
      if (strstr("BLACK",argv[x]))     responses[rcount]=130;
      if (strstr("DIALED",argv[x]))    responses[rcount]=120;
      if (strstr("ALL",argv[x]))       responses[rcount]=230;
      if (responses[rcount] == 0xFF)
         printf("\aResponse: '%s' not understood!\n",argv[x]);
      else
         rcount++;
   }
}

void badparms(void)
{
  puts("Usage:  TLOG  [DataFile]  [Responses ...]  [OutFile]");
  puts("        TLOG ?   = more help");
  exit(0);
}


void helpscreen(void)
{
   puts("Usage:  TLOG  [DataFile]  [Responses ...]  [OutFile]\n");

   puts("   [DataFile]       - ToneLoc .Datafile to read (*.DAT)");
   puts("   [Responses ...]  - Which responses to write to [OutFile]");
   puts("   [OutFile]        - File to output text to\n");

   puts(" Responses are:");
   puts("   UNDIALED  - Undialed numbers");
   puts("   BUSY      - Busy numbers");
   puts("   VOICE     - Voice answers");
   puts("   NOTE      - Noted numbers");
   puts("   ABORT     - Aborted numbers");
   puts("   RING      - Ringout numbers");
   puts("   TIME      - Timeout numbers");
   puts("   TONE      - Tone answers");
   puts("   CARRIER   - Carrier answers");
   puts("   BLACK     - Blacklisted numbers");
   puts("   DIALED    - All numbers except undialed");
   puts("   ALL       - All numbers");

   exit(0);
}
