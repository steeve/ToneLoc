/*
 * TM.C - Minor Threat's version of Tmerge,
 * because mucho couldn't write a piece of code
 * to save his life!!
 */

#include <stdio.h>
#include "toneloc.h"

#define  TONE      80
#define  CARRIER   90
#define  round(x)  ((x/10)*10)

void main(int argc, char *argv[])
{
   register int x;
   FILE *f1, *f2, *f3;
   char file1[13], file2[13], file3[13];
   struct _scan scan[2];       /* [3] */
   byte old1[10000];
   byte old2[10000];
   int foundonly, precedence;

   printf("TMERGE;  Merges two ToneLoc data files\n"
          "         by MM & MT\n\n");

   if (argc < 3) {
      printf("USAGE: TM {File1} {File2} {OutFile} [/F] [/C | /T]\n");
      printf("Merges {File1} on top of {File2}, writing to {OutFile}\n");
      printf("  {OutFile} defaults to MERGE.DAT\n");
      printf("  /F - Merge tones & carriers only\n");
      printf("  /C - Give carriers precidence (default)\n");
      printf("  /T - Give tones precidence\n");
      exit(0);
   }

   precedence = CARRIERS;
   foundonly = 0;            /* hardcode */

   strcpy(file1,argv[1]);
   strcpy(file2,argv[2]);
   strcpy(file3,"MERGE.DAT");

   printf("Merging %s onto %s, writing to %s\n",file1,file2,file3);

   f1=fopen(file1,"rb");
   if (!f1) {
      printf("\aError opening %s\n",file1);
      exit(1);
   }
   f2=fopen(file2,"rb");
   if (!f2) {
      printf("\aError opening %s\n",file2);
      exit(1);
   }

   fread(&scan[0],sizeof(struct _scan),1,f1);
   fread(&scan[1],sizeof(struct _scan),1,f2);
   fread(&old1,sizeof(old1),1,f1);
   fread(&old2,sizeof(old2),1,f2);
   fclose(f1);
   fclose(f2);

   for (x=0; x<10000; x++) {
      register int y;

      y=round(old1[x]);

      if (foundonly==1) {      /* only copy tones & carriers */
         if (y==CARRIER)
            if (round(old2[x])==TONE) {
               if (precedence==CARRIERS) {
                  old2[x]=old1[x];
               }
            } else
               old2[x]=old1[x];
         if (y==TONE)
            if (round(old2[x])==CARRIER) {
               if (precedence==TONES) {
                  old2[x]=old1[x];
               }
            } else
               old2[x]=old1[x];
      } else {
         if ((y) && ((round(old2[x]==100)) || (round(old2[x]==0)))) {
            old2[x]=old1[x];
         }
      }
      cprintf("%4d\r",x);
   }

   f3=fopen(file3,"wb");
   if (!f3) {
      printf("\aError writing to %s\n",file3);
      exit(1);
   }
   fwrite(&scan[1],sizeof(struct _scan),1,f3);
   fwrite(&old2,sizeof(old2),1,f3);
   fclose(f3);
}
