;
; serasm.asm - Low Level Communcations Routines
;              Copyright (c) 1992 by Mark Goodwin
;
; converted to borland IDEAL mode by minor threat

        IDEAL
        MODEL SMALL

;
; UART register constants
;
IIR             equ             2       ; Interrupt Identification Register
MCR             equ             4       ; Modem Control Register
LSR             equ             5       ; Line Status Register
MSR             equ             6       ; Modem Status Register

;
; UART bit mask constants
;
RX_RDY          equ              1
TX_RDY          equ             20h
INT_MASK        equ              7
RX_ID           equ              4
MC_INT          equ              8
DTR             equ              1
RTS             equ              2
CTS             equ             10h     ; Clear To Send mask
DSR             equ             20h     ; Data Set Ready mask
DCD             equ             80h     ; Carrier Detect
ICR             equ             20h     ; Interrupt control Register
EOI             equ             20h     ; End of Interrupt

;
; XON/XOFF constants
;
XON             equ             11h     ; ^Q
XOFF            equ             13h     ; ^S

;
; system clock location
;
CLOCK           equ             46Ch    ; 0000:046C = clock

;
; boolean values
;
TRUE            equ             1
FALSE           equ             0


DATASEG

;
; declare all variables as public
;
                public  _sibuff,_eibuff,_ilen,_inbuff
                public  _rx_flow,_rx_rts,_rx_dtr,_rx_xon
                public  _tx_rts,_tx_dtr,_tx_xon,_tx_xonoff
                public  _fon,_foff
                public  _base

;
; varibles
;
_sibuff         dw      ?               ; start of input buffer ptr
_eibuff         dw      ?               ; end of input buffer ptr
_ilen           dw      ?               ; input buffer length
_inbuff         dd      ?               ; input buffer far ptr
_rx_flow        dw      ?               ; input buffer full flag
_rx_rts         dw      ?               ; receive RTS/CTS flag
_rx_dtr         dw      ?               ; receive DTR/DSR flag
_rx_xon         dw      ?               ; receive XON/XOFF flag
_tx_rts         dw      ?               ; xmit RTS/CTS flag
_tx_dtr         dw      ?               ; xmit DTR/DSR flag
_tx_xon         dw      ?               ; xmit XON/XOFF flag
_tx_xonoff      dw      ?               ; xmit XOFF flag
_fon            dw      ?               ; point to turn receive flow on
_foff           dw      ?               ; point to turn receive flow off
_base           dw      ?               ; UART base ptr

CODESEG

;
; declare procedures public
;
                public  _get_serial,_put_serial,_handler,_mpeek

;-------------------------------------------------------------------
; chars_in_buff
; calculate number of character in input buffer
;-------------------------------------------------------------------
PROC    chars_in_buff
        mov     ax, [_eibuff]           ; ax=end of buffer ptr
        sub     ax, [_sibuff]           ; figure number of chars
        jae     cib1                    ; jump if ptrs haven't crossed
        mov     ax, [_ilen]             ; ax=buffer size
        sub     ax, [_sibuff]           ; ax=number chars to end of buffer
        add     ax, [_eibuff]           ; ax=number chars in buffer
cib1:
        ret                             ; return
ENDP    chars_in_buff

;----------------------------------------------------------------------
; int get_serial(void)
; get character from the serial port
;----------------------------------------------------------------------
PROC    _get_serial
        push    di                      ; save di
        push    es                      ; save es

        mov     ax, -1                  ; ax=character not available
        push    ax                      ; save it on stack
        mov     bx, [_sibuff]           ; bx=next character ptr
        cmp     bx, [_eibuff]           ; buffer empty?
        je      gs4                     ; jump if it is
        pop     ax                      ; remove dummy return value
        les     di, [_inbuff]           ; es:di=input buffer pt
; following line was "mov al, es:[di][bx]"
; modified for TASM's IDEAL mode
        mov     al, [es:di+bx]          ; al=next character
        xor     ah, ah                  ; ax=next character
        push    ax                      ; save it on stack
        inc     bx                      ; bump the next character ptr
        cmp     bx, [_ilen]             ; wrap ptr?
        jne     gs1                     ; jump if not
        xor     bx, bx                  ; point it to start of buffer
gs1:
        mov     [_sibuff], bx           ; save the new ptr
        cmp     [_rx_flow], TRUE        ; receive flow?
        jne     gs4                     ; jump if not
        call    chars_in_buff           ; ax=number of chars in buffer
        cmp     ax, [_fon]              ; turn back on?
        jg      gs4                     ; jump if not
        mov     [_rx_flow], FALSE       ; flag receive flow off
        cmp     [_rx_rts], TRUE         ; RTS/CTS?
        jne     gs2                     ; jump if not
        mov     dx, [_base]             ; dx=base ptr
        add     dx, MCR                 ; dx=modem control register
        in      al, dx                  ; get current value
        or      al, RTS                 ; assert RTS
        out     dx, al                  ; send new value to UART
