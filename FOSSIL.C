/**********************************************************************
* fossil.c - Low-Level FOSSIL Communications Routines                 *
*            by mt                                                    *
***********************************************************************/

#include <stdio.h>
#include <dos.h>
#include "fossil.h"

//#defines

int open_fossil(int comport)
{
   union REGS r;

   port = comport-1;          // set global variable
   r.h.ah = 4;
   r.x.bx = 0;
   r.x.dx = port;
   int86(0x14,&r,&r);
   if (r.x.ax != 0x1954)
      return(0);                // failure

   return(1);                   // success
}

void close_fossil(void)
{
   _AH = 5;
   _DX = port;
   geninterrupt(0x14);
}

void set_fossil_baud(long baud)
{
   union REGS r;
   r.h.ah=0;       // FOSSIL set baud function
   r.h.al=0x03;    // No parity, 8 data bits, 1 stop bit
   r.x.dx=port;

   switch (baud)
   {
      case   300L : r.h.al |= 0x40;  break;
      case  1200L : r.h.al |= 0x80;  break;
      case  2400L : r.h.al |= 0xA0;  break;
      case  4800L : r.h.al |= 0xC0;  break;
      case  9600L : r.h.al |= 0xE0;  break;
      case 14400L : // 14.4 = 19.2
      case 16800L : // 16.8 = 19.2
      case 19200L : r.h.al |= 0x00;  break;
      case 38400L : r.h.al |= 0x20;  break;
      case 57600L : r.h.al |= 0x20;  break; // 57600=38400
      default     : r.h.al |= 0xA0;         /* 2400 default */
   }
   int86(0x14,&r,&r);
}
