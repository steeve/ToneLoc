/*
 * tlreplace.c - find/replace responses
 * (c) 1993 by Minor Threat & Mucho Maas
 */


/*
 * Key:
 *
 *  (x = number of rings, more than 9 recorded as 9.)
 *
 *  00 = Undialed
 *  1x = Busy
 *  2x = Voice
 *  3x = No Dialtone
 *  40 = Noted
 *   41 = Fax
 *   42 = Girl
 *   43 = VMB
 *   44 = Yelling Asshole
 *  5x = Aborted
 *  6x = Ringout
 *  7x = Timeout
 *  8x = Tone
 *  9x = Carrier
 * 10x = Excluded
 * 11x = Omitted
 * 12x = Dialed
 * 13x = Blacklisted
 *
 */


#include <stdlib.h>
#include <stdio.h>
#include <String.h>
//#include <dos.h>
#include <process.h>
#include "ToneLoc.h"

void   helpscreen(void);                /* helpscreen */
void   badparms(void);                 /* command line help screen */
int    fix(register int num);           // rounds 10
void   mainloop(void);
void   ParseCmdLine(int, char *[]);

byte   rcount=0;
byte   responses[15];      // Array of responses to be converted
byte   exacts[15];         // Parallel array of exact flags
byte   respfinal;          // response they will be converted to
byte   exactfinal;         // exact flag for final response
byte   oldones[10000];
struct _scan scan;
FILE   *datfile;
char   datfilename[13], string[25];

void main(int argc, char * argv[])
{

   puts("TLReplace;  Replace ToneLoc .DAT tone responses with something else");
   puts("            by Minor Threat and Mucho Maas, Version 1.0\n");

   if (strchr(argv[1],'?')) helpscreen();
   if (argc < 4) badparms();

   ParseCmdLine(argc, argv);
   mainloop();

}

void mainloop(void)
{
   register int x;
   int y;
   int changecounter;
   register int z;

   if ((datfile=fopen(datfilename,"rb")) == NULL)
   {
      printf("\a\nError opening %s!\n",datfilename);
      exit(1);
   }

   fread(&scan,sizeof(scan),1,datfile);
   fread(&oldones,sizeof(oldones),1,datfile);
   fclose(datfile);

   changecounter=0;

   for (x=0; x<10000; x++)
   {
      y=fix(oldones[x]);
      for (z=0; z<rcount; z++)
      {

        if (exacts[z] == 0)  // Not looking for an exact match
          {
            if ((responses[z] == y) || (responses[z] == 230) || ((responses[z] == 120) && (y != 0)))
             {
              oldones[x]=respfinal;
              changecounter++;
             }
          }
        else                 // Looking for an exact match
          {
            if ((responses[z] == oldones[x]) || (responses[z] == 230) || ((responses[z] == 120) && (y !=0)))
             {
              oldones[x]=respfinal;
              changecounter++;
             }
          }
      }
   }
   datfile=fopen(datfilename,"wb");
   fwrite(&scan,sizeof(scan),1,datfile);
   fwrite(&oldones,sizeof(oldones),1,datfile);
   fclose(datfile);
   printf("%d responses were changed.\n\r",changecounter);
}

int fix(register int num)
{
   while (num % 10)
      num--;
   return(num);
}

