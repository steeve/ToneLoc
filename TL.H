/*
** TL.H - Header file for ToneLoc
*/

/*
 * The datafiles are a fairly simple structure.  The header (16 bytes)
 * is first, followed by 10000 bytes, one byte for each phone number.
 * The structure of the header is below, "struct _scan".  For the
 * 10,000 phone number bytes, the byte stored represent whether the
 * number has been dialed yet, and the response that we got.
 *
 * Response codes:
 *
 *  (x = number of rings, always 0 - 9)
 *
 *  00 = Undialed
 *  1x = Busy
 *  2x = Voice
 *  3x = No Dialtone
 *  40 = Noted
 *   41 = Fax
 *   42 = Girl
 *   43 = VMB
 *   44 = Yelling Asshole
 *  5x = Aborted
 *  6x = Ringout
 *  7x = Timeout
 *  8x = Tone
 *  9x = Carrier
 * 10x = Excluded
 * 11x = Omitted
 * 12x = Dialed
 * 13x = Blacklisted
 *
 * You'll notice that some of these are unfamiliar to you.  That is
 * because some of them are only for testing purposes.  If there is
 * something else that you think should be stored, let us know.  Also,
 * there are about 10 bytes of extra space in the header, so let us
 * know if there is something else you think should be kept there.
 *
 * We distribute this header file because we want people to know
 * the structure of the datafile, so people can write software for
 * toneloc.  If you have questions, please ask us. 
 */

typedef unsigned int  word;
typedef unsigned char byte;

struct _scan {                   /* header size: 16 bytes   16  */
   char  ProductCode[2];         /* should always be 'TL'    2  */
   word  VersionID;              /* 0x0103 = 1.03            2  */
   word  Minutes;                /* Minutes spent on scan    2  */
   byte  Extra[10];              /* future use              10  */
};
