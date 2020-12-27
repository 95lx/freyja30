; LIBASM.ASM -- Assembly Language DOS Interface Routines
;	Turbo C/ANSI Version, assumes Small model (64K code, separate 64K data)
;	Written April 1991 by Craig A. Finseth

; -------------------- Constants and Macros --------------------

TRUE	equ	255
FALSE	equ	0
NUL	equ	0
BEL	equ	7
CR	equ	13
LF	equ	10
SPACE	equ	32

; C function call stack layout -- SI, DI, BP, and SP must be preserved

old_bp	equ	0
ret_addr equ	2
arg1	equ	4
arg2	equ	6
arg3	equ	8
arg4	equ	10
arg5	equ	12
arg6	equ	14
arg7	equ	16
arg8	equ	18

; "safe" stack layout -- BP is not saved

sret_addr equ	0
sarg1	equ	2
sarg2	equ	4
sarg3	equ	6
sarg4	equ	8
sarg5	equ	10
sarg6	equ	12
sarg7	equ	14
sarg8	equ	16

clear	macro	x
	xor		x,x
	endm

zero	macro	x
	xor		x,x
	endm

testz	macro	x
	or		x,x
	endm

; -------------------- Data Segment --------------------

_DATA	segment	word public 'DATA'	; initialized data
	extrn	_appseg : word
	extrn	_appoff : word

	extrn	_lolseg : word
	extrn	_loloff : word
_DATA	ends

_BSS	segment	word public 'BSS'	; uninitialized data
cursor_size dw	?
_BSS	ends

DGROUP	group	_DATA, _BSS
	assume	ds:DGROUP

; -------------------- Code Segment --------------------

_TEXT	segment	byte public 'CODE'
	assume	cs:_TEXT

	public	_BGetKey, _BGetKeyE, _BIsKey, _BIsKeyE
	public	_BlockGet
	public	_c_service
	public	_c_service_far_ptr
	public	_DOSGetDir
	public	_find_str
	public	_GetSysDS
	public	_JFiniA, _JInitA
	public	_JGetType
	public	_JLightOff, _JLightOn
	public	_ListOfLists
	public	_lseeka
	public	_nearfar
	public	_PageGet, _PagePut
	public	_PSystem
	public	_SwapAlloc, _SwapFree
	public	_VidInit, _VidFini
	public	_VidBell, _VidChar, _VidClear, _VidCursor
	public	_VidCurOff, _VidCurOn

; ------------------------------------------------------------

; Return the next key.

;	int
;	BGetKey(void)
;	C Callable

_BGetKey proc	near
	zero	AH
	int	16H
	ret
_BGetKey endp


; ------------------------------------------------------------

; Return the next key using extended key input call.

;	int
;	BGetKeyE(void)
;	C Callable

_BGetKeyE proc	near
	mov	AH, 10H
	int	16H
	ret
_BGetKeyE endp


; ------------------------------------------------------------

; Return non-zero if a key is available or zero if not.

;	int
;	BIsKey(void)
;	C Callable

_BIsKey proc	near
	mov	AH,01H
	int	16H
	jz	bik_none
	mov	AX,1
	ret
bik_none: zero	AX
	ret
_BIsKey endp


; ------------------------------------------------------------

; Return non-zero if a key is available or zero if not using
; extended keyboard input checks.

;	int
;	BIsKeyE(void)
;	C Callable

_BIsKeyE proc	near
	mov	AH,11H
	int	16H
	jz	bike_none
	mov	AX,1
	ret
bike_none: zero	AX
	ret
_BIsKeyE endp


; ------------------------------------------------------------

; Get a block of data from high memory.

;	void
;	BlockGet(char *to, int len, unsigned fromseg)
;	C Callable

;	do	{
;		*to++ = *from++;
;		} while (--len > 0);

_BlockGet proc	near
	push	BP
	mov	BP,SP
	push	DI
	push	SI
	push	DS
	push	ES

	mov	DI,[BP] + arg1		; to
	mov	AX,DS			;  & segment
	mov	ES,AX

	mov	SI,0			; from
	mov	AX,[BP] + arg3		;  & segment
	mov	DS,AX

	mov	CX,[BP] + arg2		; CX is len
	cld

;mloop:	mov	AL,DS:[SI]		; *from++
;	inc	SI
;	mov	ES:[DI],AL		; *to++ =
;	inc	DI
;	loop	mloop			; while --len>0

	rep	movsb

	pop	ES
	pop	DS
	pop	SI
	pop	DI
	pop	BP
	ret
_BlockGet endp


; ------------------------------------------------------------

