/*
 * TextMap.C - ToneLoc .DAT -> .MAP file generator
 * by The Public
 */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "ToneLoc.h"

void   helpscreen(void);                /* helpscreen */
void   mainloop(void);
void   ParseCmdLine(int, char *[]);

byte   oldones[10000];
struct _scan scan;
FILE   *datfile, *logfile;
char   datfilename[60], logfilename[13], string[25];
int    printkey=1, columns=79, beginum=0, endnum=9999;

void main(int argc, char * argv[])
{
	puts("TextMap;  Creates a TEXT ToneMap from ToneLoc Data file");
	puts("          by The Public\n");

	if (strchr(argv[1],'?')) helpscreen();
	if (argc < 3 || argc > 6) helpscreen();

	ParseCmdLine(argc, argv);
	mainloop();
}

void mainloop(void)
{
	register int x, z;
	int y, RingCount, leftnum, rightnum, c=0;

	if ((datfile=fopen(datfilename,"rb")) == NULL) {
		printf("\a\nError opening %s!\n",datfilename);
		exit(1);
	}

	fread(&scan,sizeof(scan),1,datfile);
	fread(&oldones,sizeof(oldones),1,datfile);
	fclose(datfile);

	logfile=fopen(logfilename,"wt");

 	if(printkey==1)
	 {
		fprintf(logfile,"(B)usy (T)one (C)arrier (V)oice (A)borted (+) Timeout (R)ingout (D)ialed\n");
		fprintf(logfile,"(u)ndialed (o)mitted (x) excluded (n)o dialtone (b)lacklisted  (*) Noted\n\n");
	 }

	leftnum=beginum;	rightnum=leftnum+columns-11;
	if(rightnum>endnum)
	 rightnum=endnum;

	fprintf(logfile,"%04d-%04d ",leftnum,rightnum); c=0;

	for (x=beginum; x<endnum; x++)
	 {
		if(c==columns-10)
		 {
			fprintf(logfile,"\n");
			leftnum=rightnum+1; rightnum=leftnum+columns-11;
			if(rightnum>endnum)
			 rightnum=endnum;
			fprintf(logfile,"%04d-%04d ",leftnum,rightnum); c=0;
		 }
		c++;
		y=oldones[x];
		if (y != 0)
			y = ((y/10) * 10);
		switch (y)
		 {
			case   0 : fprintf(logfile,"u"); break;  /* undialed    */
			case  10 : fprintf(logfile,"B"); break;  /* busy        */
			case  20 : fprintf(logfile,"V"); break;  /* voice       */
			case  40 : fprintf(logfile,"*"); break;  /* noted       */
			case  50 : fprintf(logfile,"A"); break;  /* aborted     */
			case  60 : fprintf(logfile,"R"); break;  /* ringout     */
			case  70 : fprintf(logfile,"+"); break;  /* timeout     */
			case  80 : fprintf(logfile,"T"); break;  /* tone        */
			case  90 : fprintf(logfile,"C"); break;  /* carrier     */
			case 130 : fprintf(logfile,"b"); break;  /* blacklisted */
			default  : fprintf(logfile,"?"); break;  /* eh? */
		 }
	 }
	fclose(logfile);
}

void ParseCmdLine(int argc, char *argv[])
{
	register int x;
	char *ctemp[7], *rtemp[10], *rbegin[5], *rend[5];

	strcpy(datfilename,argv[1]);
	datfilename[8]=0;
	strcat(datfilename,".DAT");
	strupr(datfilename);
	printf("Using Data File: %s   ",datfilename);

	strcpy(logfilename,argv[2]);
	strupr(logfilename);
	printf("Writing to file: %s\n",logfilename);

	for (x=3; x<argc; x++)
	 {
		strupr(argv[x]);
		if (strncmp(argv[x],"-C",2)==0) {
			if(strlen(argv[x])>6)                          /* not too BIG */
			 columns=9999;
			else {
				strrev(argv[x]);                             /* don't ask me why */
				strncat(ctemp,argv[x],strlen(argv[x])-2);    /* this works,  but */
				strrev(ctemp);                               /* it does.    Am I */
				columns=atoi(ctemp);                         /* proud of it?? :) */
				strrev(argv[x]);                             /* I'll rev it back */
			 }             /* I guess I probably shouldn't change the argv[]'s */
		 }
		if (strncmp(argv[x],"-R",2)==0) {
			strrev(argv[x]);
			strncat(rtemp,argv[x],strlen(argv[x])-2);    /* to get rid of "-R" */
			strrev(rtemp);
			if(strchr(rtemp,'-')==0) /* is there a hyphen to seperate numbers? */
			 {
				strcat(rbegin,rtemp);                      /* nope, just use the */
				beginum=atoi(rtemp);                       /* only number there, */
				endnum=9999;                               /* and make an ending */
			 }
			else {
				beginum = atoi(rtemp);
				endnum = atoi(strrchr(rtemp,'-')+1);
			/*	Thx to Henri Stegehuis <ham@osg.npl.co.uk> of the National Physical
			 *	Laboratory (DITC/OSG) for the damn range help. */

			/*  I'm such a poor fucking 'C' dood that I couldn't figure this
			 *	bastard out -- but out of the 5 functions I *did* get off the
			 *	net, this one WORKED.  Looks like no one else is good either. */
			 }
		 }
		if (strstr(argv[x],"-K")) printkey=0;
	 }
	if(columns<0) columns=columns*(-1);                /* no negatives,  */
	if(columns<20) columns=20;                         /* not TOO small, */
	if(endnum==0 || endnum>9999) endnum=9999;					 /* and not so big */
	if(printkey==0) printf("Not writing key.  ");
	 else if(printkey==1) printf("Writing key.  ");
	printf("Using %d columns.  ",columns);
	printf("Range: %04d-%04d\n",beginum,endnum);
	if(beginum==endnum || beginum>endnum)
	 {
		puts("Error: range number problems.");
			/* or should I swap the two?  300-200 may assume 200-300 */
			/* hell, I could count down!  (nah...)                   */
		exit(1);
	 }
}

void helpscreen(void)
{
	puts("Usage:  TEXTMAP datafile outfile [-cCOLUMNS] [-rFROM-TO] [-k]\n");

	puts("   datafile     - ToneLoc .Datafile to read (*.DAT)");
	puts("   outfile      - File to output text to");
	puts("   -c###        - Number of columns in a line (default 79, minimum 20)");
	puts("   -r####-####  - Range to list (begin-end: 0 minimum, 9999 maximum)");
	puts("   -k           - Supress writing a copy of the key to the file");

	puts("Thx to Henri Stegehuis <ham@osg.npl.co.uk> of the National Physical");
	puts("Laboratory (DITC/OSG) for the damn range help.");
		/* I promised him. */
	exit(0);
}
