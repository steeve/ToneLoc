;
; TLVGA.ASM - VGA Assembly Routines for ToneMap
;             by some lamer, converted to TASM
;             and _optimized_ by Minor Threat!
;
; (in Turbo Assembler)
;

        IDEAL
        MODEL   SMALL

EQUIP_FLAG      EQU     [byte ptr ds:10h]

CGAbits         EQU     00100000b       ; bits for EQUIP_FLAG
MDAbits         EQU     00110000b       ;

        DATASEG

        CODESEG

        public  _kbhita
        public  _ReadColor
        public  _SetPixel
        public  _SetVmode

;-----------------------------------------------------------------------
; byte kbhit(void)
; returns nonzero if key is waiting in keyboard buffer (local)
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

;----------------------------------------------------------------------
; byte ReadColor(x,y);         - reads pixel attribute in 320x200x256
; x,y are coordinates to read
;----------------------------------------------------------------------
PROC    _ReadColor
        push    bp                      ;  preserve caller registers
        mov     bp,sp

        mov     ax, [bp+6]              ; AX := y
        mov     bx, [bp+4]              ; BX := x
        call    PixelAddr13             ; ES:BX -> buffer

        mov     al, [es:bx]             ; read from screen to AL
        xor     ah, ah                  ; ah = 0

        mov     sp,bp                   ; restore caller registers and
        pop     bp                      ; return
        ret
ENDP    _ReadColor


;----------------------------------------------------------------------
; PixelAddr13      - calculate buffer address of pixel in mode 13h
;  AX = y  (0-199)
;  BX = x  (0-319)
; returns:
;  ES = segment
;  BX = offset
;----------------------------------------------------------------------
PROC    PixelAddr13

        xchg    ah, al                  ; AX := 256*y
        add     bx, ax                  ; BX := 256*y + x
        shr     ax, 1
        shr     ax, 1                   ; AX := 64*y
        add     bx, ax                  ; BX := 320*y + x

        add     bx, 0                   ; BX := byte offset in video
                                        ; buffer
        mov     ax, 0A000h
        mov     es, ax                  ; ES:BX := byte address of pixel
        ret
ENDP    PixelAddr13


;----------------------------------------------------------------------
; void SetPixel(x,y,n);        - sets pixel in 320x200x256 (MCGA) mode
; x,y are coordinates, n is color
;----------------------------------------------------------------------
PROC    _SetPixel
        push    bp                      ;  preserve caller registers
        mov     bp,sp

        mov     ax, [bp+6]              ; AX := y
        mov     bx, [bp+4]              ; BX := x
        call    PixelAddr13             ; ES:BX -> buffer

        mov     al, [bp+8]              ; AL := pixel value
        mov     ah, al;                 ; AH := pixel value

        mov     [es:bx], ax             ; move to screen (2 bytes)
        add     bx, 320
        mov     [es:bx], ax

;       mov     sp,bp                   ; restore caller registers and
        pop     bp                      ; return
        ret
ENDP    _SetPixel

;----------------------------------------------------------------------
; void SetVmode(int);
; sets video mode
;----------------------------------------------------------------------
PROC    _SetVmode
        push    bp              ; preserve caller registers
        mov     bp,sp
        push    ds

        mov     ax,40h
        mov     ds,ax           ; DS -> Video Display Data Area

        mov     bl,CGAbits      ; BL := bits indicating presence
                                ; of CGA
        mov     al, [bp+4]      ; AL :=  desired video mode #

        mov     ah,al           ; test if desired mode is
                                        ; monochrome
        and     ah,7
        cmp     ah,7
        jne     L01             ; jump if desired mode not 7 or 0Fh

        mov     bl,MDAbits      ; BL := bits indicating presence of MDA

L01:    and     EQUIP_FLAG,11001111b
        or      EQUIP_FLAG,bl   ; sets bits in EQUIP_FLAG

        xor     ah,ah           ; AH := 0 (INT 10h function #)

        push    bp
        int     10h             ; call ROM BIOS to set the video mode
        pop     bp

        pop     ds              ; restore caller registers and return
        mov     sp,bp
        pop     bp
        ret
ENDP    _SetVmode

        END
