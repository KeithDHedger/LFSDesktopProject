/*
 *
 * ©K. D. Hedger. Fri 24 Jul 19:33:15 BST 2015 keithdhedger@gmail.com

 * This file (LFSTKGlobals.h) is part of LFSToolKit.

 * LFSToolKit is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * at your option) any later version.

 * LFSToolKit is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

 * You should have received a copy of the GNU General Public License
 * along with LFSToolKit.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef _LFSTKGLOBALS_
#define _LFSTKGLOBALS_

#include <X11/Xft/Xft.h>

#include <map>
#include <iostream>
#include <fstream>
#include <string>
#include <sstream>

#include <dirent.h>

#include <glib.h>
#include <cairo.h>
#include <cairo-xlib.h>

#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>


#define COPYRITE			"Copyright © 2013-2021 K.D.Hedger"
#define PERSONAL			"keithdhedger@gmail.com"
#define	MYWEBSITE			"https://keithdhedger.github.io/"

#define DEFAULTFONT			"sans:size=10"
#define DISCLOSURESIZE		6

#define USERDATA(x)			(void*)(long)(x)
#define GETUSERDATA(x)		(long)x

#define DIALOGWIDTH			450
#define DIALOGHITE			800
#define DIALOGMIDDLE		DIALOGWIDTH/2

#define GADGETWIDTH			80
#define GADGETHITE			24
#define HALFGADGETWIDTH		GADGETWIDTH/2

#define BORDER				16
#define YSPACING			32
#define HALFYSPACING		YSPACING/2
#define BUTTONGRAV			NorthWestGravity

#define TRIANGLESIZE		8u
#define CHECKBOXSIZE		12u

enum {CANCEL=0,APPLY};
enum {NONE=0,LEFT,CENTRE,RIGHT,AUTO,MENU,PRESERVEASPECT,TOOLBAR};
enum gadgetState {NORMALCOLOUR=0,PRELIGHTCOLOUR,ACTIVECOLOUR,INACTIVECOLOUR,MAXCOLOURS};

enum {BUTTONGADGET=1,MENUBUTTONGADGET,LINEEDITGADGET,LABELGADGET,TOGGLEGADGET,IMAGEGADGET,LISTGADGET,DIALOGGADGET,MULTILINEGADGET,SCROLLBARGADGET,MENUGADGET,MENUITEMGADGET,MULTIGADGET};
//enum {TYPEINT=1,TYPESTRING,TYPEBOOL};
enum {NORMAL=0,ABOVEALL,BELOWALL};

enum bevelType {BEVELIN=0,BEVELOUT,BEVELNONE};
enum indicatorType {CHECK=0,RADIO,PICTURE,DISCLOSURE,NOINDICATOR};

enum contextPostition {CONTEXTLEFT,CONTEXTRIGHT,CONTEXTATMOUSE};
//dropdata
enum dropDataType {DROPINVALID=0,DROPTEXT,DROPURI};

struct geometryStruct
{
	int						x,y;
	unsigned				w,h;
	unsigned				monitor;
};


#ifdef _ENABLEDEBUG_
__attribute__((unused)) static bool	showFileData=true;
__attribute__((unused)) static void debugFunc(const char *file,const char *func,int line,const char *fmt, ...)
{
	va_list		ap;
	char		*buffer,*subbuffer;
	const char	*bools[]={"false","true"};

	buffer=(char*)alloca(512);
	subbuffer=(char*)alloca(512);

	buffer[0]=0;
	subbuffer[0]=0;
	va_start(ap, fmt);
	while (*fmt)
		{
			subbuffer[0]=0;
			if(fmt[0]=='%')
				{
					fmt++;
					switch(*fmt)
						{
							case 's':
								sprintf(subbuffer,"%s",va_arg(ap,char*));
								break;
							case 'x':
								sprintf(subbuffer,"%x",va_arg(ap,int));
								break;
							case 'i':
								sprintf(subbuffer,"%i",va_arg(ap,int));
								break;
							case 'p':
								sprintf(subbuffer,"%p",va_arg(ap,char*));
								break;
							case 'c':
								sprintf(subbuffer,"%c",va_arg(ap,int));
								break;
							case 'f':
								sprintf(subbuffer,"%f",va_arg(ap,double));
								break;
							case 'b':
								sprintf(subbuffer,"%s",bools[va_arg(ap,int)]);
								break;
							case 'r':
								{
									geometryStruct gs=va_arg(ap,geometryStruct);
									sprintf(subbuffer,"x=%i, y=%i, w=%i, h=%i",gs.x,gs.y,gs.w,gs.h);
								}
								break;
							default:
								sprintf(subbuffer,"%c",fmt[0]);
								break;
						}
				}
			else
				sprintf(subbuffer,"%c",fmt[0]);
			strcat(buffer,subbuffer);
			fmt++;
		}
	va_end(ap);

	if(showFileData==true)
		{
			printf("\nFile: %s\nFunc: %s\nLine: %i\n",basename(file),func,line);
			printf("----USER DATA----\n%s\n----END----\n",buffer);
		}
	else
		{
			printf("%s\n",buffer);
		}
}

__attribute__((unused)) static void simpleDebug(const char *file,const char *func,int line)
{
	printf("\nFile: %s\nFunc: %s\nLine: %i\n",basename(file),func,line);
}

__attribute__((unused)) static void debugKey(const char *file,const char *func,int line,XKeyEvent *e)
{
	char		c[255];
	KeySym		keysym_return;
	const char	*typestr="XKeyPressedEvent";

	printf("\nFile: %s\nFunc: %s\nLine: %i\n",basename(file),func,line);
	XLookupString(e,(char*)&c,255,&keysym_return,NULL);
	if(e->type==KeyRelease)
		typestr="XKeyReleasedEvent";
	printf("Key=XK_%s, Type=%s\n",XKeysymToString(keysym_return),typestr);
}

#define DEBUGFUNC(x,...) debugFunc(__FILE__,__func__,__LINE__,(const char*)x,__VA_ARGS__);
#define DEBUG simpleDebug(__FILE__,__func__,__LINE__);
#define DEBUGKEY(e) debugKey(__FILE__,__func__,__LINE__,e);
#else
#define DEBUGFUNC(...) printf("Remove debug code here: %s:%i\n",__FILE__,__LINE__);
#define DEBUG printf("Remove debug code here: %s:%i\n",__FILE__,__LINE__);
#define DEBUGKEY(...) printf("Remove debug code here: %s:%i\n",__FILE__,__LINE__);
#endif

static bool	xLibErrorTK=false;
static bool	xLibWarningsTK=false;
__attribute__((unused)) static int	xErrHandler(Display *dpy,XErrorEvent *e)
{
	char buf[256];

	xLibErrorTK=true;
	if(xLibWarningsTK==true)
		{
			buf[0]=0;
			XGetErrorText(dpy,e->error_code,buf,sizeof buf);
			printf("Xlib Error: %s",buf);
		}
	return 0;
}

__attribute__((unused)) static void freeAndNull(char **data)
{
	if((data!=NULL) && (*data!=NULL))
		{
			free(*data);
			*data=NULL;
		}
}

struct args
{
	const char*				name;
	int						type;
	void*					data;
};

class LFSTK_windowClass;
class LFSTK_gadgetClass;
class LFSTK_applicationClass;
#define	NOCB				0x0
#define	MOUSEPRESSCB		0x1
#define	MOUSERELEASECB		0x2
#define	KEYPRESSCB			0x4
#define	KEYRELEASECB		0x8
#define	WINDOWDROPCB		0x10
#define	GADGETDROPCB		0x20
#define	DOUBLECLICKCB		0x40
#define	TIMERCB				0x80
#define ALLCB				0x8F

struct propertyStruct
{
	unsigned char	*data;
	int				format;
	int				nitems;
	Atom			type;
	char			*mimeType;
	int				dropX=-1;
	int				dropY=-1;
};

struct callbackStruct
{
	bool					(*mousePressCallback)(void*,void*)=NULL;
	bool					(*mouseReleaseCallback)(void*,void*)=NULL;
	bool					(*keyPressCallback)(void*,void*)=NULL;
	bool					(*keyReleaseCallback)(void*,void*)=NULL;
	bool					(*droppedWindowCallback)(LFSTK_windowClass*,void*)=NULL;
	bool					(*droppedGadgetCallback)(void*,propertyStruct *data,void*)=NULL;
	bool					(*doubleClickCallback)(void*,void*)=NULL;
	bool					(*timerCallback)(LFSTK_applicationClass*,void*)=NULL;
	void					*mouseUserData=NULL;
	void					*droppedUserData=NULL;
	void					*keyUserData=NULL;
	bool					runTheCallback=true;
	bool					ignoreOrphanModKeys=true;
	int						validCallbacks=NOCB;
};

struct pointStruct
{
	int						x,y;
};

struct rectStruct
{
	int						x,y;
	int						w,h;
};

struct	cairoColor
{
	double					r,g,b,a;
};

struct colourStruct
{
	char					*name;
	long					pixel;
	cairoColor				RGBAColour;
};

class	LFSTK_gadgetClass;
struct	mappedListener
{
	bool				(*function)(void *,XEvent *,int);
	LFSTK_gadgetClass	*gadget=NULL;
	int					type;
};

class	LFSTK_listGadgetClass;
struct listData
{
	LFSTK_listGadgetClass	*mainObject;
	unsigned				userData;
};

struct gadgetStruct
{
	colourStruct			*colour=NULL;
	bevelType				bevel=BEVELNONE;
	indicatorType			indic=NOINDICATOR;
	int						state=NORMALCOLOUR;
	int						reserveSpace=0;
	bool					buttonTile=false;
	geometryStruct			gadgetGeom={0,0,1,1};
	geometryStruct			indicatorGeom={0,0,1,1};
	bool					hasIndicator=false;
	bool					useWindowPixmap=false;
	bool					geomRelativeToMainWindow=false;
	bool					showLink=false;
	bool					showBroken=false;
};

class LFSTK_applicationClass;
struct windowInitStruct
{
	int						x=0;
	int						y=0;
	int						w=1;
	int						h=1;
	const char				*name="";
	bool					overRide=false;
	bool					loadVars=true;
	bool					shutDisplayOnExit=false;
	const char				*windowType="_NET_WM_WINDOW_TYPE_NORMAL";
	bool					decorated=true;
	int						level=NORMAL;
	Display					*display=NULL;
	LFSTK_windowClass		*wc=NULL;
	LFSTK_applicationClass	*app=NULL;
};

//menus
class	LFSTK_menuClass;
struct menuStruct
{
	char					*label=NULL;
	int						imageType=0;
	union					imageData
		{
			char			*imagePath;
			cairo_surface_t	*surface;
		}					data={NULL};

	bool					hasSubMenu=false;
	menuStruct				**subMenus=NULL;
	int						subMenuCnt=0;
	void					*userData=NULL;
};

//messaging
#define MAX_MSG_SIZE 256
#define WAIT_MSG 0

#define MSGANY 0
struct msgBuffer
{
	long mType;
	char mText[MAX_MSG_SIZE];
};

enum {DESKTOP_MSG=1000,WMANAGER_MSG,PANEL_MSG,WALLPAPER_MSG,TOOLKIT_MSG,APPEARANCE_PREFS_MSG,BACKDROP_PREFS_MSG,DESKTOP_PREFS_MSG,PANEL_PREFS_MSG,TK_PREFS_MSG,WMANAGER_PREFS_MSG};

#include "LFSTKPrefsClass.h"
#include "LFSTKLib.h"
#include "LFSTKApplication.h"
#include "LFSTKFindClass.h"
#include "LFSTKWindow.h"
#include "LFSTKGadget.h"
#include "LFSTKExpanderGadget.h"
#include "LFSTKLabel.h"
#include "LFSTKImage.h"
#include "LFSTKButton.h"
#include "LFSTKLineEdit.h"
#include "LFSTKToggleButton.h"
#include "LFSTKScrollBar.h"
#include "LFSTKListGadget.h"
#include "LFSTKFileDialog.h"
#include "LFSTKMultiLineEdit.h"
#include "LFSTKFontDialog.h"
#include "LFSTKToolWindow.h"
#include "LFSTKMenu.h"
#include "LFSTKMenuItem.h"

#endif