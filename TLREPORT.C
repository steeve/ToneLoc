/*
 * TLReport.C - Statistics Report Generator for ToneLoc
 * by Minor Threat and Mucho Maas
 */

#include <stdlib.h>
#include <stdio.h>
#include <dos.h>
#include "ToneLoc.H"
#define  round(x)  ((x/10)*10)

   void helpscreen(void);                  /* help screen */

void main(int argc, char *argv[])
{
   FILE *datfile;
   char datfilename[13];
   struct _scan scan;
   int tones=0, busys=0, voices=0,
       rings=0, carriers=0, tried=0,
       noted=0, aborted=0, timeout=0;

   byte oldones[10000];
   register int x;
   int alldialed;

   printf("\nTLReport;  Reports status of a ToneLoc data file\n"
          "           by Minor Threat\n\n");

   alldialed = 0;

   if (argc==1) helpscreen();

   strcpy(datfilename,argv[1]);
   if (!strchr(datfilename,'.'))
   {
      datfilename[8]=0;
      strcat(datfilename,".DAT");
   }
   datfile=fopen(datfilename,"rb");
   if (!datfile)
   {
      printf("\aError opening %s\n",datfilename);
      exit(1);
   }

   fread(&scan,sizeof(scan),1,datfile);
   fread(&oldones,sizeof(oldones),1,datfile);
   fclose(datfile);

   for (x=0; x<10000; x++)
   {
      if ((oldones[x]) && (oldones[x] != 100))
         tried++;
      switch (round(oldones[x]))
      {
         case 10 : busys++;           break;
         case 20 : voices++;          break;
         case 40 : noted++;           break;
         case 50 : aborted++;         break;
         case 60 : rings++;           break;
         case 70 : timeout++;         break;
         case 80 : tones++;           break;
         case 90 : carriers++;        break;
         case 130: /*blacklisted++;*/ break;
      }
   }

   alldialed = tried;

   printf("Report for %s: (v%x.%02x)\n\n",datfilename,scan.VersionID>>8,scan.VersionID&0x00FF);
   printf("                  Absolute   Relative\n");
   printf("                   Percent    Percent\n");
   printf("Dialed    =%5d  (%5.2f%%)\n", tried, (float) tried/100);
   printf("Busy      =%5d  (%5.2f%%)   (%5.2f%%)\n",busys,(float) busys/100,     100*(float) busys/alldialed);
   printf("Voice     =%5d  (%5.2f%%)   (%5.2f%%)\n",voices,(float) voices/100,   100*(float) voices/alldialed);
   printf("Noted     =%5d  (%5.2f%%)   (%5.2f%%)\n",noted,(float) noted/100,     100*(float) noted/alldialed);
   printf("Aborted   =%5d  (%5.2f%%)   (%5.2f%%)\n",aborted,(float) aborted/100, 100*(float) aborted/alldialed);
   printf("Ringout   =%5d  (%5.2f%%)   (%5.2f%%)\n",rings,(float) rings/100,     100*(float) rings/alldialed);
   printf("Timeout   =%5d  (%5.2f%%)   (%5.2f%%)\n",timeout,(float) timeout/100, 100*(float) timeout/alldialed);
   printf("Tones     =%5d  (%5.2f%%)   (%5.2f%%)\n",tones,(float) tones/100,     100*(float) tones/alldialed);
   printf("Carriers  =%5d  (%5.2f%%)   (%5.2f%%)\n\n",carriers,(float) carriers/100, 100*(float) carriers/alldialed);
   printf("Scan is %1d%% complete.\n",(tried/100));
   printf("%d:%02d spent on scan so far.\n",scan.Minutes/60,scan.Minutes%60);
   exit(0);
}

void helpscreen(void)
{
   printf("Usage: TLREPORT {Filename}\n");
   exit(0);
}
