/*
 * ToneLoc.H - Header file for ToneLoc
 * 1994 by Minor Threat and Mucho Maas
 */


#define SWAP(a,b) (a^=b,b^=a,a^=b)         /* xor a,b; xor b,a; xor a,b */
#define atob(x)   ((unsigned char) atoi(x))
#define atow(x)   ((unsigned int) atoi(x))

#define FOS 0x14                                    /* FOSSIL interrupt */

#define TONES    0                                  /* what to scan for */
#define CARRIERS 1
#define ALLBUT_T 2
#define ALLBUT_C 3

#define M_OK            0                            /* modem responses */
#define M_CONNECT       1
#define M_RINGING       2
#define M_BUSY          3
#define M_VOICE         4
#define M_NO_TONE       5
#define M_NO_CARRIER    6
#define M_NOTHING       7
#define M_FAX           8
#define M_UNKNOWN      99

#define A_KEYPRESSED    1                              /* Abort reasons */
#define A_TIMEOUT       2
#define A_RINGABORT     3
#define A_CARRIER       4
#define A_NO_TONE       5
#define A_BLACKLISTED   6
#define A_NO_CARRIER    7
#define A_BUSY          8
#define A_TONE          9
#define A_VOICE        10
#define A_CONNECT      11
#define A_FAX          12
#define A_UNKNOWN_RESP 99

typedef unsigned int word;
typedef unsigned char byte;
typedef unsigned long ulong;
typedef unsigned bit;

struct _scan90 {                     /* Structure of 0.90 DAT file */
   int   tones,
         rings,
         busys,                             /* 10010 byte datafile */
         voices,
         tried;
};

struct _scan95 {                     /* Structure of 0.95 DAT file */
   int   tones,
         carriers,
         rings,                             /* 10012 byte datafile */
         busys,
         voices,
         tried;
};

/* Starting with .98, you can no longer differentiate DAT files by */
/* size alone. The structure for .98 and 1.00 is the same. Use the */
/* VersionID to distinguish DAT files created by .98 or higher.    */

struct _scan                     /* header size: 16 bytes   16  */
{                   
   char  ProductCode[2];         /* should be "TL" always    2  */
   word  VersionID;              /* 0x0103 = 1.03            2  */
   word  Minutes;                /* Minutes spent on scan    2  */
   byte  Extra[10];              /* future use              10  */
   // add: startdate, lastdate,
   //      mask (most recent)
   //
};

struct black_number              // black list linked list struct
{
   char number[13];              // blacklisted number
   struct black_number * next;   // pointer to next one (or NULL)
};

typedef struct _FOSINFO
{
   int    info_size;            // Structure size
   char   curr_fossil;          // FOSSIL spec version
   char   curr_rev;             // Driver rev level
   word   id_ofs;               // Pointer to ASCII ID (offset)
   word   id_seg;               // Pointer to ASCII ID (segment)
   int    ibufr;                // Input buffer size
   int    ifree;                // Bytes available (input)
   int    obufr;                // Output buffer size
   int    ofree;                // Bytes available (output)
   byte   swidth;               // Screen width (chars)
   byte   sheight;              // Screeb height (chars)
   char   baud;                 // Baud rate mask
} FOSINFO;

