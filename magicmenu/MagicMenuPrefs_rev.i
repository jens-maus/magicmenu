VERSION		EQU	2
REVISION	EQU	15
DATE	MACRO
		dc.b	'22.12.96'
	ENDM
VERS	MACRO
		dc.b	'MagicMenuPrefs 2.15'
	ENDM
VSTRING	MACRO
		dc.b	'MagicMenuPrefs 2.15 (22.12.96)',13,10,0
	ENDM
VERSTAG	MACRO
		dc.b	0,'$VER: MagicMenuPrefs 2.15 (22.12.96)',0
	ENDM
