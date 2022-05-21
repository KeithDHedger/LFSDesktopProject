/*
 * Copyright 2010 Johan Veenhuizen
 *
 * Permission is hereby granted,free of charge,to any person obtaining a
 * copy of this software and associated documentation files (the "Software"),
 * to deal in the Software without restriction,including without limitation
 * the rights to use,copy,modify,merge,publish,distribute,sublicense,
 * and/or sell copies of the Software,and to permit persons to whom the
 * Software is furnished to do so,subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included
 * in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS",WITHOUT WARRANTY OF ANY KIND,EXPRESS OR
 * IMPLIED,INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
 * THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM,DAMAGES OR
 * OTHER LIABILITY,WHETHER IN AN ACTION OF CONTRACT,TORT OR OTHERWISE,
 * ARISING FROM,OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
 * OTHER DEALINGS IN THE SOFTWARE.
 */

/*
	Thanks to Johan for the original code available here:
	http://sourceforge.net/projects/windwm/?source=navbar

	Changes/additions
	©keithhedger Tue 23 Jun 09:56:25 BST 2015 keithdhedger@gmail.com

	Extra code released under GPL3

*/

//#define NDEBUG
#include <assert.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <X11/Xatom.h>
#include <X11/Xlib.h>
#include <X11/Xutil.h>

#include "client.h"
#include "list.h"
#include "wind.h"
#include "ewmh.h"
#include "lib.h"
#include "frame.h"
#include "mwm.h"
#include "x11font.h"
#include "atoms.h"

LIST_DEFINE(winstack);
//client *cc;

// Current desk
Desk curdesk=0;

// Number of desks
//Desk ndesk=1;

// True if restacking needed
Bool needrestack=False;

// Dummy window for window stacking
Window stacktop=None;

struct keymapstructclient
{
	KeySym keysym;
	unsigned modifiers;
	void (*function)(struct client *,unsigned,Time);
};

void keypress_delete(struct client *c,unsigned state,Time time);
void keypress_pushapp(struct client *c,unsigned state,Time time);
void keypress_fullscreen(struct client *c,unsigned state,Time time);
void keypress_sticky(struct client *c,unsigned state,Time time);
void keypress_group(struct client *c,unsigned state,Time time);

keymapstructclient keymapclient[] =
{
	{ XK_BackSpace,Mod1Mask,keypress_delete },
	{ XK_Escape,Mod1Mask,keypress_pushapp },
	{ XK_Return,Mod1Mask,keypress_fullscreen },
	{ XK_space,Mod1Mask,keypress_sticky },
	{ XK_exclam,Mod1Mask,keypress_group },
};

/*
 * Return client stack,from bottom (first) to top (last).
 * Caller deallocates using free(3).
 */
void getclientstack(struct client ***vp,int *np)
{
		int n=0;
	List *lp;
	LIST_FOREACH(lp,&winstack)
	n++;
	struct client **v=(client**)xmalloc(n * sizeof *v);
	struct client **p=v;
	LIST_FOREACH(lp,&winstack)
	*p++=LIST_ITEM(lp,struct client,winstack);
	*vp=v;
	*np=n;
}

/*
 * Returns true if the window is,or should be,visible.
 */
Bool cisvisible(struct client *c)
{
		return (c->desk==curdesk || c->desk==DESK_ALL) && (c->isIcon==false);
}

void restack(void)
{
		if (!needrestack)
		return;
	int n=1;
	List *lp;
	LIST_FOREACH(lp,&winstack)
	n++;
	Window *v=(Window*)xmalloc(n * sizeof *v);
	int i=0;
	//assert(stacktop != None);
	v[i++]=stacktop;

	LIST_FOREACH_REV(lp,&winstack)
	{
		struct client *c=LIST_ITEM(lp,struct client,winstack);
		v[i++]=c->frame==NULL ? c->window : fgetwin(c->frame);
	}
	//assert(i==n);
	XRestackWindows(dpy,v,n);
	free(v);
	needrestack=False;

	ewmh_notifyrestack();
}

void cmap(struct client *c)
{
	// Prevent premature mapping
	if (!c->initialized)
		return;

	if (!c->ismapped)
		{
			/*
			 * Make sure stacking order is correct before
			 * mapping the window.
			 */
			restack();

			setwmstate(c->window,NormalState);
			if (c->frame != NULL)
				{
					Window f=fgetwin(c->frame);
					XMapSubwindows(dpy,f);
					XMapWindow(dpy,f);
				}
			else
				{
					XMapWindow(dpy,c->window);
				}
			c->ismapped=True;
		}
	setprop(c->window,NET_WM_DESKTOP,XA_CARDINAL,32,(void*)&c->desk,1);
}

bool shuffleDesktop(void)
{
		List	*lp;
	
	LIST_FOREACH(lp,&winstack)
		{
			struct client *c=LIST_ITEM(lp,struct client,winstack);
			if((c->checked==false) && (c->isDesktop==true))
				{
					LIST_REMOVE(&c->winstack);
					LIST_INSERT_HEAD(&winstack,&c->winstack);
					c->checked=true;
					return(true);
				}
			if((c->checked==false) && (c->isDock==true))
				{
					LIST_REMOVE(&c->winstack);
					LIST_INSERT_TAIL(&winstack,&c->winstack);
					c->checked=true;
					return(true);
				}
		}
	return(false);
}

bool shuffleDownUp(void)
{
		List	*lp;
	
	LIST_FOREACH(lp,&winstack)
		{
			struct client *c=LIST_ITEM(lp,struct client,winstack);
			if((c->checked==false) && (c->isBelow==true))
				{
					LIST_REMOVE(&c->winstack);
					LIST_INSERT_HEAD(&winstack,&c->winstack);
					c->checked=true;
					return(true);
				}
			if((c->checked==false) && (c->isAbove==true))
				{
					LIST_REMOVE(&c->winstack);
					LIST_INSERT_TAIL(&winstack,&c->winstack);
					c->checked=true;
					return(true);
				}
		}
	return(false);
}

void shuffle(void)
{
		List	*lp;

	LIST_FOREACH(lp,&winstack)
		{
			struct client *c=LIST_ITEM(lp,struct client,winstack);
			c->checked=false;
		}
	while(shuffleDownUp()==true);
	LIST_FOREACH(lp,&winstack)
		{
			struct client *c=LIST_ITEM(lp,struct client,winstack);
			c->checked=false;
		}
	while(shuffleDesktop()==true);
}

