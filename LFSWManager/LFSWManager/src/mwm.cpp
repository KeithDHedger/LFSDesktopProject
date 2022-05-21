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

#include <X11/Xatom.h>
#include <X11/Xlib.h>
#include <X11/Xproto.h>

#include "wind.h"
#include "client.h"
#include "lib.h"
#include "mwm.h"
#include "atoms.h"

/*
 * We ignore all hints except the title decor hint. If the window
 * doesn't want a title,we remove it's frame completely.
 */
void reloadmwmhints(struct client *c)
{
		Bool undecorated=False;

	mwmhints *h=getmwmhints(c);
	if (h != NULL)
		{
			if (h->flags & MWM_HINTS_DECORATIONS)
				{
					/*
					 * If MWM_DECOR_ALL is set,it means use all
					 * decorations EXCEPT the ones specified...
					 */
					if ((h->decorations & MWM_DECOR_ALL) != 0)
						h->decorations=~h->decorations;

					if ((h->decorations & MWM_DECOR_TITLE)==0)
						undecorated=True;
				}
			XFree(h);
		}

	csetundecorated(c,undecorated);
}

mwmhints *getmwmhints(struct client *c)
{
		unsigned long n=0;
	mwmhints *h=(mwmhints*)getprop(c->window,MOTIF_WM_HINTS,MOTIF_WM_HINTS,32,&n);
	if (h != NULL && n != 5)
		{
			XFree(h);
			h=NULL;
		}
	return h;
}

void mwm_startwm(void)
{
		MOTIF_WM_HINTS=XInternAtom(dpy,"_MOTIF_WM_HINTS",False);
}

void mwm_manage(struct client *c)
{
		reloadmwmhints(c);
}

void mwm_propertynotify(struct client *c,XPropertyEvent *e)
{
		if (e->atom==MOTIF_WM_HINTS)
		reloadmwmhints(c);
}


