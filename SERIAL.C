/**********************************************************************
* serial.c - Low-Level Serial Communications Routines                 *
*            Copyright (c) 1992 By Mark D. Goodwin                    *
***********************************************************************
* modified for toneloc by minor threat
**********************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <dos.h>
#include <conio.h>
#include <alloc.h>
#include <time.h>
#include "serial.h"
#include "tlcfg.h"

/* UART register constants */
#define TX 0                            /* transmit register */
#define RX 0                            /* receive register */
#define DLL 0                           /* divisor latch low */
#define IER 1                           /* interrupt enable register */
#define DLH 1                           /* divisor latch high */
#define IIR 2                           /* interrupt id register */
#define LCR 3                           /* line control register */
#define MCR 4                           /* modem control register */
#define LSR 5                           /* line status register */
#define MSR 6                           /* modem status register */

/* interrupt enable register constants */
#define RX_INT 1                        /* received data bit mask */

/* interrupt id register constants */
#define INT_MASK 7                      /* interrupt mask */
#define RX_ID 4                         /* received data interrupt */

/* line control register constants */
#define DLAB 0x80                       /* DLAB bit mask */

/* modem control register constants */
#define DTR 1                           /* Date Terminal Ready bit mask */
#define RTS 2                           /* Request To Send bit mask */
#define MC_INT 8                        /* GPO2 bit mask */

/* line status register constants */
#define RX_RDY 0x01
#define TX_RDY 0x20

/* modem status register constants */
#define CTS 0x10
#define DSR 0x20
#define DCD 0x80

/* 8259 PIC constants */
#define IMR 0x21                         /* interrupt mask register */
#define ICR 0x20                      /* interrupt control register */

/* interrupt mask register constants */
#define IRQ0 0xFE               /* IRQ 0 interrupt mask - 1111 1110 */
#define IRQ1 0xFD               /* IRQ 1 interrupt mask - 1111 1101 */
#define IRQ2 0xFB               /* IRQ 2 interrupt mask - 1111 1011 */
#define IRQ3 0xF7               /* IRQ 3 interrupt mask - 1111 0111 */
#define IRQ4 0xEF               /* IRQ 4 interrupt mask - 1110 1111 */
#define IRQ5 0xDF               /* IRQ 5 interrupt mask - 1101 1111 */
#define IRQ6 0xBF               /* IRQ 6 interrupt mask - 1011 1111 */
#define IRQ7 0x7F               /* IRQ 7 interrupt mask - 0111 1111 */

/* interrupt control register constants */
#define EOI 0x20                           /* end of interrupt mask */

/* XON/XOFF flow control constants */
#define XON 0x11                        /* XON  char - ^Q */
#define XOFF 0x13                       /* XOFF char - ^S */

int port_open = FALSE, irq;
void interrupt (*oldvect)();

extern CONFIG cfg;

/* Check for a Port's Existence */
int port_exist(int port)
{
   return mpeek(0, 0x400 + (port - 1) * 2);
}