; Link to Jaguar special services.

_c_service proc near
	push	BP
	mov	BP,SP
	xchg	DI,[BP+4]
	pop	BP
	int	60H
	ret
_c_service endp


; ------------------------------------------------------------

; Link to Jaguar special services: return far pointer.

_c_service_far_ptr proc near
	push	BP
	mov	BP,SP
	xchg	DI,[BP+4]
	pop	BP
	int	60H
	ret
_c_service_far_ptr endp


; ------------------------------------------------------------
; Return the current directory for the specified drive.

;	void
;	DOSGetDir(int drive, char *buf)
;	C callable

_DOSGetDir proc near
	push	BP
	mov	BP, SP
	push	SI
	mov	BX, [BP + arg2]
	mov	SI, BX
	mov	DX, [BP + arg1]
	mov	AH, 47H
	clc
	int	21H
	pop	SI
	pop	BP
	ret
_DOSGetDir endp


; ------------------------------------------------------------

; Code for this routine taken from "findstr" written by someone
; at Hewlett Pakard (Everett Kaser?)

;	FLAG
;	find_str(char *str, unsigned seg, unsigned offset)
;	C callable

_find_str proc near
	push	BP
	mov	BP,SP
	push	ES
	mov	SI,[BP+arg1]
	mov	AX,[BP+arg2]
	mov	ES,AX
	mov	DI,[BP+arg3]
	cld

find1:
	push	SI
	lodsb
find2:
	sub	AL,1
	cmp	AL,ES:[DI]
	jz	find3
	add	AL,1
	inc	DI
	jnz	find2
	jmp	findfail
find3:
	add	AL,1
	push	DI
	inc	DI
find4:
	lodsb
	testz	AL
	jz	findfound

	sub	AL,1
	cmp	AL,ES:[DI]
	jnz	find5
	add	AL,1
	inc	DI
	jnz	find4
	pop	DI
	jmp	findfail
find5:
	add	AL,1
	pop	DI
	inc	DI
	pop	SI
	jmp	find1

findfail:
	pop	SI
	zero	AX
	pop	ES
	pop	BP
	ret

findfound:
	pop	DI
	pop	SI
	mov	AX,1
	mov	_appoff,DI
	mov	BX,ES
	mov	_appseg,BX
	pop	ES
	pop	BP
	ret
_find_str endp


; ------------------------------------------------------------

;	unsigned
;	GetSysDS(void)
;	C Callable

_GetSysDS proc near
	push	DS
	int	61H		; get the System Manager data segment
	mov	AX,DS
	pop	DS
	ret
_GetSysDS endp


; ------------------------------------------------------------

; Terminate any special HP95LX processing.

;	void
;	JFiniA(void)
;	C Callable

_JFiniA	proc	near
	ret
_JFiniA	endp


; ------------------------------------------------------------

; Initialize any special HP95LX processing.

;	void
;	JInitA(void)
;	C Callable

_JInitA	proc	near
	ret
_JInitA	endp


; ------------------------------------------------------------

; Get the model type.  Usable only on HP* models.

;	int
;	JGetType(void)
;	C Callable

_JGetType proc	near
	mov	AX, 4DD4H
	int	15H
	mov	AX, 53H		; 'S'

	cmp	CL, 2
	jne	jget95
	mov	AX, 43H		; 'C'

jget95:	ret
_JGetType endp


; ------------------------------------------------------------

; Turn off the light sleep mode when checking the key press status.
; HP95LX specific.

;	void
;	JLightOff(void)
;	C Callable

_JLightOff proc	near
	mov	AX,4E00H
	int	15H
	ret
_JLightOff endp


; ------------------------------------------------------------

; Turn on the light sleep mode when checking the key press status.
; HP95LX specific.

;	void
;	JLightOn(void)
;	C Callable

_JLightOn proc	near
	mov	AX,4E01H
	int	15H
	ret
_JLightOn endp


; ------------------------------------------------------------

;	void
;	ListOfLists(void)
;	C Callable

_ListOfLists proc near
	push	ES
	mov	AH,52H
	int	21H		; undocumented Get List Of Lists
	mov	_loloff,BX
	mov	BX,ES
	mov	_lolseg,BX
	pop	ES
	ret
_ListOfLists endp


; ------------------------------------------------------------

;	long
;	lseeka(int fd, long dist, int mode)
;	C callable

_lseeka	proc	near
	mov	BX,SP
	mov	DX,[BX] + sarg2
	mov	CX,[BX] + sarg3
	mov	AX,[BX] + sarg4
	mov	AH,42H
	mov	BX,[BX] + sarg1
	int	21H
	jnc	lseek_a
	mov	DX,-1
	neg	AX
