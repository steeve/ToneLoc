/*
** TMerge.C - Merges carrier & tone .DAT files
** by Mucho Maas 1992
*/

/*

 Mergemode:

 0 = Non-destructive; only overlays results into blank or excluded points.
 1 = Carriers have precedence, otherwise non-destructive.
 2 = Tones have precedence, otherwise non-destructive.
 3 = Merges ONLY carriers (with precedence)
 4 = Merges ONLY tones (with precedence)

*/

#include <stdio.h>
#include <dos.h>
#include <dir.h>
#include "toneloc.h"

void main(int argc, char *argv[])
{
   FILE *datfileone;
   FILE *datfiletwo;
   FILE *mergefile;
   char datfilename_one[13];
   char datfilename_two[13];
   char mergefilename[13];
   struct ffblk fblk;
   struct _scan scan[3];
   struct _scan check[3];
   byte old[3][10000];
   int x, round_one, round_two;
   int done, mergemode;


   printf("TMERGE;  Merges .DAT files for ToneLoc\n"
          "         by Mucho Maas 1992\n\n");

   if (argc < 3) {
      printf("USAGE: TLFIX {DataFile1} {DataFile2} {MergeFile} /C /T /OT /OC \n"
             "\n"
             "Will create MergeFile by merging Datafile1 on top of Datafile2.\n"
             "\n"
             "/C gives carriers precedence (default)\n"
             "/T give tones precedence.\n"
             "/O[ct] Merges ONLY the carriers or tones, no other results.\n");
      exit(0);
   }

   mergemode = 0 ;     /* clear it for now */
   strcpy(datfilename_one,argv[1]);
   strcpy(datfilename_two,argv[2]);
   strcpy(mergefilename,argv[3]);

   if (strcmp(mergefilename,NULL) == 0)
     strcpy(mergefilename,"MERGE.DAT");

   printf("Merging %s onto %s => %s\n",datfilename_one,datfilename_two,mergefilename);
     
   if ((datfileone=fopen(datfilename_one,"rb")) != NULL) {
      printf("Reading %s ...\n",datfilename_one);
      fread(&scan[0],sizeof(scan[0]),1,datfileone);
      fread(&old[0],sizeof(old[0]),1,datfileone);
   }
    else {
     printf("\nError reading %s, aborting...",datfilename_one);
     exit(1);
    }
     
   if ((datfiletwo=fopen(datfilename_two,"rb")) != NULL) {
      printf("Reading %s ...\n",datfilename_two);
      fread(&scan[1],sizeof(scan[1]),1,datfiletwo);
      fread(&old[1],sizeof(old[1]),1,datfiletwo);
   } else {
      printf("\nError reading %s, aborting...",datfilename_two);
      exit(1);
   }

   fclose(datfileone);
   fclose(datfiletwo);

   mergemode = 1;               /* Hardcoded to test it */

      for (x=0; x < 10000; x++) {

         old[2][x] = 0;            /* Clear it out first */

         round_one = (old[0][x]/10) * 10;
         round_two = (old[1][x]/10) * 10;

         /* It's longer this way, but it's easy to read. */

         switch (mergemode) {      
            case  0 : if ((round_one == 0) && (round_two != 0))
                        old[2][x] = old[1][x];
                      if ((round_one != 0) && (round_two == 0))
                        old[2][x] = old[0][x];
                      break;
            case 1 : if ((round_one == 0) && (round_two != 0))
                       old[2][x] = old[1][x];
                     if ((round_one !=0) && (round_two == 0))
                       old[2][x] = old[0][x];
                     if (round_one == 90)
                       old[2][x] = old[0][x];
                     break;
            case 2 : if ((round_one == 0) && (round_two !=0))
                       old[2][x] = old[1][x];
                     if ((round_one !=0) && (round_two == 0))
                        old[2][x] = old[0][x];
                     if (round_one == 80)
                       old[2][x] = old[0][x];
                     break;
            case 3 : old[2][x] = old[1][x];
                     if (round_one == 90)
                       old[2][x] = old[0][x];
                     break;
            case 4 : old[2][x] = old[1][x];
                     if (round_one == 80)
                       old[2][x] = old[0][x];
                     break;
         }
         cprintf("%4d\r",x);
      }
   printf("Oh, my pinky for a debugger... \n");

   /* Now save the merge file & quit */

   if ((mergefile=fopen(mergefilename,"wb")) != NULL) {
      fwrite(&scan[2],sizeof(scan[2]),1,mergefile);
      fwrite(&old[2],sizeof(old[2]),1,mergefile);
      fclose(mergefile);
   } else {
      printf("\nError writing %s, aborting...",mergefilename);
      exit(1);
   }
}
