
/*
	Thanks to Johan for the original code available here:
	http://sourceforge.net/projects/windwm/?source=navbar

	Changes/additions
	©keithhedger Tue 23 Jun 09:56:25 BST 2015 keithdhedger@gmail.com

	Extra code released under GPL3

*/

#ifndef _CLIENT_
#define _CLIENT_

#include "list.h"
#include "wind.h"


struct client
{
	struct listener	listener;
	List			winstack;
	Window			window;
	Colormap		colormap;

	struct geometry	geometry;

	XWMHints		*wmhints;
	XSizeHints		*wmnormalhints;
	Atom			*wmprotocols;
	int				wmprotocolscount;
	Window			wmtransientfor;

	// WM_NAME property in current locale encoding
	char			*wmname;

	// _NET_WM_NAME property in UTF-8 encoding
	char			*netwmname;

	// Application id of this client
	XID				app;

	struct frame	*frame;

	Desk			desk;

	/*
	 * If this counter is zero when an UnmapNotify event
	 * is received,the client is considered withdrawn.
	 */
	int				ignoreunmapcount;

	Bool			ismapped;
	Bool			hasfocus;
	Bool			isfull;
	Bool			skiptaskbar;
	Bool			isundecorated;
	Bool			followdesk;
	Bool			initialized;
	Bool			isDesktop;
	bool			isDock;
	Bool			isAbove;
	bool			isBelow;
	bool			checked;
	bool			canMaximize;
	bool			canMinimize;
	bool			isIcon;
	bool			beenShuffled;
	bool			beenPopped;

	unsigned		monitorNumber;
	bool			decoratedHold;
};
//extern client *cc;

struct client *manage(Window);
extern Desk curdesk;
extern Bool needrestack;

void shuffle(void);
void manageall(void);
void unmanageall(void);
void cpopapp(struct client *);
void cpushapp(struct client *);
void cdelete(struct client *,Time);
void csetdesk(struct client *,Desk);
void csetappdesk(struct client *,Desk);
void csetdock(struct client *,Bool);
void csetfull(struct client *,Bool);
void csetundecorated(struct client *,Bool);
struct client *getfocus(void);
void cfocus(struct client *,Time);
struct client *refocus(Time);
Bool cistask(struct client *);
Bool cisvisible(struct client *);
void setndesk(Desk);
void gotodesk(Desk);
void getwindowstack(Window **,size_t *);
int namewidth(struct font *,struct client *);
void drawname(Drawable,struct font *,struct fontcolor *,int,int,struct client *);
Bool chaswmproto(struct client *,Atom);
void restack(void);
int cgetgrav(struct client *);
struct geometry cgetgeom(struct client *);
void csetgeom(struct client *,struct geometry);
void csendconf(struct client *);
Bool chasfocus(struct client *);
void csetnetwmname(struct client *,const char *);
void cignoreunmap(struct client *);
Bool cisurgent(struct client *);
void chintsize(struct client *,int,int,int *,int *);
void cunmap(struct client *c);
void cmap(struct client *c);
void cpop(struct client *c);
void cpopapp(struct client *c);
void cupdatedesk(struct client *c);
void cpush(struct client *c);
void clientevent(void *self,XEvent *e);
void getclientstack(struct client ***vp,int *np);
int getMouseMonitor(client *c);

char* getMaxString(struct font *font,struct client *c,int width);
Bool cisframed(struct client *c);
void csendconf(struct client *c);

#endif
