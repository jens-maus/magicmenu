/*
**	$Id$
**
**	:ts=8
*/

/* Magic Menu Prefs Header */

struct RGBSet
{
    ULONG                   R,G,B;
};


struct MMPrefs
    {
        ULONG               mmp_Version;
        ULONG               mmp_Size;

        UWORD               mmp_MenuType;

        struct
        {
            ULONG Enabled : 1;
            ULONG MarkSub : 1;
            ULONG DblBorder : 1;
            ULONG UseLayer : 1;
            ULONG DirectDraw : 1;
            ULONG ChunkyPlanes : 1;
            ULONG NonBlocking : 1;
            ULONG KCEnabled : 1;
            ULONG KCGoTop : 1;
            ULONG KCRAltRCommand : 1;
            ULONG PUCenter : 1;
            ULONG Dummy : 21;
        } mmp_Flags;

        UWORD               mmp_PDMode;
        UWORD               mmp_PDLook;

        UWORD               mmp_PUMode;
        UWORD               mmp_PULook;

        char                mmp_KCKeyStr[200];

        UWORD               mmp_TimeOut;

        struct RGBSet       mmp_LightEdge;
        struct RGBSet       mmp_DarkEdge;
        struct RGBSet       mmp_Background;
        struct RGBSet       mmp_TextCol;
        struct RGBSet       mmp_HiCol;
        struct RGBSet       mmp_FillCol;
        struct RGBSet       mmp_GhostLoCol;
        struct RGBSet       mmp_GhostHiCol;
    };

#define MMP_MAGIC           0xaf509a24

#define mmp_Enabled mmp_Flags.Enabled
#define mmp_MarkSub mmp_Flags.MarkSub
#define mmp_DblBorder mmp_Flags.DblBorder
#define mmp_UseLayer mmp_Flags.UseLayer
#define mmp_DirectDraw mmp_Flags.DirectDraw
#define mmp_ChunkyPlanes mmp_Flags.ChunkyPlanes
#define mmp_NonBlocking mmp_Flags.NonBlocking
#define mmp_KCEnabled mmp_Flags.KCEnabled
#define mmp_KCGoTop mmp_Flags.KCGoTop
#define mmp_KCRAltRCommand mmp_Flags.KCRAltRCommand
#define mmp_PUCenter mmp_Flags.PUCenter

#define MT_PULLDOWN     0
#define MT_POPUP        1
#define MT_AUTO         2

#define LOOK_2D         0
#define LOOK_3D         1
#define LOOK_MC         2

#define MODE_STD        0
#define MODE_STICKY     1
#define MODE_SELECT     2
#define MODE_KEYBOARD   3

#define CONFIG_NAME     "MagicMenu.config"
#define PATHENV         "MagicMenuPath"

struct MMMessage
    {
        struct  Message     Message;

        ULONG               Class;

        ULONG               Arg1;
        ULONG               Arg2;
        ULONG               Arg3;

        APTR                Ptr1;
        APTR                Ptr2;
        APTR                Ptr3;
    };

#define MMC_VOID            0
#define MMC_NEWCONFIG       1
#define MMC_REMOVE          2
#define MMC_GETCONFIG       3
#define MMC_ENABLE          4

#define MMPORT_NAME         "MagicMenu"

