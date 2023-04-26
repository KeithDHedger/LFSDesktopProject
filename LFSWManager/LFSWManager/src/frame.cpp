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
#include <X11/Xlib.h>
#include <X11/cursorfont.h>
#include <X11/extensions/shape.h>

#include "wind.h"
#include "client.h"
#include "button.h"
#include "lib.h"
#include "ewmh.h"
#include "dragger.h"
#include "frame.h"
#include "atoms.h"
#include "x11font.h"

int		depth=0;
size_t	fcount;
Cursor	cursortopleft=None;
Cursor	cursortopright=None;
Cursor	cursorRight=None;
Cursor	cursorBottomRight=None;
Cursor	cursorBottom=None;
Cursor	cursorBottomLeft=None;
Cursor	cursorLeft=None;

bool	swapdesk=false;
int		nx;
Window	windowToUpdate=None;
int		newwid,newhite;
int		updatecnt=0;
int		maxupdatecnt=10;
bool	fromDragger=false;
int		lastbuttonx;

/*
 * Move and resize the frame,and update the client window.
 */

void moveresize(struct frame *f,int x,int y,int w,int h)
{
	int		offset;
	bool	left;
	int		newd=-1;
	Desk	d;

	Window			root_return;
	Window			child_return;
	int				root_x_return;
	int				root_y_return;
	int				win_x_return;
	int				win_y_return;
	unsigned int	mask_return;
	int				flipborder=10;
	int				flippedoffset=flipborder;

	if (x==f->x && y==f->y && w==f->width && h==f->height)
		return;

	doswapdesk=-1;

	XQueryPointer(dpy,DefaultRootWindow(dpy),&root_return,&child_return,&root_x_return,&root_y_return,&win_x_return,&win_y_return, &mask_return);
	flippedoffset=win_x_return-x;

	struct geometry old=cgetgeom(f->client);
	geometry mynew;
	mynew.x=x+frameLeft;
	mynew.y=y+frameTop;
	mynew.width=w-frameLeft-frameRight;
	mynew.height=h-frameTop-frameBottom;
	mynew.borderwidth=old.borderwidth;

	if(mynew.height<0)
		mynew.height=0;

	if((win_x_return>flipborder) && (win_x_return<displayWidth-flipborder))
		swapdesk=false;
	if(swapdesk==false)
		nx=x;

	if(((win_x_return<flipborder) || (win_x_return>(displayWidth-flipborder))) && (swapdesk==false))
		{

			if(win_x_return>(displayWidth-flipborder))
				{
					left=false;
					offset=flipborder-flippedoffset;
				}
			else
				{
					left=true;
					offset=displayWidth-flippedoffset;
				}
			mynew.x=offset;
			swapdesk=true;
			nx=offset;
			d=f->client->desk;
			if(left==true)
				{
					newd=(int)d-1;
					if(newd<0)
						newd=numberOfDesktops-1;
				}
			else
				{
					newd=d+1;
					if(newd>(int)numberOfDesktops-1)
						newd=0;
				}
			doswapdesk=newd;
			f->client->desk=newd;
		}

	csetgeom(f->client,mynew);

	if(f->isShaded==false)
		{
			XMoveResizeWindow(dpy,f->window,nx,y,w,h);
			windowToUpdate=f->client->window;
			newwid=mynew.width;
			newhite=mynew.height;
			f->x=nx;
			f->y=y;
			f->width=w;
			f->height=h;
		}
	else
		{
			XMoveResizeWindow(dpy,f->window,nx,y,w,frameTop);
			windowToUpdate=f->client->window;
			newwid=mynew.width;
			newhite=mynew.height;
			f->x=nx;
			f->y=y;
			f->width=w;
			f->height=frameTop;
		}

	if (mynew.width==old.width && mynew.height==old.height)
		csendconf(f->client);
	else
		{
			if((liveUpdate>0 && updatecnt>liveUpdate) || (fromDragger==false))
				{
					XResizeWindow(dpy,f->client->window,mynew.width,mynew.height);
					updatecnt=0;
				}
			else
				updatecnt++;
		}

	if(newd!=-1)
		{
			if(win_x_return>(displayWidth-flipborder))
				XWarpPointer(dpy,None,None,0,0,0,0,0-displayWidth+flipborder,0);
			else
				XWarpPointer(dpy,None,None,0,0,0,0,displayWidth-flipborder,0);
		}
}

void mydelete(void *myclient,Time t)
{
		((client*)myclient)->isundecorated=true;
	cdelete((client*)myclient,t);
}

void minimizeWindow(void *myclient,Time t)
{
		XIconifyWindow(dpy,((client*)myclient)->window,screen);
}

void shadeWindow(void *myclient,Time t)
{
		if(((client*)myclient)->frame->isShaded==false)
		{
			((client*)myclient)->frame->oldHeight=((client*)myclient)->frame->height;
			((client*)myclient)->frame->isShaded=true;
			moveresize(((client*)myclient)->frame,((client*)myclient)->frame->x,((client*)myclient)->frame->y,((client*)myclient)->frame->width,0);
		}
	else
		{
			((client*)myclient)->frame->isShaded=false;
			moveresize(((client*)myclient)->frame,((client*)myclient)->frame->x,((client*)myclient)->frame->y,((client*)myclient)->frame->width,((client*)myclient)->frame->oldHeight);
			repaint(((client*)myclient)->frame);
		}
}

int getFrameMonitor(frame *f)
{
		for(unsigned int j=0; j<numberOfMonitors; j++)
		{
			if((f->x>monitorData[j].monX) && (f->x<monitorData[j].monW+monitorData[j].monX) && (f->y>monitorData[j].monY) && (f->y<monitorData[j].monH+monitorData[j].monY))
				return(j);
		}
	return(0);

}

