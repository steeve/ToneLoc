;
; fosasm.asm - Low level FOSSIL routines
;              by minor threat for toneloc
;
;

        IDEAL
        MODEL   SMALL

        DATASEG

        public  _port          ; global com port variable

_port   dw      ?

        CODESEG

        public  _charouta      ; send char to FOSSIL
        public  _carriera      ; detect carrier
        public  _comhita       ; kbhit() for FOSSIL
        public  _flushina      ; nukes input buffer
        public  _charina       ; waits for a char from FOSSIL
        public  _setdtr        ; sets DTR via FOSSIL
        public  _set_fossil_flow ;sets FOSSIL flow control (XON/XOFF/CTS/RTS)
        public  _fossil_cts    ; gets current CTS state
        public  _dv_there      ; check for desqview
        public  _dv_release    ; release timeslice
        public  _kbhita        ; kbhit(), but faster
        public  _GetFossilInfo ; get fossil driver info struct
        public  _farputs       ; puts far string to screen via DOS

;-----------------------------------------------------------------------
; void charouta(char ch);
; sends [byte] to the FOSSIL's buffer and flushes output
;-----------------------------------------------------------------------
PROC    _charouta
        push    bp                      ; save bp
        mov     bp, sp                  ; set up to get the arguments
        mov     dx, [_port]             ; dx = comport
        mov     al, [bp+4]              ; al = chararcter to output
        mov     ah, 01h                 ; FOSSIL character out service
        int     14h                     ;  do it

        mov     ah, 08h                 ; FOSSIL 'flush' output service
        int     14h                     ; FOSSIL interrupt

        pop     bp                      ; restore bp
        ret
ENDP    _charouta

;-----------------------------------------------------------------------
; int carriera(void)
; checks for carrier, returns 0 or 1
;-----------------------------------------------------------------------
PROC    _carriera
        mov     ah, 03h                 ; 03h = carrier check
        mov     dx, [_port]             ; dx = comport
        int     14h                     ; call FOSSIL interrupt
        test    al, 80h                 ; test for bit 7 = 1 (carrier)
        jz      NoCarrier
        mov     ax, 01h                 ; CD present; return(1);
        ret
NoCarrier:
        xor     ax, ax;                 ; no CD;    return(0);
        ret
ENDP    _carriera

;-----------------------------------------------------------------------
; int  comhita(void)
; kbhit() for FOSSIL
;-----------------------------------------------------------------------
PROC    _comhita
        mov     ah, 0Ch                 ; 0C = peek-ahead for input
        mov     dx, [_port]             ; dx = comport
        int     14h                     ; FOSSIL interrupt
        cmp     ax, 0ffffh              ; is there a key in the buffer?
        jnz     KeyWaiting              ;  yes, return the key
        xor     ax, ax                  ;  no, set ax = 0
KeyWaiting:
        ret                             ; al = key waiting
ENDP    _comhita

;-----------------------------------------------------------------------
; void flushina(void)
; flushes (zeros) input buffer
;-----------------------------------------------------------------------
PROC    _flushina
        mov     ah, 0Ah                 ; 0Ah = flush input buffer
        mov     dx, [_port]             ; dx = comport
        int     14h                     ; call FOSSIL interrupt
        ret
ENDP    _flushina

;-----------------------------------------------------------------------
; char charina(void)
; waits for key from FOSSIL
;-----------------------------------------------------------------------
PROC    _charina
        mov     ah, 02h                 ; 02 = get key from FOSSIL
        mov     dx, [_port]             ; dx = comport
        int     14h                     ; call FOSSIL interrupt
        xor     ah,ah                   ; ax = character
        ret
ENDP    _charina

;-----------------------------------------------------------------------
; void setdtr(int dtr)
; 1 = high, 2 = low
;-----------------------------------------------------------------------
PROC    _setdtr
        push    bp
        mov     bp, sp

        mov     ah, 06h                 ; 06 = set dtr
        mov     dx, [_port]             ; dx = comport
        mov     al, [bp+4]              ; al = 0 or 1
        int     14h

        pop     bp
        ret
ENDP    _setdtr

