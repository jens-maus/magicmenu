VERSION		EQU	2
REVISION	EQU	8
DATE	MACRO
		dc.b	'2.11.96'
	ENDM
VERS	MACRO
		dc.b	'MagicMenu 2.8'
	ENDM
VSTRING	MACRO
		dc.b	'MagicMenu 2.8 (2.11.96)',13,10,0
	ENDM
VERSTAG	MACRO
		dc.b	0,'$VER: MagicMenu 2.8 (2.11.96)',0
	ENDM