/* Open a Serial Port */
void open_port(int port, int inlen)
{
   unsigned char irqmask;
   int actual_irq;         // What we ultimately decide the irq will be.
                           // Used with irqmask'ing.

    /* make sure a port isn't already open */
   if (port_open)
   {
      printf("Unable to open port: A port is already open!\n");
         exit(1);
   }

   /* make sure the port is 1 - 8. */
   if ((port < 1 || port > 4) && !cfg.irq)
   {
      puts("Unable to open port: Invalid port number!");
      exit(1);
   }

   /* make sure the port if valid. */
   if (((port < 1 || port > 4) || !port_exist(port)) && !cfg.irq)
   {
      puts("Unable to open port: Can't find serial port");
      exit(1);
   }

   /* allocate the space for the buffers */
   ilen = inlen;
   if ((inbuff = farmalloc(ilen)) == NULL)
   {
      printf("Unable to open port: Not enough memory for the buffer!\n");
      exit(1);
   }

   /* calculate the flow control limits */
   foff = (int)((long)ilen * 90L / 100L);      // 90%+ buffer full - XOFF
   fon = (int)((long)ilen * 80L / 100L);       // 80%- buffer full - XON
   rx_flow = FALSE;

   /* set the base address and IRQ*/

   // This routine was totally confusing and redundant.
   // I hope my re-write is clearer. - Mucho

   if (!cfg.irq) {     // If we didn't manually specify them, use COM 1-4.
      switch (port) {
         case 1: base = 0x3f8;
                 irq = 0x0c;
                 actual_irq = 4;
                 break;
         case 2: base = 0x2f8;
                 irq = 0x0b;
                 actual_irq = 3;
                 break;
         case 3: base = 0x3e8;
                 irq = 0x0c;
                 actual_irq = 4;
                 break;
         case 4: base = 0x2e8;
                 irq = 0x0b;
                 actual_irq = 3;
                 break;
        default: puts("You should never see this message! Report as an error in open_port!");
                 farfree(inbuff); // Bounds checker
                 exit(1);  // We should never see this, because we checked range already.
      }
   }
  else              // Otherwise we manually specified it, so use those values
   {
    base = cfg.portaddx;                    // Use the manual base address
    actual_irq = cfg.irq;
    switch (cfg.irq) {
        case 2 :  irq = 0x0a;               // Look up and set the vector.
                  break;
        case 3 :  irq = 0x0b;
                  break;
        case 4 :  irq = 0x0c;
                  break;
        case 5 :  irq = 0x0d;
                  break;
        case 7 :  irq = 0x0f;
                  break;
        case 9 :  actual_irq = 2;    // Automatically remapped to IRQ 2
                  irq = 0x0a;
                  break;
        default:  puts("The IRQ you specified is not supported! Complain!");
                  farfree(inbuff);  // Bounds checker
                  exit(1);
      }
   }

// irqmask = 1;
// irqmask = ~((irqmask=1) << cfg.irq);
   irqmask = ~((irqmask=1) << actual_irq);
// irqmask = ~irqmask;

   /* set up the interrupt handler */
   disable();                                  /* disable the interupts */
   oldvect = getvect(irq);                   /* save the current vector */
   setvect(irq, handler);                         /* set the new vector */
   sibuff = eibuff = 0;                      /* set the buffer pointers */
   outportb(base + MCR,
      inportb(base + MCR) | MC_INT | DTR | RTS); /* assert GPO2, RTS, DTR */
   outportb(base + IER, RX_INT);           /* set received data interrupt */
   outportb(IMR,
      inportb(IMR) & irqmask);                       /* set the interrupt */
   enable();                                     /* enable the interrupts */
   fifo(14);                              /* set FIFO buffer for 14 bytes */
   set_tx_rts(FALSE);                    /* turn off RTS/CTS flow control */
   set_tx_dtr(FALSE);                    /* turn off DTR/DSR flow control */
   set_tx_xon(FALSE);                   /* turn off XON/XOFF flow control */
   set_rx_rts(FALSE);                    /* turn off RTS/CTS flow control */
   set_rx_dtr(FALSE);                    /* turn off DTR/DSR flow control */
   set_rx_xon(FALSE);                   /* turn off XON/XOFF flow control */
   port_open = TRUE;                                 /* flag port is open */

}

