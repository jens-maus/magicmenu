VERSION		EQU	2
REVISION	EQU	26

DATE	MACRO
		dc.b '11.10.1999'
		ENDM

VERS	MACRO
		dc.b 'MagicMenu 2.26'
		ENDM

VSTRING	MACRO
		dc.b 'MagicMenu 2.26 (11.10.1999)',13,10,0
		ENDM

VERSTAG	MACRO
		dc.b 0,'$VER: MagicMenu 2.26 (11.10.1999)',0
		ENDM