gs2:
        cmp     [_rx_dtr], TRUE         ; DTR/DSR?
        jne     gs3                     ; jump if not
        mov     dx, [_base]             ; dx=base ptr
        add     dx, MCR                 ; dx=modem control register
        in      al, dx                  ; get current value
        or      al, DTR                 ; assert DTR
        out     dx, al                  ; send new value to UART
gs3:
        cmp     [_rx_xon], TRUE         ; XON/XOFF?
        jne     gs4                     ; Jump if not
        cli                             ; disable the interrupts
        mov     dx, [_base]             ; dx=xmit register
        mov     al, XON                 ; al=XON value
        out     dx, al                  ; send it to remote
        sti                             ; enable the interrupts
gs4:
        pop     ax                      ; get the character
        pop     es                      ; restore es
        pop     di                      ; restore di
        ret                             ; return
ENDP    _get_serial

;---------------------------------------------------------------------
; void put_serial(byte n)
; send a byte out through the serial port
;---------------------------------------------------------------------
PROC    _put_serial
        push    bp                      ; save bp
        mov     bp,sp                   ; bp=stack frame ptr
        push    di                      ; save di
        push    es                      ; save es

        xor     ax, ax                  ; ax=segment 0000h
        mov     es, ax                  ; es=segment 0000h ptr
        mov     di, CLOCK               ; es:di=system clock ptr
        mov     bx, [es:di]             ; bx=current value
        mov     cx, 18                  ; cx=1 second timeout value
        mov     dx, [_base]             ; dx=base register
        add     dx, MCR                 ; dx=modem control register
        ; ^ add was mov (mistake i think)
        in      al, dx                  ; al=current value
        or      al, MC_INT or DTR or RTS ; assert GPO2, DTR, RTS
        out     dx, al                  ; send it to UART
        cmp     [_tx_rts], TRUE         ; RTS/CTS?
        jne     ps2                     ; Jump if not
        mov     dx, [_base]             ; dx=base register
        add     dx, MSR                 ; dx=modem status register
ps1:
        in      al, dx                  ; al=current value
        and     al, CTS                 ; CTS asserted?
        jnz     ps2                     ; jump if it is
        cmp     bx, [es:di]             ; system clock changed?
        je      ps1                     ; loop if not
        mov     bx, [es:di]             ; bx=new system clock value
        loop    ps1                     ; loop till time out
        jmp     ps9                     ; jump for time out
ps2:
        cmp     [_tx_dtr], TRUE         ; DTR/DSR?
        jne     ps4                     ; jump if not
        mov     dx, [_base]             ; dx=base register
        add     dx, MSR                 ; dx=modem status register
ps3:
        in      al, dx                  ; al=current value
        and     al, DSR                 ; DSR asserted?
        jnz     ps4                     ; jump if it is
        cmp     bx, [es:di]             ; system clock changed?
        je      ps3                     ; loop if not
        mov     bx, [es:di]             ; bx=new system clock value
        loop    ps3                     ; loop till time out
        jmp     ps9                     ; jump for time out
ps4:
        cmp     [_tx_xon], TRUE         ; XON/XOFF?
        jne     ps6                     ; jump if not
ps5:
        cmp     [_tx_xonoff], TRUE      ; XOFF?
        jne     ps6                     ; jump if not
        mov     dx, [_base]             ; dx=base register
        add     dx, MSR                 ; dx=modem status register
        in      al, dx                  ; al=current value
        and     al, DCD                 ; carrier?
        jnz     ps5                     ; loop if it is
ps6:
        mov     dx, [_base]             ; dx=base register
        add     dx, LSR                 ; dx=line status register
ps7:
        in      al, dx                  ; al=current value
        and     al, TX_RDY              ; xmitter ready?
        jnz     ps8                     ; jump if it is
        cmp     bx, [es:di]             ; system clock changed?
        je      ps3                     ; loop if not
        mov     bx, [es:di]             ; bx=new system clock value
        loop    ps7                     ; loop till time out
        jmp     ps9                     ; jump for time out
ps8:
        cli                             ; disable the interrupts
        mov     dx, [_base]             ; dx=xmitter register
        mov     ax, [bp+4]              ; al=character to send
        out     dx, al                  ; send it
        sti                             ; enable the interrupts
ps9:
        pop     es                      ; restore es
        pop     di                      ; restore di
        mov     sp, bp                  ; restore the stack ptr
        pop     bp                      ; restore bp
        ret                             ; return
ENDP    _put_serial

