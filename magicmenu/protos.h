
/* main.c */
BPTR ClonePath(BPTR StartPath);
VOID AttachCLI(struct WBStartup *Startup);
STRPTR GetString(ULONG ID);
VOID Activate(VOID);
VOID Deactivate(VOID);
VOID FreeMenuRemember(struct MenuRemember *Remember);
struct MenuRemember *MakeMenuRemember(struct Window *Win);
VOID FreeGlobalRemember(VOID);
BOOL MakeGlobalRemember(VOID);
VOID ClearRemember(struct Window *Win);
BOOL UpdateRemember(struct Window *Window);
VOID SetRemember(struct Window *Win);
VOID ResetMenu(struct Menu *Menu, BOOL MenNull);
VOID CleanUpMenu(VOID);
BOOL MenuSelected(BOOL LastSelect);
BOOL CheckCxMsgAct(CxMsg *Msg, BOOL *Cancel);
VOID ProcessIntuiMenu(VOID);
VOID EndIntuiMenu(BOOL ReleaseMenuAct);
BOOL CheckCxMsg(CxMsg *Msg);
BOOL CheckMMMsgPort(struct MMMessage *MMMsg);
BOOL RealWindow(struct Window *ThisWindow);
VOID ProcessCommodity(VOID);
VOID StopPrefs(VOID);
VOID StartPrefs(VOID);
VOID MyArgString(char *Result, struct DiskObject *DO, char *TT, char *Default, LONG Len, BOOL Upcase);
LONG MyArgInt(struct DiskObject *DO, char *TT, LONG Default);
VOID CheckArguments(VOID);
BOOL LoadPrefs(char *ConfigFile, BOOL Report);
VOID ResetBrokerSetup(VOID);
VOID ChangeBrokerSetup(VOID);
VOID CleanupMenuActiveData(VOID);
BOOL SetupMenuActiveData(VOID);
VOID CloseAll(VOID);
VOID ExitTrap(VOID);
VOID ErrorPrc(char *ErrTxt);
int main(int argc, char **argv);

