/*
** Tlsumm.C - summary program
** by Minor Threat 1994`
*/

#include <stdio.h>
#include <dos.h>
#include <dir.h>
#include "toneloc.h"

struct _total {
   long  tones,
         carriers,
         rings,
         busys,
         voices,
         tried,
         timeouts,
         minutes;
};

void main(int argc, char *argv[])
{
   FILE *f;
   char filename[13];
   struct ffblk fblk;
   struct _scan scan;
//   struct _scan95 check;
   struct _total check;
   struct _total total;
   byte old[10000];
   register int x, round;
   int done, minutes;
   long t=0, m=0;

   printf("TLSUMM;  Summarizes a group of datafiles\n"
          "         by Minor Threat 1994\n\n");

   if (argc==1)
   {
      printf("USAGE: TLSUMM {DataFile}\n"
             "Wildcards accepted.  'TLSUMM *' does *.DAT\n");
      exit(1);
   }

   strcpy(filename,argv[1]);
   if (!strchr(filename,'.'))
   {
      filename[8]=0;
      strcat(filename,".DAT");
   }

   printf("Summarizing %s ...\n\n",filename);

   printf("filename.dat:  tried  rings  voice  busys  carrs  tones  timeouts   spent\n");
   printf("ÄÄÄÄÄÄÄÄÄÄÄÄÄ  ÄÄÄÄÄ  ÄÄÄÄÄ  ÄÄÄÄÄ  ÄÄÄÄÄ  ÄÄÄÄÄ  ÄÄÄÄÄ  ÄÄÄÄÄÄÄÄ   ÄÄÄÄÄ\n");

   memset(&total,0,sizeof(total));
   done = findfirst(filename,&fblk,0);

   while (!done)
   {
      memset(&check,0,sizeof(check));
      minutes=0;

      printf("%-12s: ",fblk.ff_name);
      if (fblk.ff_fsize != (sizeof(scan)+sizeof(old)))
      {
         printf("\aERROR - wrong filesize, try TCONVERT\n");
         goto nextfile;               /* next WHILE loop */
      }
      f=fopen(fblk.ff_name,"rb");
      if (!f) {
         printf("\aERROR opening!\n");
         goto nextfile;               /* next WHILE loop */
      }
      fread(&scan,sizeof(scan),1,f);
      fread(&old,sizeof(old),1,f);
      fclose(f);

      t++;                  // total datafiles
      minutes = scan.Minutes;
      m += minutes;


      for (x=0; x < 10000; x++)
      {
         if ((old[x] != 0) && (old[x] != 100)) {
            check.tried++;
            total.tried++;
         }
         round = (old[x]/10)*10;
         switch (round)
         {
            case 10 : check.busys++;    total.busys++;    break;
            case 20 : check.voices++;   total.voices++;   break;
            case 60 : check.rings++;    total.rings++;    break;
            case 70 : check.timeouts++; total.timeouts++; break;
            case 80 : check.tones++;    total.tones++;    break;
            case 90 : check.carriers++; total.carriers++; break;
         }
      }

      printf(" %5ld  %5ld  %5ld  %5ld  %5ld  %5ld     %5ld   %2d:%02d\n",check.tried,
         check.rings,check.voices,check.busys,check.carriers,check.tones,check.timeouts,
         minutes/60,minutes%60);

      nextfile:
      done = findnext(&fblk);
   }
   printf("ÄÄÄÄÄÄÄÄÄÄÄÄÄ  ÄÄÄÄÄ  ÄÄÄÄÄ  ÄÄÄÄÄ  ÄÄÄÄÄ  ÄÄÄÄÄ  ÄÄÄÄÄ  ÄÄÄÄÄÄÄÄ   ÄÄÄÄÄ\n");
   printf("Totals:       %6ld %6ld %6ld %6ld %6ld %6ld    %6ld %4ld:%02ld\n",
      total.tried,total.rings,total.voices,total.busys,total.carriers,
      total.tones,total.timeouts,m/60L,m%60L);
   printf("ÄÄÄÄÄÄÄÄÄÄÄÄÄ  ÄÄÄÄÄ  ÄÄÄÄÄ  ÄÄÄÄÄ  ÄÄÄÄÄ  ÄÄÄÄÄ  ÄÄÄÄÄ  ÄÄÄÄÄÄÄÄ   ÄÄÄÄÄ\n");
   printf("Averages:      %5ld  %5ld  %5ld  %5ld  %5ld  %5ld     %5ld  %3ld:%02ld\n",
      total.tried/t,total.rings/t,total.voices/t,total.busys/t,
      total.carriers/t,total.tones/t,total.timeouts/t,(m/60L)/t,(m%60L)/t);
   printf("ÄÄÄÄÄÄÄÄÄÄÄÄÄ  ÄÄÄÄÄ  ÄÄÄÄÄ  ÄÄÄÄÄ  ÄÄÄÄÄ  ÄÄÄÄÄ  ÄÄÄÄÄ  ÄÄÄÄÄÄÄÄ   ÄÄÄÄÄ\n");
   printf("%-3d DatFiles   tried  rings  voice  busys  carrs  tones  timeouts   spent\n",t);

   // add percentages:   busys / tried * 100 get it?

   exit(0);
}