;--------------------------------------------------------------------
; void interrupt handler(void)
; serial i/o interrupt handler
;--------------------------------------------------------------------
PROC    _handler      FAR
        push    ax                      ; save ax
        push    bx                      ; save bx
        push    cx                      ; save cx
        push    dx                      ; save dx
        push    si                      ; save si
        push    di                      ; save di
        push    bp                      ; save bp
        push    es                      ; save es
        push    ds                      ; save ds

        mov     ax,seg _base            ; ax=segment address
        mov     ds,ax                   ; ds=segment address
        mov     dx,[_base]              ; dx=base register
        add     dx,IIR                  ; dx=interrupt id register
        in      al,dx                   ; al=current value
        and     al,INT_MASK             ; mask it
        cmp     al,RX_ID                ; receive interrupt?
        je      h1                      ; jump if it is
        jmp     h8                      ; jump if not
h1:
        mov     dx,[_base]              ; dx=receive register
        in      al,dx                   ; al=new character
        cmp     [_tx_xon],TRUE          ; XON/XOFF?
        jne     h3                      ; jump if not
        cmp     al,XOFF                 ; XOFF?
        jne     h2                      ; jump if not
        mov     [_tx_xonoff],TRUE       ; flag XOFF
        jmp     h5                      ; jump
h2:
        cmp     al,XON                  ; XON?
        jne     h3                      ; jump if not
        mov     [_tx_xonoff],FALSE      ; flag not XOFF
        jmp     h5                      ; jump
h3:
        mov     [_tx_xonoff], FALSE     ; flag not XOFF
        mov     bx, [_eibuff]           ; bx=next char ptr
        les     di, [_inbuff]           ; es:di=input buffer ptr
; following line was "mov es:[di][bx],al"
; modified for TASM's IDEAL mode
        mov     [es:di+bx],al           ; save the char  ???
        inc     bx                      ; bump the buffer ptr
        cmp     bx,[_ilen]              ; wrap it?
        jne     h4                      ; jump if not
        xor     bx,bx                   ; point to start of buffer
h4:
        mov     [_eibuff],bx            ; save new ptr
h5:
        mov     dx,[_base]              ; dx=base register
        add     dx,LSR                  ; dx=line status register
        in      al,dx                   ; al=current value
        and     al,RX_RDY               ; another character available
        jnz     h1                      ; loop if it is
        cmp     [_rx_flow],TRUE         ; receive flow on?
        je      h8                      ; jump if it is
        call    chars_in_buff           ; ax=no chars in buffer
        cmp     ax,[_foff]              ; turn receive off?
        jb      h8                      ; jump if not
        mov     [_rx_flow],TRUE         ; flag receive flow on
        cmp     [_rx_rts],TRUE          ; RTS/CTS?
        jne     h6                      ; jump if not
        mov     dx,[_base]              ; dx=base register
        add     dx,MCR                  ; dx=modem control register
        in      al,dx                   ; al=current value
        and     al,not RTS              ; unassert RTS
        out     dx,al                   ; send it to UART
h6:
        cmp     [_rx_dtr],TRUE          ; DTR/DSR?
        jne     h7                      ; jump if not
        mov     dx,[_base]              ; dx=base register
        add     dx,MCR                  ; dx=modem control register
        in      al,dx                   ; al=current value
        and     al,not DTR              ; unassert DTR
        out     dx,al                   ; send it to UART
h7:
        cmp     [_rx_xon],TRUE          ; XON/XOFF?
        jne     h8                      ; jump if not
        mov     dx,[_base]              ; dx=xmit register
        mov     al,XOFF                 ; al=XOFF
        out     dx,al                   ; send it
h8:
        mov     dx,ICR                  ; dx=interrupt control register
        mov     al,EOI                  ; al=end of interrupt command
        out     dx,al                   ; send it

        pop     ds                      ; restore ds
        pop     es                      ; restore es
        pop     bp                      ; restore bp
        pop     di                      ; restore di
        pop     si                      ; restore si
        pop     dx                      ; restore dx
        pop     cx                      ; restore cx
        pop     bx                      ; restore bx
        pop     ax                      ; restore ax
        iret                            ; return
ENDP    _handler

;---------------------------------------------------------------------
; word mpeek(word seg, word off)
; get word of memory routine
;---------------------------------------------------------------------
PROC    _mpeek
srcseg          equ     <[bp + 4]>      ; segment parameter
srcoff          equ     <[bp + 6]>      ; offset parameter

        push    bp                      ; save bp
        mov     bp,sp                   ; bp=stack frame ptr
        push    di                      ; save di
        push    es                      ; save es

        mov     es, srcseg              ; es=segment address
        mov     di, srcoff              ; di=offset address
        mov     ax, [es:di]             ; ax=memory word

        pop     es                      ; restore es
        pop     di                      ; restore di
        pop     bp                      ; restore bp
        ret                             ; return;
ENDP    _mpeek

        END
