VERSION		EQU	2
REVISION	EQU	8
DATE	MACRO
		dc.b	'29.10.96'
	ENDM
VERS	MACRO
		dc.b	'MagicMenuPrefs 2.8'
	ENDM
VSTRING	MACRO
		dc.b	'MagicMenuPrefs 2.8 (29.10.96)',13,10,0
	ENDM
VERSTAG	MACRO
		dc.b	0,'$VER: MagicMenuPrefs 2.8 (29.10.96)',0
	ENDM