void ParseCmdLine(int argc, char *argv[])
{
   register int x,y;
   char *p;

   strcpy(datfilename,argv[1]);
   p=(char *) strchr(datfilename,'.');
   if (p)
      *p=0;
   else
      datfilename[8]=0;
   strcat(datfilename,".DAT");
   strupr(datfilename);

   printf("Using Data File: %s\n",datfilename);

   rcount=0;

   for (x=2; x<argc; x++)
   {
      strupr(argv[x]);
      responses[rcount] = 0xFF;
      exacts[rcount] = 0x00;     // default = not exact
      if (strstr("UNDIALED",argv[x]))  responses[rcount]=0;
      if (strstr("BUSY",argv[x]))      responses[rcount]=10;
      if (strstr("VOICE",argv[x]))     responses[rcount]=20;
      if (strstr("NODIAL",argv[x]))    responses[rcount]=30;
      if (strstr("NOTE",argv[x]))      responses[rcount]=40;
      if (strstr("FAX",argv[x]))     {  responses[rcount]=41; exacts[rcount] = 1; }
      if (strstr("GIRL",argv[x]))    {  responses[rcount]=42; exacts[rcount] = 1; }
      if (strstr("VMB",argv[x]))     {  responses[rcount]=43; exacts[rcount] = 1; }
      if (strstr("YELL",argv[x]))    {  responses[rcount]=44; exacts[rcount] = 1; }
      if (strstr("MUCHO",argv[x]))   {  responses[rcount]=49; exacts[rcount] = 1; }
      if (strstr("ABORTED",argv[x]))   responses[rcount]=50;
      if (strstr("RINGOUT",argv[x]))   responses[rcount]=60;
      if (strstr("TIMEOUT",argv[x]))   responses[rcount]=70;
      if (strstr("TONE",argv[x]))      responses[rcount]=80;
      if (strstr("CARRIER",argv[x]))   responses[rcount]=90;
      if (strstr("EXCLUDE",argv[x]))   responses[rcount]=100;
      if (strstr("OMITTED",argv[x]))   responses[rcount]=110;
      if (strstr("DIALED",argv[x]))    responses[rcount]=120;
      if (strstr("BLACKLIST",argv[x])) responses[rcount]=130;
      if (strstr("ALL",argv[x]))       responses[rcount]=230;
      if ((atoi(argv[x]) < 255) && (atoi(argv[x]) > 0))  // exact number
         {
          responses[rcount] = atoi(argv[x]);
          exacts[rcount] = 1;
         }
      if (responses[rcount] == 0xFF)
         printf("\aResponse: '%s' not understood!\n",argv[x]);
      else
         rcount++;
   }
//   printf("rcount = %d\n",rcount);

   respfinal  = responses[rcount-1];
   exactfinal = exacts[rcount-1];
   rcount--;

   printf("\nMarking ");
      for (x=0; x<rcount; x++)
       {
       switch(fix(responses[x]))
        {
           case   0 : printf("UNDIALED"); break;
           case  10 : printf("BUSY");    break;
           case  20 : printf("VOICE");   break;
           case  30 : printf("NODIAL");  break;
           case  40 : printf("NOTE");    break;
           case  50 : printf("ABORTED"); break;
           case  60 : printf("RINGOUT"); break;
           case  70 : printf("TIMEOUT"); break;
           case  80 : printf("TONE");    break;
           case  90 : printf("CARRIER"); break;
           case 100 : printf("EXCLUDE"); break;
           case 110 : printf("OMITTED"); break;
           case 120 : printf("DIALED");  break;
           case 130 : printf("BLACKLIST"); break;
           case 230 : printf("ALL");     break;
        }
         if ((responses[x] < 41) || (responses[x] > 49)) // not one of the strange notes
          {
          if (exacts[x] == 1)
            printf("[%d] ",responses[x]-fix(responses[x]));
           else
            printf(" ");
          }
         else  // one of the strange notes
          {
           switch (responses[x])
            {
              case 41 : printf(": FAX ");   break;
              case 42 : printf(": GIRL ");  break;
              case 43 : printf(": VMB ");   break;
              case 44 : printf(": YELL ");  break;
              case 49 : printf(": MuCh0 "); break;
            }
          }
      }
  printf("responses as ");

         switch (fix(respfinal))
         {
            case   0 : printf("UNDIALED");    break;
            case  10 : printf("BUSY");        break;
            case  20 : printf("VOICE");       break;
            case  30 : printf("NODIAL");      break;
            case  40 : printf("NOTE");       break;
            case  50 : printf("ABORTED");     break;
            case  60 : printf("RINGOUT");     break;
            case  70 : printf("TIMEOUT");     break;
            case  80 : printf("TONE");        break;
            case  90 : printf("CARRIER");     break;
            case 100 : printf("EXCLUDE");     break;
            case 110 : printf("OMITTED");     break;
            case 120 : printf("DIALED");      break;
            case 130 : printf("BLACKLIST");   break;
            case 230 : printf("ALL");         break;
         }
         if ((responses[x] < 41) || (responses[x] > 49)) // not one of the strange notes
          {
          if (exactfinal == 1)
            printf("[%d].\n\r",respfinal-fix(respfinal));
          else
            printf(".\n\r");
          }
         else  // one of the strange notes
          {
           switch (responses[x])
            {
              case 41 : printf(": FAX.\n\r");   break;
              case 42 : printf(": GiRL.\n\r");  break;
              case 43 : printf(": VMB.\n\r");   break;
              case 44 : printf(": YELL.\n\r");  break;
              case 49 : printf(": MuCh0.\n\r"); break;
            }
          }
}

void badparms(void)
{
  puts("Usage:  TLREPLAC  [DataFile] [Old Response(s)] [New Response]\n");
  puts("        TLREPLAC /?  = more help");
  exit(0);
}


void helpscreen(void)
{
   puts("Usage:  TLREPLAC  [DataFile]  [Old Response] [New Response]\n");

   puts("   [DataFile]       - ToneLoc .DAT file to change");
   puts("   [Old Response]   - Response to replace");
   puts("   [New Response]   - What to replace it with\n");
   
   puts(" Valid Responses are:\n");
   puts("   UNDIALED  [00]  - Undialed numbers");
   puts("   BUSY      [1x]  - Busy numbers");
   puts("   VOICE     [2x]  - Voice numbers");
   puts("   NODIAL    [30]  - No Dialtone");
   puts("   NOTE      [40]  - Noted Numbers");
   puts("   FAX       [41]  - Fax Machine");
   puts("   GIRL      [42]  - Female");
   puts("   VMB       [43]  - Voicemail Box");
   puts("   YELL      [44]  - Yelling Asshole");
   puts("   ABORTED   [5x]  - Aborted numbers");
   puts("   RINGOUT   [6x]  - Ringout numbers");
   puts("   TIMEOUT   [7x]  - Timeout numbers");
   puts("   TONE      [8x]  - Tone answers");
   puts("   CARRIER   [9x]  - Carrier answers");
   puts("   EXCLUDE   [100] - Excluded numbers");
   puts("   OMITTED   [110] - Omitted numbers");
   puts("   DIALED    [120] - ALL numbers except undialed");
   puts("   BLACKLIST [130] - Blacklisted numbers");
   puts("   ALL       [230] - All numbers");
   puts("");
   puts("Where X is the number of rings");

exit(0);

}
