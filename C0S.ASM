; C0S.ASM -- Assembly Language Startup Routine for HP95LX System Manager
;	Compliant Programs
;	Turbo C/ANSI Version, must be Small model (64K code, separate 64K data)
;	Written March 1992 by Craig A. Finseth

	DOSSEG			; macro that defines segment structure

	.model	small		; specifies model

	.stack	16384		; this defines your stack size

	.data?			; start data segment

segpad	db	15 dup (?)	; this is unused, but ensures that
				; the code ends in a different
				; paragraph from where the usable data
				; starts

	.code			; switch to code

	org	10H		; skip 16 bytes

	end			; that's it, the program counter will
				; drop through to whatever is next