void maximizeWindow(void *myclient,Time t)
{
		int		monnum=getFrameMonitor(((client*)myclient)->frame);

	if(((client*)myclient)->frame->isMaximized==false)
		{
			((client*)myclient)->frame->oldX=((client*)myclient)->frame->x;
			((client*)myclient)->frame->oldY=((client*)myclient)->frame->y;
			((client*)myclient)->frame->oldWidth=((client*)myclient)->frame->width;
			((client*)myclient)->frame->oldHeight=((client*)myclient)->frame->height;
			((client*)myclient)->frame->isMaximized=true;
			moveresize(((client*)myclient)->frame,monitorData[monnum].monX,monitorData[monnum].monY,monitorData[monnum].monW,monitorData[monnum].monH);
			changestate(((client*)myclient)->window,NET_WM_STATE_ADD,NET_WM_STATE_MAXIMIZED_VERT);
			changestate(((client*)myclient)->window,NET_WM_STATE_ADD,NET_WM_STATE_MAXIMIZED_HORZ);
		}
	else
		{
			moveresize(((client*)myclient)->frame,((client*)myclient)->frame->oldX,((client*)myclient)->frame->oldY,((client*)myclient)->frame->oldWidth,((client*)myclient)->frame->oldHeight);
			((client*)myclient)->frame->x=((client*)myclient)->frame->oldX;
			((client*)myclient)->frame->y=((client*)myclient)->frame->oldY;
			((client*)myclient)->frame->width=((client*)myclient)->frame->oldWidth;
			((client*)myclient)->frame->height=((client*)myclient)->frame->oldHeight;
			((client*)myclient)->frame->isMaximized=false;
			changestate(((client*)myclient)->window,NET_WM_STATE_REMOVE,NET_WM_STATE_MAXIMIZED_VERT);
			changestate(((client*)myclient)->window,NET_WM_STATE_REMOVE,NET_WM_STATE_MAXIMIZED_HORZ);
		}
}

/*
 * XXX: We cheat here and always estimate normal frame
 * extents,even if the window is of a type that will
 * not get a frame. This is hopefully okay since most
 * clients requesting estimates of frame extents will
 * probably be interested in having a frame.
 */
struct extents estimateframeextents(Window w)
{
		return (struct extents)
	{
		.top=frameTop,
		 .bottom=frameBottom,
		  .left=frameLeft,
		   .right=frameRight
	};
}

void reorder(Window ref,Window below)
{
		Window	w[2]= {ref,below};
	XRestackWindows(dpy,w,2);
}

void setgrav(Window win,int grav)
{
		XSetWindowAttributes	wa;
	wa.win_gravity=grav;

	XChangeWindowAttributes(dpy,win,CWWinGravity,&wa);
}

void gravitate(int wingrav,int borderwidth,int *dx,int *dy)
{
		switch (wingrav)
		{
		case NorthWestGravity:
			*dx=0;
			*dy=0;
			break;
		case NorthGravity:
			*dx=borderwidth-(frameLeft+frameRight) / 2;
			*dy=0;
			break;
		case NorthEastGravity:
			*dx=(2 * borderwidth)-(frameLeft+frameRight);
			*dy=0;
			break;
		case WestGravity:
			*dx=0;
			*dy=borderwidth-(frameTop+frameBottom) / 2;
			break;
		case CenterGravity:
			*dx=borderwidth-(frameLeft+frameRight) / 2;
			*dy=borderwidth-(frameTop+frameBottom) / 2;
			break;
		case EastGravity:
			*dx=(2 * borderwidth)-(frameLeft+frameRight);
			*dy=borderwidth-(frameTop+frameBottom) / 2;
			break;

		case SouthWestGravity:
			*dx=0;
			*dy=(2 * borderwidth)-(frameTop+frameBottom);
			break;
		case SouthGravity:
			*dx=borderwidth-(frameLeft+frameRight) / 2;
			*dy=(2 * borderwidth)-(frameTop+frameBottom);
			break;
		case SouthEastGravity:
			*dx=(2 * borderwidth)-(frameLeft+frameRight);
			*dy=(2 * borderwidth)-(frameTop+frameBottom);
			break;

		case StaticGravity:
			*dx=borderwidth-frameLeft;
			*dy=borderwidth-frameTop;;
			break;

		default:
			errorf("unknown window gravity %d",wingrav);
			*dx=0;
			*dy=0;
			break;
		}
}