/* close serial port routine */
void close_port(void)
{
   unsigned char bits_to_clear;

   /* check to see if a port is open */
   if (!port_open)
      return;

   port_open = FALSE;                             /* flag port not opened */

   switch (irq) {                        // Set up for non-standard com ports
      case 0x0a : bits_to_clear = ~IRQ2;
                  break;
      case 0x0b : bits_to_clear = ~IRQ3;
                  break;
      case 0x0c : bits_to_clear = ~IRQ4;
                  break;
      case 0x0d : bits_to_clear = ~IRQ5;
                  break;
      case 0x0f : bits_to_clear = ~IRQ7;
                  break;
      default   : bits_to_clear = ~IRQ4;      // Com 1
                  break;
      }


   disable();                                   /* disable the interrupts */

//  outportb(IMR, inportb(IMR) | (irq == 0x0b ? ~IRQ3 : ~IRQ4)); /* clear the interrupt */
// ^^ Doesn't work for nonstandard com ports! ^^

   outportb(IMR, inportb(IMR) | bits_to_clear);                  /* clear the interrupt */

   outportb(base + IER, 0);                   /* clear recceived data int */
   outportb(base + MCR,
      inportb(base + MCR) & ~MC_INT);                    /* unassert GPO2 */
   setvect(irq, oldvect);                     /* reset the old int vector */
   enable();                                     /* enable the interrupts */
      outportb(base + MCR,
   inportb(base + MCR) & ~RTS);                           /* unassert RTS */
   farfree(inbuff) ;          // bounds checker
}

/* purge input buffer */
void purge_in(void)
{
   disable();                                   /* disable the interrupts */
   sibuff = eibuff = 0;                        /* set the buffer pointers */
   enable();                                     /* enable the interrupts */
}

/* set the baud rate */
void set_baud(long baud)
{
   int c, n;

   /* check for 0 baud */
   if (baud == 0L)
      return;
   n = (int)(115200L / baud);                       /* figure the divisor */
   disable();                                   /* disable the interrupts */
   c = inportb(base + LCR);                       /* get line control reg */
   outportb(base + LCR, (c | DLAB));             /* set divisor latch bit */
   outportb(base + DLL, n & 0x00ff);          /* set LSB of divisor latch */
   outportb(base + DLH, (n >> 8) & 0x00ff);   /* set MSB of divisor latch */
   outportb(base + LCR, c);                   /* restore line control reg */
   enable();                                     /* enable the interrupts */
}

/* get the baud rate */
long get_baud(void)
{
   int c, n;

   disable();                                   /* disable the interrupts */
   c = inportb(base + LCR);                       /* get line control reg */
   outportb(base + LCR, (c | DLAB));             /* set divisor latch bit */
   n = inportb(base + DLH) << 8;              /* get MSB of divisor latch */
   n |= inportb(base + DLL);                  /* get LSB of divisor latch */
   outportb(base + LCR, c);               /* restore the line control reg */
   enable();                                     /* enable the interrupts */
   return 115200L / (long)n;                      /* return the baud rate */
}

/* get number of data bits */
int get_bits(void)
{
   return (inportb(base + LCR) & 3) + 5;    /* return number of data bits */
}

/* get parity */
int get_parity(void)
{
   switch ((inportb(base + LCR) >> 3) & 7)
   {
      case 0: return NO_PARITY;
      case 1: return ODD_PARITY;
      case 3: return EVEN_PARITY;
   }
   return -1;
}

/* get number of stop bits */
int get_stopbits(void)
{
   if (inportb(base + LCR) & 4)
      return 2;
   return 1;
}

void set_data_format(int bits, int parity, int stopbit)
{
   int n;

   /* check parity value */
   if (parity < NO_PARITY || parity > ODD_PARITY)
      return;

   /* check number of bits */
   if (bits < 5 || bits > 8)
      return;

   /* check number of stop bits */
   if (stopbit < 1 || stopbit > 2)
      return;

   n = bits - 5;                                 /* figure the bits value */
   n |= ((stopbit == 1) ? 0 : 4);            /* figure the stop bit value */

   /* figure the parity value */
   switch (parity)
   {
        case   NO_PARITY : break;
        case  ODD_PARITY : n |= 0x08; break;
        case EVEN_PARITY : n |= 0x18; break;
   }

   disable();                                   /* disable the interrupts */
   outportb(base + LCR, n);                     /* set the port */
   enable();                                    /* enable the interrupts */
}