void cunmap(struct client *c)
{
		if (c->ismapped)
		{
			setwmstate(c->window,IconicState);
			if (c->frame != NULL)
				{
					Window f=fgetwin(c->frame);
					XUnmapWindow(dpy,f);
					XUnmapSubwindows(dpy,f);
				}
			else
				XUnmapWindow(dpy,c->window);
			c->ignoreunmapcount++;
			c->ismapped=False;
		}
}

void csetdesk(struct client *c,Desk d)
{
		if (d >= numberOfDesktops && d != DESK_ALL)
		d=numberOfDesktops-1;

	c->desk=d;
	ewmh_notifyclientdesktop(c->window,d);

	if (cisvisible(c))
		cmap(c);
	else
		cunmap(c);

	// May have become sticky
	if (c->frame != NULL)
		fupdate(c->frame);
}

void gotodesk(Desk d)
{
		if (d==curdesk || d >= numberOfDesktops || d==DESK_ALL)
		return;

	curdesk=d;

	/*
	 * Minimize the number of window exposures by first mapping
	 * windows from the new desk top-down,and then unmapping
	 * the windows from the old desk bottom-up.
	 */
	struct client **v;
	int n;
	getclientstack(&v,&n);
	for (int i=n-1; i >= 0; i--)
		if (cisvisible(v[i]))
			cmap(v[i]);
	for (int i=0; i<n; i++)
		if (v[i]->followdesk && v[i]->desk != DESK_ALL)
			csetdesk(v[i],curdesk);
		else if (!cisvisible(v[i]))
			cunmap(v[i]);
	free(v);

	ewmh_notifycurdesk(curdesk);
}

void setndesk(Desk val)
{
		if (val==0 || val >= 0xffffffffUL)
		return;
	Desk oldval=numberOfDesktops;
	numberOfDesktops=val;

	if (val >= oldval)
		ewmh_notifyndesk(val);

	if (curdesk >= val)
		gotodesk(val-1);

	struct client **v;
	int n;
	getclientstack(&v,&n);
	for (int i=n-1; i >= 0; i--)
		if (v[i]->desk != DESK_ALL && v[i]->desk >= val)
			csetdesk(v[i],val-1);
	free(v);

	if (val<oldval)
		ewmh_notifyndesk(val);
}

void csetappdesk(struct client *c,Desk d)
{
		struct client **v;
	int n;
	getclientstack(&v,&n);
	for (int i=0; i<n; i++)
		if (v[i]->app==c->app)
			csetdesk(v[i],d);
	free(v);
}

Bool cisframed(struct client *c)
{
		return !c->isfull && !c->isDock && !c->isundecorated;
}

void creframe(struct client *c)
{
	if (cisframed(c))
		{
			if (c->frame==NULL)
				c->frame=fcreate(c);
		}
	else if (c->frame != NULL)
		{
			fdestroy(c->frame);
			c->frame=NULL;
		}
}

void csetdock(struct client *c,Bool isdock)
{
		c->isDock=isdock;
	creframe(c);
}

/*
 * Change input focus to the specified client,which must be
 * mapped on the current desktop.
 *
 * This function ignores input hints and the WM_TAKE_FOCUS protocol.
 */
void cfocus(struct client *c,Time time)
{
		if (!c->ismapped)
		return;

	XSetInputFocus(dpy,c->window,RevertToPointerRoot,time);
}

void csetfull(struct client *c,Bool enabled)
{
		if (enabled && !c->isfull)
		{
			Bool f=c->hasfocus;
			cunmap(c);
			if (c->frame != NULL)
				{
					fdestroy(c->frame);
					c->frame=NULL;
				}
			c->isfull=True;
			XMoveResizeWindow(dpy,c->window,
			                  -c->geometry.borderwidth,
			                  -c->geometry.borderwidth,
			                  DisplayWidth(dpy,screen),
			                  DisplayHeight(dpy,screen));
			if (cisvisible(c))
				cmap(c);
			if (f)
				cfocus(c,CurrentTime);
			ewmh_notifyfull(c->window,True);
		}
	else if (!enabled && c->isfull)
		{
			//assert(c->frame==NULL);
			Bool f=c->hasfocus;
			cunmap(c);
			c->isfull=False;
			XMoveResizeWindow(dpy,c->window,
			                  c->geometry.x,c->geometry.y,
			                  c->geometry.width,c->geometry.height);
			if (cisframed(c))
				c->frame=fcreate(c);
			if (cisvisible(c))
				cmap(c);
			if (f)
				cfocus(c,CurrentTime);
			ewmh_notifyfull(c->window,False);
		}
}

void csetundecorated(struct client *c,Bool enabled)
{
		c->isundecorated=enabled;
	creframe(c);
}

/*
 * Return client window stack,from bottom (first) to top (last).
 * Caller deallocates using free(3).
 */
void getwindowstack(Window **vp,size_t *np)
{
		List *lp;

	size_t n=0;
	LIST_FOREACH(lp,&winstack)
	n++;

	Window *v=(Window*)xmalloc(n * sizeof v[0]);
	size_t i=0;
	LIST_FOREACH(lp,&winstack)
	{
		struct client *c=LIST_ITEM(lp,struct client,winstack);
		v[i++]=c->window;
	}

	*vp=v;
	*np=n;
}

void cpush(struct client *c)
{
		if (LIST_HEAD(&winstack) != &c->winstack)
		{
			LIST_REMOVE(&c->winstack);
			LIST_INSERT_HEAD(&winstack,&c->winstack);
			needrestack=True;
		}
}

void cpushapp(struct client *c)
{
		struct client **v;
	int n;
	getclientstack(&v,&n);
	for (int i=n-1; i >= 0; i--)
		if (v[i]->app==c->app)
			cpush(v[i]);
	free(v);
}

void cpop(struct client *c)
{
		if (LIST_TAIL(&winstack) != &c->winstack)
		{
			LIST_REMOVE(&c->winstack);
			LIST_INSERT_TAIL(&winstack,&c->winstack);
			needrestack=True;
			//if(c->isAbove==true)
			if(c->beenPopped==false)
				{
					cfocus(c,CurrentTime);
					c->beenPopped=true;
				}
		}
}

