VERSION		EQU	2
REVISION	EQU	9
DATE	MACRO
		dc.b	'5.11.96'
	ENDM
VERS	MACRO
		dc.b	'MagicMenuPrefs 2.9'
	ENDM
VSTRING	MACRO
		dc.b	'MagicMenuPrefs 2.9 (5.11.96)',13,10,0
	ENDM
VERSTAG	MACRO
		dc.b	0,'$VER: MagicMenuPrefs 2.9 (5.11.96)',0
	ENDM