void repaint(struct frame *f)
{
	int					namewidth=f->namewidth;
	int					partoffset;
	GC					gc;
	int					ends;
	int					titlewidth;
	int					framecenter;
	int					titlespacecentre;
	int					maxtitlewidthllowed;
	int					title1width;
	int					title5width;
	int					titlehalfwidth;
	int					title1x;
	int					title2x;
	int					title3x;
	int					title4x;
	int					title5x;
	char				*wintitle;
	struct fontcolor	*usecolour;
//TODO//
//clean this
	int					titleblock;
	int					titleblockstart;
	int					titleblockcentre;
	int					txtwidth;
	char				*txt;

	if(f==NULL)
		return;
	if (f->client==NULL)
		return;
	if(f->client->ismapped==false)
		return;
	if(f->client->isfull==true)
		return;
	if(f->client->isundecorated==true)
		return;

	namewidth=MIN(namewidth,f->width-2 * (1+font->size));
	namewidth=MAX(namewidth,0);

	if(chasfocus(f->client)==true)
		partoffset=0;
	else
		partoffset=1;

	if(theme.useTheme==true)
		{

			if(f->mask!=None)
				XFreePixmap(dpy,f->mask);

			gc=XCreateGC(dpy,f->window,0,NULL);
			XSetClipMask(dpy,gc,None);
			XSetFillStyle(dpy,gc,FillTiled);

			f->mask=XCreatePixmap(dpy,f->window,f->width,f->height,1);
			XSetForeground(dpy,f->maskGC,blackColor);
			XSetClipMask(dpy,f->maskGC,None);
			XSetFillStyle(dpy,f->maskGC,FillSolid);
			XFillRectangle(dpy,f->mask,f->maskGC,0,0,f->width,f->height);
			XSetFillStyle(dpy,f->maskGC,FillTiled);

			framecenter=(f->width)/2;
			ends=theme.partsWidth[TOPLEFTACTIVE+partoffset]+theme.partsWidth[TOPRIGHTACTIVE+partoffset];
			titlewidth=f->namewidth;

			if(theme.gotPart[TITLE2ACTIVE]==true && theme.gotPart[TITLE4ACTIVE]==true)
				maxtitlewidthllowed=f->width-(ends+theme.partsWidth[TITLE2ACTIVE]+theme.partsWidth[TITLE4ACTIVE]+f->buttonBarWith+theme.buttonOffset);
			else
				maxtitlewidthllowed=f->width-ends;

			if(titlewidth>maxtitlewidthllowed)
				titlewidth=maxtitlewidthllowed;

			f->maxNameWidth=f->namewidth;

			if(theme.gotPart[TITLE2ACTIVE]==true && theme.gotPart[TITLE4ACTIVE]==true)
				{
					titlewidth=titlewidth-(ends+theme.partsWidth[TITLE2ACTIVE]+theme.partsWidth[TITLE4ACTIVE]+f->buttonBarWith+theme.buttonOffset);
					if(titlewidth<f->namewidth)
						titlewidth=f->namewidth;

					if(titlewidth>=f->width-(f->buttonBarWith+theme.partsWidth[TITLE2ACTIVE]+theme.partsWidth[TITLE4ACTIVE]+ends+theme.buttonOffset+64))
						{
							titlewidth=f->width-(f->buttonBarWith+theme.partsWidth[TITLE2ACTIVE]+theme.partsWidth[TITLE4ACTIVE]);
							titlewidth=200;
							titlewidth=f->width-(f->buttonBarWith+theme.partsWidth[TITLE2ACTIVE]+theme.partsWidth[TITLE4ACTIVE]+ends+theme.buttonOffset+64);
							f->maxNameWidth=titlewidth;
						}

					titlehalfwidth=titlewidth/2;
					title1width=f->width-framecenter-titlehalfwidth-theme.partsWidth[TOPLEFTACTIVE+partoffset]-theme.partsWidth[TITLE2ACTIVE+partoffset];
					title5width=f->width-framecenter-titlehalfwidth-theme.partsWidth[TOPRIGHTACTIVE+partoffset]-theme.partsWidth[TITLE4ACTIVE+partoffset];
					title2x=framecenter-titlehalfwidth-theme.partsWidth[TITLE2ACTIVE+partoffset];
					title3x=framecenter-titlehalfwidth;
					title4x=framecenter+titlehalfwidth;
					title5x=f->width-(theme.partsWidth[TOPRIGHTACTIVE+partoffset]+title5width);
				}
			else
				{
					titlewidth=titlewidth-(f->buttonBarWith);

					if(titlewidth<f->namewidth)
						titlewidth=f->namewidth;

					if(titlewidth>=f->width-(f->buttonBarWith+ends+theme.buttonOffset+64))
						{
							titlewidth=f->width-(f->buttonBarWith+ends+theme.buttonOffset+64);
							f->maxNameWidth=titlewidth;
						}

					titlehalfwidth=titlewidth/2;
					title1width=f->width-framecenter-titlehalfwidth-theme.partsWidth[TOPLEFTACTIVE+partoffset]-theme.partsWidth[TITLE2ACTIVE+partoffset];
					title5width=f->width-framecenter-titlehalfwidth-theme.partsWidth[TOPRIGHTACTIVE+partoffset]-theme.partsWidth[TITLE4ACTIVE+partoffset];
					title3x=framecenter-titlehalfwidth;
					title5x=f->width-(theme.partsWidth[TOPRIGHTACTIVE+partoffset]+title5width);
				}

			title1x=theme.partsWidth[TOPLEFTACTIVE+partoffset];

			/*
				L|	1	|	|	2	|	|	3	|	|	4	|	|	5	|R
				<--------------------------------------------------------> 	= f->width
				<>															=left top width
				 <------>													= title1 width
							<------->										= left title endcap width
										<------->							= theme.titleWidth
													<------->				=rite title endcap width
																<------->	=title5 width
																		 <>	=rite top width
			*/


//top corners
//left top

			XSetClipMask(dpy,*f->background,theme.masks[TOPLEFTACTIVE+partoffset]);
			XSetClipOrigin(dpy,*f->background,0,0);
			XCopyArea(dpy,theme.pixmaps[TOPLEFTACTIVE+partoffset],f->window,*f->background,0,0,theme.partsWidth[TOPLEFTACTIVE+partoffset],theme.partsHeight[TOPLEFTACTIVE+partoffset],0,0);
			XCopyArea(dpy,theme.masks[TOPLEFTACTIVE+partoffset],f->mask,f->maskGC,0,0,theme.partsWidth[TOPLEFTACTIVE+partoffset],theme.partsHeight[TOPLEFTACTIVE+partoffset],0,0);

//rite top
			XSetClipMask(dpy,*f->background,theme.masks[TOPRIGHTACTIVE+partoffset]);
			XSetClipMask(dpy,*f->background,None);
			XCopyArea(dpy,theme.pixmaps[TOPRIGHTACTIVE+partoffset],f->window,*f->background,0,0,theme.partsWidth[TOPRIGHTACTIVE+partoffset],theme.partsHeight[TOPRIGHTACTIVE+partoffset],f->width-theme.partsWidth[TOPRIGHTACTIVE+partoffset],0);
			XCopyArea(dpy,theme.masks[TOPRIGHTACTIVE+partoffset],f->mask,f->maskGC,0,0,theme.partsWidth[TOPRIGHTACTIVE+partoffset],theme.partsHeight[TOPRIGHTACTIVE+partoffset],f->width-theme.partsWidth[TOPRIGHTACTIVE+partoffset],0);

			XSetTSOrigin(dpy,f->maskGC,0,0);

//title1
			if(theme.gotPart[TITLE1ACTIVE+partoffset]==true)
				{
					XSetTile(dpy,gc,theme.pixmaps[TITLE1ACTIVE+partoffset]);
					XFillRectangle(dpy,f->window,gc,title1x,0,title1width,theme.partsHeight[TITLE1ACTIVE+partoffset]);

					XSetTile(dpy,f->maskGC,theme.masks[TITLE1ACTIVE+partoffset]);
					XFillRectangle(dpy,f->mask,f->maskGC,title1x,0,title1width,theme.partsHeight[TITLE1ACTIVE+partoffset]);
				}
//title2 end cap
			if(theme.gotPart[TITLE2ACTIVE+partoffset]==true)
				{
					XSetClipMask(dpy,*f->background,None);
					XCopyArea(dpy,theme.pixmaps[TITLE2ACTIVE+partoffset],f->window,*f->background,0,0,theme.partsWidth[TITLE2ACTIVE+partoffset],theme.partsHeight[TITLE2ACTIVE+partoffset],title2x,0);
					XCopyArea(dpy,theme.masks[TITLE2ACTIVE+partoffset],f->mask,f->maskGC,0,0,theme.partsWidth[TITLE2ACTIVE+partoffset],theme.partsHeight[TITLE2ACTIVE+partoffset],title2x,0);
				}

//title5 button back
			if(theme.gotPart[TITLE5ACTIVE+partoffset]==true)
				{
					XSetTile(dpy,gc,theme.pixmaps[TITLE5ACTIVE+partoffset]);
					XFillRectangle(dpy,f->window,gc,title5x,0,title5width,theme.partsHeight[TITLE5ACTIVE+partoffset]);

					XSetTile(dpy,f->maskGC,theme.masks[TITLE5ACTIVE+partoffset]);
					XFillRectangle(dpy,f->mask,f->maskGC,title5x,0,title5width,theme.partsHeight[TITLE5ACTIVE+partoffset]);
				}

//title4 rite endcap
			if(theme.gotPart[TITLE4ACTIVE+partoffset]==true)
				{
					XSetClipMask(dpy,*f->background,None);
					XCopyArea(dpy,theme.pixmaps[TITLE4ACTIVE+partoffset],f->window,*f->background,0,0,theme.partsWidth[TITLE4ACTIVE+partoffset],theme.partsHeight[TITLE4ACTIVE+partoffset],title4x,0);
					XCopyArea(dpy,theme.masks[TITLE4ACTIVE+partoffset],f->mask,f->maskGC,0,0,theme.partsWidth[TITLE4ACTIVE+partoffset],theme.partsHeight[TITLE4ACTIVE+partoffset],title4x,0);
				}

//title3 main centre bit
			if(theme.gotPart[TITLE3ACTIVE+partoffset]==true)
				{
					XSetTile(dpy,gc,theme.pixmaps[TITLE3ACTIVE+partoffset]);
					XFillRectangle(dpy,f->window,gc,title3x,0,titlewidth,theme.partsHeight[TITLE3ACTIVE+partoffset]);

					titleblock=f->width-theme.partsWidth[TITLE2ACTIVE+partoffset]-theme.partsWidth[TOPLEFTACTIVE+partoffset]-theme.partsWidth[TITLE4ACTIVE+partoffset]-lastbuttonx;

					txt=getMaxString(font,f->client,titleblock);
					if(txt!=NULL)
						{
							txtwidth=fttextwidth_utf8(font,txt);
							titleblockcentre=(titleblock/2)+theme.partsWidth[TOPLEFTACTIVE+partoffset]+theme.partsWidth[TITLE2ACTIVE+partoffset];

							if(partoffset==0)
								usecolour=fhighlight;
							else
								usecolour=fnormal;

							if (f->client->netwmname != NULL)
								ftdrawstring_utf8(f->window,font,usecolour,titleblockcentre-(txtwidth/2),(theme.titleBarHeight/2)+((font->ascent-2)/2)+theme.titleOffset,txt);
							else if (f->client->wmname != NULL)
								ftdrawstring(f->window,font,usecolour,titleblockcentre-(txtwidth/2),(theme.titleBarHeight/2)+((font->ascent-2)/2)+theme.titleOffset,txt);
							free(txt);
						}

					XSetTile(dpy,f->maskGC,theme.masks[TITLE3ACTIVE]);
					XFillRectangle(dpy,f->mask,f->maskGC,title3x,0,titlewidth,theme.partsHeight[TITLE3ACTIVE+partoffset]);
				}

			if(f->isShaded==false)
				{
//left
					if(theme.gotPart[LEFTACTIVE+partoffset]==true)
						{
							XSetTile(dpy,gc,theme.pixmaps[LEFTACTIVE+partoffset]);
							XFillRectangle(dpy,f->window,gc,0,theme.partsHeight[TOPLEFTACTIVE+partoffset],theme.partsWidth[LEFTACTIVE+partoffset],f->height-theme.partsHeight[BOTTOMLEFTACTIVE+partoffset]-theme.partsHeight[TOPLEFTACTIVE+partoffset]);

							XSetTile(dpy,f->maskGC,theme.masks[LEFTACTIVE+partoffset]);
							XSetTSOrigin(dpy,f->maskGC,0,0);
							XFillRectangle(dpy,f->mask,f->maskGC,0,theme.partsHeight[TOPLEFTACTIVE+partoffset],theme.partsWidth[LEFTACTIVE+partoffset],f->height-theme.partsHeight[BOTTOMLEFTACTIVE+partoffset]-theme.partsHeight[TOPLEFTACTIVE+partoffset]);
						}
//right
					if(theme.gotPart[RIGHTACTIVE+partoffset]==true)
						{
							XSetTSOrigin(dpy,gc,f->width-theme.partsWidth[RIGHTACTIVE+partoffset],0);
							XSetTile(dpy,gc,theme.pixmaps[RIGHTACTIVE+partoffset]);
							XFillRectangle(dpy,f->window,gc,f->width-theme.partsWidth[RIGHTACTIVE+partoffset],theme.partsHeight[TOPRIGHTACTIVE+partoffset],theme.partsWidth[RIGHTACTIVE+partoffset],f->height-theme.partsHeight[BOTTOMRIGHTACTIVE+partoffset]-theme.partsHeight[TOPRIGHTACTIVE+partoffset]);

							XSetTSOrigin(dpy,f->maskGC,f->width-theme.partsWidth[RIGHTACTIVE+partoffset],0);
							XSetTile(dpy,f->maskGC,theme.masks[RIGHTACTIVE+partoffset]);
							XFillRectangle(dpy,f->mask,f->maskGC,f->width-theme.partsWidth[RIGHTACTIVE+partoffset],theme.partsHeight[TOPRIGHTACTIVE+partoffset],theme.partsWidth[RIGHTACTIVE+partoffset],f->height-theme.partsHeight[BOTTOMRIGHTACTIVE+partoffset]-theme.partsHeight[TOPRIGHTACTIVE+partoffset]);
						}

//left bottom
					if(theme.gotPart[BOTTOMLEFTACTIVE+partoffset]==true)
						{
							XSetClipMask(dpy,*f->background,None);
							XCopyArea(dpy,theme.pixmaps[BOTTOMLEFTACTIVE+partoffset],f->window,*f->background,0,0,theme.partsWidth[BOTTOMLEFTACTIVE+partoffset],theme.partsHeight[BOTTOMLEFTACTIVE+partoffset],0,f->height-theme.partsHeight[BOTTOMLEFTACTIVE+partoffset]);
							XCopyArea(dpy,theme.masks[BOTTOMLEFTACTIVE+partoffset],f->mask,f->maskGC,0,0,theme.partsWidth[BOTTOMLEFTACTIVE+partoffset],theme.partsHeight[BOTTOMLEFTACTIVE+partoffset],0,f->height-theme.partsHeight[BOTTOMLEFTACTIVE+partoffset]);
						}
//right bottom
					if(theme.gotPart[BOTTOMRIGHTACTIVE+partoffset]==true)
						{
							XSetClipMask(dpy,*f->background,None);
							XCopyArea(dpy,theme.pixmaps[BOTTOMRIGHTACTIVE+partoffset],f->window,*f->background,0,0,theme.partsWidth[BOTTOMRIGHTACTIVE+partoffset],theme.partsHeight[BOTTOMLEFTACTIVE+partoffset],f->width-theme.partsWidth[BOTTOMRIGHTACTIVE+partoffset],f->height-theme.partsHeight[BOTTOMRIGHTACTIVE+partoffset]);
							XCopyArea(dpy,theme.masks[BOTTOMRIGHTACTIVE+partoffset],f->mask,f->maskGC,0,0,theme.partsWidth[BOTTOMRIGHTACTIVE+partoffset],theme.partsHeight[BOTTOMLEFTACTIVE+partoffset],f->width-theme.partsWidth[BOTTOMRIGHTACTIVE+partoffset],f->height-theme.partsHeight[BOTTOMRIGHTACTIVE+partoffset]);
						}

//bottom
					if(theme.gotPart[BOTTOMACTIVE+partoffset]==true)
						{
							XSetTSOrigin(dpy,gc,0,f->height-theme.partsHeight[BOTTOMACTIVE+partoffset]);
							XSetTile(dpy,gc,theme.pixmaps[BOTTOMACTIVE+partoffset]);
							XFillRectangle(dpy,f->window,gc,theme.partsWidth[BOTTOMLEFTACTIVE],f->height-theme.partsHeight[BOTTOMACTIVE+partoffset],f->width-theme.partsWidth[BOTTOMRIGHTACTIVE]-theme.partsWidth[BOTTOMLEFTACTIVE],theme.partsHeight[BOTTOMACTIVE+partoffset]);

							XSetTile(dpy,f->maskGC,theme.masks[BOTTOMACTIVE+partoffset]);
							XSetTSOrigin(dpy,f->maskGC,0,f->height-theme.partsHeight[BOTTOMACTIVE+partoffset]);
							XFillRectangle(dpy,f->mask,f->maskGC,theme.partsWidth[BOTTOMLEFTACTIVE],f->height-theme.partsHeight[BOTTOMACTIVE+partoffset],f->width-theme.partsWidth[BOTTOMRIGHTACTIVE]-theme.partsWidth[BOTTOMLEFTACTIVE],theme.partsHeight[BOTTOMACTIVE+partoffset]);
						}
				}
			XSetFillStyle(dpy,f->maskGC,FillSolid);
			XSetForeground(dpy,f->maskGC,whiteColor);
			XFillRectangle(dpy,f->mask,f->maskGC,theme.leftWidth,theme.titleBarHeight,f->width-theme.rightWidth-theme.leftWidth,f->height-theme.titleBarHeight-theme.bottomHeight);
			XShapeCombineMask(dpy,f->window,ShapeBounding,0,0,f->mask,ShapeSet);
		}
	else
		{
			// Title area
			int x=1;
			unsigned long  framefill;
			unsigned long  frame;

			if(partoffset==0)
				{
					framefill=activeFrameFill;
					frame=activeFrame;
					usecolour=fhighlight;
				}
			else
				{
					framefill=inactiveFrameFill;
					frame=inactiveFrame;
					usecolour=fnormal;
				}

			XSetForeground(dpy,mainGC,framefill);
			XSetClipMask(dpy,mainGC,None);
			XFillRectangle(dpy,f->window,mainGC,x,1,f->width-2,frameTop);

//title  string
			if (f->client->netwmname != NULL)
				ftdrawstring_utf8(f->window,font,usecolour,4,(frameTop/2)+((font->ascent-2)/2),f->client->netwmname);
			else if (f->client->wmname != NULL)
				ftdrawstring(f->window,font,usecolour,4,(frameTop/2)+((font->ascent-2)/2),f->client->wmname);

			// Border
			XSetForeground(dpy,mainGC,frame);
			XDrawRectangle(dpy,f->window,mainGC,0,0,f->width-1,f->height-1);

			// Title bottom border
			XDrawLine(dpy,f->window,mainGC,frameLeft,frameTop-1,f->width-frameRight-1,frameTop-1);

			// Window area
			XSetForeground(dpy,mainGC,framefill);
			XFillRectangle(dpy,f->window,mainGC,1,frameTop,f->width-2,f->height-1-frameTop);

			XFillRectangle(dpy,f->window,mainGC,1,frameTop-1,frameLeft-1,1);
			XFillRectangle(dpy,f->window,mainGC,f->width-frameRight,frameTop-1,frameRight-1,1);
		}
}