lseek_a: ret
_lseeka	endp


; ------------------------------------------------------------
;	A call to this routine is patched in with makeexm -p.  This
;	routine fixes up the stack and converts a near call into a far call.

_nearfar proc	near

; the stack now has:
;		SP	return address, which needs to be bumped

	push	AX	; reserve space for CS
	push	AX	; reserve space for where we jump to
	pushf
	push	AX	; save to make working room
	push	BX
	mov	BX,SP

; the stack now has:
;		SP + 10	return address
;		SP + 8	placeholder
;		SP + 6	placeholder
;		SP + 4	flags
;		SP + 2	saved AX
;		SP	saved BX

	mov	AX,[BX + 10]
	inc	AX		; skip over our data
	inc	AX
	mov	[BX + 8],AX	; put in correct place
	mov	AX,CS
	mov	[BX + 10],AX	; put in correct place

; the stack now has:
;		SP + 10	CS
;		SP + 8	correct return IP
;		SP + 6	place for us to jump to (not initialized yet)
;		SP + 4	flags
;		SP + 2	saved AX
;		SP	saved BX

	mov	AX,[BX + 8]
	dec	AX
	dec	AX
	push	BX
	mov	BX,AX
	mov	AX,CS:[BX]
	pop	BX
	mov	[BX + 6],AX

; the stack now has:
;		SP + 10	CS
;		SP + 8	correct IP for the far return
;		SP + 6	place for us to jump to
;		SP + 4	flags
;		SP + 2	saved AX
;		SP	saved BX

	pop	BX
	pop	AX
	popf
	ret		; take off!
_nearfar endp


; ------------------------------------------------------------

; Get a page of data from high memory.

;	void
;	PageGet(char *to, char huge *from, int len)
;	C Callable

;	do	{
;		*to++ = *from++;
;		} while (--len > 0);

_PageGet proc	near
	push	BP
	mov	BP,SP
	push	DI
	push	SI
	push	DS
	push	ES

	mov	DI,[BP] + arg1		; to
	mov	AX,DS			;  & segment
	mov	ES,AX

	mov	SI,[BP] + arg2		; from
	mov	AX,[BP] + arg3		;  & segment
	mov	DS,AX

	mov	CX,[BP] + arg4		; CX is len
	cld

;mloop:	mov	AL,DS:[SI]		; *from++
;	inc	SI
;	mov	ES:[DI],AL		; *to++ =
;	inc	DI
;	loop	mloop			; while --len>0

	rep	movsb

	pop	ES
	pop	DS
	pop	SI
	pop	DI
	pop	BP
	ret
_PageGet endp


; ------------------------------------------------------------

; Put a page of data into high memory.

;	void
;	PagePut(char huge *to, char *from, int len)
;	C Callable

;	do	{
;		*to++ = *from++;
;		} while (--len > 0);

_PagePut proc	near
	push	BP
	mov	BP,SP
	push	DI
	push	SI
	push	ES

	mov	DI,[BP] + arg1		; to
	mov	AX,[BP] + arg2		;  & segment
	mov	ES,AX

	mov	SI,[BP] + arg3		; from

	mov	CX,[BP] + arg4		; CX is len
	cld

;mloop:	mov	AL,DS:[SI]		; *from++
;	inc	SI
;	mov	ES:[DI],AL		; *to++ =
;	inc	DI
;	loop	mloop			; while --len>0

	rep	movsb

	pop	ES
	pop	SI
	pop	DI
	pop	BP
	ret
_PagePut endp


; ------------------------------------------------------------

;	int
;	PSystem(int AH, ...) /* int DX, int CX, int BX, int AL, int SI, int DI */
;	C callable
;	Note:  if the carry flag is set, the return code is negated

_PSystem proc	near
	push	BP
	mov	BP,SP
	push	DI
	push	SI

	mov	AX,[BP] + arg5		; AL
	mov	BX,[BP] + arg1		; AH
	mov	AH,BL
	mov	DX,[BP] + arg2
	mov	CX,[BP] + arg3
	mov	BX,[BP] + arg4
	mov	SI,[BP] + arg6
	mov	DI,[BP] + arg7

	clc
	int	21H
	jnc	psyst_ok
	neg	AX
	pop	SI
	pop	DI
	pop	BP
	ret

psyst_ok: and	AX,7FFFH
	pop	SI
	pop	DI
	pop	BP
	ret
_PSystem endp


; ------------------------------------------------------------

; Allocate a block of memory and return the starting paragraph number
; or zero.

