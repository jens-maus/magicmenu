/*
**   $Id$
**
**   :ts=8
*/

extern struct DosLibrary *DOSBase;
extern struct ExecBase *SysBase;
extern struct Library *CxBase;
extern struct Library *IconBase;
extern struct Library *LayersBase;
extern struct IntuitionBase *IntuitionBase;
extern struct GfxBase *GfxBase;
extern struct Library *TimerBase;
extern struct Library *InputBase;
extern struct Library *GadToolsBase;
extern struct Library *KeymapBase;
extern struct Library *UtilityBase;

extern struct Library *LocaleBase;
extern struct Catalog *Catalog;

extern BOOL V39;

extern struct MsgPort *TimerPort;
extern struct timerequest *TimerIO;

extern struct MsgPort *InputPort;
extern struct IOStdReq *InputIO;

extern BOOL MagicActive;

extern UWORD MenuMainTimeOut;

extern CxObj *Broker;
extern CxObj *KbdFilter;
extern CxObj *StdKbdFilter;
extern CxObj *StdKbdSender;
extern CxObj *StdKbdTransl;
extern CxObj *PrefsFilter;
extern CxObj *MouseFilter;
extern CxObj *MouseSender;
extern CxObj *MouseTransl;
extern CxObj *ActKbdFilter;
extern CxObj *ActKbdSender;
extern CxObj *ActKbdTransl;
extern CxObj *MouseMoveFilter;
extern CxObj *MouseMoveSender;
extern CxObj *MousePositionFilter;
extern CxObj *MousePositionSender;
extern CxObj *MouseNewPositionFilter;
extern CxObj *MouseNewPositionSender;
extern CxObj *TickFilter;
extern CxObj *TickSignal;
extern struct MsgPort *CxMsgPort;
extern struct MsgPort *ReplyPort;
extern struct MsgPort *MMMsgPort;
extern LONG TickSigNum;
extern ULONG TickSigMask;

extern struct MsgPort *IMsgReplyPort;
extern ULONG IMsgReplyCount;

extern struct MsgPort *TimeoutPort;
extern struct timerequest *TimeoutRequest;

extern struct SignalSemaphore *GetPointerSemaphore;
extern struct SignalSemaphore *MenuActSemaphore;
extern struct SignalSemaphore *RememberSemaphore;

extern BOOL LayersLocked;
extern BOOL CxChanged;
extern BOOL LibPatches;


extern struct TextFont *MenFont;
extern struct TextFont *MenOpenedFont;

extern struct MenuItem *FirstSelectedMenu;
extern struct MenuItem *LastSelectedMenu;
extern UWORD FirstMenuNum;
extern UWORD MenuNum;
extern UWORD SubItemNum;
extern UWORD ItemNum;

extern UWORD MenLightEdge;
extern UWORD MenDarkEdge;
extern UWORD MenBackGround;
extern UWORD MenTextCol;
extern UWORD MenHiCol;
extern UWORD MenFillCol;
extern UWORD MenBlockPen;
extern UWORD MenDetailPen;
extern UWORD MenTrimPen;
extern UWORD MenComplMsk;
extern UWORD MenMenuTextCol;
extern UWORD MenMenuBackCol;
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

extern UWORD MenSectGrey;

extern UWORD MenStdGrey0;
extern UWORD MenStdGrey1;
extern UWORD MenStdGrey2;

extern UWORD MenActiveGrey0;
extern UWORD MenActiveGrey1;
extern UWORD MenActiveGrey2;

extern UWORD MenSeparatorGrey0;
extern UWORD MenSeparatorGrey2;

extern UBYTE SelBoxOffs;
extern BOOL Look3D;
extern BOOL LookMC;
extern BOOL ScrHiRes;
extern BOOL HelpPressed;
extern BOOL DblBorder;

extern struct MenuRemember *GlobalRemember;
extern struct MenuRemember *AktMenuRemember;
extern struct MenuRmb *AktMenRmb;
extern struct ItemRmb *AktItemRmb;

extern UWORD StripWidth;
extern UWORD StripHeight;
extern UWORD StripDepth;
extern UWORD StripTopOffs;
extern UWORD StripLeftOffs;
extern UWORD StripMinHeight;
extern UWORD StripMinWidth;
extern UWORD StripLeft;
extern UWORD StripTop;
extern UWORD StripDrawLeft;
extern UWORD StripDrawTop;
extern UWORD StripDrawOffs;
extern struct RastPort *StripRPort, *StripDrawRPort;
extern struct BitMap *StripBitMap;
extern struct Layer_Info *StripLayerInfo;
extern struct Layer *StripLayer;
extern struct ClipRect *StripCRect;
extern struct Window *StripWin;
extern BOOL StripPopUp;
extern BOOL MenuStripSwapped;