void fupdate(struct frame *f)
{
		int sz;
	int	buttonx;
	int	buttonspace;

	if(f==NULL)
		return;
	if(f->client->isundecorated==true)
		return;
	if(theme.useTheme==true)
		{
			sz=frameTop;
			buttonx=theme.buttonOffset+theme.partsWidth[RIGHTACTIVE];
			buttonspace=theme.buttonXSpacing;
		}
	else
		{
			sz=frameTop;
			buttonx=sz;
			buttonspace=0;
		}

	if(theme.useTheme==true)
		{
			if(cisframed(f->client)==true)
			{
//close button

			if(chaswmproto(f->client,WM_DELETE_WINDOW))
				{
					if (f->deletebutton==NULL)
						{
							f->deletebutton=bcreate(mydelete,f->client,deletebitmap,f->window,f->width-theme.partsWidth[CLOSEACTIVE]- buttonx,(theme.titleBarHeight/2)-(theme.partsHeight[CLOSEACTIVE]/2),theme.partsWidth[CLOSEACTIVE],theme.partsHeight[CLOSEACTIVE],NorthEastGravity,CLOSEACTIVE,f);
						}
					buttonx+=theme.partsWidth[CLOSEACTIVE]+buttonspace;
				}
			else if (f->deletebutton != NULL)
				{
					bdestroy(f->deletebutton);
					f->deletebutton=NULL;
				}

//max button
			if(f->client->canMaximize==true)
				{
					if (f->maximize==NULL)
						{
							f->maximize=bcreate(maximizeWindow,f->client,maximizeBitmap,f->window,f->width-theme.partsWidth[MAXACTIVE]- buttonx,(theme.titleBarHeight/2)-(theme.partsHeight[MAXACTIVE]/2),theme.partsWidth[MAXACTIVE],theme.partsHeight[MAXACTIVE],NorthEastGravity,MAXACTIVE,f);
						}
					buttonx+=theme.partsWidth[MAXACTIVE]+buttonspace;
				}
//min button
			if(f->client->canMinimize==true)
				{
					if (f->minimize==NULL)
						{
							f->minimize=bcreate(minimizeWindow,f->client,maximizeBitmap,f->window,f->width-theme.partsWidth[MINACTIVE]- buttonx,(theme.titleBarHeight/2)-(theme.partsHeight[MINACTIVE]/2),theme.partsWidth[MINACTIVE],theme.partsHeight[MINACTIVE],NorthEastGravity,MINACTIVE,f);
						}
					buttonx+=theme.partsWidth[MINACTIVE]+buttonspace;
				}
//shade button
			if (f->shade==NULL)
				{
					f->shade=bcreate(shadeWindow,f->client,maximizeBitmap,f->window,f->width-theme.partsWidth[SHADEACTIVE]- buttonx,(theme.titleBarHeight/2)-(theme.partsHeight[SHADEACTIVE]/2),theme.partsWidth[SHADEACTIVE],theme.partsHeight[SHADEACTIVE],NorthEastGravity,SHADEACTIVE,f);
				}

			buttonx+=theme.partsWidth[SHADEACTIVE]+buttonspace;

			f->buttonBarWith= buttonx;
			}
		}
	else
		{
			if (chaswmproto(f->client,WM_DELETE_WINDOW))
				{
					if (f->deletebutton==NULL)
						{
							f->deletebutton=bcreate(mydelete,f->client,deletebitmap,f->window,f->width-1-font->size-buttonx,0,sz,sz,NorthEastGravity,CLOSEACTIVE,f);
							buttonx+=sz;
						}
				}
			else if (f->deletebutton != NULL)
				{
					bdestroy(f->deletebutton);
					f->deletebutton=NULL;
				}

			if(f->client->canMaximize==true)
				{
					if (f->maximize==NULL)
						{
							f->maximize=bcreate(maximizeWindow,f->client,maximizeBitmap,f->window,f->width-font->size-buttonx,0,sz,sz,NorthEastGravity,MAXACTIVE,f);
							buttonx+=sz;
						}
				}

			if(f->client->canMinimize==true)
				{
					if (f->minimize==NULL)
						{
							f->minimize=bcreate(minimizeWindow,f->client,minimizeBitmap,f->window,f->width-buttonx-font->size,0,sz,sz,NorthEastGravity,MINACTIVE,f);
							buttonx+=sz;
						}
				}

			if (f->shade==NULL)
				{
					f->shade=bcreate(shadeWindow,f->client,shadeBitmap,f->window,f->width-buttonx-font->size,0,sz,sz,NorthEastGravity,SHADEACTIVE,f);
				}
		}

	f->background=&activeGC;

	f->namewidth=namewidth(font,f->client);

	if(f->deletebutton!=NULL)
		update(f->deletebutton);
	if(f->maximize!=NULL)
		update(f->maximize);
	if(f->minimize!=NULL)
		update(f->minimize);
	if(f->shade!=NULL)
		update(f->shade);
	lastbuttonx=buttonx;
	repaint(f);
}

