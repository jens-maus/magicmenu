VERSION		EQU	2
REVISION	EQU	31

DATE	MACRO
		dc.b '2.3.2001'
		ENDM

VERS	MACRO
		dc.b 'MagicMenuPrefs 2.31'
		ENDM

VSTRING	MACRO
		dc.b 'MagicMenuPrefs 2.31 (2.3.2001)',13,10,0
		ENDM

VERSTAG	MACRO
		dc.b 0,'$VER: MagicMenuPrefs 2.31 (2.3.2001)',0
		ENDM