;	unsigned
;	SwapAlloc(unsigned para)
;	C Callable

_SwapAlloc proc near
	mov	BX,SP
	mov	BX,[BX] + sarg1
	mov	AH,48H
	clc
	int	21H
	jc	alloc_no
	ret
alloc_no: zero	AX
	ret
_SwapAlloc endp


; ------------------------------------------------------------

; Free a block of memory.

;	void
;	SwapFree(unsigned para)
;	C Callable

_SwapFree proc	near
	mov	BX,SP
	mov	BX,[BX] + sarg1
	push	ES
	mov	ES,BX
	mov	AH,49H
	int	21H
	pop	ES
	ret
_SwapFree endp


; ------------------------------------------------------------

; Initialize the screen to the specified mode and zoom.  Also, set the
; cursor to end at the specified line.

;	int
;	VidInit(int mode, int zoom, int curline)
;	C callable

_VidInit proc	near
	push	BP
	mov	BP, SP

	mov	AH, 0FH		; get current video mode
	int	10H
	and	AX, 0FFH
	push	AX		; save old mode

	mov	AX, [BP] + arg1
	cmp	AX, 100		; means to not set it
	je	vidi_a	

	zero	AH		; set the video mode
	int	10H

vidi_a:
	mov	AX, [BP] + arg2
	cmp	AX, 0		; means to not set it
	je	vidi_b
	mov	AH, 0D0H
	int	10H

vidi_b:
	mov	CX, [BP] + arg3
	mov	cursor_size, CX
	and	CX, 000FH
	jz	vidi_c
	mov	AH, 01H		; set the cursor type
	int	10H

vidi_c:
	pop	AX		; old mode
	pop	BP
	ret
_VidInit endp


; ------------------------------------------------------------

; Restore the screen to the specified mode.

;	void
;	VidFini(int mode)
;	C callable

_VidFini proc	near
	push	BP
	mov	BP, SP
	mov	AX, [BP] + arg1
	zero	AH		; set the video mode
	int	10H
	pop	BP
	call	_VidCurOn
	ret
_VidFini endp


; ------------------------------------------------------------

; Ring the bell.  Usable for both BIOS and direct memory output.

;	void
;	VidBell(void)
;	C callable

_VidBell proc	near
	mov	AL,BEL
	zero	BL
	mov	AH,14		; write TTY
	int	10H
	ret
_VidBell endp


; ------------------------------------------------------------

; Write the specified character and attribute to the current cursor location. 
; Usable for BIOS output only.

;	void
;	VidChar(char char, char attr)
;	C callable

_VidChar proc	near
	push	BP
	mov	BP,SP

	mov	AL,[BP] + arg1
	mov	BX,[BP] + arg2
	zero	BH			; page
	mov	CX,1			; repeat count

	mov	AH,9			; write char and attribute
	int	10H
	pop	BP
	ret
_VidChar endp


; ------------------------------------------------------------

; Clear the next COUNT characters.  Usable for BIOS output only.

;	void
;	VidClear(int count, char attrib)
;	C callable

_VidClear proc	near
	push	BP
	mov	BP,SP

	mov	AL,SPACE
	mov	BX,[BP] + arg2
	zero	BH			; page
	mov	CX,[BP] + arg1		; repeat count: # of char to clear
	test	CX,0FFFFH
	jz	vidc_ret

	mov	AH,9			; write char and attribute
	int	10H
vidc_ret:
	pop	BP
	ret
_VidClear endp


; ------------------------------------------------------------

; Set the cursor to the specified row/column.  Usable for both BIOS
; and direct memory output.

;	void
;	VidCursor(int row, int col)
;	C callable

_VidCursor proc	near
	push	BP
	mov	BP,SP

	mov	CL,[BP] + arg1
	mov	DL,[BP] + arg2		; col
	mov	DH,CL			; row

	zero	BH			; page
	mov	AH,2			; set cursor position
	int	10H
	pop	BP
	ret
_VidCursor endp


; ------------------------------------------------------------

; Tur the cursor off.

;	void
;	VidCurOff(void)
;	C callable

_VidCurOff proc near
	mov	AH,1		; set cursor size
	mov	CX,0FF00H
	int	10H
	ret
_VidCurOff endp


; ------------------------------------------------------------

; Set the cursor to a block cursor.

;	void
;	VidCurOn(void)
;	C callable

_VidCurOn proc near
	mov	AH, 1		; set cursor size
	mov	CX, cursor_size
	int	10H
	ret
_VidCurOn endp

_TEXT	ends

		end

; end of LIBASM.ASM -- Assembly Language DOS Interface Routines
