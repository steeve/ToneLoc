/*
 * TLCFG.H - Structure for TONELOC.CFG new-style config file
 *           by Minor Threat 1993
 */

typedef unsigned int word;
typedef unsigned char byte;
#define atob(x)   ((unsigned char) atoi(x))
#define atow(x)   ((unsigned int) atoi(x))


typedef struct
{
   char    ProductCode[2];          // 'TL'
   word    VersionID;               // 0x0100 = 1.00
   int     Fossil;
   word    port;
   int     ctek_enabled;            // Elite flag that gets thrown if they have enabled CTEK support
   int     device_type;
   char    ournum[32];
   unsigned long ouresn;
   int     tumbl_esn;
   int     tumbl_min;
   long    baudrate;
   int     sfx;
   int     foundsfx;
   byte    act_win,
           act_text,
           mod_win,
           mod_text,
           tone_text,
           carrier_text,
           stats_win,
           stats_items,
           stats_text,
           meter_back,
           meter_fore;
   char    meterfront,
           meterback;
   char    initstring[90],
           initresp[90],
           prefix[90],
           suffix[90],
           connect_string[90],
           tone_string[90],
           fax_string[90],
           ringing_string[90],
           busy_string[90],
           voice_string[90],
           notone_string[90],
           nocarrier_string[90],
           speakon[90],
           speakoff[90],
           vol[10][90],
           c_hangup[90],
           t_hangup[90],
           hangup[90],
           exitstring[90],
           toshell[90],
           fromshell[90];
   word    command_delay,
           between_delay,
           wait_delay;
   int     between_wipe,
           NoDupes,
           MaxRing,
           scan_type,
           ignore_cd,
           ignore_cts,
           ignore_unknown,
           AutoInt,
	   busy_redial;
   char    foundname[13],
           carrierlogname[13],
           blackfilename[13],
           logname[13],
           AltScreen[13];
   int     DialMethod,
           logging,
           NoToneAbort,
           getdelay;
   word    pacing;
   char    flogstring[90];
   int     CarrierLogging;
   char    NudgeString[90];
   int    /* PostNudgeDelay,  */
           AutoE71,
           LFstrip;
   word    PostNudgeDelay;
   int     irq, portaddx;
   byte    freespace[283];
} CONFIG;

