/********************************************************************
* fossil.h - Header file for FOSSIL.C FOSSIL routines               *
*            by mt                                                  *
********************************************************************/

//#defines

typedef unsigned int  word;
typedef unsigned char byte;

extern volatile word port;                   // global port variable

/* SERIAL.C functions */
int  open_fossil(int port);         // inits fossil port
void close_fossil(void);            // de-inits fossil port
void set_fossil_baud(long baud);    // sets fossil baud rate

/* FOSASM.ASM functions */
void charouta(byte);         /* sends character to FOSSIL */
int  comhita(void);          /* kbhit() for FOSSIL */
int  charina(void);          /* getch() for FOSSIL */
void setdtr(int);            /* sets DTR high or low */
int  fossil_cts(void);       /* returns CTS state */
void set_fossil_flow(byte);  // sets fossil flow control
void flushina(void);         /* nukes input buffer */
int  carriera(void);         /* returns 0 if no carrier detect */
int  dv_there(void);         /* checks for DESQview */
void dv_release(void);       /* release timeslice to DV */
byte kbhita(void);           /* kbhit() in assembly uses int16 */
int  GetFossilInfo(word,word,word); /* loads buffer with FOSSIL info */
void farputs(word seg, word ofs);   /* puts a far string to stdout */