;-----------------------------------------------------------------------
; void set_fossil_flow(byte bitmask)
; sets cts/rts/xon/xoff flow control
;-----------------------------------------------------------------------
PROC    _set_fossil_flow
        push    bp
        mov     bp, sp

        mov     ah, 0Fh                 ; 0F = set flow control
        mov     dx, [_port]             ; dx = comport
        mov     al, [bp+4]              ; al = bits for xon/xoff/cts/rts
        int     14h

        pop     bp
        ret
ENDP    _set_fossil_flow

;-----------------------------------------------------------------------
; int fossil_cts(void)
; returns current CTS state
;-----------------------------------------------------------------------
PROC    _fossil_cts
        mov     ah, 03h                 ; 03 = serial port status
        mov     dx, [_port]             ; dx = comport
        int     14h                     ; FOSSIL interrupt
        and     al, 00010000b           ; test for CTS  (bit 4)
        shr     al, 4                   ; make it 0 or 1
        xor     ah, ah                  ;  no, set ax = 0
        ret                             ; al = key waiting
ENDP    _fossil_cts

;-----------------------------------------------------------------------
; int dv_there(void)
; returns DV version in AX,  or 0 if no DV
;-----------------------------------------------------------------------
PROC    _dv_there

        push    bx
        push    cx
        push    dx

        mov     cx, 'DE'        ; cx:dx = 'DESQ'
        mov     dx, 'SQ'
        mov     ax, 02B01h      ; DOS set date function
        int     21h
        cmp     al, 0FFh        ; did DOS see it as invalid?
        jz      NO_DESQview     ; if so, DESQview isn't there
        mov     ax, bx          ; AH=major version, AL=minor version
        jmp     DVGV_X          ;   other routines
NO_DESQview:
        xor     ax, ax          ;   and return 0
DVGV_X:
        pop     dx
        pop     cx
        pop     bx

        ret
ENDP    _dv_there

;-----------------------------------------------------------------------
; void dv_release(void)
;  releases remaining timeslice
;-----------------------------------------------------------------------
PROC    _dv_release
        mov     ax, 01000h
        int     15h
        ret
ENDP    _dv_release

;-----------------------------------------------------------------------
; byte kbhit(void)
; returns nonzero if hey is waiting in keyboard buffer (local)
;-----------------------------------------------------------------------
PROC    _kbhita
        mov    ah, 01h                  ; keyboard status service
        int    16h                      ; keyboard, i/o interrupt
        jz     NoKeyHit
        mov    ax, 01h                  ; key was hit, return true
        ret
NoKeyHit:
        xor    ax, ax                   ; no key hit, return zero
        ret
ENDP    _kbhita

;-----------------------------------------------------------------------
; int GetFossilInfo(word bufsize, word seg, word ofs)
; returns bytes transferred to buffer in AX
;-----------------------------------------------------------------------
PROC    _GetFossilInfo
        push    bp
        mov     bp, sp

        mov     cx, [bp+4]              ; cx = size of buffer
        mov     ax, [bp+6]              ; ax = segment of buffer
        mov     es, ax                  ; es = ax
        mov     di, [bp+8]              ; di = offset of buffer
        mov     dx, [_port]             ; dx = port
        mov     ah, 01Bh                ; GetDriverInfo fossil service
        int     14h                     ; call FOSSIL int

        pop     bp
        ret
ENDP    _GetFossilInfo

;-----------------------------------------------------------------------
; void farputs(word segment, word offset);
; send far string to stdout
;-----------------------------------------------------------------------
PROC    _farputs
        push    bp
        mov     bp, sp
        push    ds

        mov     dx, [bp+4]              ; dx = segment of string
        mov     ds, dx                  ; ds = dx
        mov     si, [bp+6]              ; dx = offset of string
        cld                             ;
@@next_char:
        lodsb                           ; load byte into al, increment si?di?
        or      al, al                  ; if end of string,
        jz      @@end_of_string         ; jump
        mov     dl, al                  ; else dl = al
        mov     ah, 02h                 ; DOS output char routine
        int     21h                     ; do it
        jmp     @@next_char
@@end_of_string:
        pop     ds
        pop     bp
        ret
ENDP    _farputs

        END