void confrequest(struct frame *f,XConfigureRequestEvent *e)
{
		struct geometry g=cgetgeom(f->client);

	if (e->value_mask & CWBorderWidth)
		{
			g.borderwidth=e->border_width;
			csetgeom(f->client,g);
		}

	int dx,dy;
	gravitate(cgetgrav(f->client),g.borderwidth,&dx,&dy);

	int x=f->x;
	int y=f->y;

	// Fetch requested geometry
	if (e->value_mask & CWX)
		x=e->x+dx;
	if (e->value_mask & CWY)
		y=e->y+dy;
	if (e->value_mask & CWWidth)
		g.width=e->width;
	if (e->value_mask & CWHeight)
		g.height=e->height;

	int width=g.width+frameLeft+frameRight;
	int height=g.height+frameTop+frameBottom;

	moveresize(f,x,y,width,height);
}

void buttonpress(struct frame *f,XButtonEvent *e)
{
		if (e->button==Button1)
		{
			cpopapp(f->client);
			cfocus(f->client,e->time);

			if (e->y<frameTop || (e->state & Mod1Mask) != 0)
				{
					f->grabbed=True;
					f->downx=e->x;
					f->downy=e->y;
					XGrabPointer(dpy,f->window,False,Button1MotionMask | ButtonReleaseMask,GrabModeAsync,GrabModeAsync,None,None,e->time);
				}
		}
}

