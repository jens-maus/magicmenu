VERSION		EQU	2
REVISION	EQU	25
DATE	MACRO
		dc.b	'9.3.99'
	ENDM
VERS	MACRO
		dc.b	'MagicMenuPrefs 2.25'
	ENDM
VSTRING	MACRO
		dc.b	'MagicMenuPrefs 2.25 (9.3.99)',13,10,0
	ENDM
VERSTAG	MACRO
		dc.b	0,'$VER: MagicMenuPrefs 2.25 (9.3.99)',0
	ENDM
