/*
** MIRROR.C - internal use only by the el1te
** programmers of Tonel0c, the best wardialer
** pozzible
*/

#include <stdio.h>
#include <dos.h>
#include <dir.h>
#include "toneloc.h"

void main(int argc, char *argv[])
{
   byte fuckers[10000];
   struct _scan scan;
   char filename[13];
   struct ffblk fblk;
   int done;
   int Mtype;         // Mirror type. 1 = horizontal, 2 = vertical.
   FILE *f;
   register int x, y;

   printf("MIRROR;  Makes a mirror-image of a ToneLoc data file\n"
	  "         by Minor Threat & Mucho Mas 1993\n"
	  "         INTERNAL USE 0NLY!\n\n");

   if (argc<3) {
      printf("USAGE: MIRROR {DataFile} /h[orizontal] /v[ertical]\n");
      printf("Wildcards accepted.  'MIRROR *' will mirror ALL DAT files\n");
      exit(0);
   }

   strcpy(filename,argv[1]);
   if (!strchr(filename,'.')) {
      filename[8]=0;
      strcat(filename,".DAT");
   }

   if(strcmp(argv[2],"/h") == 0)
      Mtype = 1;
   else
    if(strcmp(argv[2],"/v") == 0)
      Mtype = 2;
    else
       Mtype = 2;               // Default is vertical so MT doesn't
				// shit his pants.

   printf("MIRRORing %s ",filename);
   if (Mtype == 1)
     printf("horizontally ...\n");
    else
     if (Mtype == 2)
      printf("vertically ...\n");
      else
       printf("...\n");

   done = findfirst(filename,&fblk,0);

   while (!done) {

      printf("%-12s: ",fblk.ff_name);

      f=fopen(fblk.ff_name,"rb");
      if (!f) {
	 printf("\aERROR opening!\n");
	 break; /* next loop */
      }

      fread(&scan,sizeof(scan),1,f);
      fread(&fuckers,sizeof(fuckers),1,f);
      fclose(f);

      printf("mirroring, ");


      if (Mtype == 2)                 // Vertical Mirror  (MT)
	 {
	   for (x=0; x <= 9900; x += 100)
	    for (y=0; y<50; y++)
	       SWAP (fuckers[x+y],fuckers[x+(99-y)]);
	 }

      if (Mtype == 1)                // Horizontal Mirror (MM)
	 {
	   for (y=0; y <= 9900; y += 100)
	    for (x=0; x<50; x++)
	       SWAP (fuckers[y+x],fuckers[y+(99-x)]);
	 }


      printf("saving, ");

      f=fopen(fblk.ff_name,"wb");
      if (!f) {
         printf("ERROR opening for output\n");
         exit(1);
      }

      fwrite(&scan,sizeof(scan),1,f);
      fwrite(&fuckers,sizeof(fuckers),1,f);
      fclose(f);
      printf("done\n");

      done = findnext(&fblk);
   }
}