void cpopapp(struct client *c)
{
		struct client	**v;
	int				n;
	unsigned char	*data;
	Atom			*atoms;
	int				status,real_format;
	Atom			real_type;
	unsigned long	items_read,items_left;
	int				result=1;

	c->beenPopped=false;
	status=XGetWindowProperty(dpy,c->window,NET_WM_WINDOW_TYPE,0L,1L,False,XA_ATOM,&real_type,&real_format,&items_read,&items_left,&data);
	if(status==Success)
		{
			atoms=(Atom *)data;

			if(items_read && (atoms[0]==NET_WM_WINDOW_TYPE_DESKTOP ))
				{
					result=0;
				}
			XFree(data);
		}

	if(result==0)
		return;

	getclientstack(&v,&n);

	cpop(c);
	for(int i=0;i<n;i++)
		{
			if(v[i]->app==c->app)
				{
					if(v[i]->wmtransientfor!=None)
						cpop(v[i]);
				}
		}

//	if (c->wmtransientfor != None)
//		{
//		break;
////		for(int i=0;i<n;i++)
////		{
////			if (v[i]->app==c->app)
////			{
////				printf(">>>%s--%s<<<\n",v[i]->wmname,v[i]->netwmname);
////			}
////		}	
//			for (int i=0; i<n; i++)
//				if (v[i]->window==c->wmtransientfor)
//					{
//						cpop(v[i]);
//						break;
//					}
//			for (int i=0; i<n; i++)
//				if (v[i]->wmtransientfor==c->wmtransientfor)
//					cpop(v[i]);
//			cpop(c);
//		}
//	else
//		{
//			cpop(c);
//			for (int i=0; i<n; i++)
//				if (v[i]->wmtransientfor==c->window)
//					cpop(v[i]);
//		}

	c->beenPopped=false;
	free(v);

//	printf("-----\n");
}

void cupdatedesk(struct client *c)
{
		Desk d=c->desk;
	struct client **v;
	int n;
	getclientstack(&v,&n);
	if (c->wmtransientfor != None)
		{
			for (int i=n-1; i >= 0; i--)
				if (v[i]->window==c->wmtransientfor)
					{
						d=v[i]->desk;
						break;
					}
		}
	else if (c->app != c->window)
		{
			for (int i=n-1; i >= 0; i--)
				if (v[i]->app==c->app && v[i] != c)
					{
						d=v[i]->desk;
						break;
					}
		}
	free(v);
	if (d != c->desk)
		csetdesk(c,d);
}

void reloadwmtransientfor(struct client *c)
{
		c->wmtransientfor=None;
	XGetTransientForHint(dpy,c->window,&c->wmtransientfor);

	if (c->wmtransientfor != None)
		{
			c->app=c->wmtransientfor;
			XWMHints *h=XGetWMHints(dpy,c->wmtransientfor);
			if (h != NULL)
				{
					if (h->flags & WindowGroupHint)
						c->app=h->window_group;
					XFree(h);
				}
			cupdatedesk(c);
		}
}


Bool cisurgent(struct client *c)
{
		return c->wmhints != NULL && (c->wmhints->flags & XUrgencyHint) != 0;
}

void reloadwmhints(struct client *c)
{
		if (c->wmhints != NULL)
		XFree(c->wmhints);
	c->wmhints=XGetWMHints(dpy,c->window);

	if (c->wmtransientfor==None)
		{
			c->app=c->window;
			if (c->wmhints != NULL)
				{
					if (c->wmhints->flags & WindowGroupHint)
						c->app=c->wmhints->window_group;
				}
			cupdatedesk(c);
		}

	if (cisurgent(c) && c->initialized)
		{
			XBell(dpy,0);
			cpopapp(c);
			gotodesk(c->desk);
			cfocus(c,CurrentTime);
		}
}

void reloadwmnormalhints(struct client *c)
{
		if (c->wmnormalhints==NULL)
		c->wmnormalhints=XAllocSizeHints();
	if (c->wmnormalhints != NULL)
		{
			c->wmnormalhints->flags=0;
			long dummy;
			XGetWMNormalHints(dpy,c->window,c->wmnormalhints,&dummy);
		}
}

void reloadwmname(struct client *c)
{
		free(c->wmname);
	c->wmname=NULL;

	XTextProperty p;
	if (XGetWMName(dpy,c->window,&p) != 0)
		{
			c->wmname=decodetextproperty(&p);
			if (p.value != NULL)
				XFree(p.value);
		}

	if (c->frame != NULL)
		fupdate(c->frame);
}

void reloadwmprotocols(struct client *c)
{
		if (c->wmprotocols != NULL)
		{
			XFree(c->wmprotocols);
			c->wmprotocols=NULL;
		}
	c->wmprotocolscount=0;
	XGetWMProtocols(dpy,c->window,
	                &c->wmprotocols,&c->wmprotocolscount);

	if (c->frame != NULL)
		fupdate(c->frame);
}

void buttonpress(struct client *c,XButtonEvent *e)
{
		cpopapp(c);
	cfocus(c,e->time);
	XAllowEvents(dpy,ReplayPointer,e->time);
}

void keypress(struct client *c,XKeyEvent *e)
{
		for (unsigned int i=0; i<NELEM(keymapclient); i++)
		if (XKeysymToKeycode(dpy,keymapclient[i].keysym)==e->keycode)
			keymapclient[i].function(c,e->state,e->time);
}

Bool chaswmproto(struct client *c,Atom protocol)
{
		for (int i=0; i<c->wmprotocolscount; i++)
		if (c->wmprotocols[i]==protocol)
			return True;
	return False;
}

void csendwmproto(struct client *c,Atom protocol,Time time)
{
		XEvent e;

	memset(&e,0,sizeof e);
	e.xclient.type=ClientMessage;
	e.xclient.window=c->window;
	e.xclient.message_type=WM_PROTOCOLS;
	e.xclient.format=32;
	e.xclient.data.l[0]=protocol;
	e.xclient.data.l[1]=time;

	XSendEvent(dpy,c->window,False,0L,&e);
}

void cdelete(struct client *c,Time time)
{
		if (chaswmproto(c,WM_DELETE_WINDOW))
		csendwmproto(c,WM_DELETE_WINDOW,time);
}