extern UWORD BoxWidth;
extern UWORD BoxHeight;
extern WORD BoxTop;
extern WORD BoxLeft;
extern WORD BoxDrawTop;
extern WORD BoxDrawLeft;
extern UWORD BoxDepth;
extern WORD BoxTopOffs;
extern WORD BoxLeftOffs;
extern WORD BoxLeftBorder;
extern WORD BoxTopBorder;
extern UWORD BoxDrawOffs;
extern UWORD BoxRightEdge;
extern UWORD BoxCmdOffs;
extern BOOL BoxGhosted;
extern struct RastPort *BoxRPort, *BoxDrawRPort;
extern struct BitMap *BoxBitMap;
extern struct Layer_Info *BoxLayerInfo;
extern struct Layer *BoxLayer;
extern struct ClipRect *BoxCRect;
extern struct MenuItem *BoxItems;
extern struct Window *BoxWin;
extern BOOL MenuBoxSwapped;

extern UWORD SubBoxWidth;
extern UWORD SubBoxHeight;
extern WORD SubBoxTop;
extern WORD SubBoxLeft;
extern WORD SubBoxDrawTop;
extern WORD SubBoxDrawLeft;
extern UWORD SubBoxDepth;
extern WORD SubBoxTopOffs;
extern WORD SubBoxLeftOffs;
extern WORD SubBoxLeftBorder;
extern WORD SubBoxTopBorder;
extern UWORD SubBoxDrawOffs;
extern UWORD SubBoxRightEdge;
extern UWORD SubBoxCmdOffs;
extern BOOL SubBoxGhosted;
extern struct RastPort *SubBoxRPort, *SubBoxDrawRPort;
extern struct BitMap *SubBoxBitMap;
extern struct Layer_Info *SubBoxLayerInfo;
extern struct Layer *SubBoxLayer;
extern struct ClipRect *SubBoxCRect;
extern struct MenuItem *SubBoxItems;
extern struct Window *SubBoxWin;
extern BOOL MenuSubBoxSwapped;

extern struct Menu *AktMenu;
extern struct MenuItem *AktItem;
extern struct MenuItem *AktSubItem;

extern ULONG DMROldSecs;
extern ULONG DMROldMics;

extern BOOL GlobalDMRequest;
extern BOOL GlobalLastWinDMREnable;
extern WORD GlobalDMRCount;

extern struct MMPrefs DefaultPrefs;

extern char CommText[5];
extern struct IntuiText CommandText;

extern struct NewBroker NewBroker;

extern struct InputXpression TickIX;

extern struct InputXpression StdKbdIX;

extern struct InputXpression MouseIX;

extern struct InputXpression ActiveMouseIX;

extern struct InputXpression ActiveMouseMoveIX;

extern struct InputXpression ActiveKbdIX;

extern struct InputXpression ActiveMousePositionIX;
extern struct InputXpression ActiveMouseNewPositionIX;

/*extern UWORD __chip MXImage1PlPattern[];*/
extern struct Image MXImage1Pl;


/*extern UWORD __chip NoMXImage1PlPattern[];*/
extern struct Image NoMXImage1Pl;


/*extern UWORD __chip MXImageLow1PlPattern[];*/
extern struct Image MXImageLow1Pl;


/*extern UWORD __chip NoMXImageLow1PlPattern[];*/
extern struct Image NoMXImageLow1Pl;


/*extern UWORD __chip MXImage2PlPattern[];*/
extern struct Image MXImage2Pl;


/*extern UWORD __chip NoMXImage2PlPattern[];*/
extern struct Image NoMXImage2Pl;


/*extern UWORD __chip MXImageLow2PlPattern[];*/
extern struct Image MXImageLow2Pl;

/*extern UWORD __chip NoMXImageLow2PlPattern[];*/
extern struct Image NoMXImageLow2Pl;


/*extern UWORD __chip CheckImage2PlPattern[];*/
extern struct Image CheckImage2Pl;

/*extern UWORD __chip CheckImageLow2PlPattern[];*/
extern struct Image CheckImageLow2Pl;

extern struct Image NoCheckImage2Pl;
extern struct Image NoCheckImageLow2Pl;

/*extern UWORD __chip AmigaImage1PlPattern[];*/
extern struct Image AmigaImage1Pl;

/*extern UWORD __chip AmigaImageLow1PlPattern[];*/
extern struct Image AmigaImageLow1Pl;

/*extern UWORD __chip AmigaImage2PlPattern[];*/
extern struct Image AmigaImage2Pl;

/*extern UWORD __chip AmigaImageLow2PlPattern[];*/
extern struct Image AmigaImageLow2Pl;