void buttonrelease(struct frame *f,XButtonEvent *e)
{
		if (e->button==Button1 && f->grabbed)
		{
			XUngrabPointer(dpy,e->time);
			f->grabbed=False;
		}
}

void motionnotify(struct frame *f,XMotionEvent *e)
{
		moveresize(f,e->x_root-f->downx,e->y_root-f->downy,f->width,f->height);
}

void maprequest(struct frame *f,XMapRequestEvent *e)
{
		Window win=f->client->window;
	if (e->window==win)
		redirect((XEvent *)e,win);
}

void expose(struct frame *f,XExposeEvent *e)
{
		if (e->count==0)
		repaint(f);
}

bool frameevent(void *self,XEvent *e,int type)
{
	//printf("frame event\n");
	switch (e->type)
		{
		case Expose:
//printf("expose event\n");
			expose((frame*)self,&e->xexpose);
			break;
		case MotionNotify:
			motionnotify((frame*)self,&e->xmotion);
			break;
		case ButtonPress:
			buttonpress((frame*)self,&e->xbutton);
			break;
		case ButtonRelease:
			buttonrelease((frame*)self,&e->xbutton);
			break;
		case ConfigureRequest:
//printf("ConfigureRequest event\n");
			confrequest((frame*)self,&e->xconfigurerequest);
			break;
		case MapRequest:
//printf("MapRequest event\n");
			maprequest((frame*)self,&e->xmaprequest);
			break;
		}
	return(true);
}

void adjustDraggers(struct frame *f)
{
	struct dragger	*d;

//bottom
	d=f->bottomResizer;
	d->width=f->width-d->wadjust;
	XResizeWindow(dpy,d->window,d->width,d->height);

//left
	d=f->leftResizer;
	d->height=f->height-d->hadjust;
	XResizeWindow(dpy,d->window,d->width,d->height);
//right
	d=f->rightResizer;
	d->height=f->height-d->hadjust;
	XResizeWindow(dpy,d->window,d->width,d->height);
}

void resizeLeft(void *self,int xdrag,int ydrag,unsigned long counter,Time t)
{
		struct frame	*f=(frame*)self;
	struct dragger	*d=f->leftResizer;
	int				x,w,h;
	int				woffset=d->width;

	if(f->isShaded==true)
		return;

	w=f->width-(xdrag-f->x);
	h=f->height;

	w -= woffset;
	chintsize(f->client,w,h,&w,&h);
	w += woffset;
	x=f->x+f->width-w;

	if (counter==0)
		{
			cpopapp(f->client);
			cfocus(f->client,t);
		}
	moveresize(f,x,f->y,w,f->height);
	adjustDraggers(f);
}

