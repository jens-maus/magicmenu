/*
**   $Id$
**
**   :ts=8
*/

#define MAGIC_MENU_NAME   "Magic Menu"
#define MAGIC_MENU_DESCR  "Improves the Intuition menus"

#define INPUTNAME         "input.device"

#define ANSWER_LEN        10
#define LONGANSWER_LEN    99

#define TT_CX_PRI         "CX_PRIORITY"
#define TT_CX_POPUP       "CX_POPUP"
#define TT_CX_POPKEY      "CX_POPKEY"

#define DT_CX_PRI         0
#define DT_CX_POPUP       "YES"
#define DT_CX_POPKEY      "control alt space"

#define ESC         0x45
#define RETURN      0x44
#define ENTER       0x43
#define CRSUP       0x4c
#define CRSDOWN     0x4d
#define CRSLEFT     0x4f
#define CRSRIGHT    0x4e
#define HELP        0x5f
#define CTRL        0x63

#define STRIP_YDIST    2
#define LEFT_OVERLAP   3

#define EVT_KBDMENU         1
#define EVT_MOUSEMENU       2
#define EVT_MOUSEMOVE       3
#define EVT_POPPREFS        4
#define EVT_KEYBOARD        5

#define SENDINTUI_OK            0
#define SENDINTUI_NOPORT        1
#define SENDINTUI_NOUSERPORT    2
#define SENDINTUI_TIMEOUT       3

#define PREF_IDCMPFLAGS        (IDCMP_CLOSEWINDOW | IDCMP_REFRESHWINDOW | IDCMP_GADGETDOWN | IDCMP_GADGETUP | IDCMP_MENUPICK | IDCMP_VANILLAKEY)

#define CHECKIMGWIDTH           (CHECKWIDTH - 4)
#define LOWCHECKIMGWIDTH        (LOWCHECKWIDTH - 4)

#define MINTERM_ZERO		0
#define MINTERM_ONE		ABC | ABNC | ANBC | ANBNC | NABC | NABNC | NANBC | NANBNC
#define MINTERM_COPY		ABC | ABNC | NABC | NABNC
#define MINTERM_NOT_C		ABNC | ANBNC | NABNC | NANBNC
#define MINTERM_B_AND_C		ABC | NABC
#define MINTERM_NOT_B_AND_C	ANBC | NANBC
#define MINTERM_B_OR_C		ABC | ABNC | NABC | NABNC | ANBC | NANBC
#define MINTERM_B_XOR_C		ANBC | NANBC | ABNC | NABNC

#define PORTMASK(p)	(1L<<((p)->mp_SigBit))

#define MAX_FILENAME_LENGTH	256

#define REG(x)	register __##x

#define DB(x)	;
/*#define DB(x) x */