/*extern UWORD __chip SubArrowImage2PlPattern[];*/
extern struct Image SubArrowImage2Pl;

/*extern UWORD __chip SubArrowImage1PlPattern[];*/
extern struct Image SubArrowImage1Pl;

/*extern UWORD __chip SubArrowImageLow2PlPattern[];*/
extern struct Image SubArrowImageLow2Pl;

/*extern UWORD __chip SubArrowImageLow1PlPattern[];*/
extern struct Image SubArrowImageLow1Pl;

/******************************************************************************/

extern struct Image AmigaNormal;
extern struct Image AmigaGhosted;
extern struct Image ArrowNormal;
extern struct Image ArrowGhosted;
extern struct Image CheckNormal;
extern struct Image CheckGhosted;
extern struct Image MXDownNormal;
extern struct Image MXDownGhosted;
extern struct Image MXUpNormal;
extern struct Image MXUpGhosted;

extern struct Image Amiga8_Normal;
extern struct Image Amiga8_Ghosted;
extern struct Image Arrow8_Normal;
extern struct Image Arrow8_Ghosted;
extern struct Image Check8_Normal;
extern struct Image Check8_Ghosted;
extern struct Image MXDown8_Normal;
extern struct Image MXDown8_Ghosted;
extern struct Image MXUp8_Normal;
extern struct Image MXUp8_Ghosted;

/******************************************************************************/

extern struct Image AmigaNormal4;
extern struct Image CheckNormal4;
extern struct Image MXDownNormal4;
extern struct Image MXUpNormal4;
extern struct Image ArrowNormal4;
extern struct Image AmigaNormal8_4;
extern struct Image CheckNormal8_4;
extern struct Image MXDownNormal8_4;
extern struct Image MXUpNormal8_4;
extern struct Image ArrowNormal8_4;

/******************************************************************************/

extern ULONG OldOpenWindow;
extern ULONG OldOpenWindowTagList;
extern ULONG OldClearMenuStrip;
extern ULONG OldSetMenuStrip;
extern ULONG OldResetMenuStrip;
extern ULONG OldCloseWindow;
extern ULONG OldActivateWindow;
extern ULONG OldWindowToFront;
extern ULONG OldWindowToBack;
extern ULONG OldModifyIDCMP;
extern ULONG OldObtainGIRPort;
extern ULONG OldLendMenus;
extern ULONG OldOffMenu;
extern ULONG OldOnMenu;
extern ULONG OldSetWindowTitles;
extern ULONG OldRefreshWindowFrame;

extern ULONG OldCreateUpfrontHookLayer;
extern ULONG OldCreateUpfrontLayer;

extern LONG Cx_Pri;
extern BOOL Cx_Popup;
extern char Cx_PopupStr[ANSWER_LEN + 1];
extern char Cx_Popkey[LONGANSWER_LEN + 1];

extern BOOL DoGlobalQuit;

extern char TitleText[41];

extern UBYTE StdRemapArray[256];
extern UBYTE DreiDRemapArray[256];
extern UBYTE DreiDGhostedRemapArray[256];
extern UBYTE DreiDActiveRemapArray[256];

extern struct MMPrefs AktPrefs;
extern char ConfigPath[201];
extern char ConfigFile[232];

extern UBYTE MenuMode;

extern BOOL SelectSpecial;

extern ULONG CxSignalMask;

extern struct Window *MenWin;
extern struct Screen *MenScr;
extern struct Menu *MenStrip;
extern struct Rectangle MenDispClip;
extern struct RastPort ScrRPort;
extern struct TextAttr MenTextAttr;
extern struct Image *CheckImage, *NoCheckImage;
extern struct Image *MXImage, *NoMXImage;
extern struct Image *CommandImage, *SubArrowImage;
extern struct Image *CommandImageGhosted, *CheckImageGhosted;
extern struct Image *SubArrowImageGhosted;
extern struct Image *CheckImageActive, *NoCheckImageActive;
extern struct Image *CommandImageActive, *SubArrowImageActive;
extern struct Image *CheckImageRmp, *NoCheckImageRmp;
extern struct Image *CommandImageRmp, *SubArrowImageRmp;
extern struct Image *CommandImageGhostedRmp, *CheckImageGhostedRmp;
extern struct Image *SubArrowImageGhostedRmp;

extern struct Image *MXImageRmp;
extern struct Image *NoMXImageRmp;
extern struct Image *MXImageGhosted;
extern struct Image *NoMXImageGhosted;
extern struct Image *MXImageActive;
extern struct Image *NoMXImageActive;

extern ULONG IBaseLock;

extern struct WBStartup *WBMsg;
extern char ProgName[32];