void keypress_delete(struct client *c,unsigned state,Time time)
{
		if (!c->isDock)
		cdelete(c,time);
}

/*
 * Returns true if the window is a task,i.e. if it appears in taskbars.
 */
Bool cistask(struct client *c)
{
		return !c->skiptaskbar && c->wmtransientfor==None;
}

struct client *getfronttask(void)
{
		List *lp;
	LIST_FOREACH_REV(lp,&winstack)
	{
		struct client *c=LIST_ITEM(lp,struct client,winstack);
		if (cisvisible(c) && cistask(c))
			return c;
	}
	return NULL;
}


/*
 * Returns true if the window expects keyboard input focus.
 * A window that does not specify an input hint is considered
 * to expect focus.
 */
Bool expectsfocus(struct client *c)
{
		return c->wmhints==NULL ||
	       !(c->wmhints->flags & InputHint) ||
	       c->wmhints->input;
}

void cfocusapp(struct client *c,Time time)
{
		struct client *topmost=NULL;
	struct client *focus=NULL;

	// Find a window of the application that expects focus.
	List *lp;
	LIST_FOREACH_REV(lp,&winstack)
	{
		struct client *x=LIST_ITEM(
		                     lp,struct client,winstack);
		if (x->app==c->app && cisvisible(x))
			{
				if (topmost==NULL)
					topmost=x;
				if (focus==NULL && expectsfocus(x))
					focus=x;
			}
	}

	if (focus==NULL)
		focus=topmost;

	//assert(focus != NULL);

	cfocus(focus,time);
}

/*
 * Focus the front window and return it.
 */
struct client *refocus(Time time)
{
		struct client *c=getfronttask();
	if (c != NULL)
		cfocusapp(c,time);
	return c;
}

void keypress_pushapp(struct client *c,unsigned state,Time time)
{
		cpushapp(c);
	refocus(time);
}

void keypress_fullscreen(struct client *c,unsigned state,Time time)
{
		if (!c->isDock)
		csetfull(c,!c->isfull);
}

void keypress_sticky(struct client *c,unsigned state,Time time)
{
		if (c->isDock)
		return;

	if (c->desk==DESK_ALL)
		csetappdesk(c,curdesk);
	else
		{
			csetappdesk(c,DESK_ALL);

			// Make sure we are still on top when switching desks.
			cpopapp(c);
		}
}

void keypress_group(struct client *c,unsigned state,Time time)
{
		struct client	**v;
	int				n;
	int				cd=curdesk;

	getclientstack(&v,&n);
	for (int i=0; i<n; i++)
		{
		if (v[i]->app==c->app)
			{
				v[i]->desk=curdesk;
			}
		}
	free(v);
	curdesk=-1;
	gotodesk(cd);
}

void cinstallcolormaps(struct client *c)
{
		XInstallColormap(dpy,c->colormap==None ?
	                 DefaultColormap(dpy,screen) : c->colormap);
}

void focusin(struct client *c,XFocusChangeEvent *e)
{
		if (e->mode==NotifyUngrab || e->detail==NotifyPointerRoot ||
	        e->detail==NotifyPointer)
		return;

	// This shouldn't happen.
	if (c->hasfocus || !c->ismapped)
		return;

	c->hasfocus=True;

	ungrabbutton(AnyButton,AnyModifier,c->window);

	cinstallcolormaps(c);

	if (c->frame != NULL)
		fupdate(c->frame);

	ewmh_notifyfocus(None,c->window);
}

void focusout(struct client *c,XFocusChangeEvent *e)
{
		if (e->mode==NotifyGrab)
		return;

	if (e->detail==NotifyPointerRoot ||
	        e->detail==NotifyPointer ||
	        e->detail==NotifyInferior)
		return;

	// This shouldn't happen.
	if (!c->hasfocus)
		return;

	c->hasfocus=False;

	grabbutton(AnyButton,AnyModifier,c->window,True,0,
	           GrabModeSync,GrabModeAsync,None,None);

	if (c->frame != NULL)
		fupdate(c->frame);

	ewmh_notifyfocus(c->window,None);
}

struct geometry cgetgeom(struct client *c)
{
		if (c->isfull)
		return (struct geometry)
		{
			.x=-c->geometry.borderwidth,
			 .y=-c->geometry.borderwidth,
			  .width=DisplayWidth(dpy,screen),
			   .height=DisplayHeight(dpy,screen),
			    .borderwidth=c->geometry.borderwidth
		};
	else
		return c->geometry;
}

void csendconf(struct client *c)
{
		struct geometry g=cgetgeom(c);
	XConfigureEvent ce;

	ce.type=ConfigureNotify;
	ce.event=c->window;
	ce.window=c->window;
	ce.x=g.x;
	ce.y=g.y;
	ce.width=g.width;
	ce.height=g.height;
	ce.border_width=g.borderwidth;
	ce.above=None;
	ce.override_redirect=False;

	XSendEvent(dpy,c->window,False,StructureNotifyMask,(XEvent*)&ce);
}


void configurerequest(struct client *c,XConfigureRequestEvent *e)
{
		XWindowChanges	wc;

	if (c->frame != NULL)
		{
			/*
			 * If this happens,we are processing an event that
			 * was sent before we created the frame. We need to
			 * redirect the event manually. Note that this should
			 * only happen immediately after creating a frame.
			 *
			 * XMMS is one program that triggers this particularly
			 * often,and so is the "Save As" dialog of Firefox.
			 */
			redirect((XEvent *)e,fgetwin(c->frame));
			return;
		}

	if (c->isfull)
		{
			// Deny fullscreen windows to reconfigure themselves.
			csendconf(c);
			return;
		}

	unsigned long mask=e->value_mask &
	                   (CWX | CWY | CWWidth | CWHeight | CWBorderWidth);

	if (mask & CWX)
		c->geometry.x=e->x;
	if (mask & CWY)
		c->geometry.y=e->y;
	if (mask & CWWidth)
		c->geometry.width=e->width;
	if (mask & CWHeight)
		c->geometry.height=e->height;
	if (mask & CWBorderWidth)
		c->geometry.borderwidth=e->border_width;

	wc.x=c->geometry.x;
	wc.y=c->geometry.y;
	wc.width=c->geometry.width;
	wc.height=c->geometry.height;
	wc.border_width=c->geometry.borderwidth;

	XConfigureWindow(dpy,c->window,mask,&wc);
}

