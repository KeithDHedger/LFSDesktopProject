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
#include <stdlib.h>
#include <X11/Xlib.h>

#include "wind.h"
#include "lib.h"
#include "frame.h"
#include "dragger.h"
#include "client.h"

void buttonpress(struct dragger *d,XButtonEvent *e)
{
	d->counter=0;
	d->x=e->x-d->x0;
	d->y=e->y-d->y0;
	if (d->dragnotify != NULL)
		d->dragnotify(d->arg,e->x_root-d->x,e->y_root-d->y,d->counter++,e->time);
}

void motionnotify(struct dragger *d,XMotionEvent *e)
{
	if (d->dragnotify != NULL)
		d->dragnotify(d->arg,e->x_root-d->x,e->y_root-d->y,d->counter++,e->time);
}

bool draggerevent(void *self,XEvent *e,int type)
{
		switch (e->type)
		{
		case ButtonRelease:
			if((windowToUpdate!=None) )
				{
					XResizeWindow(dpy,windowToUpdate,newwid,newhite);
					windowToUpdate=None;
				}
			fromDragger=false;
			csendconf(((frame*)((dragger*)self)->arg)->client);
			break;

		case MotionNotify:
			motionnotify((dragger*)self,&e->xmotion);
			break;
		case ButtonPress:
			fromDragger=true;
			buttonpress((dragger*)self,&e->xbutton);
			break;
		}
	return(true);
}

struct dragger *dcreate(Window parent,int x,int y,int width,int height,int gravity,Cursor cursor,void (*dragnotify)(void *,int,int,unsigned long,Time),void *arg)
{
		XSetWindowAttributes sa;

	sa.win_gravity=gravity;
	sa.cursor=cursor;

	struct dragger *d=(dragger*)xmalloc(sizeof *d);
	d->window=XCreateWindow(dpy,parent,x,y,width,height,0,CopyFromParent,InputOnly,CopyFromParent,CWWinGravity | CWCursor,&sa);
//	d->window=XCreateWindow(dpy,parent,x,y,width,height,0,CopyFromParent,InputOutput,CopyFromParent,CWWinGravity | CWCursor,&sa);
	d->listener.function=draggerevent;
	d->listener.pointer=d;
	setlistener(d->window,&d->listener);
	d->counter=0;

	switch (gravity)
		{
		case NorthWestGravity:
		case WestGravity:
		case SouthWestGravity:
			d->x0=0;
			break;
		case NorthGravity:
		case CenterGravity:
		case SouthGravity:
			d->x0=width / 2;
			break;
		case NorthEastGravity:
		case EastGravity:
		case SouthEastGravity:
			d->x0=width-1;
			break;
		default:
			d->x0=0;
			break;
		}

	switch (gravity)
		{
		case NorthWestGravity:
		case NorthGravity:
		case NorthEastGravity:
			d->y0=0;
			break;
		case WestGravity:
		case CenterGravity:
		case EastGravity:
			d->y0=height / 2;
			break;
		case SouthWestGravity:
		case SouthGravity:
		case SouthEastGravity:
			d->y0=height-1;
			break;
		default:
			d->y0=0;
			break;
		}

	d->x=0;
	d->y=0;
	d->width=width;
	d->height=height;
	d->dragnotify=dragnotify;
	d->arg=arg;
	d->wadjust=((frame*)arg)->width-width;
	d->hadjust=((frame*)arg)->height-height;
	XGrabButton(dpy,Button1,AnyModifier,d->window,False,Button1MotionMask|ButtonReleaseMask,GrabModeAsync,GrabModeAsync,None,cursor);
	XMapWindow(dpy,d->window);

	return d;
}

void ddestroy(struct dragger *d)
{
		setlistener(d->window,NULL);
	XDestroyWindow(dpy,d->window);
	free(d);
}