/* misc.c */
BOOL MMCheckScreen(void);
BOOL MMCheckWindow(struct Window *Win);
ULONG __asm __saveds MMOpenWindow(REG (a0 )struct NewWindow *NW);
ULONG __asm __saveds MMOpenWindowTagList(REG (a0 )struct NewWindow *NW, REG (a1 )struct TagItem *TI);
ULONG __asm __saveds MMClearMenuStrip(REG (a0 )struct Window *W);
ULONG __asm __saveds MMSetMenuStrip(REG (a0 )struct Window *W, REG (a1 )struct Menu *MI);
ULONG __asm __saveds MMResetMenuStrip(REG (a0 )struct Window *W, REG (a1 )struct Menu *MI);
ULONG __asm __saveds MMCloseWindow(REG (a0 )struct Window *W);
ULONG __asm __saveds MMActivateWindow(REG (a0 )struct Window *W);
ULONG __asm __saveds MMWindowToFront(REG (a0 )struct Window *W);
ULONG __asm __saveds MMWindowToBack(REG (a0 )struct Window *W);
ULONG __asm __saveds MMMoveWindowInFrontOf(REG (a0 )struct Window *Window, REG (a1 )struct Window *Behind);
ULONG __asm __saveds MMModifyIDCMP(REG (a0 )struct Window *window, REG (d0 )ULONG flags);
ULONG __asm __saveds MMOffMenu(REG (a0 )struct Window *window, REG (d0 )ULONG number);
ULONG __asm __saveds MMOnMenu(REG (a0 )struct Window *window, REG (d0 )ULONG number);
struct RastPort *__asm __saveds MMObtainGIRPort(REG (a0 )struct GadgetInfo *GInfo);
struct Layer *__saveds __asm MMCreateUpfrontHookLayer(REG (a0 )struct Layer_Info *LayerInfo, REG (a1 )struct BitMap *BitMap, REG (d0 )LONG x0, REG (d1 )LONG y0, REG (d2 )LONG x1, REG (d3 )LONG y1, REG (d4 )ULONG Flags, REG (a3 )struct Hook *Hook, REG (a2 )struct BitMap *Super, REG (a6 )struct Library *LayersBase);
struct Layer *__asm MMCreateUpfrontLayer(REG (a0 )struct Layer_Info *LayerInfo, REG (a1 )struct BitMap *BitMap, REG (d0 )LONG x0, REG (d1 )LONG y0, REG (d2 )LONG x1, REG (d3 )LONG y1, REG (d4 )ULONG Flags, REG (a2 )struct BitMap *Super, REG (a6 )struct Library *LayersBase);
LONG __asm __saveds MMLendMenus(REG (a0 )struct Window *FromWindow, REG (a1 )struct Window *ToWindow);
VOID CreateBitMapFromImage(struct Image *Image, struct BitMap *BitMap);
VOID RecolourBitMap(struct BitMap *Src, struct BitMap *Dst, UBYTE *Mapping, LONG DestDepth, LONG Width, LONG Height);
BOOL MakeRemappedImage(struct Image **DestImage, struct Image *SrcImage, UWORD Depth, UBYTE *RemapArray);
VOID FreeRemappedImage(struct Image *Image);
VOID StartTimeRequest(struct timerequest *TimeRequest, ULONG Seconds, ULONG Micros);
VOID StopTimeRequest(struct timerequest *TimeRequest);
LONG ShowRequest(STRPTR Gadgets, STRPTR Text, ...);
BOOL CheckReply(struct Message *Msg);
BOOL CheckEnde(void);
void disposeBitMap(struct BitMap *BitMap, LONG Width, LONG Height, BOOL IsChipMem);
struct BitMap *allocBitMap(LONG Depth, LONG Width, LONG Height, struct BitMap *Friend, BOOL WantChipMem);
void FreeRPort(struct BitMap *BitMap, struct Layer_Info *LayerInfo, struct Layer *Layer, LONG Width, LONG Height);
struct Hook *GetNOPFillHook(VOID);
BOOL InstallRPort(LONG Depth, LONG Width, LONG Height, struct RastPort **RastPortPtr, struct BitMap **BitMapPtr, struct Layer_Info **LayerInfoPtr, struct Layer **LayerPtr, struct BitMap *Friend);
void SwapRPortClipRect(struct RastPort *RPort, struct ClipRect *ClipRect);
struct ClipRect *GetClipRect(struct BitMap *BitMap, LONG x1, LONG y1, LONG x2, LONG y2);
void CheckDispClipVisible(WORD MinX, WORD MinY, WORD MaxX, WORD MaxY);
void Draw3DRect(struct RastPort *rp, LONG x, LONG y, LONG Width, LONG Height, BOOL Upward, BOOL HiRes, BOOL DoubleBorder);
void DrawNormRect(struct RastPort *rp, LONG x, LONG y, LONG Width, LONG Height);
void GhostRect(struct RastPort *rp, LONG x, LONG y, LONG Width, LONG Height);
void CompRect(struct RastPort *rp, LONG x, LONG y, LONG Width, LONG Height);
void HiRect(struct RastPort *rp, LONG x, LONG y, LONG Width, LONG Height, BOOL Highlited);
BOOL MoveMouse(LONG NewX, LONG NewY, BOOL AddEvent, struct InputEvent *Event, struct Screen *Scr);
VOID SPrintf(STRPTR buffer, STRPTR formatString, ...);
VOID SetPens(struct RastPort *RPort, ULONG FgPen, ULONG BgPen, ULONG DrawMode);
VOID SetFgPen(struct RastPort *rp, LONG pen);
VOID SetDrawMode(struct RastPort *rp, LONG mode);
VOID SetDrawMask(struct RastPort *rp, LONG mask);
ULONG GetBitMapDepth(struct BitMap *BitMap);
LONG findchar(char *Text, char Zeichen);
VOID SafeObtainSemaphoreShared(struct SignalSemaphore *Semaphore);
VOID DrawLine(struct RastPort *rp, LONG x0, LONG y0, LONG x1, LONG y1);
VOID DrawLinePairs(struct RastPort *rp, LONG totalPairs, LONG left, LONG top, ...);
VOID PlaceText(struct RastPort *RPort, LONG Left, LONG Top, STRPTR String, LONG Len);
LONG AllocateColour(struct ColorMap *ColorMap, ULONG Red, ULONG Green, ULONG Blue);

/* data.c */

/* wedge.c */
VOID RemovePatches(VOID);
BOOL InstallPatches(VOID);

/* windowglyphs.c */
VOID WindowGlyphExit(VOID);
VOID WindowGlyphInit(VOID);
VOID DiscardWindowGlyphs(struct Window *Window);
VOID ObtainGlyphs(struct Window *Window, struct Image **TickGlyph, struct Image **AmigaGlyph);
struct Window *FindLending(struct Window *From);
VOID RegisterLending(struct Window *From, struct Window *To);
VOID RegisterGlyphs(struct Window *Window, struct NewWindow *NewWindow, struct TagItem *Tags);

