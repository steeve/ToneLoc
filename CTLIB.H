/*
 header for for cellular telephone library routines
 Copyright (C) 1993 by Network Wizards
*/

#define MODE_NORMAL  0x53
#define MODE_TEST    0xf1

#define	TONE_LOW     0
#define	TONE_HIGH    1

#define AUDIO_EXTERNAL	0
#define AUDIO_EARPIECE	1
#define AUDIO_SOUNDER	2

#define DTMF_DECODE_START 0
#define DTMF_DECODE_GET	  1
#define DTMF_DECODE_END	  2

#define MSG_SETUP    0
#define MSG_GET	     1
#define MSG_ABORT    2

/* keypress codes for MODE_NORMAL */
#define CT_KEY_RELEASE	0x00
#define CT_KEY_1	0x01
#define CT_KEY_2	0x02
#define CT_KEY_3	0x03
#define CT_KEY_4	0x04
#define CT_KEY_5	0x05
#define CT_KEY_6	0x06
#define CT_KEY_7	0x07
#define CT_KEY_8	0x08
#define CT_KEY_9	0x09
#define CT_KEY_0	0x0a
#define CT_KEY_STAR	0x0b
#define CT_KEY_POUND	0x0c
#define CT_KEY_SND	0x0d
#define CT_KEY_END	0x0e
#define CT_KEY_CLR	0x11
#define CT_KEY_STO	0x12
#define CT_KEY_RCL	0x13
#define CT_KEY_DOWN	0x14
#define CT_KEY_UP	0x15
#define CT_KEY_ALPH	0x1a
#define CT_KEY_MENU	0x16

/* function prototypes */
extern int  ct_lib_init();
extern void ct_lib_done();
extern int  ct_on();
extern void ct_off();
extern void ct_set_carrier();
extern void ct_set_tx_power();
extern void ct_set_signalling_tone();
extern void ct_set_rx_audio();
extern void ct_set_tx_audio();
extern void ct_set_sat();
extern void ct_set_comp_exp();
extern void ct_set_volume();
extern void ct_set_audio_tone();
extern void ct_set_audio_path();
extern int  ct_dtmf_decode();
extern void ct_set_dtmf();
extern void ct_set_channel();
extern int  ct_get_rss();
extern int  ct_get_battery_level();
extern int  ct_fcc_msg();
extern int  ct_fvc_msg();
extern int  ct_rcc_msg();
extern int  ct_rvc_msg();
extern int  ct_read_mem();
extern int  ct_read_block();
extern int  ct_set_block();
extern int  ct_get_esn();
extern char *ct_get_romver();
extern char *ct_get_prog_pwd();
extern char *ct_get_cust_msg();
extern int  ct_set_cust_msg();
extern int  ct_stream();
extern void ct_keypress();
extern word ct_inc_channel();
extern word ct_dec_channel();
extern int  ct_channel_type();
extern char *ct_decode_order();
extern int  ct_decode_fcc_msg();
extern int  ct_decode_fvc_msg();
extern void ct_tel2min();
extern word ct_gen_bch();

extern int  ct_get_nams();
struct nam_data {
  char number[16];
  char sid[8];
  char sys;
  char ipch[8];
  char olc[4];
  char gim[4];
};
extern struct nam_data nam_info[];

struct ct_state_struct {
  bool carrier;
  bool sigtone;
  bool rxaudio;
  bool txaudio;
  bool compexp;
  int  powerlevel;
  int  sat;
  int  volume;
  int  audiopath;
  int  channel;
  int  stream;
  int  namindex;
  int  oldnam;
};
extern struct ct_state_struct ct_state;
