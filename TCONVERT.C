/*
 * ToneLoc .DAT Convert anything -> v1.00
 * 1993 (c) by Mucho Maas, mt, and friends
 */

#include <stdio.h>
#include <dos.h>
#include <dir.h>
#include <time.h>
#include "toneloc.h"

word count_tried(byte * old);                   // counts #'s tried

void main(int argc, char *argv[])
{
   char    filename[13];
   struct  ffblk fblk;
   struct  _scan90 scan90;
   struct  _scan95 scan95;
   struct  _scan scan;
   FILE    *f;
   register int x;
   int     done;
   char    *p;
   word    secs=0;
   byte    oldones[10000];

   printf("TCONVERT;  ToneLoc .DAT file conversion utility to 1.00 datafiles\n");
   printf("           by Mucho Maas and Minor Threat 1994\n\n");

   if (argc==1) {
      printf("USAGE: TCONVERT {DataFile}\n");
      printf("Wildcards accepted.  'TCONVERT *' will convert ALL DAT files\n");
      printf("                      Back up your .DAT files before proceeding!\n");
      exit(0);
   }

   strcpy(filename,argv[1]);
   p = (char *) strchr(filename,'.');
   if (!p)
      filename[8]=0;
   else
      *p = 0;
   strcat(filename,".DAT");

   if (argc > 2) {
      secs = (word)atoi(argv[2]);
      secs *= 60;
   }

   printf("Converting %s to 1.00 format ...\n\n",filename);

   done = findfirst(filename,&fblk,0);

   while (!done) {
      long fsize;
      int  version;

      printf("%-12s: ",fblk.ff_name);

      f=fopen(fblk.ff_name,"rb");
      if (!f) {
         printf("\`ERROR opening %s\n",fblk.ff_name);
         break;
      }

      fsize=fblk.ff_fsize;

      switch (fsize) {
         case 10010 : printf("0.90 "); version = 90;  break;
         case 10012 : printf("0.95 "); version = 95;  break;
         case 10016 :/* printf("0.98 "); */ version = 98;  break;
         default    : printf("Unknown "); version=0;  break;
      }

      switch (version) {
         case 90 : fread(&scan90,sizeof(scan90),1,f);
                   fread(&oldones,sizeof(oldones),1,f);
                   for (x=0; x < 10000; x++)
                      if (oldones[x])
                         oldones[x] = 40;   /* Dialed (generic) */
                   break;
         case 95 : fread(&scan95,sizeof(scan95),1,f);
                   fread(&oldones,sizeof(oldones),1,f);
                   break;
         case 98 : fread(&scan,sizeof(scan),1,f);
  		   switch ( scan.VersionID )
   		   {
		     case 0x0098 : printf("0.98 "); break;
		     case 0x0100 : printf("1.00 "); break;
		     default:      printf("Unknown (1.x?) ");
		   }
                   fread(&oldones,sizeof(oldones),1,f);
                   break;
         case 00 : /* read in last 10000 bytes */
                   //fseek(f,10000,SEEK_END);
                   //fread(&oldones,sizeof(oldones),1,f);
                   break;
      }

      fclose(f);
      scan.ProductCode[0] = 'T';
      scan.ProductCode[1] = 'L';
      scan.VersionID      = 0x0100;
      if ((version != 98) || (scan.Minutes==0))
         scan.Minutes     = (word) ((long) ((long) secs * (long) count_tried(oldones)) / 60L);
      memset(&scan.Extra,0,sizeof(scan.Extra));

      f=fopen(fblk.ff_name,"wb");
      if (!f) {
         printf("\aError writing to %s\n",fblk.ff_name);
         break;
      }

      printf("-> 1.00 ");
      fwrite(&scan,sizeof(scan),1,f);
      fwrite(&oldones,sizeof(oldones),1,f);
      fclose(f);

      printf("Ok\n");

      done = findnext(&fblk);
   }
}

word count_tried(byte * old)
{
   register int x,z=0;

   for (x=0; x<10000; x++) {
      if ((*(old+x) != 0) && (*(old+x) != 100))
         z++;
   }
   return(z);
}