void propertynotify(struct client *c,XPropertyEvent *e)
{
		switch (e->atom)
		{
		case XA_WM_NAME:
			reloadwmname(c);
			break;
		case XA_WM_HINTS:
			reloadwmhints(c);
			break;
		case XA_WM_NORMAL_HINTS:
			reloadwmnormalhints(c);
			break;
		case XA_WM_TRANSIENT_FOR:
			reloadwmtransientfor(c);
			break;
		default:
			if (e->atom==WM_PROTOCOLS)
				reloadwmprotocols(c);
			break;
		}

	ewmh_propertynotify(c,e);
	mwm_propertynotify(c,e);
}

/*
 * We don't listen to this event ourselves,but get it redirected to
 * us from the root listener and from the frame listener.
 */
void maprequest(struct client *c,XMapRequestEvent *e)
{
		ewmh_maprequest(c);
	cpopapp(c);
	if (cisvisible(c))
		{
			cmap(c);
			cfocus(c,CurrentTime);
		}
}

struct client *getfocus(void)
{
		List *lp;
	LIST_FOREACH_REV(lp,&winstack)
	{
		struct client *c=LIST_ITEM(lp,struct client,winstack);
		if (c->hasfocus)
			{
				//assert(c->desk==curdesk || c->desk==DESK_ALL);
				//assert(c->ismapped);
				return c;
			}
	}
	return NULL;
}

void crelease(struct client *c,Bool clientrequested)
{
		// Unset this or fdestroy() will refocus the window.
	c->hasfocus=False;
	c->isundecorated=true;

	ungrabkey(AnyKey,AnyModifier,c->window);

	if (c->frame != NULL)
		{
			fdestroy(c->frame);
			c->frame=NULL;
		}
	LIST_REMOVE(&c->winstack);
	needrestack=True;


	XSelectInput(dpy,c->window,0);
	setlistener(c->window,NULL);
	if (!clientrequested)
		XMapWindow(dpy,c->window);
	XRemoveFromSaveSet(dpy,c->window);

	if (c->wmnormalhints != NULL)
		XFree(c->wmnormalhints);
	if (c->wmhints != NULL)
		XFree(c->wmhints);
	if (c->wmprotocols != NULL)
		XFree(c->wmprotocols);
	free(c->wmname);
	free(c->netwmname);
	free(c);

	if (getfocus()==NULL)
		refocus(CurrentTime);
}

void cwithdraw(struct client *c)
{
		ewmh_withdraw(c);
	setwmstate(c->window,WithdrawnState);
	crelease(c,True);
}


void unmapnotify(struct client *c,XUnmapEvent *e)
{
		if (c->ignoreunmapcount>0)
		c->ignoreunmapcount--;
	else
		cwithdraw(c);
}

void destroynotify(struct client *c,XDestroyWindowEvent *e)
{
		cwithdraw(c);
}

Bool chasfocus(struct client *c)
{
		return c->hasfocus;
}

void clientmessage(struct client *c,XClientMessageEvent *e)
{
		ewmh_clientmessage(c,e);
}

void colormapnotify(struct client *c,myXColormapEvent *e)
{
		if (e->mynew)
		{
			c->colormap=e->colormap;
			if (c->hasfocus)
				cinstallcolormaps(c);
		}
}

void csetgeom(struct client *c,struct geometry g)
{
		c->geometry=g;
}

bool clientevent(void *self,XEvent *e,int type)
{
		switch (e->type)
		{
		case ButtonRelease:
			break;
		case ButtonPress:
			buttonpress((client*)self,&e->xbutton);
			break;
		case KeyPress:
			keypress((client*)self,&e->xkey);
			break;
		case FocusIn:
//printf(">> clientevent(void *self,XEvent *e) FocusIn<<\n");
			focusin((client*)self,&e->xfocus);
			break;
		case FocusOut:
			focusout((client*)self,&e->xfocus);
			break;
		case ConfigureRequest:
			configurerequest((client*)self,&e->xconfigurerequest);
			break;
		case PropertyNotify:
			propertynotify((client*)self,&e->xproperty);
			break;
		case MapRequest:
//printf("MapRequest client event\n");
			// We get this redirected to us from the root listener
			// and from the frame listener.
			maprequest((client*)self,&e->xmaprequest);
			break;
		case UnmapNotify:
//printf("UnmapNotify client event\n");
			unmapnotify((client*)self,&e->xunmap);
			break;
		case DestroyNotify:
//printf("DestroyNotify client event\n");
			destroynotify((client*)self,&e->xdestroywindow);
			break;
		case ClientMessage:
//printf("ClientMessage client event\n");
			clientmessage((client*)self,&e->xclient);
			break;
		case ColormapNotify:
			colormapnotify((client*)self,(myXColormapEvent*)&e->xcolormap);
			break;
		case Expose:
//printf("expose event\n");
			break;
		}
	return(true);
}

/*
 * Return true if,and only if,the two clients are visible on the same desk.
 */
Bool samedesk(struct client *c1,struct client *c2)
{
		return c1->desk==c2->desk ||
	       c1->desk==DESK_ALL || c2->desk==DESK_ALL;
}

/*
 * Find a random location for the specified geometry.
 */
void randomPosition(struct geometry *g,int monnum)
{
		int wid;
	int hite;
	int xoff;
	int yoff;
	int maxx;
	int maxy;

	if(monnum==-1)
		{
			wid=displayWidth;
			hite=displayHeight;
			xoff=0;
			yoff=0;
		}
	else
		{
			wid=monitorData[monnum].monW;
			hite=monitorData[monnum].monH;
			xoff=monitorData[monnum].monX;
			yoff=monitorData[monnum].monY;
		}

	maxx=wid-(g->width+2 * g->borderwidth);
	maxy=hite-(g->height+2 * g->borderwidth);
	
	if(maxx>0)
		g->x=(rand() % maxx)+xoff;
	else
		g->x=0;

	if(maxy>0)
		g->y=(rand() % maxy)+yoff;
	else
		g->y=0;
}


