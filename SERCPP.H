/********************************************************************
* sercpp.h - C++ Header File for Serial Comm Prog in C and C++      *
*                Copyright (c) 1992 By Mark D. Goodwin              *
********************************************************************/
#ifndef __SERCPPH__
#define __SERCPPH__

#include "serial.h"

class SERIALPORT {
	int port;
public:
	SERIALPORT(int n, int l) { open_port(n, l); }
	~SERIALPORT(void) { close_port(); }
	int carrier(void) { return ::carrier(); }
	void fifo(int n) { ::fifo(n); }
	long get_baud(void) { return ::get_baud(); }
	int get_bits(void) { return ::get_bits(); }
	int get_parity(void) { return ::get_parity(); }
	int get_rx_dtr(void) { return ::get_rx_dtr(); }
	int get_rx_rts(void) { return ::get_rx_rts(); }
	int get_rx_xon(void) { return ::get_rx_xon(); }
	int get(void) { return get_serial(); }
	int get_stopbits(void) { return ::get_stopbits(); }
	int get_tx_dtr(void) { return ::get_tx_dtr(); }
	int get_tx_rts(void) { return ::get_tx_rts(); }
	int get_tx_xon(void) { return ::get_tx_xon(); }
	int in_ready(void) { return ::in_ready(); }
	void purge_in(void) { ::purge_in(); }
	int put(unsigned char n) { return put_serial(n); }
	void set_baud(long baud) { ::set_baud(baud); }
	void set_data_format(int bits = 8, int parity = NO_PARITY,
		int stopbit = 1) { ::set_data_format(bits, parity, stopbit); }
	void set_dtr(int n) { ::set_dtr(n); }
	void set_port(long baud, int bits = 8, int parity = NO_PARITY,
		int stopbit = 1) { ::set_port(baud, bits, parity, stopbit); }
	void set_rx_dtr(int n) { ::set_rx_dtr(n); }
	void set_rx_rts(int n) { ::set_rx_rts(n); }
	void set_rx_xon(int n) { ::set_rx_xon(n); }
	void set_tx_dtr(int n) { ::set_tx_dtr(n); }
	void set_tx_rts(int n) { ::set_tx_rts(n); }
	void set_tx_xon(int n) { ::set_tx_xon(n); }
};

class XFERPORT : public SERIALPORT {
public:
	XFERPORT(int n, int l) : SERIALPORT(n, l)  { }
	int receive(int xtype, int(*error_handler)(int c, long p, char *s),
		char *path)	{ return recv_file(xtype, error_handler, path); }
	int transmit(int xtype, int(*error_handler)(int c, long p, char *s),
		char *files[]) { return xmit_file(xtype, error_handler, files); }
};

class ANSI {
public:
	ANSI(void) { ansi_dsr = put_serial; ansi_dsr_flag = TRUE; }
	void out(int c) { ansiout(c); }
	void string(char *s) { ansistring(s); }
	int printf(char *s, ... );
};

#endif