void resizetopleft(void *self,int xdrag,int ydrag,unsigned long counter,Time t)
{
		struct frame	*f=(frame*)self;
	int				x,y,w,h;

	w=f->width-(xdrag-f->x);
	h=f->height-(ydrag-f->y);

	w -= frameLeft+frameRight;
	h -= frameTop+frameBottom;
	chintsize(f->client,w,h,&w,&h);
	w += frameLeft+frameRight;
	h += frameTop+frameBottom;

	x=f->x+f->width-w;
	if(f->isShaded==true)
		y=ydrag;
	else
		y=f->y+f->height-h;
	if (counter==0)
		{
			cpopapp(f->client);
			cfocus(f->client,t);
		}
	moveresize(f,x,y,w,h);
	adjustDraggers(f);
}

void resizeBottom(void *self,int xdrag,int ydrag,unsigned long counter,Time t)
{
	struct frame	*f=(frame*)self;
	struct dragger	*d=f->bottomResizer;
	int				w,h;
	int				hoffset=d->height;

	if(f->isShaded==true)
		return;

	w=f->width;
	h=ydrag+1-f->y;
	h -= hoffset;

	chintsize(f->client,w,h,&w,&h);

	h += hoffset;

	if (counter==0)
		{
			cpopapp(f->client);
			cfocus(f->client,t);
		}
	moveresize(f,f->x,f->y,f->width,h);
	repaint(f);
	adjustDraggers(f);
}

void resizeRight(void *self,int xdrag,int ydrag,unsigned long counter,Time t)
{
		struct frame	*f=(frame*)self;
	struct dragger	*d=f->rightResizer;
	int				w,h;
	int				woffset=d->width;

	if(f->isShaded==true)
		return;

	w=xdrag+1-f->x;
	w -= woffset;
	h=f->height;

	chintsize(f->client,w,h,&w,&h);
	w += woffset;

	if (counter==0)
		{
			cpopapp(f->client);
			cfocus(f->client,t);
		}
	moveresize(f,f->x,f->y,w,f->height);
	adjustDraggers(f);
}

void resizeBottomRight(void *self,int xdrag,int ydrag,unsigned long counter,Time t)
{
	struct frame	*f=(frame*)self;
	struct dragger	*d=f->bottomRightResizer;

	int				w,h;
	int				woffset=d->width;
	int				hoffset=d->height;

	if(f->isShaded==true)
		return;

	w=xdrag+1-f->x;
	h=ydrag+1-f->y;

	h -= hoffset;
	w -= woffset;
	chintsize(f->client,w,h,&w,&h);
	w += woffset;
	h += hoffset;

	if (counter==0)
		{
			cpopapp(f->client);
			cfocus(f->client,t);
		}
	moveresize(f,f->x,f->y,w,h);
	adjustDraggers(f);
}

void resizeBottomLeft(void *self,int xdrag,int ydrag,unsigned long counter,Time t)
{
		struct frame	*f=(frame*)self;
	struct dragger	*d=f->bottomRightResizer;
	int				x,w,h;
	int				woffset=d->width;
	int				hoffset=d->height;

	if(f->isShaded==true)
		return;

	w=f->width-(xdrag-f->x);
	h=ydrag+1-f->y;

	h -= hoffset;
	w -= woffset;
	chintsize(f->client,w,h,&w,&h);
	w += woffset;
	h += hoffset;

	x=f->x+f->width-w;

	if (counter==0)
		{
			cpopapp(f->client);
			cfocus(f->client,t);
		}

	moveresize(f,x,f->y,w,h);
	adjustDraggers(f);
}

void resizetopright(void *self,int xdrag,int ydrag,unsigned long counter,Time t)
{
		struct frame	*f=(frame*)self;
	int				x,y,w,h;

	w=xdrag+1-f->x;
	h=f->height-(ydrag-f->y);

	w -= frameLeft+frameRight;
	h -= frameTop+frameBottom;
	chintsize(f->client,w,h,&w,&h);
	w += frameLeft+frameRight;
	h += frameTop+frameBottom;

	x=f->x;
	if(f->isShaded==true)
		y=ydrag;
	else
		y=f->y+f->height-h;

	if (counter==0)
		{
			cpopapp(f->client);
			cfocus(f->client,t);
		}
	moveresize(f,x,y,w,h);
	adjustDraggers(f);
}

struct frame *fcreate(struct client *c)
{
	XSetWindowAttributes	wa;
	int						topleftdw;
	int						topleftdh;
	int						bottomleftdw;
	int						bottomleftdh;
	int						toprightdw;
	int						toprightdh;
	int						bottomrightdw;
	int						bottomrightdh;
	int						bottomdw;
	int						bottomdh;
	int						leftdw;
	int						leftdh;
	int						rightdw;
	int						rightdh;

	if (fcount==0)
		{
			cursortopleft=XCreateFontCursor(dpy,XC_top_left_corner);
			cursortopright=XCreateFontCursor(dpy,XC_top_right_corner);
			cursorRight=XCreateFontCursor(dpy,XC_right_side);
			cursorBottomRight=XCreateFontCursor(dpy,XC_bottom_right_corner);
			cursorBottom=XCreateFontCursor(dpy,XC_bottom_side);
			cursorBottomLeft=XCreateFontCursor(dpy,XC_bottom_left_corner);
			cursorLeft=XCreateFontCursor(dpy,XC_left_side);
		}
	fcount++;

	struct frame *f=(frame*)xmalloc(sizeof *f);

	f->client=c;
	f->pixmap=None;
	f->namewidth=0;

	struct geometry g=cgetgeom(c);
	int dx,dy;
	gravitate(cgetgrav(c),g.borderwidth,&dx,&dy);

	f->x=g.x+dx;
	f->y=g.y+dy;

	f->width=g.width+frameLeft+frameRight;
	f->height=g.height+frameTop+frameBottom;

	f->grabbed=False;
	f->oldX=f->x;
	f->oldY=f->y;
	f->oldWidth=f->width;
	f->oldHeight=f->height;
	f->isMaximized=false;
	f->isShaded=false;

	f->buttonBarWith=0;
	wa.bit_gravity=NorthWestGravity;

	f->window=XCreateWindow(dpy,root,f->x,f->y,f->width,f->height,0,CopyFromParent,InputOutput,CopyFromParent,CWBitGravity,&wa);

	f->mask=XCreatePixmap(dpy,f->window,f->width,f->height,1);
	f->maskGC=XCreateGC(dpy,f->mask,0,NULL);
	XSetForeground(dpy,f->maskGC,blackColor);
	XFillRectangle(dpy,f->mask,f->maskGC,0,0,f->width,f->height);

