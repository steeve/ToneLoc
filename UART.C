/*
 * UART.C - Tells you what kind of UARTs you have
 * by Minor Threat 1993
 */

#include <stdio.h>
#include <dos.h>

char * uart_id(int port);
int port_exist(int port);
unsigned mpeek(unsigned, unsigned);


void main(int argc, char *argv[])
{
   int arg;

   if (argc==1) {
      printf("usage: UART [port]          - where port is 1 - 4\n");
      exit(1);
   }
   arg=atoi(argv[1]);
   if (port_exist(arg))
      printf("COM%d: %s UART detected\n",arg,uart_id(arg));
   else
      printf("Port does not exist!\n");
   exit(0);
}

char * uart_id(int port)
{
   int portid;
   unsigned char c;

   switch (port) {
      case 1 : portid = 0x3f8; break;
      case 2 : portid = 0x2f8; break;
      case 3 : portid = 0x3e8; break;
      case 4 : portid = 0x2e8; break;
      default: printf("%d is invalid, lamer\a\n",port);
               exit(1);
   }

// c = inportb(portid+2);               // read IIR register
   outportb(portid+2,0xC1);             // enable 14 byte buffer FIFO
   c = inportb(portid+2);               // see if it 'took'
   if (c & 0xC0)                        // if bits 7&6 set,
      return("16550A");                 // we have a 16550A

   outportb(portid+2,0);                // write a 0 back out for 82510s
                                        // or old 16550s
   if ((c & 0x80) || (c & 0x40))        // if only 1 high bit set
      return("16550");                  // its an old buggy 16550


   c = 0xAA;                   // 10101010 binary - test byte for ...
   outportb(portid+7,c);             // write to scratch register
   c = inportb(portid+7);            // read from scratch register
   if (c == 0xAA)                    // if scratch register exists,
      return("16450");               // we should have a 16450

   return("8250");
}

int port_exist(int port)
{
   return mpeek(0, 0x400 + (port - 1) * 2);
}

unsigned mpeek(unsigned seg, unsigned off)
{
   unsigned far * fp;

   fp = (unsigned far *) MK_FP(seg,off);

   return *fp;
}
