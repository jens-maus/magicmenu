/*
**	$Id$
**
**	:ts=8
*/

/* ************************************************************************ */
/* ************************************************************************ */
/* */
/*             System Variablen                                             */
/* */
/* ************************************************************************ */
/* ************************************************************************ */

extern struct Library   *IntuitionBase;
extern struct Library   *GfxBase;
extern struct Library   *AslBase;
extern struct Library *UtilityBase;
extern struct ExecBase  *SysBase;
extern struct DOSBase   *DOSBase;
extern struct Library   *ColorWheelBase;
extern struct Library   *GradientSliderBase;


extern ULONG   GfxVersion;

/* ************************************************************************ */
/* ************************************************************************ */
/* */
/*             Definierte Variablen                                         */
/* */
/* ************************************************************************ */
/* ************************************************************************ */

extern struct MMPrefs DefaultPrefs;


extern struct Image    SampleMenuImage;

/* ************************************************************************ */
/* ************************************************************************ */
/* */
/*             Variablen                                                    */
/* */
/* ************************************************************************ */
/* ************************************************************************ */

extern struct FileRequester    *FileReq;
extern struct MsgPort          *ReplyPort;

extern struct MMPrefs AktPrefs;
extern char ConfigPath[201];
extern char ConfigFile[232];
extern char    Cx_Popkey[100];
extern char    WindowTitle[200];
extern BOOL    MMEnabled;

extern BOOL     PensAllocated;
extern UWORD    GradPens;
extern UWORD    GradientPens[30];
extern UBYTE    RemapArray[256];

extern struct ColorWheelRGB        ColorWheelRGB;

extern struct Image    *SampleMenuImageRmp;

extern UWORD MenLightEdge;
extern UWORD MenDarkEdge;
extern UWORD MenBackground;
extern UWORD MenTextCol;
extern UWORD MenHiCol;
extern UWORD MenFillCol;
extern UWORD MenGhostLoCol;
extern UWORD MenGhostHiCol;
extern UWORD MenXENGrey0;
extern UWORD MenXENBlack;
extern UWORD MenXENWhite;
extern UWORD MenXENBlue;
extern UWORD MenXENGrey1;
extern UWORD MenXENGrey2;
extern UWORD MenXENBeige;
extern UWORD MenXENPink;

extern UWORD *AktPen;
extern struct RGBSet *AktPrefsCol;

extern struct Library          *GTLayoutBase;
extern LayoutHandle             *GUI_Handle;
extern struct Menu              *GUI_Menu;
extern struct Screen            *GUI_Screen;


