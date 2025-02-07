
#include <time.h>

#include "lfswm2Class.h"

const char	*errorFile=NULL;
int			errorLine=-1;


int xError(Display *display,XErrorEvent *e)
{
	if(errorFile==NULL)
		return(0);

	time_t		now=time(0);
	struct tm	tstruct;
    char			buf[256];
 
    tstruct=*localtime(&now);
    strftime(buf,sizeof(buf),"%Y-%m-%d.%X",&tstruct);

	fprintf(stderr,"\nXError @ %s\nFrom file=%s @~ line=%i\n",buf,errorFile,errorLine);

	const char *const	X_REQUEST_CODE_NAMES[]=
		{
			"",
			"CreateWindow",
			"ChangeWindowAttributes",
			"GetWindowAttributes",
			"DestroyWindow",
			"DestroySubwindows",
			"ChangeSaveSet",
			"ReparentWindow",
			"MapWindow",
			"MapSubwindows",
			"UnmapWindow",
			"UnmapSubwindows",
			"ConfigureWindow",
			"CirculateWindow",
			"GetGeometry",
			"QueryTree",
			"InternAtom",
			"GetAtomName",
			"ChangeProperty",
			"DeleteProperty",
			"GetProperty",
			"ListProperties",
			"SetSelectionOwner",
			"GetSelectionOwner",
			"ConvertSelection",
			"SendEvent",
			"GrabPointer",
			"UngrabPointer",
			"GrabButton",
			"UngrabButton",
			"ChangeActivePointerGrab",
			"GrabKeyboard",
			"UngrabKeyboard",
			"GrabKey",
			"UngrabKey",
			"AllowEvents",
			"GrabServer",
			"UngrabServer",
			"QueryPointer",
			"GetMotionEvents",
			"TranslateCoords",
			"WarpPointer",
			"SetInputFocus",
			"GetInputFocus",
			"QueryKeymap",
			"OpenFont",
			"CloseFont",
			"QueryFont",
			"QueryTextExtents",
			"ListFonts",
			"ListFontsWithInfo",
			"SetFontPath",
			"GetFontPath",
			"CreatePixmap",
			"FreePixmap",
			"CreateGC",
			"ChangeGC",
			"CopyGC",
			"SetDashes",
			"SetClipRectangles",
			"FreeGC",
			"ClearArea",
			"CopyArea",
			"CopyPlane",
			"PolyPoint",
			"PolyLine",
			"PolySegment",
			"PolyRectangle",
			"PolyArc",
			"FillPoly",
			"PolyFillRectangle",
			"PolyFillArc",
			"PutImage",
			"GetImage",
			"PolyText8",
			"PolyText16",
			"ImageText8",
			"ImageText16",
			"CreateColormap",
			"FreeColormap",
			"CopyColormapAndFree",
			"InstallColormap",
			"UninstallColormap",
			"ListInstalledColormaps",
			"AllocColor",
			"AllocNamedColor",
			"AllocColorCells",
			"AllocColorPlanes",
			"FreeColors",
			"StoreColors",
			"StoreNamedColor",
			"QueryColors",
			"LookupColor",
			"CreateCursor",
			"CreateGlyphCursor",
			"FreeCursor",
			"RecolorCursor",
			"QueryBestSize",
			"QueryExtension",
			"ListExtensions",
			"ChangeKeyboardMapping",
			"GetKeyboardMapping",
			"ChangeKeyboardControl",
			"GetKeyboardControl",
			"Bell",
			"ChangePointerControl",
			"GetPointerControl",
			"SetScreenSaver",
			"GetScreenSaver",
			"ChangeHosts",
			"ListHosts",
			"SetAccessControl",
			"SetCloseDownMode",
			"KillClient",
			"RotateProperties",
			"ForceScreenSaver",
			"SetPointerMapping",
			"GetPointerMapping",
			"SetModifierMapping",
			"GetModifierMapping",
			"NoOperation",
		};

	const int	MAX_ERROR_TEXT_LENGTH=1024;
	char			error_text[MAX_ERROR_TEXT_LENGTH];

	XGetErrorText(display,e->error_code,error_text,sizeof(error_text));
	fprintf(stderr,"Received X error:\nRequest: %i %s\n%i %s Resource ID: %x\n\n",int(e->request_code),X_REQUEST_CODE_NAMES[e->request_code],int(e->error_code),error_text,(unsigned int)e->resourceid);
	return 0;
}