void set_port(long baud, int bits, int parity, int stopbit)
{
   /* check for open port */
   if (!port_open)
      return;

   set_baud(baud);                                 /* set the baud rate */
   set_data_format(bits, parity, stopbit);       /* set the data format */
}

/* check for byte in input buffer */
int in_ready(void)
{
   return !(sibuff == eibuff);
}

/* check for carrier routine */
int carrier(void)
{
   return inportb(base + MSR) & DCD ? TRUE : FALSE;
}

/* set DTR routine */
void set_dtr(int n)
{
   if (n)
      outportb(base + MCR, inportb(base + MCR) | DTR); /* assert DTR */
   else
      outportb(base + MCR, inportb(base + MCR) & ~DTR); /* unassert RTS */
}

/* 16550 FIFO routine */
void fifo(int n)
{
   int i;

   switch (n) {
      case  1: i = 1;                     /* 1 byte FIFO buffer */
               break;
      case  4: i = 0x41;                  /* 4 byte FIFO buffer */
               break;
      case  8: i = 0x81;                  /* 8 byte FIFO buffer */
               break;
      case 14: i = 0xc1;                  /* 14 byte FIFO buffer */
               break;
      default: i = 0;                     /* turn FIFO off for all others */
   }
   outportb(base + IIR, i);                       /* set the FIFO buffer */
}

/* set transmit RTS/CTS flag */
void set_tx_rts(int n)
{
   tx_rts = n;
}

/* set transmit DTR/DSR flag */
void set_tx_dtr(int n)
{
   tx_dtr = n;
}

/* set transmit XON/XOFF flag */
void set_tx_xon(int n)
{
   tx_xon = n;
   tx_xonoff = FALSE;
}

/* set receive RTS/CTS flag */
void set_rx_rts(int n)
{
   rx_rts = n;
}

/* set receive DTR/DSR flag */
void set_rx_dtr(int n)
{
   rx_dtr = n;
}

/* set receive XON/XOFF flag */
void set_rx_xon(int n)
{
   rx_xon = n;
}

/* get transmit RTS flag */
int get_tx_rts(void)
{
   return tx_rts;
}

/* get transmit DTR/DSR flag */
int get_tx_dtr(void)
{
   return tx_dtr;
}

/* get transmit XON/XOFF flag */
int get_tx_xon(void)
{
   return tx_xon;
}

/* get receive RTS/CTS flag */
int get_rx_rts(void)
{
   return rx_rts;
}

/* get receive DTR/DSR flag */
int get_rx_dtr(void)
{
   return rx_dtr;
}

/* get receive XON/XOFF flag */
int get_rx_xon(void)
{
   return rx_xon;
}

/* get current CTS state */
int serial_cts(void)
{
   return inportb(base + MSR) & 0x10 ? TRUE : FALSE;
}

char * uart_id(void)
{
   unsigned char c;

   if (!port_open)
      return(0);

   outportb(base + IIR,0xC1);                // enable 14 byte buffer FIFO
   c = inportb(base + IIR);                  // see if it 'took'
   if (c & 0xC0)                             // if it 'took',
      return("16550A");                      // we have 16550A

   outportb(base + IIR,0);                   // fix old 16550s and 82510s

   if ((c & 0x80) || (c & 0x40))             // if only 1 high bit set,
      return("16550");                       // it's an old buggy 16550

   c = 0xAA;           // 10101010 binary - test byte for ...
   outportb(base + 7,c);             // write to scratch register
   c = inportb(base + 7);            // read from scratch register
   if (c == 0xAA)                    // if scratch register exists,
      return("16450");               // we should have a 16450

   return("8250");
}

unsigned int serial_status(void)
{
   unsigned x;
   x = inportb(base + MSR);
   x |= (inportb(base + LSR) << 8);
   return(x);
}