unsigned long overlaparea(struct geometry g1,struct geometry g2)
{
		int x1=g1.x;
	int x2=g2.x;
	int x3=g1.x+g1.width+2 * g1.borderwidth;
	int x4=g2.x+g2.width+2 * g2.borderwidth;
	int y1=g1.y;
	int y2=g2.y;
	int y3=g1.y+g1.height+2 * g1.borderwidth;
	int y4=g2.y+g2.height+2 * g2.borderwidth;

	if (x1<x4 && x2<x3 && y1<y4 && y2<y3)
		{
			unsigned long x=MIN(x4-x1,x3-x2);
			unsigned long y=MIN(y4-y1,y3-y2);
			unsigned long area=x * y;
			return area;
		}
	else
		return 0;
}

/*
 * XXX: We move a window by simulating a ConfigureRequest from
 *      the client.
 */
void move(struct client *c,int x,int y)
{
		Window parent=c->frame==NULL ? root : fgetwin(c->frame);
	XEvent e;
	XConfigureRequestEvent cre;

	cre.type=ConfigureRequest;
	cre.serial=0;
	cre.send_event=True;
	cre.display=dpy;
	cre.parent=parent;
	cre.window=c->window;
	cre.x=x;
	cre.y=y;
	cre.value_mask=CWX | CWY;
	e.xconfigurerequest=(XConfigureRequestEvent)cre;
	redirect(&e,parent);
}

int getMouseMonitor(client *c)
{
		Window			root_return;
	Window			child_return;
	int				root_x_return;
	int				root_y_return;
	int				win_x_return;
	int				win_y_return;
	unsigned int	mask_return;

	XQueryPointer(dpy,c->window,&root_return,&child_return,&root_x_return,&root_y_return,&win_x_return,&win_y_return, &mask_return);
	for(unsigned int j=0;j<numberOfMonitors;j++)
		{
			if((root_x_return>monitorData[j].monX) && (root_x_return<monitorData[j].monW+monitorData[j].monX) && (root_y_return>monitorData[j].monY) && (root_y_return<monitorData[j].monH+monitorData[j].monY))
				return(j);
		}
	return(0);
}

/*
 * Find a good location for the specified client and move it there.
 *
 * A 'good' location is found by testing lots of random locations and
 * picking the one with the lowest 'badness' score. Overlapping another
 * window is very bad. Being far from screen edges is pretty bad,as
 * that tends to break up free areas.
 *
 * Window placement is about the only intelligent task of a window
 * manager,and it's worth to spend some extra CPU time here in order
 * to find a really good location.
 */
void smartpos(struct client *c)
{
		Window			root_return;
	Window			child_return;
	int				root_x_return;
	int				root_y_return;
	int				win_x_return;
	int				win_y_return;
	unsigned int	mask_return;
	int				inmonitor;

	geometry		g;

	if(c->frame==NULL)
		g=cgetgeom(c);
	else
		g=fgetgeom(c->frame);

	switch(placement)
		{
			case UNDERMOUSE:
				XQueryPointer(dpy,c->window,&root_return,&child_return,&root_x_return,&root_y_return,&win_x_return,&win_y_return, &mask_return);
				root_x_return=win_x_return-(g.width/2);
				if(root_x_return<0)
					root_x_return=0;
				if(root_x_return+g.width>displayWidth)
					root_x_return=displayWidth-g.width;

				root_y_return=win_y_return-(g.height/2);
				if(root_y_return<0)
					root_y_return=0;
				if(root_y_return+g.height>displayHeight)
					root_y_return=displayHeight-g.height;

				move(c,root_x_return,root_y_return);
				return;
				break;

			case CENTRESCREEN:
				win_x_return=(displayWidth/2)-(g.width/2);
				win_y_return=(displayHeight/2)-(g.height/2);
				move(c,win_x_return,win_y_return);
				return;
				break;
	
			case CENTREMMONITOR:
				inmonitor=getMouseMonitor(c);
				win_x_return=monitorData[inmonitor].monX+((monitorData[inmonitor].monW/2)-(g.width/2));
				win_y_return=monitorData[inmonitor].monY+((monitorData[inmonitor].monH/2)-(g.height/2));
				move(c,win_x_return,win_y_return);
				return;
				break;

			case MOUSEMONITOR:
				inmonitor=getMouseMonitor(c);
				break;

			case NOPLACE:
				inmonitor=-1;
				break;
		}

	struct client **v;
	int n;
	getclientstack(&v,&n);

	// Exclude the window itself,and clients on other desks
	for (int i=0; i<n; i++)
		//if (v[i]==c || !samedesk(v[i],c))
		if (v[i]==c || !samedesk(v[i],c))
			v[i--]=v[--n];

	unsigned long min=~0;
	struct geometry best=g;
	for (int k=0; min != 0 && k<100; k++)
		{
			//randpos(&g);
			randomPosition(&g,inmonitor);
			unsigned long badness=0;
			unsigned overlaps=0;

			// Compute overlapping area.
			for (int i=0; i<n; i++)
				{
					struct geometry g2=v[i]->frame==NULL ?
					                   cgetgeom(v[i]) : fgetgeom(v[i]->frame);
					unsigned long area=overlaparea(g,g2);
					if (area>0)
						{
							badness += area;
							overlaps++;
						}
				}
			// The more overlapping windows the worse.
			badness *= overlaps * overlaps;

			// Prefer to position a window near the edges of the display.
			unsigned x2=DisplayWidth(dpy,screen)-(g.x+g.width);
			badness += MIN((int)g.x,(int)x2);
			unsigned y2=DisplayHeight(dpy,screen)-(g.y+g.height);
			badness += MIN((int)g.y,(int)y2);

			if (badness<min)
				{
					min=badness;
					best=g;
				}
		}

	move(c,best.x,best.y);

	free(v);
}

struct client *manage(Window window)
{
	XWindowAttributes	attr;
	Atom				type;
	int					format;
	unsigned long		nItem,bytesAfter;
	unsigned char		*properties=NULL;
	bool 				down=false;
	bool 				up=false;
	unsigned long		n=0;
	Atom				*types=NULL;
	int					thisdesk=curdesk;
	mwmhints			*hints;

	if (!XGetWindowAttributes(dpy,window,&attr))
		return NULL;
	if (attr.override_redirect)
		return NULL;

	long wmstate=getwmstate(window);
	if (wmstate==WithdrawnState)
		{
			XWMHints *h=XGetWMHints(dpy,window);
			if (h==NULL)
				wmstate=NormalState;
			else
				{
					if (h->flags & StateHint)
						wmstate=h->initial_state;
					else
						wmstate=NormalState;
					XFree(h);
				}
		}
	if (wmstate==WithdrawnState)
		return NULL;

