/*
**   $Id$
**
**   :ts=8
*/

#ifndef _SYSTEM_HEADERS_H
#define _SYSTEM_HEADERS_H 1

#include <dos/filehandler.h>
#include <dos/dosextens.h>
#include <dos/datetime.h>
#include <dos/dostags.h>
#include <dos/dosasl.h>
#include <dos/stdio.h>
#include <dos/rdargs.h>

#include <exec/execbase.h>
#include <exec/memory.h>

#include <intuition/intuitionbase.h>
#include <intuition/gadgetclass.h>
#include <intuition/intuition.h>
#include <intuition/screens.h>

#include <graphics/videocontrol.h>
#include <graphics/gfxmacros.h>
#include <graphics/gfxbase.h>
#include <graphics/modeid.h>

#include <libraries/commodities.h>
#include <libraries/gadtools.h>
#include <libraries/locale.h>
#include <libraries/asl.h>

#include <devices/inputevent.h>
#include <devices/input.h>

#include <hardware/blit.h>

#include <workbench/startup.h>
#include <workbench/workbench.h>

#include <gadgets/gradientslider.h>
#include <gadgets/colorwheel.h>

#include <clib/commodities_protos.h>
#include <clib/gadtools_protos.h>
#include <clib/intuition_protos.h>
#include <clib/graphics_protos.h>
#include <clib/utility_protos.h>
#include <clib/keymap_protos.h>
#include <clib/locale_protos.h>
#include <clib/layers_protos.h>
#include <clib/input_protos.h>
#include <clib/timer_protos.h>
#include <clib/icon_protos.h>
#include <clib/alib_protos.h>
#include <clib/exec_protos.h>
#include <clib/dos_protos.h>
#include <clib/asl_protos.h>
#include <clib/graphics_protos.h>
#include <clib/colorwheel_protos.h>

#include <pragmas/commodities_pragmas.h>
#include <pragmas/gadtools_pragmas.h>
#include <pragmas/intuition_pragmas.h>
#include <pragmas/graphics_pragmas.h>
#include <pragmas/utility_pragmas.h>
#include <pragmas/keymap_pragmas.h>
#include <pragmas/locale_pragmas.h>
#include <pragmas/layers_pragmas.h>
#include <pragmas/input_pragmas.h>
#include <pragmas/timer_pragmas.h>
#include <pragmas/icon_pragmas.h>
#include <pragmas/exec_sysbase_pragmas.h>
#include <pragmas/dos_pragmas.h>
#include <pragmas/asl_pragmas.h>
#include <pragmas/graphics_pragmas.h>
#include <pragmas/colorwheel_pragmas.h>

#define USE_BUILTIN_MATH 1

#include <string.h>
#include <stdlib.h>
#include <stdarg.h>
#include <stddef.h>
/*#include <stdio.h>*/
/*#include <math.h>*/

/*#include <ctype.h>*/

#include "libraries/gtlayout.h"
#include "clib/gtlayout_protos.h"
#include "pragmas/gtlayout_pragmas.h"

#include "cybergraphx/cybergraphics.h"
#include "clib/cybergraphics_protos.h"
#include "pragmas/cybergraphics_pragmas.h"

#endif	/* _SYSTEM_HEADERS_H */
