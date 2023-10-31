/*
 *
 * ©K. D. Hedger. Tue  9 Aug 20:42:38 BST 2022 keithdhedger@gmail.com

 * This file (clientClass.cpp) is part of LFSWM2.

 * LFSWM2 is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.

 * LFSWM2 is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

 * You should have received a copy of the GNU General Public License
 * along with LFSWM2.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "clientClass.h"

LFSWM2_clientClass::LFSWM2_clientClass(LFSWM2_Class *mainclass,Window id)
{
	this->mainClass=mainclass;
	this->picFormat=XRenderFindStandardFormat(this->mainClass->display,PictStandardRGB24);
	this->pa.subwindow_mode=IncludeInferiors;
}

LFSWM2_clientClass::~LFSWM2_clientClass(void)
{
	XWindowAttributes	x_window_attrs;

	this->mainClass->LFSWM2_pushXErrorHandler();
		this->mainClass->mainWindowClass->LFSWM2_freeHints(this->windowHints);
		this->mainClass->mainWindowClass->LFSWM2_setClientList(this->contentWindow,false);

	//if(this->frameWindow!=None)
		this->mainClass->mainWindowClass->LFSWM2_deleteClientEntry(this->frameWindow);
		this->mainClass->mainWindowClass->LFSWM2_deleteClientEntry(this->contentWindow);

		if(this->isBorderless==true)
			{
				XGetWindowAttributes(this->mainClass->display,this->frameWindow,&x_window_attrs);
				XReparentWindow(this->mainClass->display,this->contentWindow,this->mainClass->rootWindow,x_window_attrs.x,x_window_attrs.y);
			}
		else
			{
				if(this->transientFor==None)
					XReparentWindow(this->mainClass->display,this->contentWindow,this->mainClass->rootWindow,this->frameWindowRect.x+this->mainClass->leftSideBarSize,frameWindowRect.y+this->mainClass->titleBarSize);
				else
					XReparentWindow(this->mainClass->display,this->contentWindow,this->mainClass->rootWindow,-10000,-1000);
			}

		XRemoveFromSaveSet(this->mainClass->display,this->contentWindow);	
		XUnmapWindow(this->mainClass->display,this->frameWindow);

		if(resizeWindow!=None)
			XDestroyWindow(this->mainClass->display,this->resizeWindow);

		if(this->closeControlStruct.controlGC!=None)
			XFreeGC(this->mainClass->display,this->closeControlStruct.controlGC);
		if(this->maximizeControlStruct.controlGC!=None)
			XFreeGC(this->mainClass->display,this->maximizeControlStruct.controlGC);
		if(this->minimizeControlStruct.controlGC!=None)
			XFreeGC(this->mainClass->display,this->minimizeControlStruct.controlGC);
		if(this->shadeControlStruct.controlGC!=None)
			XFreeGC(this->mainClass->display,this->shadeControlStruct.controlGC);
		if(this->menuControlStruct.controlGC!=None)
			XFreeGC(this->mainClass->display,this->menuControlStruct.controlGC);
		if(this->frameGC!=None)
			XFreeGC(this->mainClass->display,this->frameGC);

	this->mainClass->LFSWM2_popXErrorHandler();
}

void LFSWM2_clientClass::LFSWM2_setWindowName(void)
{
	char					*namex=NULL;
	XTextProperty		p;
	long unsigned int	nitems_return;

	this->name="";
	this->mainClass->LFSWM2_pushXErrorHandler();

	namex=(char*)this->mainClass->mainWindowClass->LFSWM2_getProp(this->contentWindow,this->mainClass->atomshashed.at(this->mainClass->prefs.LFSTK_hashFromKey("_NET_WM_NAME")),this->mainClass->atomshashed.at(this->mainClass->prefs.LFSTK_hashFromKey("UTF8_STRING")),&nitems_return);
	if (namex!=NULL)
		{
			this->name=namex;
			this->nameIsUTF=true;
		}
	else
		{
			if(XGetWMName(this->mainClass->display,this->contentWindow,&p)!=0)
				{
					namex=this->mainClass->mainWindowClass->LFSWM2_decodeTextProp(&p);
					if(namex!=NULL)
						{
							this->name=namex;
							this->nameIsUTF=false;;
						}
					XFree(p.value);
				}
		}

	if(namex!=NULL)
		free(namex);

	this->mainClass->LFSWM2_popXErrorHandler();
}

void LFSWM2_clientClass::drawMousePressed(Window id,Pixmap pm,controlData data)
{
	std::string		cn=data.controlName+"-pressed";
	long unsigned	col=(0xff000000|(~this->mainClass->frameFG->pixel&0xffffff));

	if(this->mainClass->useTheme==true)
		{
			XSetClipMask(this->mainClass->display,this->frameGC,this->mainClass->mainWindowClass->theme.masks[this->mainClass->prefs.LFSTK_hashFromKey(cn)]);
			XSetClipOrigin(this->mainClass->display,this->frameGC,0,0);
			XCopyArea(this->mainClass->display,this->mainClass->mainWindowClass->theme.pixmaps[this->mainClass->prefs.LFSTK_hashFromKey(cn)],id,this->frameGC,0,0,this->mainClass->mainWindowClass->theme.partsWidth[this->mainClass->prefs.LFSTK_hashFromKey(cn)],this->mainClass->mainWindowClass->theme.partsHeight[this->mainClass->prefs.LFSTK_hashFromKey(cn)],0,0);
			XShapeCombineMask(this->mainClass->display,id,ShapeBounding,0,0,this->mainClass->mainWindowClass->theme.masks[this->mainClass->prefs.LFSTK_hashFromKey(cn)],ShapeSet);
			return;
		}

	XSetForeground(this->mainClass->display,data.controlGC,col);
	XSetClipMask(this->mainClass->display,data.controlGC,None);
	XSetLineAttributes(this->mainClass->display,data.controlGC,2,LineSolid,CapNotLast,JoinMiter);
	XDrawRectangle(this->mainClass->display,id,data.controlGC,1,1,DEFAULTCONTROLSIZE-2,DEFAULTCONTROLSIZE-2);
}

void LFSWM2_clientClass::drawMouseEnter(Window id,Pixmap pm,controlData data)
{
	std::string		cn=data.controlName+"-prelight";
	long unsigned	col=(0xff000000|(this->mainClass->frameFG->pixel&0xffffff));

	if(this->isActive==false)
		cn=data.controlName+"-inactive";

	if(this->mainClass->useTheme==true)
		{
			XSetClipMask(this->mainClass->display,this->frameGC,this->mainClass->mainWindowClass->theme.masks[this->mainClass->prefs.LFSTK_hashFromKey(cn)]);
			XSetClipOrigin(this->mainClass->display,this->frameGC,0,0);
			XCopyArea(this->mainClass->display,this->mainClass->mainWindowClass->theme.pixmaps[this->mainClass->prefs.LFSTK_hashFromKey(cn)],id,this->frameGC,0,0,this->mainClass->mainWindowClass->theme.partsWidth[this->mainClass->prefs.LFSTK_hashFromKey(cn)],this->mainClass->mainWindowClass->theme.partsHeight[this->mainClass->prefs.LFSTK_hashFromKey(cn)],0,0);
			XShapeCombineMask(this->mainClass->display,id,ShapeBounding,0,0,this->mainClass->mainWindowClass->theme.masks[this->mainClass->prefs.LFSTK_hashFromKey(cn)],ShapeSet);
			return;
		}

	XSetForeground(this->mainClass->display,data.controlGC,col);
	XSetClipMask(this->mainClass->display,data.controlGC,None);
	XSetLineAttributes(this->mainClass->display,data.controlGC,2,LineSolid,CapNotLast,JoinMiter);
	XDrawRectangle(this->mainClass->display,id,data.controlGC,1,1,DEFAULTCONTROLSIZE-2,DEFAULTCONTROLSIZE-2);
}

void LFSWM2_clientClass::LFSWM2_drawMouseLeave(Window id,Pixmap pm,controlData data)
{
	std::string		cn=data.controlName+"-active";
	long unsigned	col=(this->mainClass->frameAlpha|(this->mainClass->frameBG->pixel&0xffffff));

	if(this->isActive==false)
		cn=data.controlName+"-inactive";

	if(this->mainClass->useTheme==true)
		{
			XSetClipMask(this->mainClass->display,this->frameGC,this->mainClass->mainWindowClass->theme.masks[this->mainClass->prefs.LFSTK_hashFromKey(cn)]);
			XSetClipOrigin(this->mainClass->display,this->frameGC,0,0);
			XCopyArea(this->mainClass->display,this->mainClass->mainWindowClass->theme.pixmaps[this->mainClass->prefs.LFSTK_hashFromKey(cn)],id,this->frameGC,0,0,this->mainClass->mainWindowClass->theme.partsWidth[this->mainClass->prefs.LFSTK_hashFromKey(cn)],this->mainClass->mainWindowClass->theme.partsHeight[this->mainClass->prefs.LFSTK_hashFromKey(cn)],0,0);
			XShapeCombineMask(this->mainClass->display,id,ShapeBounding,0,0,this->mainClass->mainWindowClass->theme.masks[this->mainClass->prefs.LFSTK_hashFromKey(cn)],ShapeSet);
			return;
		}

	XSetForeground(this->mainClass->display,data.controlGC,col);
	XSetFillStyle(this->mainClass->display,data.controlGC,FillSolid);
	XSetClipMask(this->mainClass->display,data.controlGC,None);
	XFillRectangle(this->mainClass->display,id,data.controlGC,0,0,DEFAULTCONTROLSIZE,DEFAULTCONTROLSIZE);

	col=(0xff000000|(this->mainClass->frameText->pixel&0xffffff));
  	XSetForeground(this->mainClass->display,data.controlGC,col);
	XSetClipOrigin(this->mainClass->display,data.controlGC,DEFAULTCONTROLXYOFFSET,DEFAULTCONTROLXYOFFSET);
	XSetClipMask(this->mainClass->display,data.controlGC,pm);
	XFillRectangle(this->mainClass->display,id,data.controlGC,DEFAULTCONTROLXYOFFSET,DEFAULTCONTROLXYOFFSET,DEFAULTCONTROLBITMAPSIZE,DEFAULTCONTROLBITMAPSIZE);
}

void LFSWM2_clientClass::LFSWM2_sendCloseWindow(void)
{
	XClientMessageEvent	em;
//this->mainClass->mainWindowClass->LFSWM2_windowClass::LFSWM2_removeProp(this->contentWindow,this->mainClass->prefs.LFSTK_hashFromKey("_NET_FRAME_EXTENTS"));
	em.type=ClientMessage;
	em.window=this->contentWindow;
	em.message_type=this->mainClass->atomshashed.at(this->mainClass->prefs.LFSTK_hashFromKey("WM_PROTOCOLS"));
	em.format=32;
	em.data.l[0]=this->mainClass->atomshashed.at(this->mainClass->prefs.LFSTK_hashFromKey("WM_DELETE_WINDOW"));;
	em.data.l[1]=CurrentTime;
	XSendEvent(this->mainClass->display,this->contentWindow,false,NoEventMask,(XEvent *)&em);
	this->visible=false;
	XSync(this->mainClass->display,false);
}

void LFSWM2_clientClass::adjustContentWindow(void)
{
	XWindowAttributes	frameattr;
	rectStruct		r;

	if(this->isFullscreen==true)
		return;
	if(this->isBorderless==true)
		return;

	XGetWindowAttributes(this->mainClass->display,this->frameWindow,&frameattr);
	r.x=frameattr.x+this->mainClass->leftSideBarSize;
	r.y=frameattr.y+this->mainClass->titleBarSize;
	r.w=this->frameWindowRect.w-this->mainClass->leftSideBarSize-this->mainClass->riteSideBarSize;
	r.h=this->frameWindowRect.h-this->mainClass->titleBarSize-this->mainClass->bottomBarSize;

	this->mainClass->mainEventClass->LFSWM2_sendConfigureEvent(this->contentWindow,r);
	this->setWindowRects(true);
}

void LFSWM2_clientClass::resetContentWindow(void)
{
	if(this->isBorderless==true)
		return;
	XMoveResizeWindow(this->mainClass->display,this->contentWindow,this->mainClass->leftSideBarSize,this->mainClass->titleBarSize,this->frameWindowRect.w-this->mainClass->leftSideBarSize-this->mainClass->riteSideBarSize,this->frameWindowRect.h-this->mainClass->titleBarSize-this->mainClass->bottomBarSize);
}

bool LFSWM2_clientClass::doResizeDraggers(XEvent *e)
{
	rectStruct				r;
	XSizeHints				xh;
	long						dummy;
	int						contenthadjust=this->mainClass->titleBarSize+this->mainClass->bottomBarSize;
	int						contentwadjust=this->mainClass->leftSideBarSize+this->mainClass->riteSideBarSize;

	if(this->isBorderless==true)
		return(true);

	switch(e->type)
		{
			case ButtonPress:
				{
					this->mainClass->mainEventClass->noRestack=true;
					this->LFSWM2_unSpecial();
					this->buttonDown=true;
					this->sx=e->xbutton.x;
					this->rsx=e->xbutton.x_root;
					this->rsy=e->xbutton.y_root;
					this->sy=e->xbutton.y;
					steps=0;
					this->setWindowRects(false);
					this->dragRect=this->frameWindowRect;
					this->mainClass->restackCnt=2;

					switch(this->resizeMode)
						{
							case FASTRESIZE:
								XMoveWindow(this->mainClass->display,this->contentWindow,this->frameWindowRect.w+10,0);
								break;
							case LIVERESIZE:
								break;
							case SIZERESIZE:
								break;
							case SCALERESIZE:
								{
									this->currentRootPixmap=XCreatePixmap(this->mainClass->display,this->contentWindow,this->contentWindowRect.w,this->contentWindowRect.h,this->mainClass->depth);
									XSetClipMask(this->mainClass->display,this->mainClass->mainGC,None);
									XSetClipOrigin(this->mainClass->display, this->mainClass->mainGC,0,0);
									this->resizeImage=XGetImage(this->mainClass->display,this->contentWindow,0,0,this->contentWindowRect.w,this->contentWindowRect.h,AllPlanes,ZPixmap);

									XSetClipMask(this->mainClass->display,this->mainClass->mainGC,None);
									XSetClipOrigin(this->mainClass->display,this->mainClass->mainGC,0,0);
									XPutImage(this->mainClass->display,this->currentRootPixmap, this->mainClass->mainGC,this->resizeImage,0,0,0,0,this->contentWindowRect.w,this->contentWindowRect.h);

									this->ow=this->contentWindowRect.w;
									this->oh=this->contentWindowRect.h;								
									this->resizeWindow=XCreateSimpleWindow(this->mainClass->display,this->frameWindow,this->mainClass->leftSideBarSize,this->mainClass->titleBarSize,this->contentWindowRect.w,this->contentWindowRect.h,BORDER_WIDTH,this->mainClass->frameFG->pixel,this->mainClass->frameBG->pixel);
									XMoveWindow(this->mainClass->display,this->contentWindow,this->mainClass->displayWidth+10,0);
									XMapWindow(this->mainClass->display,this->resizeWindow);
									XRaiseWindow(this->mainClass->display,this->resizeWindow);
									primaryPicture=XRenderCreatePicture(this->mainClass->display,this->currentRootPixmap,picFormat,CPSubwindowMode,&pa);
									windowPicture=XRenderCreatePicture(this->mainClass->display,this->resizeWindow,picFormat,CPSubwindowMode,&pa);

									XRenderComposite(this->mainClass->display,PictOpSrc,primaryPicture,0,windowPicture,0,0,0,0,0,0,this->ow,this->oh);
								}
								break;
						}
				}
				break;

			case ButtonRelease:
				this->mainClass->mainEventClass->noRestack=false;
				this->buttonDown=false;
				this->sx=0;
				this->sy=0;
				this->isShaded=false;
				this->adjustContentWindow();
				this->resetContentWindow();
				if(this->currentRootPixmap!=None)
					XFreePixmap(this->mainClass->display,this->currentRootPixmap);
				if(this->primaryPicture!=None)
					XRenderFreePicture(this->mainClass->display,this->primaryPicture);
				if(this->windowPicture!=None)
					XRenderFreePicture(this->mainClass->display,this->windowPicture);
				if(this->resizeImage!=NULL)
					XDestroyImage(this->resizeImage);

				this->currentRootPixmap=None;
				this->primaryPicture=None;
				this->windowPicture=None;
				this->resizeImage=None;

				XMoveResizeWindow(this->mainClass->display,this->resizeWindow,this->mainClass->displayWidth+10,0,1,1);
				this->setWindowRects(true);
				this->mainClass->restackCnt=0;
				break;

			case MotionNotify:
				if(buttonDown==true)
					{
						this->mainClass->restackCnt=2;
						this->steps++;
						if(this->steps>this->smoothness)
							{
								r.x=this->frameWindowRect.x;
								r.y=this->frameWindowRect.y;
								r.w=this->frameWindowRect.w;
								r.h=this->frameWindowRect.h;

								if(e->xany.window==this->topLeftDragger)
									{
										r.x=e->xmotion.x_root-this->sx;
										r.y=e->xmotion.y_root-this->sy;
										r.w=this->dragRect.w-(e->xmotion.x_root-this->rsx);
										r.h=this->dragRect.h-(e->xmotion.y_root-this->rsy);
									}
								if(e->xany.window==this->topRightDragger)
									{
										r.y=e->xmotion.y_root-this->sy;
										r.w=this->dragRect.w+(e->xmotion.x_root-this->rsx);
										r.h=this->dragRect.h-(e->xmotion.y_root-this->rsy);
									}
								if(e->xany.window==this->bottomLeftDragger)
									{
										r.x=e->xmotion.x_root-this->sx;
										r.w=this->dragRect.w-(e->xmotion.x_root-this->rsx);
										r.h=this->dragRect.h+(e->xmotion.y_root-this->rsy);
									}
								if(e->xany.window==this->bottomRightDragger)
									{
										r.w=this->dragRect.w+(e->xmotion.x_root-this->rsx);
										r.h=this->dragRect.h+(e->xmotion.y_root-this->rsy);
									}

								if(e->xany.window==this->bottomDragger)
									{
										r.h=this->dragRect.h+(e->xmotion.y_root-this->rsy);
									}

								if(e->xany.window==this->leftSideDragger)
									{
										r.x=e->xmotion.x_root-this->sx;
										r.w=this->dragRect.w-(e->xmotion.x_root-this->rsx);
									}
								if(e->xany.window==this->rightSideDragger)
									{
										r.w=this->dragRect.w+(e->xmotion.x_root-this->rsx);
									}

								if(XGetWMNormalHints(this->mainClass->display,this->contentWindow,&xh,&dummy)!=0)
									{
										if(r.w-contentwadjust<xh.min_width)
											{
												this->buttonDown=false;
												break;
											}
										if(r.h-contenthadjust<xh.min_height)
											{
												this->buttonDown=false;
												break;
											}
									}

								this->frameWindowRect=r;
								XMoveResizeWindow(this->mainClass->display,this->frameWindow,r.x,r.y,r.w,r.h);

								if(this->resizeMode==SCALERESIZE)
									{
										XResizeWindow(this->mainClass->display,this->resizeWindow,r.w-contentwadjust,r.h-contenthadjust);
										double x_scale=(double)ow /((double)this->frameWindowRect.w-(double)(this->mainClass->leftSideBarSize+this->mainClass->riteSideBarSize));
										double y_scale=(double)oh /((double)this->frameWindowRect.h-(double)(this->mainClass->titleBarSize+this->mainClass->bottomBarSize));
										XTransform transform_matrix1=
											{{
												{XDoubleToFixed(x_scale),XDoubleToFixed(0),XDoubleToFixed(0)},
												{XDoubleToFixed(0),XDoubleToFixed(y_scale),XDoubleToFixed(0)},
												{XDoubleToFixed(0),XDoubleToFixed(0),XDoubleToFixed(1.0)}  
											}};
										XResizeWindow(this->mainClass->display,this->contentWindow,r.w-contentwadjust,r.h-contenthadjust);
										XRenderSetPictureTransform(this->mainClass->display,primaryPicture,&transform_matrix1);
										XRenderComposite(this->mainClass->display,PictOpSrc,primaryPicture,0,windowPicture,0,0,0,0,0,0,r.w-contentwadjust,r.h-contenthadjust);

										this->steps=0;
										break;
									}

								if(this->resizeMode==LIVERESIZE)
									{
										this->contentWindowRect={this->mainClass->leftSideBarSize,this->mainClass->titleBarSize,r.w-contentwadjust,r.h-contenthadjust};
										XResizeWindow(this->mainClass->display,this->contentWindow,r.w-contentwadjust,r.h-contenthadjust);
									}
								else
									{
										XMoveWindow(this->mainClass->display,this->contentWindow,this->frameWindowRect.w+10,0);
									}

								this->steps=0;
								break;
							}
					}
				break;
		}
	return(true);
}

void LFSWM2_clientClass::resizeContentWindow(int w,int h,bool useframerect)
{
	if(this->isBorderless==true)
		return;

	if(useframerect==true)
		{
			XResizeWindow(this->mainClass->display,this->contentWindow,this->frameWindowRect.w-(this->mainClass->leftSideBarSize+this->mainClass->riteSideBarSize),this->frameWindowRect.h-(this->mainClass->titleBarSize+this->mainClass->bottomBarSize));
			this->contentWindowRect={this->contentWindowRect.x,this->contentWindowRect.y,this->frameWindowRect.w-(this->mainClass->leftSideBarSize+this->mainClass->riteSideBarSize),this->frameWindowRect.h-(this->mainClass->titleBarSize+this->mainClass->bottomBarSize)};
		}
	else
		{
			XResizeWindow(this->mainClass->display,this->contentWindow,w-(this->mainClass->leftSideBarSize+this->mainClass->riteSideBarSize),h-(this->mainClass->titleBarSize+this->mainClass->bottomBarSize));
			this->contentWindowRect={this->contentWindowRect.x,this->contentWindowRect.y,w-(this->mainClass->leftSideBarSize+this->mainClass->riteSideBarSize),h-(this->mainClass->titleBarSize+this->mainClass->bottomBarSize)};
		}
}

void LFSWM2_clientClass::resizeFrameWindow(void)
{
	XResizeWindow(this->mainClass->display,this->frameWindow,this->contentWindowRect.w+(this->mainClass->leftSideBarSize+this->mainClass->riteSideBarSize),this->contentWindowRect.h+(this->mainClass->titleBarSize+this->mainClass->bottomBarSize));
}

void LFSWM2_clientClass::LFSWM2_unSpecial(void)
{
	if(this->isMaximized==true)
		{
			this->mainClass->mainWindowClass->LFSWM2_removeProp(this->contentWindow,this->mainClass->atomshashed.at(this->mainClass->prefs.LFSTK_hashFromKey("_NET_WM_STATE_MAXIMIZED_HORZ")));
			this->mainClass->mainWindowClass->LFSWM2_removeProp(this->contentWindow,this->mainClass->atomshashed.at(this->mainClass->prefs.LFSTK_hashFromKey("_NET_WM_STATE_MAXIMIZED_VERT")));
			this->mainClass->mainWindowClass->LFSWM2_removeProp(this->contentWindow,this->mainClass->atomshashed.at(this->mainClass->prefs.LFSTK_hashFromKey("_NET_WM_STATE_HIDDEN")));
			this->isMaximized=false;
		}
	this->isShaded=false;
	this->mainClass->mainWindowClass->LFSWM2_reloadWindowState(this->contentWindow);
}

void LFSWM2_clientClass::LFSWM2_maxWindow(void)
{
	if(this->isMaximized==true)
		{
			this->mainClass->mainWindowClass->LFSWM2_removeProp(this->contentWindow,this->mainClass->atomshashed.at(this->mainClass->prefs.LFSTK_hashFromKey("_NET_WM_STATE_MAXIMIZED_HORZ")));
			this->mainClass->mainWindowClass->LFSWM2_removeProp(this->contentWindow,this->mainClass->atomshashed.at(this->mainClass->prefs.LFSTK_hashFromKey("_NET_WM_STATE_MAXIMIZED_VERT")));
		}
	else
		{
			this->mainClass->mainWindowClass->LFSWM2_addState(this->contentWindow,this->mainClass->atomshashed.at(this->mainClass->prefs.LFSTK_hashFromKey("_NET_WM_STATE_MAXIMIZED_HORZ")));
			this->mainClass->mainWindowClass->LFSWM2_addState(this->contentWindow,this->mainClass->atomshashed.at(this->mainClass->prefs.LFSTK_hashFromKey("_NET_WM_STATE_MAXIMIZED_VERT")));
		}
	this->mainClass->mainWindowClass->LFSWM2_reloadWindowState(this->contentWindow);
}

void LFSWM2_clientClass::LFSWM2_minWindow(void)
{
	this->mainClass->mainWindowClass->LFSWM2_addState(this->contentWindow,this->mainClass->atomshashed.at(this->mainClass->prefs.LFSTK_hashFromKey("_NET_WM_STATE_HIDDEN")));
	this->mainClass->mainWindowClass->LFSWM2_reloadWindowState(this->contentWindow);
	this->LFSWM2_hideWindow();
}

void LFSWM2_clientClass::LFSWM2_shadeWindow(void)
{
	if(this->isShaded==false)
				{
					this->setWindowRects(true);
					this->clientPreShade=this->frameWindowRect.h;
					XMoveWindow(this->mainClass->display,this->contentWindow,this->frameWindowRect.w+10,this->frameWindowRect.y-10000);
					XSync(this->mainClass->display,false);
					XResizeWindow(this->mainClass->display,this->frameWindow,this->frameWindowRect.w,this->mainClass->titleBarSize+this->mainClass->bottomBarSize);
					this->setWindowRects(true);
				}
			else
				{
					XResizeWindow(this->mainClass->display,this->frameWindow,this->frameWindowRect.w,this->clientPreShade);
					XMoveWindow(this->mainClass->display,this->contentWindow,this->mainClass->leftSideBarSize,this->mainClass->titleBarSize);
					this->setWindowRects(true);
				}
			this->isShaded=!this->isShaded;
}

bool LFSWM2_clientClass::wmCB(void *p,void* ud)
{
	LFSWM2_clientClass *cc=static_cast<LFSWM2_clientClass*>(ud);

	if(strcmp(static_cast<LFSTK_gadgetClass*>(p)->LFSTK_getLabel(),"Maximize")==0)
		cc->LFSWM2_maxWindow();
	if(strcmp(static_cast<LFSTK_gadgetClass*>(p)->LFSTK_getLabel(),"Minimize")==0)
		cc->LFSWM2_minWindow();
	if(strcmp(static_cast<LFSTK_gadgetClass*>(p)->LFSTK_getLabel(),"Shade")==0)
		cc->LFSWM2_shadeWindow();
	if(strcmp(static_cast<LFSTK_gadgetClass*>(p)->LFSTK_getLabel(),"Fullscreen")==0)
		{
			cc->LFSWM2_fullscreenWindow(true,true);
			cc->mainClass->mainWindowClass->LFSWM2_addState(cc->contentWindow,cc->mainClass->atomshashed.at(cc->mainClass->prefs.LFSTK_hashFromKey("_NET_WM_STATE_FULLSCREEN")));
		}
	if(strcmp(static_cast<LFSTK_gadgetClass*>(p)->LFSTK_getLabel(),"On Top")==0)
		{
			if(cc->onTop==false)
				{
					cc->mainClass->mainWindowClass->LFSWM2_addState(cc->contentWindow,cc->mainClass->atomshashed.at(cc->mainClass->prefs.LFSTK_hashFromKey("_NET_WM_STATE_ABOVE")));
					cc->mainClass->mainWindowClass->LFSWM2_removeProp(cc->contentWindow,cc->mainClass->atomshashed.at(cc->mainClass->prefs.LFSTK_hashFromKey("_NET_WM_STATE_BELOW")));
				}
			else
				cc->mainClass->mainWindowClass->LFSWM2_removeProp(cc->contentWindow,cc->mainClass->atomshashed.at(cc->mainClass->prefs.LFSTK_hashFromKey("_NET_WM_STATE_ABOVE")));
			cc->mainClass->mainEventClass->LFSWM2_restack();
		}
	if(strcmp(static_cast<LFSTK_gadgetClass*>(p)->LFSTK_getLabel(),"On Bottom")==0)
		{
			if(cc->onBottom==false)
				{
					cc->mainClass->mainWindowClass->LFSWM2_addState(cc->contentWindow,cc->mainClass->atomshashed.at(cc->mainClass->prefs.LFSTK_hashFromKey("_NET_WM_STATE_BELOW")));
					cc->mainClass->mainWindowClass->LFSWM2_removeProp(cc->contentWindow,cc->mainClass->atomshashed.at(cc->mainClass->prefs.LFSTK_hashFromKey("_NET_WM_STATE_ABOVE")));
				}
			else
				cc->mainClass->mainWindowClass->LFSWM2_removeProp(cc->contentWindow,cc->mainClass->atomshashed.at(cc->mainClass->prefs.LFSTK_hashFromKey("_NET_WM_STATE_BELOW")));
			cc->mainClass->mainEventClass->LFSWM2_restack();
		}

	if(strcmp(static_cast<LFSTK_gadgetClass*>(p)->LFSTK_getLabel(),"Close")==0)
		cc->LFSWM2_sendCloseWindow();
	if(strcmp(static_cast<LFSTK_gadgetClass*>(p)->LFSTK_getLabel(),"Fast Resize")==0)
		cc->resizeMode=FASTRESIZE;
	if(strcmp(static_cast<LFSTK_gadgetClass*>(p)->LFSTK_getLabel(),"Live Resize")==0)
		cc->resizeMode=LIVERESIZE;
	if(strcmp(static_cast<LFSTK_gadgetClass*>(p)->LFSTK_getLabel(),"Scale Resize")==0)
		cc->resizeMode=SCALERESIZE;

	if(strcmp(static_cast<LFSTK_gadgetClass*>(p)->LFSTK_getLabel(),"About")==0)
		system("lfsabout &");

	static_cast<LFSTK_gadgetClass*>(p)->wc->LFSTK_hideWindow();
	return(true);	
}

void LFSWM2_clientClass::showWMMenu(int x,int y)
{
	LFSTK_applicationClass	*apc=NULL;
	LFSTK_windowClass		*wc=NULL;
	LFSTK_menuClass			*menu=NULL;

	int			NUMMENUS=14;
	const char	*menuitemlabels[]={"Maximize","Minimize","Shade","Fullscreen","--","On Top","On Bottom","--","Fast Resize","Live Resize","Scale Resize","Close","--","About"};

	apc=new LFSTK_applicationClass();
	apc->LFSTK_addWindow(NULL,"");
	wc=apc->mainWindow;

	menu=new LFSTK_menuClass(wc,x,y,1,1);

	wc->LFSTK_resizeWindow(1,1,true);
	wc->LFSTK_hideWindow();

	menuStruct	**mms=new menuStruct*[NUMMENUS];
	for (int j=0; j<NUMMENUS; j++)
		{
			mms[j]=new menuStruct;
			asprintf(&mms[j]->label,"%s",menuitemlabels[j]);
			mms[j]->hasSubMenu=false;
			mms[j]->subMenus=NULL;
			mms[j]->userData=(void*)this;
			mms[j]->imageType=NOTHUMB;
		}

	menu->LFSTK_setMouseCallBack(NULL,wmCB,(void*)0x888);
	menu->LFSTK_addMainMenus(mms,NUMMENUS);

	menu->LFSTK_showMenu();

	delete menu;
	delete apc;
#ifdef __DEBUG__
	cairo_debug_reset_static_data();
#endif
	return;
}

bool LFSWM2_clientClass::LFSWM2_handleControls(XEvent *e)
{
	bool					retval=false;
	Pixmap				pm=0;
	struct controlData	data;

	if((this->isShaded==false) && ((e->xany.window==this->topLeftDragger) || (e->xany.window==this->bottomRightDragger) || (e->xany.window==this->topRightDragger) || (e->xany.window==this->bottomLeftDragger)|| (e->xany.window==this->bottomDragger)|| (e->xany.window==this->leftSideDragger)|| (e->xany.window==this->rightSideDragger)))
		{
			retval=this->doResizeDraggers(e);
			return(true);
		}

	if((e->xany.window!=this->closeButton) && (e->xany.window!=this->maximizeButton) && (e->xany.window!=this->minimizeButton) && (e->xany.window!=this->shadeButton) && (e->xany.window!=this->menuButton))
		return(false);

	if(e->xany.window==this->closeButton)
		{
			pm=this->mainClass->closeBitMap;
			data=this->closeControlStruct;
		}
	if(e->xany.window==this->maximizeButton)
		{
			pm=this->mainClass->maximizeBitMap;
			data=this->maximizeControlStruct;
		}
	if(e->xany.window==this->minimizeButton)
		{
			pm=this->mainClass->minimizeBitMap;
			data=this->minimizeControlStruct;
		}
	if(e->xany.window==this->shadeButton)
		{
			pm=this->mainClass->shadeBitMap;
			data=this->shadeControlStruct;
		}
	if(e->xany.window==this->menuButton)
		{
			pm=this->mainClass->menuBitMap;
			data=this->menuControlStruct;
		}

	retval=false;
	switch(e->type)
		{
			case Expose:
				//fprintf(stderr,"Expose clientClass.cpp\n");
				break;
			case EnterNotify:
				this->inControl=true;
				this->drawMouseEnter(e->xany.window,pm,data);
				break;
			case LeaveNotify:
				this->inControl=false;
				this->LFSWM2_drawMouseLeave(e->xany.window,pm,data);
				break;
			case ButtonPress:
				this->drawMousePressed(e->xany.window,pm,data);
				break;
			case ButtonRelease:
				if(this->inControl==false)
					break;
				this->LFSWM2_drawMouseLeave(e->xany.window,pm,data);

				if(e->xbutton.window==this->menuButton)
					{
						this->showWMMenu(this->frameWindowRect.x+data.controlRect.x,this->frameWindowRect.y+data.controlRect.y+data.controlRect.h);
						break;
					}

				if(e->xbutton.window==this->closeButton)
					{
						this->LFSWM2_sendCloseWindow();
						retval=true;
					}

				if(e->xbutton.window==this->maximizeButton)
					{
						this->LFSWM2_maxWindow();
						retval=true;
					}

				if(e->xbutton.window==this->minimizeButton)
					{
						this->LFSWM2_minWindow();
						retval=true;
					}

				if(e->xbutton.window==this->shadeButton)
					{
						this->LFSWM2_shadeWindow();
						retval=true;
					}

				break;
			default:
				retval=false;
		}

	return(retval);
}

void LFSWM2_clientClass::LFSWM2_fullscreenWindow(bool isfull,bool force)
{
	rectStruct	r;

	if((isfull==this->isFullscreen) && (force==false))
		return;

	if(isfull==true)
		{
			this->setWindowRects();
			pointStruct	fp={this->frameWindowRect.x,this->frameWindowRect.y};
			for(int j=0;j<this->mainClass->numberOfMonitors;j++)
				{
					geometryStruct	mg={this->mainClass->monitors.at(j).x,this->mainClass->monitors.at(j).y,(unsigned)this->mainClass->monitors.at(j).w,(unsigned)this->mainClass->monitors.at(j).h,0};
					if(this->mainClass->lfstkLib->LFSTK_pointInRect(&fp,&mg))
						{
							this->setWindowRects();
							this->clientPreFSRect=this->frameWindowRect;
							this->mainClass->mainWindowClass->LFSWM2_addState(this->contentWindow,this->mainClass->atomshashed.at(this->mainClass->prefs.LFSTK_hashFromKey("_NET_WM_STATE_FULLSCREEN")));
							XMoveResizeWindow(this->mainClass->display,this->frameWindow,mg.x,mg.y,mg.w,mg.h);
							XMoveResizeWindow(this->mainClass->display,this->contentWindow,0,0,mg.w,mg.h);
						}
				}
		}
	else
		{
			this->mainClass->mainWindowClass->LFSWM2_removeProp(this->contentWindow,this->mainClass->atomshashed.at(this->mainClass->prefs.LFSTK_hashFromKey("_NET_WM_STATE_FULLSCREEN")));
			XMoveResizeWindow(this->mainClass->display,this->frameWindow,this->clientPreFSRect.x,this->clientPreFSRect.y,this->clientPreFSRect.w,this->clientPreFSRect.h);
			XMoveResizeWindow(this->mainClass->display,this->contentWindow,this->mainClass->leftSideBarSize,this->mainClass->titleBarSize,this->clientPreFSRect.w-(this->mainClass->leftSideBarSize+this->mainClass->riteSideBarSize),this->clientPreFSRect.h-this->mainClass->bottomBarSize-this->mainClass->titleBarSize);
		}

	this->isFullscreen=isfull;
	this->setWindowRects();
	this->mainClass->mainWindowClass->LFSWM2_setControlRects(this);
}

void LFSWM2_clientClass::LFSWM2_maxWindow(bool ismaxed,bool force)
{
	int	monnum=0;

	if((ismaxed==this->isMaximized) && (force==false))
		return;

	if(ismaxed==true)
		{
			pointStruct	fp={this->frameWindowRect.x,this->frameWindowRect.y};
			for(int j=0;j<this->mainClass->numberOfMonitors;j++)
				{
					geometryStruct	mg={this->mainClass->monitors.at(j).x,this->mainClass->monitors.at(j).y,(unsigned)this->mainClass->monitors.at(j).w,(unsigned)this->mainClass->monitors.at(j).h,0};
					if(this->mainClass->lfstkLib->LFSTK_pointInRect(&fp,&mg))
						{
							monnum=j;
							break;
						}
				}
			this->framePreMaxRect=this->mainClass->mainWindowClass->LFSWM2_getWindowRect(this->frameWindow,this->mainClass->rootWindow,false);
			this->clientPreMaxRect=this->mainClass->mainWindowClass->LFSWM2_getWindowRect(this->contentWindow,this->mainClass->rootWindow,false);
			rectStruct rr={this->mainClass->monitors.at(monnum).x,this->mainClass->monitors.at(monnum).y,(int)this->mainClass->monitors.at(monnum).w,(int)this->mainClass->monitors.at(monnum).h};
			rr.w=this->mainClass->monitors.at(monnum).w-((this->mainClass->leftSideBarSize+BORDER_WIDTH+this->mainClass->riteSideBarSize+BORDER_WIDTH));//TODO//
			rr.h=this->mainClass->monitors.at(monnum).h-(this->mainClass->titleBarSize+this->mainClass->bottomBarSize+(BORDER_WIDTH*2));
			rr.x=this->mainClass->leftSideBarSize;
			rr.y=this->mainClass->titleBarSize;

			XMoveResizeWindow(this->mainClass->display,this->frameWindow,this->mainClass->monitors.at(monnum).x,this->mainClass->monitors.at(monnum).y,this->mainClass->monitors.at(monnum).w,this->mainClass->monitors.at(monnum).h);
			XMoveResizeWindow(this->mainClass->display,this->contentWindow,rr.x,rr.y,rr.w,rr.h);
		}
	else
		{
			XMoveResizeWindow(this->mainClass->display,this->frameWindow,this->framePreMaxRect.x,this->framePreMaxRect.y,this->framePreMaxRect.w,this->framePreMaxRect.h);
			XMoveResizeWindow(this->mainClass->display,this->contentWindow,this->clientPreMaxRect.x,this->clientPreMaxRect.y,this->clientPreMaxRect.w,this->clientPreMaxRect.h);
		}
	this->isMaximized=ismaxed;
	this->setWindowRects();
}

void LFSWM2_clientClass::setWindowRects(bool resize)
{
	if(this->isBorderless==true)
		return;

	this->contentWindowRect=this->mainClass->mainWindowClass->LFSWM2_getWindowRect(this->contentWindow,this->mainClass->rootWindow);
	this->frameWindowRect=this->mainClass->mainWindowClass->LFSWM2_getWindowRect(this->frameWindow,this->mainClass->rootWindow);

	if(resize==true)
		this->LFSWM2_resizeControls();
}

void LFSWM2_clientClass::LFSWM2_showWindow(bool checkstate)
{
	if((checkstate==true) && ((this->isWithdrawn==true) || (this->isHidden==true)))
		return;

	this->visible=true;
	if(this->isWithdrawn==true)
		this->mainClass->mainWindowClass->LFSWM2_setWindowState(this->contentWindow,NormalState);
	this->isWithdrawn=false;
	this->isHidden=false;
	XMapWindow(this->mainClass->display,this->frameWindow);
}

void LFSWM2_clientClass::LFSWM2_hideWindow(bool withdraw)
{
	this->visible=false;
	if(withdraw==true)
		{
			this->holdFrameRect=this->frameWindowRect;
			this->isWithdrawn=true;
			this->mainClass->mainWindowClass->LFSWM2_setWindowState(this->contentWindow,WithdrawnState);
		}
	XUnmapWindow(this->mainClass->display,this->frameWindow);
}

//void LFSWM2_clientClass::LFSWM2_showWindow(void)
//{
//	XMapWindow(this->mainClass->display,this->frameWindow);
//}
//
//void LFSWM2_clientClass::LFSWM2_hideWindow(void)
//{
//	XUnmapWindow(this->mainClass->display,this->frameWindow);
//}

void LFSWM2_clientClass::LFSWM2_resizeControls(void)
{
	if(this->isBorderless==true)
		return;

	XResizeWindow(this->mainClass->display,this->bottomDragger,this->frameWindowRect.w,this->dragsize);
	XResizeWindow(this->mainClass->display,this->leftSideDragger,this->dragsize,this->frameWindowRect.h-(this->dragsize*2));
	XResizeWindow(this->mainClass->display,this->rightSideDragger,this->dragsize,this->frameWindowRect.h-(this->dragsize*2));
}

void LFSWM2_clientClass::LFSWM2_setWMState(XEvent *e)
{
	long unsigned int	n;
	Atom					*states;

	this->onBottom=false;
	this->onTop=false;
	this->visibleOnAllDesks=false;

	states=(Atom*)this->mainClass->mainWindowClass->LFSWM2_getFullProp(e->xproperty.window,this->mainClass->atomshashed.at(this->mainClass->prefs.LFSTK_hashFromKey("_NET_WM_STATE")),XA_ATOM,32,&n);

	if(n>0)
		{
			for(long unsigned int g=0;g<n;g++)
				{
					//this->mainClass->DEBUG_printAtom(states[g]);
					this->mainClass->mainWindowClass->LFSWM2_changeState(e->xproperty.window,NET_WM_STATE_ADD,states[g]);
				}
		}

	if(!this->mainClass->mainWindowClass->LFSWM2_hasState(e->xproperty.window,this->mainClass->atomshashed.at(this->mainClass->prefs.LFSTK_hashFromKey("_NET_WM_STATE_HIDDEN"))))
		{
			this->visible=true;
			this->LFSWM2_showWindow();
		}

	if(this->mainClass->mainWindowClass->LFSWM2_hasState(e->xproperty.window,this->mainClass->atomshashed.at(this->mainClass->prefs.LFSTK_hashFromKey("_NET_WM_STATE_STICKY"))))
		{
			this->visibleOnAllDesks=true;
		}

	if(this->mainClass->mainWindowClass->LFSWM2_hasState(e->xproperty.window,this->mainClass->atomshashed.at(this->mainClass->prefs.LFSTK_hashFromKey("_NET_WM_STATE_BELOW"))))
		{
			this->onBottom=true;
			this->onTop=false;
			XLowerWindow(this->mainClass->display,this->contentWindow);
			this->mainClass->restackCnt++;
		}

	if(this->mainClass->mainWindowClass->LFSWM2_hasState(e->xproperty.window,this->mainClass->atomshashed.at(this->mainClass->prefs.LFSTK_hashFromKey("_NET_WM_STATE_ABOVE"))))
		{
			this->onBottom=false;
			this->onTop=true;
			XRaiseWindow(this->mainClass->display,this->contentWindow);
			this->mainClass->mainWindowClass->LFSWM2_setProp(this->mainClass->rootWindow,this->mainClass->atomshashed.at(this->mainClass->prefs.LFSTK_hashFromKey("_NET_ACTIVE_WINDOW")),XA_WINDOW,32,&this->contentWindow,1);
			this->mainClass->restackCnt++;
		}
	if(states!=NULL)
		XFree(states);
	this->mainClass->mainEventClass->noRestack=false;
}

bool LFSWM2_clientClass::LFSWM2_doFrameMoveEvents(XEvent *e)
{
	LFSWM2_clientClass	*cc;
	int					lastx=0;
	int					direction=-1;
	int					lasttime=0;

	if(this->isBorderless==true)
		return(true);

	while(true)
		{
			XEvent ee;
			sleep(0.1);
			XNextEvent(this->mainClass->display,&ee);

			switch (ee.type)
				{
					case Expose:
					//fprintf(stderr,"Expose clientClass.cpp\n");
						if(ee.xexpose.count>0)
							break;
						cc=this->mainClass->mainWindowClass->LFSWM2_getClientClass(ee.xexpose.window);
						if(cc!=NULL)
							cc->mainClass->mainWindowClass->LFSWM2_refreshFrame(cc,(XExposeEvent*)&ee);
						break;
					case MotionNotify:
						{
						//fprintf(stderr,"MotionNotify clientClass.cpp\n");
							if(lastx<ee.xbutton.x_root)
								direction=1;
							else
								direction=0;
							
							int xdiff=ee.xbutton.x_root-e->xbutton.x_root;
							int ydiff=ee.xbutton.y_root-e->xbutton.y_root;
							if(ee.xmotion.time>lasttime+SKIPTIME)
								{
									lasttime=ee.xmotion.time;
									XMoveWindow(this->mainClass->display,this->frameWindow,this->frameWindowRect.x+xdiff,this->frameWindowRect.y+ydiff);
								}
	
							if((ee.xbutton.x_root<20) && (direction==0))
								{
									this->onDesk=this->mainClass->LFSWM2_getLowerDesktop(this->onDesk);
									this->mainClass->LFSWM2_setCurrentDesktop(this->onDesk);
									XWarpPointer(this->mainClass->display,None,None,0,0,0,0,this->mainClass->displayWidth-30,0);
									direction=-1;
									ee.xbutton.x_root=100000;
									this->mainClass->mainWindowClass->LFSWM2_setProp(this->contentWindow,this->mainClass->atomshashed.at(this->mainClass->prefs.LFSTK_hashFromKey("_NET_CURRENT_DESKTOP")),XA_CARDINAL,32,&this->onDesk,1);
								}
							else 	if((ee.xbutton.x_root>this->mainClass->displayWidth-20) && (direction==1))
								{
									this->onDesk=this->mainClass->LFSWM2_getHigherDesktop(this->onDesk);
									this->mainClass->LFSWM2_setCurrentDesktop(this->onDesk);
									XWarpPointer(this->mainClass->display,None,None,0,0,0,0,(-this->mainClass->displayWidth-30),0);
									ee.xbutton.x_root=-1;
									this->mainClass->mainWindowClass->LFSWM2_setProp(this->contentWindow,this->mainClass->atomshashed.at(this->mainClass->prefs.LFSTK_hashFromKey("_NET_CURRENT_DESKTOP")),XA_CARDINAL,32,&this->onDesk,1);
								}
						}
						lastx=ee.xbutton.x_root;
						break;
						
					case ButtonPress:
						this->mainClass->mainEventClass->noRestack=true;
						break;
					case ButtonRelease:
						{
							if(this->isShaded==true)
								return(true);

							this->adjustContentWindow();
							this->mainClass->mainEventClass->noRestack=false;
							return(true);
						}
						break;
				}
		}
	return(true);
}

rectStruct LFSWM2_clientClass::setTitlePosition(void)
{
	rectStruct	r=this->frameWindowRect;
	XGlyphInfo	extents;
	int			namewidth;
	int			offset=this->controlCnt*CONTROL_GAP;
	if(this->isBorderless==true)
		return(r);

	if(this->nameIsUTF==true)
		XftTextExtentsUtf8(this->mainClass->display,this->mainClass->frameFont,(XftChar8*)this->name.c_str(),this->name.length(),&extents);	
	else
		XftTextExtents8(this->mainClass->display,this->mainClass->frameFont,(XftChar8*)this->name.c_str(),this->name.length(),&extents);

	r.y=(extents.y/2)+(this->mainClass->titleBarSize/2);
	namewidth=extents.width-extents.x;
	switch(this->mainClass->titlePosition)
		{
			case 1:
				r.x=CONTROL_GAP;
				break;
			case 2:
				r.x=((r.w-offset)/2)-(namewidth/2);
				break;
			case 3:
				r.x=r.w-namewidth-offset;
				break;
		}
	r.w=namewidth;
	r.h=extents.height;
	return(r);
}

bool LFSWM2_clientClass::LFSWM2_handleEvents(XEvent *e)
{
//this->mainClass->DEBUG_printEventData(e,true);
	switch(e->type)
		{
			case ButtonRelease:
				this->adjustContentWindow();
				break;

			case MotionNotify:
				{
				//std::cerr<<"MotionNotify"<<std::endl;
					bool domove=false;

					if((this->frameWindow==e->xmotion.window) && (e->xmotion.state==Button1Mask))
						domove=true;
					if((this->contentWindow==e->xmotion.window) && (e->xmotion.state==Button1Mask+(this->mainClass->modKeys)))
						domove=!this->isFullscreen;

					if(domove==true)
						{
							this->mainClass->mainEventClass->noRestack=true;
							this->LFSWM2_doFrameMoveEvents(e);
							this->mainClass->restackCnt=2;
							return(true);
						}
					break;
				}

			case Expose:
				{
				//fprintf(stderr,"expose\n");
					if(e->xexpose.count!=0)
						break;
					else
						{
							this->mainClass->mainWindowClass->LFSWM2_refreshFrame(this,(XExposeEvent*)e);
							return(true);
						}
				}
				break;

			case MapNotify:
				{
					//std::cerr<<"client MapNotify"<<std::endl;
				}
				break;
			case UnmapNotify:
				{
				//fprintf(stderr,"UnmapNotify from client\n");
					this->LFSWM2_hideWindow(true);
					return(true);
				}
				break;

			case MapRequest:
				//std::cerr<<"client MapRequest"<<std::endl;
				this->LFSWM2_showWindow(false);
				break;

			case DestroyNotify:
				{
				//fprintf(stderr,"DestroyNotify from client\n");
					bool					loop=false;
					LFSWM2_clientClass	*cc;
					do
						{
							loop=false;
contloop:
							for(auto& x:this->mainClass->mainWindowClass->clientList)
								{
				    					cc=this->mainClass->mainWindowClass->LFSWM2_getClientClass(x.first);
									if((cc!=NULL) && (cc->transientFor==this->contentWindow))
										{
											if(cc->frameWindow!=None)
				   								XUnmapWindow(this->mainClass->display,cc->frameWindow);
				   							cc->LFSWM2_sendCloseWindow();
				   							delete cc;
				   							goto contloop;
   										}
 	 							}
						}
					while(loop==true);
					delete this;
					return(true);
				}
				break;

			case ConfigureNotify:
				//std::cerr<<">>>>>>>>>>>>>>>>>client ConfigureNotify"<<std::endl;
				//fprintf(stderr,"e->xconfigurerequest.y=%i\n",e->xconfigurerequest.y);
				break;

			case ConfigureRequest:
				//fprintf(stderr,"isframed=%s",this->mainClass->DEBUG_printBool(this->rendered));
				//fprintf(stderr,">>>>>>>>>>>>>>>>>>>>>e->xconfigurerequest.y=%i\n",e->xconfigurerequest.y);
				if(this->isBorderless==true)
					return(false);
				if(this->holdFrameRect.w==-1)
					return(false);
				this->frameWindowRect=this->holdFrameRect;
				XMoveWindow(this->mainClass->display,this->frameWindow,this->frameWindowRect.x,this->frameWindowRect.y);
				XRaiseWindow(this->mainClass->display,this->frameWindow);
				this->mainClass->mainWindowClass->LFSWM2_setProp(this->mainClass->rootWindow,this->mainClass->atomshashed.at(this->mainClass->prefs.LFSTK_hashFromKey("_NET_ACTIVE_WINDOW")),XA_WINDOW,32,&this->contentWindow,1);
				XSetInputFocus(this->mainClass->display,this->contentWindow,RevertToNone,CurrentTime);
				break;
			default:
				return(false);
		}
	return(false);
}

void LFSWM2_clientClass::resizeContentWindow(rectStruct r,bool moveorigin)
{
	XConfigureRequestEvent	ce;

	ce.display=this->mainClass->display;
	ce.parent=this->frameWindow;
	ce.window=this->contentWindow;
	ce.type=ConfigureRequest;
	ce.serial=0;
	ce.send_event=True;
	ce.border_width=BORDER_WIDTH;//TODO//

	if(moveorigin==true)
		ce.value_mask=CWWidth|CWHeight|CWX|CWY;
	else
		ce.value_mask=CWWidth|CWHeight;

	ce.detail=Above;
	ce.above=None;
	ce.x=r.x;
	ce.y=r.y;
	ce.width=r.w;
	ce.height=r.h;

	XSendEvent(this->mainClass->display,this->contentWindow,true,StructureNotifyMask,(XEvent*)&ce);
}

void LFSWM2_clientClass::renderFrame(bool isfirst,int x,int y)
{
	if(isfirst==true)
		{
			this->firstx=x;
			this->firsty=y;
		}
	else
		{
			XMapWindow(this->mainClass->display,this->contentWindow);
			XMapSubwindows(this->mainClass->display,this->contentWindow);	
			this->setWindowRects(true);
			this->rendered=true;
			this->mainClass->mainWindowClass->LFSWM2_setProp(this->mainClass->rootWindow,this->mainClass->atomshashed.at(this->mainClass->prefs.LFSTK_hashFromKey("_NET_ACTIVE_WINDOW")),XA_WINDOW,32,(void*)&this->contentWindow,1);
			this->mainClass->restackCnt=0;
			if(this->isBorderless==true)
				{
					XMoveWindow(this->mainClass->display,this->frameWindow,this->firstx+this->mainClass->leftSideBarSize,this->firsty+this->mainClass->titleBarSize);
					XMoveWindow(this->mainClass->display,this->contentWindow,0,0);
					return;
				}

			XMoveWindow(this->mainClass->display,this->frameWindow,this->firstx,this->firsty);
			XMapSubwindows(this->mainClass->display,this->frameWindow);	
			this->mainClass->mainWindowClass->LFSWM2_reloadWindowState(this->contentWindow);
			if(this->mainClass->useTheme==true)
				this->mainClass->mainWindowClass->LFSWM2_refreshThemeFrame(this);
		}
}

void LFSWM2_clientClass::LFSWM2_setFrameExtents(void)
{
	if(this->isBorderless==false)
		{
			unsigned long v[10]={(unsigned long)this->mainClass->leftSideBarSize,(unsigned long)this->mainClass->riteSideBarSize,(unsigned long)this->mainClass->titleBarSize,(unsigned long)this->mainClass->bottomBarSize,0,0,0,0,0};
			this->mainClass->mainWindowClass->LFSWM2_setProp(this->contentWindow,this->mainClass->atomshashed.at(this->mainClass->prefs.LFSTK_hashFromKey("_NET_FRAME_EXTENTS")),XA_CARDINAL,32,&v,4);
		}
	else
		{
			unsigned long v[10]={0,0,0,0,0,0,0,0,0};
			this->mainClass->mainWindowClass->LFSWM2_setProp(this->contentWindow,this->mainClass->atomshashed.at(this->mainClass->prefs.LFSTK_hashFromKey("_NET_FRAME_EXTENTS")),XA_CARDINAL,32,&v,4);
		}
}