	Window clientwin=f->client->window;

	reorder(clientwin,f->window);

	f->listener.function=frameevent;
	f->listener.pointer=f;
	setlistener(f->window,&f->listener);

	XSelectInput(dpy,f->window,SubstructureRedirectMask | ButtonPressMask | ButtonReleaseMask | ExposureMask);

	grabbutton(Button1,Mod1Mask,f->window,False,ButtonReleaseMask,GrabModeAsync,GrabModeAsync,None,None);
	/*
	 * The order in which the resizers and the delete button
	 * are created is important since it determines their
	 * stacking order. For very small windows it is important
	 * that the right resizer and the delete button are above
	 * the left resizer.
	 */

	if(theme.useTheme==true)
		{
			topleftdw=theme.partsWidth[TOPLEFTACTIVE];
			topleftdh=theme.partsHeight[TOPLEFTACTIVE];

			bottomleftdw=theme.partsWidth[BOTTOMLEFTACTIVE];
			bottomleftdh=theme.partsHeight[BOTTOMLEFTACTIVE];

			toprightdw=theme.partsWidth[TOPRIGHTACTIVE];
			toprightdh=theme.partsHeight[TOPRIGHTACTIVE];

			bottomrightdw=theme.partsWidth[BOTTOMRIGHTACTIVE];
			bottomrightdh=theme.partsHeight[BOTTOMRIGHTACTIVE];

			bottomdw=f->width-bottomleftdw-bottomrightdw;
			bottomdh=theme.partsHeight[BOTTOMACTIVE];

			leftdw=theme.partsWidth[LEFTACTIVE];
			leftdh=f->height-topleftdh-bottomleftdh;

			rightdw=theme.partsWidth[RIGHTACTIVE];
			rightdh=f->height-toprightdh-bottomrightdh;
		}
	else
		{
			topleftdw=font->size+1;
			topleftdh=frameTop+2;

			bottomleftdw=topleftdw;
			bottomleftdh=topleftdh;

			toprightdw=topleftdw;
			toprightdh=topleftdh;

			bottomrightdw=topleftdw;
			bottomrightdh=topleftdh;

			bottomdw=f->width-bottomleftdw-bottomrightdw;
			bottomdh=topleftdh;

			leftdw=frameLeft;
			leftdh=f->height-topleftdh-bottomleftdh;

			rightdw=toprightdw;
			rightdh=f->height-toprightdh-bottomrightdh;
	}

	f->topleftresizer=dcreate(f->window,0,0,bottomleftdw,topleftdh,NorthWestGravity,cursortopleft,resizetopleft,f);
	f->toprightresizer=dcreate(f->window,f->width-toprightdw,0,toprightdw,toprightdh,NorthEastGravity,cursortopright,resizetopright,f);
	f->bottomResizer=dcreate(f->window,bottomleftdw,f->height-bottomdh,bottomdw,bottomdh,SouthWestGravity,cursorBottom,resizeBottom,f);
	f->leftResizer=dcreate(f->window,0,topleftdh,leftdw,leftdh,NorthWestGravity,cursorLeft,resizeLeft,f);
	f->bottomLeftResizer=dcreate(f->window,0,f->height-bottomleftdh,bottomleftdw,bottomleftdh,SouthWestGravity,cursorBottomLeft,resizeBottomLeft,f);
	f->rightResizer=dcreate(f->window,f->width-rightdw,toprightdh,rightdw,rightdh,NorthEastGravity,cursorRight,resizeRight,f);
	f->bottomRightResizer=dcreate(f->window,f->width-bottomrightdw,f->height-bottomrightdh,bottomrightdw,bottomrightdh,SouthEastGravity,cursorBottomRight,resizeBottomRight,f);

	f->deletebutton=NULL;
	f->maximize=NULL;
	f->minimize=NULL;
	f->shade=NULL;

	XSetWindowBorderWidth(dpy,clientwin,0);
	setgrav(clientwin,NorthWestGravity);
	if (f->client->ismapped)
		cignoreunmap(f->client);

	XReparentWindow(dpy,clientwin,f->window,frameLeft,frameTop);

	g.x += frameLeft-leftdw;
	g.y += frameTop-toprightdh;
	csetgeom(f->client,g);
	csendconf(f->client);

	extents	ext;

	ext.top=frameTop;
	ext.bottom=frameBottom;
	ext.right=frameRight;
	ext.left=frameLeft;

	ewmh_notifyframeextents(clientwin,ext);

	fupdate(f);

	if (f->client->ismapped)
		XMapWindow(dpy,f->window);

	return f;
}

void fdestroy(struct frame *f)
{
	Bool hadfocus=chasfocus(f->client);
	struct geometry g=cgetgeom(f->client);

	XUnmapWindow(dpy,f->window);

	Window clientwin=f->client->window;

	XSetWindowBorderWidth(dpy,clientwin,g.borderwidth);

	int grav=cgetgrav(f->client);
	setgrav(clientwin,grav);
	int dx,dy;
	gravitate(grav,g.borderwidth,&dx,&dy);
	if (f->client->ismapped)
		cignoreunmap(f->client);
	g.x=f->x-dx;
	g.y=f->y-dy;
	csetgeom(f->client,g);
	XReparentWindow(dpy,clientwin,root,g.x,g.y);

	ewmh_notifyframeextents(clientwin,(struct extents)
	{
		.top=0,
		 .bottom=0,
		  .left=0,
		   .right=0
	});

	reorder(f->window,clientwin);
	if (hadfocus)
		cfocus(f->client,CurrentTime);
	setlistener(f->window,NULL);
	ddestroy(f->topleftresizer);
	ddestroy(f->toprightresizer);
	if (f->deletebutton != NULL)
		bdestroy(f->deletebutton);
	if (f->maximize != NULL)
		bdestroy(f->maximize);
	if (f->minimize != NULL)
		bdestroy(f->minimize);
	if (f->shade != NULL)
		bdestroy(f->shade);

	if (f->pixmap != None)
		XFreePixmap(dpy,f->pixmap);
	XDestroyWindow(dpy,f->window);
	free(f);

	fcount--;
	if (fcount==0)
		{
			XFreeCursor(dpy,cursortopleft);
			XFreeCursor(dpy,cursortopright);
		}
}

Window fgetwin(struct frame *f)
{
		return f->window;
}

struct geometry fgetgeom(struct frame *f)
{
		return (struct geometry)
	{
		.x=f->x,
		 .y=f->y,
		  .width=f->width,
		   .height=f->height,
		    .borderwidth=0
	};
}