	struct client *c=(client*)xmalloc(sizeof *c);

	LIST_INIT(&c->winstack);
	LIST_INSERT_TAIL(&winstack,&c->winstack);
	needrestack=True;

	c->desk=curdesk;
	c->frame=NULL;
	c->wmname=NULL;
	c->netwmname=NULL;
	c->wmhints=NULL;
	c->wmnormalhints=NULL;
	c->wmprotocols=NULL;
	c->wmprotocolscount=0;
	c->wmtransientfor=None;

	c->window=window;
	c->app=window;
	c->colormap=attr.colormap;

	c->ignoreunmapcount=0;

	c->hasfocus=False;
	c->isfull=False;
	c->isDock=False;
	c->skiptaskbar=False;
	c->isundecorated=False;
	c->followdesk=False;
	c->initialized=False;
	c->isDesktop=False;
	c->monitorNumber=0;
	c->isAbove=false;
	c->isBelow=false;
	c->isDock=false;
	c->canMaximize=true;
	c->canMinimize=true;
	c->isIcon=false;
	c->beenPopped=false;

	csetgeom(c,(struct geometry)
	{
		attr.x,
		attr.y,
		attr.width,
		attr.height,
		attr.border_width
	});

	XAddToSaveSet(dpy,c->window);

	/*
	 * Everything initialized to default values. Register
	 * for events and THEN (re)load all attributes and
	 * properties. This avoids losing update events.
	 */
	c->listener.function=clientevent;
	c->listener.pointer=c;
	setlistener(c->window,&c->listener);
	XSelectInput(dpy,c->window,StructureNotifyMask | PropertyChangeMask | ColormapChangeMask | FocusChangeMask);

	XSync(dpy,False);
	/*
	 * Done registering for events. What we read now is
	 * safe to use,since any updates will be notified
	 * to our event listener.
	 */

	c->ismapped=ismapped(c->window);

	if (!XGetWindowAttributes(dpy,c->window,&attr))
		{
			// The window disappeared.
			crelease(c,True);
			return NULL;
		}

	csetgeom(c,(struct geometry)
	{
		attr.x,
		attr.y,
		attr.width,
		attr.height,
		attr.border_width
	});

	c->colormap=attr.colormap;

	reloadwmname(c);
	reloadwmhints(c);
	reloadwmnormalhints(c);
	reloadwmprotocols(c);
	reloadwmtransientfor(c);


	properties=NULL;
	XGetWindowProperty(dpy,c->window,NET_WM_STATE,0,(~0L),False,AnyPropertyType,&type,&format,&nItem,&bytesAfter,&properties);
	for (unsigned int j=0;j<nItem;j++)
		{
			if(((unsigned long *)(properties))[j]==NET_WM_STATE_STICKY)
				{
					c->followdesk=true;
					//printf("sticky-%s\n",c->wmname);
					down=true;
				}

			if(((unsigned long *)(properties))[j]==NET_WM_STATE_BELOW)
				{
					//printf("below-%s\n",c->wmname);
					c->isBelow=true;
					c->isAbove=false;
					down=true;
				}

			if(((unsigned long *)(properties))[j]==NET_WM_STATE_ABOVE)
				{
					//printf("above-%s\n",c->wmname);
					c->isAbove=true;
					c->isBelow=false;
					up=true;
				}
		}
	XFree(properties);

	types=(Atom*)getprop(c->window,NET_WM_WINDOW_TYPE,XA_ATOM,32,&n);
	if (types != NULL)
		{
			for (unsigned long i=0; i<n; i++)
				{
				if (types[i]==NET_WM_WINDOW_TYPE_DESKTOP)
					{
						c->followdesk=true;
						c->isDesktop=True;
						c->isundecorated=true;
						down=true;
					}
				if (types[i]==NET_WM_WINDOW_TYPE_DOCK)
					{
						c->followdesk=true;
						c->isDock=true;
						c->isundecorated=true;
						up=true;
					}
				}
			XFree(types);
			types=NULL;
		}

	/*
	 * Let the hints create the frame,if there should be one.
	 */

	hints=getmwmhints(c);
	if (hints!=NULL)
		{
			if (hints->flags & MWM_HINTS_DECORATIONS)
				{
					/*
					 * If MWM_DECOR_ALL is set,it means use all
					 * decorations EXCEPT the ones specified...
					 */
					if ((hints->decorations & MWM_DECOR_ALL) != 0)
						hints->decorations=~hints->decorations;
					if ((hints->decorations & MWM_DECOR_MAXIMIZE) == 0)
						c->canMaximize=false;
					if ((hints->decorations & MWM_DECOR_MINIMIZE) == 0)
						c->canMinimize=false;

					if (hints->decorations==0)
						c->isundecorated=true;

				}
			XFree(hints);
		}

	ewmh_manage(c);
	mwm_manage(c);

	grabbutton(AnyButton,AnyModifier,c->window,True,0,
	           GrabModeSync,GrabModeAsync,None,None);

	for (unsigned int i=0; i<NELEM(keymapclient); i++)
		grabkey(XKeysymToKeycode(dpy,keymapclient[i].keysym),
		        keymapclient[i].modifiers,c->window,True,
		        GrabModeAsync,GrabModeAsync);

	if (c->geometry.width==DisplayWidth(dpy,screen) &&
	        c->geometry.height==DisplayHeight(dpy,screen))
		csetfull(c,True);

	if (!cisframed(c))
		ewmh_notifyframeextents(c->window,(struct extents)
		{
			.top=0,
			 .bottom=0,
			  .left=0,
			   .right=0
		});

	XSizeHints *h=c->wmnormalhints;
	if (runlevel != RL_STARTUP && (h==NULL || (h->flags & (USPosition | PPosition))==0))
		smartpos(c);

	c->desk=curdesk;
	/*
	 * Make sure WM_STATE is always initiated. We can't trust
	 * the first call to cmap/cunmap.
	 */
	setwmstate(c->window,cisvisible(c) ? NormalState : IconicState);
	c->initialized=True;