/* memory.c */
APTR AllocVecPooled(ULONG Size, ULONG Flags);
VOID FreeVecPooled(APTR Memory);
VOID MemoryExit(VOID);
BOOL MemoryInit(VOID);

/* menuboxes.c */
VOID DrawMenuItem(struct RastPort *rp, struct MenuItem *Item, LONG x, LONG y, UWORD CmdOffs, BOOL GhostIt, BOOL Highlighted);
BOOL GetSubItemContCoor(struct MenuItem *MenuItem, LONG *t, LONG *l, LONG *w, LONG *h);
VOID CleanUpMenuSubBox(VOID);
BOOL DrawHiSubItem(struct MenuItem *Item);
BOOL DrawNormSubItem(struct MenuItem *Item);
VOID DrawMenuSubBoxContents(struct MenuItem *Item, struct RastPort *RPort, UWORD Left, UWORD Top);
BOOL DrawMenuSubBox(struct Menu *Menu, struct MenuItem *Item, BOOL ActivateItem);
BOOL GetItemContCoor(struct MenuItem *MenuItem, LONG *t, LONG *l, LONG *w, LONG *h);
VOID CleanUpMenuBox(VOID);
BOOL DrawHiItem(struct MenuItem *Item);
BOOL DrawNormItem(struct MenuItem *Item);
VOID DrawMenuBoxContents(struct Menu *Menu, struct RastPort *RPort, UWORD Left, UWORD Top);
BOOL DrawMenuBox(struct Menu *Menu, BOOL ActivateItem);
BOOL GetMenuContCoor(struct Menu *Menu, LONG *t, LONG *l, LONG *w, LONG *h);
BOOL DrawHiMenu(struct Menu *Menu);
BOOL GhostMenu(struct Menu *Menu, struct RastPort *RPort, UWORD Left, UWORD Top);
VOID DrawNormMenu(struct Menu *Menu);
VOID CleanUpMenuStrip(VOID);
VOID GetSubItemCoors(struct MenuItem *Item, UWORD *x1, UWORD *y1, UWORD *x2, UWORD *y2);
VOID GetItemCoors(struct MenuItem *Item, UWORD *x1, UWORD *y1, UWORD *x2, UWORD *y2);
VOID GetMenuCoors(struct Menu *Menu, UWORD *x1, UWORD *y1, UWORD *x2, UWORD *y2);
VOID ChangeAktSubItem(struct MenuItem *NewItem, UWORD NewSubItemNum);
BOOL FindSubItemChar(char Search, BOOL *Single);
UWORD SelectNextSubItem(WORD NeuItemNum);
UWORD SelectPrevSubItem(WORD NeuItemNum);
VOID ChangeAktItem(struct MenuItem *NewItem, UWORD NewItemNum);
BOOL FindItemChar(char Search, BOOL *Single);
UWORD SelectNextItem(WORD NeuItemNum);
UWORD SelectPrevItem(WORD NeuItemNum);
VOID ChangeAktMenu(struct Menu *NewMenu, UWORD NewMenuNum);
BOOL FindMenuChar(char Search, BOOL *Single);
UWORD SelectNextMenu(WORD NeuMenuNum);
UWORD SelectPrevMenu(WORD NeuMenuNum);
BOOL LookMouse(UWORD MouseX, UWORD MouseY, BOOL NewSelect);
VOID CopyImageDimensions(struct Image *Dest, struct Image *Source);
VOID DrawMenuStripContents(struct RastPort *RPort, UWORD Left, UWORD Top);
VOID SetMCPens(BOOL Vanilla);
BOOL DrawMenuStrip(BOOL PopUp, UBYTE NewLook, BOOL ActivateMenu);

/* dointuimenu.c */
BOOL DoIntuiMenu(UWORD NewMenuMode, BOOL PopUp, BOOL SendMenuDown);

/* sendintuimessage.c */
UWORD SendIntuiMessage(ULONG Class, UWORD *Code, UWORD Qualifier, APTR IAddress, struct Window *ReceivingWindow, ULONG IntuiLock, BOOL WaitForReply);

/* betatitle.c */
VOID StartHihoTask(VOID);
VOID StopHihoTask(VOID);

/* remap.a */
