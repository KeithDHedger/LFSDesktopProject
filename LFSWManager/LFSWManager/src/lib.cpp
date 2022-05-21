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

#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/stat.h>

#include <X11/Xlib.h>

#include "wind.h"
#include "atoms.h"

// Lock-mask permutations that should be grabbed in addition
// to the modifiers specified to the functions below.
const unsigned lockmasks[] =
{
	0,LockMask,	// Caps Lock
	Mod2Mask,	// Num Lock
	Mod5Mask,	// Scroll Lock
	LockMask | Mod2Mask,LockMask | Mod5Mask,Mod2Mask | Mod5Mask,LockMask | Mod2Mask | Mod5Mask
};

void *xmalloc(size_t size)
{
		void *p;
	while ((p=malloc(size))==NULL && size != 0)
		{
			errorf("out of memory");
			sleep(1);
		}
	return p;
}

void *xrealloc(const void *p,size_t size)
{
		void *q;
	while ((q=realloc((void *)p,size))==NULL && size != 0)
		{
			errorf("out of memory");
			sleep(1);
		}
	return q;
}

char *xstrdup(const char *s)
{
	//MAYBEBAD//
	return(strdup(s));
//	size_t n=strlen(s)+1;
//	return ((char*)memcpy(xmalloc(n),s,n));
}

/*
 * Grabs a key. See manual page of XGrabKey.
 */
void grabkey(int keycode,unsigned modifiers,Window grabwin,Bool ownerevents,int ptrmode,int keymode)
{
		if (modifiers==AnyModifier)
		XGrabKey(dpy,keycode,modifiers,grabwin,ownerevents,ptrmode,keymode);
	else
		for (unsigned int i=0; i<NELEM(lockmasks); i++)
			XGrabKey(dpy,keycode,modifiers | lockmasks[i],grabwin,ownerevents,ptrmode,keymode);
}

/*
 * Ungrabs a key. See manual page of XUngrabKey.
 */
void ungrabkey(int keycode,unsigned modifiers,Window grabwin)
{
		if (modifiers==AnyModifier)
		XUngrabKey(dpy,keycode,AnyModifier,grabwin);
	else
		for (unsigned int i=0; i<NELEM(lockmasks); i++)
			XUngrabKey(dpy,keycode,modifiers | lockmasks[i],grabwin);
}

/*
 * Ungrabs a pointer button. See manual page of XGrabButton.
 */
void grabbutton(unsigned button,unsigned modifiers,Window grabwin,Bool ownerevents,unsigned eventmask,int ptrmode,int keymode,Window confineto,Cursor cursor)
{
		if (modifiers==AnyModifier)
		XGrabButton(dpy,button,AnyModifier,grabwin,ownerevents,eventmask,ptrmode,keymode,confineto,cursor);
	else
		for (unsigned int i=0; i<NELEM(lockmasks); i++)
			XGrabButton(dpy,button,modifiers | lockmasks[i],grabwin,ownerevents,eventmask,ptrmode,keymode,confineto,cursor);
}

/*
 * Ungrabs a pointer button. See manual page of XUngrabButton.
 */
void ungrabbutton(unsigned button,unsigned modifiers,Window grabwin)
{
		if (modifiers==AnyModifier)
		XUngrabButton(dpy,button,modifiers,grabwin);
	else
		for (unsigned int i=0; i<lockmasks[i]; i++)
			XUngrabButton(dpy,button,modifiers | lockmasks[i],grabwin);
}

/*
 * Returns the WM_STATE hint of a client window
 */
long getwmstate(Window w)
{
		unsigned long nitems,bytesafter;
	unsigned char *prop;
	Atom actualtype;
	int actualformat;
	long state=WithdrawnState;
	if (XGetWindowProperty(dpy,w,WM_STATE,0L,2L,False,WM_STATE,
	                       &actualtype,&actualformat,&nitems,
	                       &bytesafter,&prop)==Success)
		{
			if (nitems>0)
				state=((long *)prop)[0];
			XFree(prop);
		}
	return state;
}

/*
 * Sets the WM_STATE hint of a client window. The state can
 * be one of Normalstate,IconicState,or WithdrawnState.
 */
void setwmstate(Window w,long state)
{
		long data[2]= { state,None };
	XChangeProperty(dpy,w,WM_STATE,WM_STATE,32,PropModeReplace,(unsigned char *)data,2);
}

/*
 * Tests if a client window is mapped.
 */
Bool ismapped(Window w)
{
		XWindowAttributes a;
	return XGetWindowAttributes(dpy,w,&a) && a.map_state != IsUnmapped;
}

char *decodetextproperty(XTextProperty *p)
{
	char *s=NULL;
	char **v=NULL;
	int n=0;
	XmbTextPropertyToTextList(dpy,p,&v,&n);
	if (n>0)
		s=xstrdup(v[0]);
	if (v != NULL)
		XFreeStringList(v);
	return s;
}

void setprop(Window w,Atom prop,Atom type,int fmt,void *ptr,int nelem)
{
		XChangeProperty(dpy,w,prop,type,fmt,PropModeReplace,(const unsigned char*)ptr,nelem);
}

void *getprop(Window w,Atom prop,Atom type,int fmt,unsigned long *rcountp)
{
	void *ptr=NULL;
	unsigned long count=32;
	Atom rtype;
	int rfmt;
	unsigned long rafter;
	for (;;)
		{
			if (XGetWindowProperty(dpy,w,prop,0L,count,False,type,&rtype,&rfmt,rcountp,
			                       &rafter,(unsigned char **)&ptr) != Success)
				{
					// Error
					return NULL;
				}
			else if (rtype != type || rfmt != fmt)
				{
					// Does not exist (type=None),or wrong type/format
					return NULL;
				}
			else if (rafter>0)
				{
					XFree(ptr);
					ptr=NULL;
					count *= 2;
				}
			else
				{
					return ptr;
				}
		}
}

void drawbitmap(Drawable d,GC gc,struct bitmap *b,int x,int y)
{
		if (b->pixmap==None)
		b->pixmap=XCreateBitmapFromData(dpy,d,(char *)b->bits,b->width,b->height);
	XCopyPlane(dpy,b->pixmap,d,gc,0,0,b->width,b->height,x,y,1);
}

unsigned long getpixel(const char *name)
{
		XColor tc,sc;
	XAllocNamedColor(dpy,DefaultColormap(dpy,screen),name,&sc,&tc);
	return sc.pixel;
}

int fileExists(char *name)
{
	struct stat buffer;
	return (stat(name,&buffer));
}

