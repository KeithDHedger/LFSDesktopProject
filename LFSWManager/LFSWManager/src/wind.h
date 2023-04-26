
/*
	Thanks to Johan for the original code available here:
	http://sourceforge.net/projects/windwm/?source=navbar

	Changes/additions
	©keithhedger Tue 23 Jun 09:56:25 BST 2015 keithdhedger@gmail.com

	Extra code released under GPL3

*/

#ifndef _WIND_H_
#define _WIND_H_

#define DBG0 0 //no debug
#define DBG1 1 //debug to stderr and /tmp/lfswmanager.error.log

#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <lfstk/LFSTKGlobals.h>


#include "config.h"

#ifndef MIN
#define MIN(a,b) ((a)<(b) ? (a) : (b))
#endif
#ifndef MAX
#define MAX(a,b) ((a)>(b) ? (a) : (b))
#endif

#define NELEM(v) (sizeof v / sizeof v[0])

// The desktop number of sticky windows
#define DESK_ALL 0xffffffffUL

typedef unsigned long Desk;

enum {NOPLACE=0,UNDERMOUSE,CENTREMMONITOR,CENTRESCREEN,MOUSEMONITOR};
//enum {FOCUSEDFORE=0,FOCUSEDBACK,FORE,BACK};

struct geometry
{
	int x;
	int y;
	int width;
	int height;
	int borderwidth;
};

struct extents
{
	int top;
	int bottom;
	int left;
	int right;
};

//struct listener
//{
//	void (*function)(void *,XEvent *);
//	void *pointer;
//};
struct listener
{
	bool		(*function)(void *,XEvent *,int);
	void		*pointer;
	int			type;
};


struct bitmap
{
	unsigned char *bits;
	int width;
	int height;
	Pixmap pixmap;
};

#define DEFINE_BITMAP(name) \
		struct bitmap name={ \
			.bits=name##_bits,\
			.width=name##_width,\
			.height=name##_height,\
			.pixmap=None \
		}

struct font
{
	int ascent;
	int descent;
	int size;
	void *data;
};

typedef struct
{
	int type;       /* ColormapNotify */
	unsigned long serial;   /* # of last request processed by server */
	Bool send_event;        /* true if this came from a SendEvent request */
	Display *display;       /* Display the event was read from */
	Window window;
	Colormap colormap;      /* colormap or None */
	Bool mynew;
	int state;      /* ColormapInstalled,ColormapUninstalled */
} myXColormapEvent;

struct	monitors
{
	int		monW;
	int		monH;
	int		monX;
	int		monY;
};


enum mainThemeParts {TOPLEFTACTIVE=0,TOPLEFTINACTIVE,TOPRIGHTACTIVE,TOPRIGHTINACTIVE,TITLE1ACTIVE,TITLE1INACTIVE,TITLE2ACTIVE,TITLE2INACTIVE,TITLE3ACTIVE,TITLE3INACTIVE,TITLE4ACTIVE,TITLE4INACTIVE,TITLE5ACTIVE,TITLE5INACTIVE,LEFTACTIVE,LEFTINACTIVE,RIGHTACTIVE,RIGHTINACTIVE,BOTTOMLEFTACTIVE,BOTTOMLEFTINACTIVE,BOTTOMACTIVE,BOTTOMINACTIVE,BOTTOMRIGHTACTIVE,BOTTOMRIGHTINACTIVE,NUMTHEMEPARTS};

enum closeButtonParts {CLOSEACTIVE=NUMTHEMEPARTS,CLOSEINACTIVE,CLOSEPRELIGHT,CLOSEPRESSED};
enum maxButtonParts {MAXACTIVE=NUMTHEMEPARTS+4,MAXINACTIVE,MAXPRELIGHT,MAXPRESSED};
enum minButtonParts {MINACTIVE=NUMTHEMEPARTS+8,MININACTIVE,MINPRELIGHT,MINPRESSED};
enum shadeButtonParts {SHADEACTIVE=NUMTHEMEPARTS+12,SHADEINACTIVE,SHADEPRELIGHT,SHADEPRESSED};

//buttons
enum {SHADEBUTTON=0,MINBUTTON,MAXBUTTON,CLOSEBUTTON};
enum {ACTIVEFRAME=0,ACTIVEFRAMEFILL,INACTIVEFRAME,INACTIVEFRAMEFILL,TEXTCOLOUR};

#define NUMBEROFPARTS NUMTHEMEPARTS+sizeof(closeButtonParts)+sizeof(maxButtonParts)+sizeof(minButtonParts)+sizeof(shadeButtonParts)

struct	themeStruct
{
	bool			useTheme;
	char			*pathToTheme;
	bool			gotPart[NUMBEROFPARTS];
	int				titleBarHeight;
	int				leftWidth;
	int				rightWidth;
	int				bottomHeight;
	int				partsWidth[NUMBEROFPARTS];
	int				partsHeight[NUMBEROFPARTS];
	Pixmap			pixmaps[NUMBEROFPARTS];
	Pixmap			masks[NUMBEROFPARTS];
	int				buttonOffset;
	int				buttonXSpacing;
	int				titleOffset;
};

extern enum runlevel
{
	RL_STARTUP,RL_NORMAL,RL_SHUTDOWN,
} runlevel;

// The display name argument used in call to XOpenDisplay
extern const char		*displayname;

// The last X error reported
extern const char		*xerror;

extern Display			*dpy;
extern Window			root;

//new colours
//active frame fill
extern unsigned long	activeFrameFill;
extern unsigned long	activeFrame;
extern unsigned long	inactiveFrameFill;
extern unsigned long	inactiveFrame;
extern unsigned long	widgetColour;
//buttons
extern GC				activeGC;

extern struct font		*font;
extern struct fontcolor	*fhighlight;
extern struct fontcolor	*fnormal;

extern unsigned int		screen;
extern Visual			*visual;
extern int				displayWidth;
extern int				displayHeight;
extern monitors			*monitorData;
extern unsigned int		numberOfMonitors;
extern XContext			listeners;
extern const char		*progname;
extern int				blackColor;
extern int				whiteColor;
extern GC				mainGC;

//wm prefs
extern int				placement;
extern	unsigned int	numberOfDesktops;
extern char				*titleFont;
extern char				*fontColours[5];
extern int				liveUpdate;
extern int				prefsRescan;
extern char				*terminalCommand;

//theme
extern themeStruct		theme;
extern char				*fontName;
extern int				fontSize;
extern char				*pathToTheme;

extern int				frameTop;
extern int				frameBottom;
extern int				frameLeft;
extern int				frameRight;

//debug
extern bool				xLibWarnings;

void errorf(const char *,...);
void setlistener(Window,const struct listener *);
int redirect(XEvent *,Window);

extern int doswapdesk;

#endif