	if (wmstate==IconicState && runlevel==RL_NORMAL)
		{
			// Closest thing to iconic state
			cpush(c);
			if (cisvisible(c))
				cmap(c);
		}
	else
		{
			cpopapp(c);
			if (cisurgent(c) && runlevel==RL_NORMAL)
				{
					XBell(dpy,0);
					gotodesk(c->desk);
				}
			if (cisvisible(c))
				{
					cmap(c);
					if (runlevel==RL_NORMAL)
						cfocus(c,CurrentTime);
				}
		}

	csetdesk(c,thisdesk);

	if(down==true)
		{
			LIST_REMOVE(&c->winstack);
			LIST_INSERT_HEAD(&winstack,&c->winstack);
			if(c->isDesktop==true)
				reloadwindowdesktop(c);
			needrestack=True;
		}

	if(up==true)
		{
			LIST_REMOVE(&c->winstack);
			LIST_INSERT_TAIL(&winstack,&c->winstack);
			needrestack=True;
		}

	return c;
}

void manageall(void)
{
		//assert(stacktop==None);
	stacktop=XCreateWindow(dpy,root,0,0,100,100,0,CopyFromParent,InputOnly,CopyFromParent,0,NULL);
	Window r,p,*stack;
	unsigned n;

	if (XQueryTree(dpy,root,&r,&p,&stack,&n) != 0)
		{
			for (unsigned int i=0; i<n; i++)
				{
					if (ismapped(stack[i]))
						manage(stack[i]);
				}
			if (stack != NULL)
				XFree(stack);
		}
	restack();
}

void cunmanage(struct client *c)
{
		ewmh_unmanage(c);
	setwmstate(c->window,NormalState);
	crelease(c,False);
}

void unmanageall(void)
{
	struct client **v;
	int n;
	getclientstack(&v,&n);
	for (int i=n-1; i >= 0; i--)
		cunmanage(v[i]);
	free(v);

	if (stacktop != None)
		{
			XDestroyWindow(dpy,stacktop);
			stacktop=None;
		}
}

int namewidth(struct font *font,struct client *c)
{
	if (c->netwmname != NULL)
		return fttextwidth_utf8(font,c->netwmname);
	else if (c->wmname != NULL)
		return fttextwidth(font,c->wmname);
	else
		return 0;
}

char* getMaxString(struct font *font,struct client *c,int width)
{
		char	*buffer;
	int		maxwid;

	if (c->netwmname != NULL)
		{
			buffer=strdup(c->netwmname);
			maxwid=fttextwidth_utf8(font,buffer);
			while(maxwid>width)
				{
					if(strlen(buffer)>1)
						{
							buffer[strlen(buffer)-1]=0;
							maxwid=fttextwidth_utf8(font,buffer);
						}
					else
						return(buffer);
				}
			return(buffer);
		}

	if (c->wmname != NULL)
		{
			buffer=strdup(c->wmname);
			maxwid=fttextwidth(font,buffer);
			while(maxwid>width)
				{
					if(strlen(buffer)>1)
						{
							buffer[strlen(buffer)-1]=0;
							maxwid=fttextwidth(font,buffer);
						}
					else
						return(buffer);
				}
			return(buffer);
		}
	return(NULL);
}

void drawname(Drawable d,struct font *font,struct fontcolor *color,int x,int y,struct client *c)
{
		if (c->netwmname != NULL)
		ftdrawstring_utf8(d,font,color,x,y,c->netwmname);
	else if (c->wmname != NULL)
		ftdrawstring(d,font,color,x,y,c->wmname);
}

int cgetgrav(struct client *c)
{
		if (c->wmnormalhints != NULL &&
	        (c->wmnormalhints->flags & PWinGravity) != 0)
		return c->wmnormalhints->win_gravity;
	else
		return NorthWestGravity;
}

void chintsize(struct client *c,int width,int height,int *rwidth,int *rheight)
{
		width=MAX(1,width);
	height=MAX(1,height);

	XSizeHints *h=c->wmnormalhints;
	if (h==NULL)
		{
			*rwidth=width;
			*rheight=height;
			return;
		}

	/*
	 * Aspect ratio hints,with the following meaning:
	 *
	 *       min_aspect.x    width     max_aspect.x
	 *   0<------------ <= ------ <= ------------<inf
	 *       min_aspect.y    height    max_aspect.y
	 *
	 * Ignore the hints if the values are negative,zero,or
	 * otherwise out of range. This also avoids division by zero.
	 *
	 * FIXME: We should avoid screwing up these limits further
	 * down when adjusting for size increments and min/max size.
	 */
	if ((h->flags & PAspect) != 0 &&
	        h->min_aspect.x>0 && h->min_aspect.y>0 &&
	        h->max_aspect.x>0 && h->max_aspect.y>0 &&
	        h->min_aspect.x * h->max_aspect.y <=
	        h->max_aspect.x * h->min_aspect.y)
		{
			int minwidth=height * h->min_aspect.x / h->min_aspect.y;
			if (width<minwidth)
				width=minwidth;
			int minheight=width * h->max_aspect.y / h->max_aspect.x;
			if (height<minheight)
				height=minheight;
		}

	int basewidth;
	int baseheight;

	if (h->flags & PBaseSize)
		{
			basewidth=h->base_width;
			baseheight=h->base_height;
		}
	else if (h->flags & PMinSize)
		{
			basewidth=h->min_width;
			baseheight=h->min_height;
		}
	else
		{
			basewidth=0;
			baseheight=0;
		}

	// Cannot be less than the base (or minimum)
	width=MAX(basewidth,width);
	height=MAX(baseheight,height);

	if (h->flags & PResizeInc)
		{
			if (h->width_inc != 0)
				width -= (width-basewidth) % h->width_inc;
			if (h->height_inc != 0)
				height -= (height-baseheight) % h->height_inc;
		}

	if (h->flags & PMaxSize)
		{
			width=MIN(h->max_width,width);
			height=MIN(h->max_height,height);
		}

	width=MAX(1,width);
	height=MAX(1,height);

	*rwidth=width;
	*rheight=height;
}

void csetnetwmname(struct client *c,const char *name)
{
		free(c->netwmname);
	c->netwmname=(name==NULL) ? NULL : xstrdup(name);

	if (c->frame != NULL)
		fupdate(c->frame);
}

void cignoreunmap(struct client *c)
{
		//assert(c->ismapped);
	c->ignoreunmapcount++;
}

