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

#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <X11/Xlib.h>

#include "wind.h"
#include "lib.h"
#include "client.h"
#include "ewmh.h"
#include "root.h"
#include "atoms.h"
#include "frame.h"

void fnkey(KeySym keysym,unsigned state,Time time,int arg)
{
		if (state & ShiftMask)
		setndesk(arg);
	gotodesk(arg-1);
	refocus(time);
}

keybind keymap[] =
{
	{ XK_F1,Mod1Mask,fnkey,1 },
	{ XK_F2,Mod1Mask,fnkey,2 },
	{ XK_F3,Mod1Mask,fnkey,3 },
	{ XK_F4,Mod1Mask,fnkey,4 },
	{ XK_F5,Mod1Mask,fnkey,5 },
	{ XK_F6,Mod1Mask,fnkey,6 },
	{ XK_F7,Mod1Mask,fnkey,7 },
	{ XK_F8,Mod1Mask,fnkey,8 },
	{ XK_F9,Mod1Mask,fnkey,9 },
	{ XK_F10,Mod1Mask,fnkey,10 },
	{ XK_F11,Mod1Mask,fnkey,11 },
	{ XK_F12,Mod1Mask,fnkey,12 },

	{ XK_F1,ShiftMask | Mod1Mask,fnkey,1 },
	{ XK_F2,ShiftMask | Mod1Mask,fnkey,2 },
	{ XK_F3,ShiftMask | Mod1Mask,fnkey,3 },
	{ XK_F4,ShiftMask | Mod1Mask,fnkey,4 },
	{ XK_F5,ShiftMask | Mod1Mask,fnkey,5 },
	{ XK_F6,ShiftMask | Mod1Mask,fnkey,6 },
	{ XK_F7,ShiftMask | Mod1Mask,fnkey,7 },
	{ XK_F8,ShiftMask | Mod1Mask,fnkey,8 },
	{ XK_F9,ShiftMask | Mod1Mask,fnkey,9 },
	{ XK_F10,ShiftMask | Mod1Mask,fnkey,10 },
	{ XK_F11,ShiftMask | Mod1Mask,fnkey,11 },
	{ XK_F12,ShiftMask | Mod1Mask,fnkey,12 },
};

// True if the pointer is on this screen
Bool	pointerhere;
int		scrollcnt;
int		scrollcntmax=2;

void maprequest(XMapRequestEvent *e)
{
		// Already managed?
	if (redirect((XEvent *)e,e->window)==0)
		return;

	// Try to manage it,otherwise just map it.
	if (manage(e->window)==NULL)
		XMapWindow(dpy,e->window);
}

/*
 * Refer to the ICCCM section 4.1.4,"Changing Window State",
 * for information on the synthetic UnmapNotify event sent
 * by clients to the root window on withdrawal.
 */
void unmapnotify(XUnmapEvent *e)
{
		if (e->send_event)
		redirect((XEvent *)e,e->window);
}

/*
 * Refocus whenever the pointer enters our root window from
 * another screen.
 */
void enternotify(XCrossingEvent *e)
{
		if (e->detail==NotifyNonlinear ||
	        e->detail==NotifyNonlinearVirtual)
		{
			pointerhere=True;
			refocus(e->time);
		}
}

/*
 * Give up focus if the pointer leaves our screen.
 */
void leavenotify(XCrossingEvent *e)
{
		if (e->detail==NotifyNonlinear ||
	        e->detail==NotifyNonlinearVirtual)
		{
			pointerhere=False;
			XSetInputFocus(dpy,PointerRoot,RevertToPointerRoot,e->time);
		}
}
void configurerequest(XConfigureRequestEvent *e)
{
		// First try to redirect the event.
	if (redirect((XEvent *)e,e->window)==0)
		return;

	// Nobody listens to this window so we'll just
	// do whatever it wants us to do.

	// Ignore stacking requests for now.
	e->value_mask &= ~(CWSibling | CWStackMode);

	XWindowChanges	wc;

	wc.x=e->x;
	wc.y=e->y;
	wc.width=e->width;
	wc.height=e->height;
	wc.border_width=e->border_width;
	wc.sibling=e->above;
	wc.stack_mode=e->detail;
	XConfigureWindow(dpy,e->window,e->value_mask,&wc);
}

void keypress(XKeyEvent *e)
{
	for (unsigned int i=0; i<NELEM(keymap); i++)
		if (keymap[i].keycode==e->keycode)
			{
				keymap[i].function(keymap[i].keysym,e->state,e->time,keymap[i].arg);
				break;
			}
}

void clientmessage(XClientMessageEvent *e)
{
		ewmh_rootclientmessage(e);
}

bool event(void *self,XEvent *e,int type)
{
		switch (e->type)
		{
		case ButtonRelease:
			scrollcnt++;
			if(scrollcnt>scrollcntmax)
				{
					scrollcnt=0;
					int newdesk=curdesk;
					newdesk=curdesk;
					if(e->xbutton.button==Button4)
						{
							newdesk--;
							if(newdesk<0)
								newdesk=numberOfDesktops-1;
							gotodesk(newdesk);
						}

					if(e->xbutton.button==Button5)
						{
							newdesk++;
							if(newdesk>(int)numberOfDesktops-1)
								newdesk=0;
							gotodesk(newdesk);
						}
				}
			break;

		case ButtonPress:
			break;
		case MapRequest:
			maprequest(&e->xmaprequest);
			break;
		case ConfigureRequest:
			configurerequest(&e->xconfigurerequest);
			break;
		case KeyPress:
			keypress(&e->xkey);
			break;
		case ClientMessage:
			clientmessage(&e->xclient);
			break;
		case UnmapNotify:
			unmapnotify(&e->xunmap);
			break;
		case EnterNotify:
			enternotify(&e->xcrossing);
			break;
		case LeaveNotify:
			leavenotify(&e->xcrossing);
			break;
		}

	if(doswapdesk!=-1)
		{
			gotodesk(doswapdesk);
			doswapdesk=-1;
		}
	return(true);
}

listener listener=
{
	.function=event,
	.pointer=NULL,
};

void initroot(void)
{
	listener.function=event;
	listener.pointer=NULL;

	setlistener(root,&listener);

	XSync(dpy,False);
	xerror=NULL;
	XSelectInput(dpy,root,ExposureMask|ButtonPress|ButtonReleaseMask|EnterWindowMask |
	             LeaveWindowMask |
	             SubstructureRedirectMask |
	             SubstructureNotifyMask|ButtonPressMask);
	XSync(dpy,False);
	if (xerror != NULL)
		{
			errorf("display \"%s\" already has a window manager",XDisplayName(displayname));
			exit(1);
		}

	for (unsigned int i=0; i<NELEM(keymap); i++)
		{
			struct keybind *k=&keymap[i];
			k->keycode=XKeysymToKeycode(dpy,k->keysym);
			grabkey(k->keycode,k->modifiers,root,True,GrabModeAsync,GrabModeAsync);
		}

	Window r,c;
	int rx,ry,x,y;
	unsigned m;
	XQueryPointer(dpy,root,&r,&c,&rx,&ry,&x,&y,&m);
	pointerhere=(r==root);
}
