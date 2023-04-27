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
	this->mainClass->LFSWM2_pushXErrorHandler();
		this->mainClass->mainWindowClass->LFSWM2_setClientList(this->contentWindow,false);

		this->mainClass->mainWindowClass->LFSWM2_deleteClientEntry(this->frameWindow);
		this->mainClass->mainWindowClass->LFSWM2_deleteClientEntry(this->contentWindow);

		XRemoveFromSaveSet(this->mainClass->display,this->contentWindow);	
		if(this->transientFor==None)
			XReparentWindow(this->mainClass->display,this->contentWindow,this->mainClass->rootWindow,this->frameWindowRect.x+this->mainClass->leftSideBarSize,frameWindowRect.y+this->mainClass->titleBarSize);
		else
			XReparentWindow(this->mainClass->display,this->contentWindow,this->mainClass->rootWindow,-10000,-10000);
		XUnmapWindow(this->mainClass->display,this->frameWindow);

		if(this->sizeHints!=NULL)
			XFree(this->sizeHints);
		if(this->mwmHints!=NULL)
			XFree(this->mwmHints);

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

	if(this->maskGC!=None)
		XFreeGC(this->mainClass->display,this->maskGC);

	this->mainClass->LFSWM2_popXErrorHandler();
}

void LFSWM2_clientClass::LFSWM2_setWindowName(void)
{
	char					*namex=NULL;
	XTextProperty		p;
	long unsigned int	nitems_return;

	this->name="";
	this->mainClass->LFSWM2_pushXErrorHandler();

	namex=(char*)this->mainClass->mainWindowClass->LFSWM2_getProp(this->contentWindow,this->mainClass->atoms.at("_NET_WM_NAME"),this->mainClass->atoms.at("UTF8_STRING"),&nitems_return);
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

void LFSWM2_clientClass::drawMouseEnter(Window id,Pixmap pm,controlData data)
{
	std::string cn=data.controlName+"-prelight";
	if(this->isActive==false)
		cn=data.controlName+"-inactive";

	if(this->mainClass->useTheme==true)
		{
			XSetClipMask(this->mainClass->display,this->mainClass->mainGC,this->mainClass->mainWindowClass->theme.masks[this->mainClass->prefs.LFSTK_hashFromKey(cn)]);
			XSetClipOrigin(this->mainClass->display,this->mainClass->mainGC,0,0);
			XCopyArea(this->mainClass->display,this->mainClass->mainWindowClass->theme.pixmaps[this->mainClass->prefs.LFSTK_hashFromKey(cn)],id,this->mainClass->mainGC,0,0,this->mainClass->mainWindowClass->theme.partsWidth[this->mainClass->prefs.LFSTK_hashFromKey(cn)],this->mainClass->mainWindowClass->theme.partsHeight[this->mainClass->prefs.LFSTK_hashFromKey(cn)],0,0);
			XShapeCombineMask(this->mainClass->display,id,ShapeBounding,0,0,this->mainClass->mainWindowClass->theme.masks[this->mainClass->prefs.LFSTK_hashFromKey(cn)],ShapeSet);
			return;
		}

	XSetForeground(this->mainClass->display,data.controlGC,this->mainClass->frameFG->pixel);
	XSetClipMask(this->mainClass->display,data.controlGC,None);
	XSetLineAttributes(this->mainClass->display,data.controlGC,2,LineSolid,CapNotLast,JoinMiter);
	XDrawRectangle(this->mainClass->display,id,data.controlGC,1,1,defaultControlSize-2,defaultControlSize-2);
}

void LFSWM2_clientClass::LFSWM2_drawMouseLeave(Window id,Pixmap pm,controlData data)
{
	std::string cn=data.controlName+"-active";
	if(this->isActive==false)
		cn=data.controlName+"-inactive";

	if(this->mainClass->useTheme==true)
		{
			XSetClipMask(this->mainClass->display,this->mainClass->mainGC,this->mainClass->mainWindowClass->theme.masks[this->mainClass->prefs.LFSTK_hashFromKey(cn)]);
			XSetClipOrigin(this->mainClass->display,this->mainClass->mainGC,0,0);
			XCopyArea(this->mainClass->display,this->mainClass->mainWindowClass->theme.pixmaps[this->mainClass->prefs.LFSTK_hashFromKey(cn)],id,this->mainClass->mainGC,0,0,this->mainClass->mainWindowClass->theme.partsWidth[this->mainClass->prefs.LFSTK_hashFromKey(cn)],this->mainClass->mainWindowClass->theme.partsHeight[this->mainClass->prefs.LFSTK_hashFromKey(cn)],0,0);
			XShapeCombineMask(this->mainClass->display,id,ShapeBounding,0,0,this->mainClass->mainWindowClass->theme.masks[this->mainClass->prefs.LFSTK_hashFromKey(cn)],ShapeSet);
			return;
		}

//maximizeBitMap
	XSetForeground(this->mainClass->display,data.controlGC,this->mainClass->frameBG->pixel);
	XSetClipMask(this->mainClass->display,data.controlGC,None);
	XFillRectangle(this->mainClass->display,id,data.controlGC,0,0,defaultControlSize,defaultControlSize);

  	XSetForeground(this->mainClass->display,data.controlGC,this->mainClass->frameText->pixel);
	XSetClipOrigin(this->mainClass->display,data.controlGC,defaultControlXYOffset,defaultControlXYOffset);
	XSetClipMask(this->mainClass->display,data.controlGC,pm);
	XFillRectangle(this->mainClass->display,id,data.controlGC,defaultControlXYOffset,defaultControlXYOffset,defaultControlBitmapSize,defaultControlBitmapSize);
}

void LFSWM2_clientClass::LFSWM2_sendCloseWindow(void)
{
	XClientMessageEvent	em;

	em.type=ClientMessage;
	em.window=this->contentWindow;
	em.message_type=this->mainClass->atoms.at("WM_PROTOCOLS");
	em.format=32;
	em.data.l[0]=this->mainClass->atoms.at("WM_DELETE_WINDOW");;
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
	XMoveResizeWindow(this->mainClass->display,this->contentWindow,this->mainClass->leftSideBarSize,this->mainClass->titleBarSize,this->frameWindowRect.w-this->mainClass->leftSideBarSize-this->mainClass->riteSideBarSize,this->frameWindowRect.h-this->mainClass->titleBarSize-this->mainClass->bottomBarSize);
}

bool LFSWM2_clientClass::doResizeDraggers(XEvent *e)
{
	rectStruct				r;
	XSizeHints				xh;
	long						dummy;
	int						contenthadjust=this->mainClass->titleBarSize+this->mainClass->bottomBarSize;
	int						contentwadjust=this->mainClass->leftSideBarSize+this->mainClass->riteSideBarSize;

	switch(e->type)
		{
			case ButtonPress:
				{
					this->LFSWM2_unSpecial();
					this->buttonDown=true;
					this->sx=e->xmotion.x;
					this->sy=e->xmotion.y;
					steps=0;
					this->setWindowRects(false);

					switch(this->mainClass->resizeMode)
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
									this->resizeImage=XGetImage(this->mainClass->display,this->contentWindow,0,0,this->contentWindowRect.w,this->contentWindowRect.h, AllPlanes, ZPixmap);

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

				this->buttonDown=false;
				this->sx=0;
				this->sy=0;
				this->mainClass->doingMove=false;
				this->isShaded=false;

				this->setWindowRects(true);
				this->resetContentWindow();

				if(currentRootPixmap!=None)
					XFreePixmap(this->mainClass->display,this->currentRootPixmap);
				if(primaryPicture!=None)
					XRenderFreePicture(this->mainClass->display,this->primaryPicture);
				if(windowPicture!=None)
					XRenderFreePicture(this->mainClass->display,this->windowPicture);
				if(resizeImage!=NULL)
					XDestroyImage(this->resizeImage);

				XMoveResizeWindow(this->mainClass->display,this->resizeWindow,this->mainClass->displayWidth+10,0,1,1);
				break;

			case MotionNotify:
				if(buttonDown==true)
					{
						this->mainClass->doingMove=true;
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
										r.w=this->frameWindowRect.w-(e->xmotion.x-this->sx);
										r.h=this->frameWindowRect.h-(e->xmotion.y-this->sy);
									}
								if(e->xany.window==this->topRightDragger)
									{
										r.y=e->xmotion.y_root-this->sy;
										r.w=this->frameWindowRect.w+(e->xmotion.x-this->sx);
										r.h=this->frameWindowRect.h-(e->xmotion.y-this->sy);
									}
								if(e->xany.window==this->bottomLeftDragger)
									{
										r.x=e->xmotion.x_root-this->sx;
										r.w=this->frameWindowRect.w-(e->xmotion.x-this->sx);
										r.h=this->frameWindowRect.h+(e->xmotion.y-this->sy);
									}
								if(e->xany.window==this->bottomRightDragger)
									{
										r.w=this->frameWindowRect.w+(e->xmotion.x-this->sx);
										r.h=this->frameWindowRect.h+(e->xmotion.y-this->sy);
									}

								if(e->xany.window==this->bottomDragger)
									{
										r.h=this->frameWindowRect.h+(e->xmotion.y-this->sy);
									}

								if(e->xany.window==this->leftSideDragger)
									{
										r.x=e->xmotion.x_root-this->sx;
										r.w=this->frameWindowRect.w-(e->xmotion.x-this->sx);
									}
								if(e->xany.window==this->rightSideDragger)
									{
										r.w=this->frameWindowRect.w+(e->xmotion.x-this->sx);
									}

								XGetWMNormalHints(this->mainClass->display,this->contentWindow,&xh,&dummy);
								if(r.w-contentwadjust<xh.min_width)
									break;
								if(r.h-contenthadjust<xh.min_height)
									break;

								this->frameWindowRect=r;

								XMoveResizeWindow(this->mainClass->display,this->frameWindow,r.x,r.y,r.w,r.h);
								if(this->mainClass->resizeMode==SCALERESIZE)
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

								if(this->mainClass->resizeMode==LIVERESIZE)
									{
										this->contentWindowRect={this->mainClass->leftSideBarSize,this->mainClass->titleBarSize,r.w-contentwadjust,r.h-contenthadjust};
										this->frameWindowRect={r.x,r.y,r.w,r.h};
										XResizeWindow(this->mainClass->display,this->contentWindow,r.w-contentwadjust,r.h-contenthadjust);
										//XResizeWindow(this->mainClass->display,this->contentWindow,r.w,r.h);
										//XSync(this->mainClass->display,false);
									}
								else
									{
										XMoveWindow(this->mainClass->display,this->contentWindow,this->frameWindowRect.w+10,0);
										//XSync(this->mainClass->display,false);
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
			this->mainClass->mainWindowClass->LFSWM2_removeProp(this->contentWindow,this->mainClass->atoms.at("_NET_WM_STATE_MAXIMIZED_HORZ"));
			this->mainClass->mainWindowClass->LFSWM2_removeProp(this->contentWindow,this->mainClass->atoms.at("_NET_WM_STATE_MAXIMIZED_VERT"));
			this->mainClass->mainWindowClass->LFSWM2_removeProp(this->contentWindow,this->mainClass->atoms.at("_NET_WM_STATE_HIDDEN"));
			this->isMaximized=false;
		}
	this->isShaded=false;
	this->mainClass->mainWindowClass->LFSWM2_reloadWindowState(this->contentWindow);
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

	if((e->xany.window!=this->closeButton) && (e->xany.window!=this->maximizeButton) && (e->xany.window!=this->minimizeButton) && (e->xany.window!=this->shadeButton))
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

	retval=false;
	switch(e->type)
		{
			case Expose:
				fprintf(stderr,"Expose\n");
				break;
			case EnterNotify:
				this->drawMouseEnter(e->xany.window,pm,data);
				break;
			case LeaveNotify:
				this->LFSWM2_drawMouseLeave(e->xany.window,pm,data);
				break;
			case ButtonPress:
				if(e->xbutton.window==this->closeButton)
					{
						this->LFSWM2_sendCloseWindow();
						retval=true;
					}

				if(e->xbutton.window==this->maximizeButton)
					{
						if(this->isMaximized==true)
							{
								this->mainClass->mainWindowClass->LFSWM2_removeProp(this->contentWindow,this->mainClass->atoms.at("_NET_WM_STATE_MAXIMIZED_HORZ"));
								this->mainClass->mainWindowClass->LFSWM2_removeProp(this->contentWindow,this->mainClass->atoms.at("_NET_WM_STATE_MAXIMIZED_VERT"));
							}
						else
							{
								this->mainClass->mainWindowClass->LFSWM2_addState(this->contentWindow,this->mainClass->atoms.at("_NET_WM_STATE_MAXIMIZED_HORZ"));
								this->mainClass->mainWindowClass->LFSWM2_addState(this->contentWindow,this->mainClass->atoms.at("_NET_WM_STATE_MAXIMIZED_VERT"));
							}
						this->mainClass->mainWindowClass->LFSWM2_reloadWindowState(this->contentWindow);
						retval=true;
					}

				if(e->xbutton.window==this->minimizeButton)
					{
						this->mainClass->mainWindowClass->LFSWM2_addState(this->contentWindow,this->mainClass->atoms.at("_NET_WM_STATE_HIDDEN"));
						this->mainClass->mainWindowClass->LFSWM2_reloadWindowState(this->contentWindow);
						this->LFSWM2_hideWindow();
						retval=true;
					}

				if(e->xbutton.window==this->shadeButton)
					{
						if(this->isShaded==false)
							{
								this->setWindowRects(true);
								this->clientPreShade=this->frameWindowRect.h;
								XMoveWindow(this->mainClass->display,this->contentWindow,this->frameWindowRect.w+10,this->frameWindowRect.y);
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
							XMoveResizeWindow(this->mainClass->display,this->frameWindow,mg.x,mg.y,mg.w,mg.h);
							XMoveResizeWindow(this->mainClass->display,this->contentWindow,0,0,mg.w,mg.h);
						}
				}
		}
	else
		{
			XMoveResizeWindow(this->mainClass->display,this->frameWindow,this->clientPreFSRect.x,this->clientPreFSRect.y,this->clientPreFSRect.w,this->clientPreFSRect.h);
			XMoveResizeWindow(this->mainClass->display,this->contentWindow,this->mainClass->leftSideBarSize,this->mainClass->titleBarSize,this->clientPreFSRect.w-(this->mainClass->leftSideBarSize+this->mainClass->riteSideBarSize),this->clientPreFSRect.h-this->mainClass->bottomBarSize-this->mainClass->titleBarSize);
		}

	this->isFullscreen=isfull;
	this->setWindowRects();
}

void LFSWM2_clientClass::LFSWM2_maxWindow(bool ismaxed,bool force)
{
	if((ismaxed==this->isMaximized) && (force==false))
		return;

	if(ismaxed==true)
		{
			this->framePreMaxRect=this->mainClass->mainWindowClass->LFSWM2_getWindowRect(this->frameWindow,this->mainClass->rootWindow,false);
			this->clientPreMaxRect=this->mainClass->mainWindowClass->LFSWM2_getWindowRect(this->contentWindow,this->mainClass->rootWindow,false);
			rectStruct rr={this->mainClass->monitors.at(0).x,this->mainClass->monitors.at(0).y,(int)this->mainClass->monitors.at(0).w,(int)this->mainClass->monitors.at(0).h};
			rr.w=this->mainClass->monitors.at(0).w-((this->mainClass->leftSideBarSize+BORDER_WIDTH+this->mainClass->riteSideBarSize+BORDER_WIDTH));//TODO//
			rr.h=this->mainClass->monitors.at(0).h-(this->mainClass->titleBarSize+this->mainClass->bottomBarSize+(BORDER_WIDTH*2));
			rr.x=this->mainClass->leftSideBarSize;
			rr.y=this->mainClass->titleBarSize;

			XMoveResizeWindow(this->mainClass->display,this->frameWindow,this->mainClass->monitors.at(0).x,this->mainClass->monitors.at(0).y,this->mainClass->monitors.at(0).w,this->mainClass->monitors.at(0).h);
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
	this->contentWindowRect=this->mainClass->mainWindowClass->LFSWM2_getWindowRect(this->contentWindow,this->mainClass->rootWindow);
	this->frameWindowRect=this->mainClass->mainWindowClass->LFSWM2_getWindowRect(this->frameWindow,this->mainClass->rootWindow);

	if(resize==true)
		this->LFSWM2_resizeControls();
}

void LFSWM2_clientClass::LFSWM2_showWindow(void)
{
		XMapWindow(this->mainClass->display,this->frameWindow);
}

void LFSWM2_clientClass::LFSWM2_hideWindow(void)
{
		XUnmapWindow(this->mainClass->display,this->frameWindow);
}

void LFSWM2_clientClass::LFSWM2_resizeControls(void)
{
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

	states=(Atom*)this->mainClass->mainWindowClass->LFSWM2_getFullProp(e->xproperty.window,this->mainClass->atoms.at("_NET_WM_STATE"),XA_ATOM,32,&n);

	if(n>0)
		{
			for(long unsigned int g=0;g<n;g++)
				{
					//this->mainClass->DEBUG_printAtom(states[g]);
					this->mainClass->mainWindowClass->LFSWM2_changeState(e->xproperty.window,NET_WM_STATE_ADD,states[g]);
				}
		}

	if(this->mainClass->mainWindowClass->LFSWM2_hasState(e->xproperty.window,this->mainClass->atoms.at("_NET_WM_STATE_STICKY")))
		{
			this->visibleOnAllDesks=true;
		}

	if(this->mainClass->mainWindowClass->LFSWM2_hasState(e->xproperty.window,this->mainClass->atoms.at("_NET_WM_STATE_BELOW")))
		{
			this->onBottom=true;
			XLowerWindow(this->mainClass->display,this->contentWindow);
			////XSync(this->mainClass->display,false);
			this->mainClass->restackCnt++;
		}

	if(this->mainClass->mainWindowClass->LFSWM2_hasState(e->xproperty.window,this->mainClass->atoms.at("_NET_WM_STATE_ABOVE")))
		{
			this->onTop=true;
			XRaiseWindow(this->mainClass->display,this->contentWindow);
			this->mainClass->mainWindowClass->LFSWM2_setProp(this->mainClass->rootWindow,this->mainClass->atoms.at("_NET_ACTIVE_WINDOW"),XA_WINDOW,32,&this->contentWindow,1);
			////XSync(this->mainClass->display,false);
			this->mainClass->restackCnt++;
		}
	if(states!=NULL)
		XFree(states);
}

bool LFSWM2_clientClass::LFSWM2_doFrameMoveEvents(XEvent *e)
{
	LFSWM2_clientClass	*cc;

	while(true)
		{
			XEvent ee;
			XNextEvent(this->mainClass->display,&ee);

			switch (ee.type)
				{
					case Expose:
						cc=this->mainClass->mainWindowClass->LFSWM2_getClientClass(ee.xexpose.window);
						if(cc!=NULL)
							cc->mainClass->mainWindowClass->LFSWM2_refreshFrame(cc,(XExposeEvent*)&ee);
						break;
					case MotionNotify:
						{
							int xdiff=ee.xbutton.x_root-e->xbutton.x_root;
							int ydiff=ee.xbutton.y_root-e->xbutton.y_root;
							XMoveWindow(this->mainClass->display,this->frameWindow,this->frameWindowRect.x+xdiff,this->frameWindowRect.y+ydiff);
						}
						break;
					case ButtonRelease:
						{
							if(this->isShaded==true)
								return(true);

							this->adjustContentWindow();
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
	return(r);
}

//void LFSWM2_clientClass::LFSWM2_refreshFrame(XExposeEvent *e)//TODO//prevent flicker
//{
//	rectStruct	r;
//
//	if((e!=NULL) && (e->count!=0))
//		return;
//
//	if(this->mainClass->useTheme==true)
//		{
//			this->mainClass->mainWindowClass->LFSWM2_refreshThemeFrame(this);
//			return;
//		}
//
//	r=this->frameWindowRect;
//
//	XSetClipMask(this->mainClass->display,this->mainClass->mainGC,None);
//	//XSetClipMask(this->mainClass->display,this->maskGC,this->mask);
//	XSetClipOrigin(this->mainClass->display,this->mainClass->mainGC,0,0);
//	//XSetClipOrigin(this->mainClass->display,this->maskGC,0,0);
//	XSetForeground(this->mainClass->display,this->mainClass->mainGC,this->mainClass->frameBG->pixel);
//	//XSetForeground(this->mainClass->display,this->maskGC,this->mainClass->frameFG->pixel);
//
//	if(this->mainClass->resizeMode==SCALERESIZE)
//			XFillRectangle(this->mainClass->display,this->frameWindow,this->mainClass->mainGC,0,0,r.w,this->mainClass->titleBarSize);
//	else
//		XFillRectangle(this->mainClass->display,this->frameWindow,this->mainClass->mainGC,0,0,r.w,r.h);
////		//XFillRectangle(this->mainClass->display,this->frameWindow,this->maskGC,0,0,r.w,r.h);
//
////	XSetForeground(this->mainClass->display,this->mainClass->mainGC,this->mainClass->whiteColour);
////XSetClipOrigin(this->mainClass->display,this->mainClass->mainGC,0,0);
////XFillRectangle(this->mainClass->display,this->frameWindow,this->mainClass->mainGC,0,0,r.w,r.h);
////
//////if(this->mask!=None)
////
//////
////	XSetClipOrigin(this->mainClass->display,this->mainClass->mainGC,0,0);
////	XSetForeground(this->mainClass->display,this->mainClass->mainGC,this->mainClass->frameBG->pixel);
////	XFillRectangle(this->mainClass->display,this->frameWindow,this->mainClass->mainGC,100,10,400,400);
////
////	XSetClipOrigin(this->mainClass->display,this->maskGC,0,0);
////	XSetForeground(this->mainClass->display,this->maskGC,this->mainClass->whiteColour);
////	XFillRectangle(this->mainClass->display,this->mask,this->maskGC,0,0,200,50);
////
////	XSetClipMask(this->mainClass->display,this->mainClass->mainGC,this->mask);
////	XSetClipOrigin(this->mainClass->display,this->mainClass->mainGC,0,0);
////	XSetForeground(this->mainClass->display,this->mainClass->mainGC,this->mainClass->frameText->pixel);
////	XFillRectangle(this->mainClass->display,this->frameWindow,this->mainClass->mainGC,20,20,100,100);
////
////XShapeCombineMask(this->mainClass->display,this->frameWindow,ShapeBounding,0,0,this->mask,ShapeSet);
////
////
////
////
////	XSetForeground(this->mainClass->display,this->maskGC,this->mainClass->frameFG->pixel);
////	XSetClipOrigin(this->mainClass->display,this->maskGC,0,0);
////	//XSetClipMask(this->mainClass->display,this->maskGC,this->mask);
////XFillRectangle(this->mainClass->display,this->frameWindow,this->maskGC,10,10,800,800);
//
//	XSetForeground(this->mainClass->display,this->mainClass->mainGC,this->mainClass->frameText->pixel);
//	XftDrawChange(this->mainClass->frameText->draw,this->frameWindow);
//
//	if(this->name.length()>0)
//		{
//			r=this->setTitlePosition();
//			if(this->nameIsUTF==true)
//				XftDrawStringUtf8(this->mainClass->frameText->draw,&(this->mainClass->frameText->color),this->mainClass->frameFont,r.x,r.y,(XftChar8*)this->name.c_str(),strlen(this->name.c_str()));
//			else
//				XftDrawString8(this->mainClass->frameText->draw,&(this->mainClass->frameText->color),this->mainClass->frameFont,r.x,r.y,(XftChar8*)this->name.c_str(),strlen(this->name.c_str()));
//		}
//
////buttons
//	this->LFSWM2_drawMouseLeave(this->closeButton,this->mainClass->closeBitMap,this->mainClass->closeControlStruct);
//	this->LFSWM2_drawMouseLeave(this->maximizeButton,this->mainClass->maximizeBitMap,this->mainClass->maximizeControlStruct);
//	this->LFSWM2_drawMouseLeave(this->minimizeButton,this->mainClass->minimizeBitMap,this->mainClass->minimizeControlStruct);
//	this->LFSWM2_drawMouseLeave(this->shadeButton,this->mainClass->shadeBitMap,this->mainClass->shadeControlStruct);
//}

bool LFSWM2_clientClass::LFSWM2_handleEvents(XEvent *e)
{
//this->mainClass->DEBUG_printEventData(e,false);

	switch(e->type)
		{
			case ButtonRelease:
				this->adjustContentWindow();
				//XMoveResizeWindow(this->mainClass->display,this->resizeWindow,-10,-10,1,1);
				break;

			case MotionNotify:
				{
					bool domove=false;

					if((this->frameWindow==e->xmotion.window) && (e->xmotion.state==Button1Mask))
						domove=true;
					if((this->contentWindow==e->xmotion.window) && (e->xmotion.state==Button1Mask+Mod4Mask))
						domove=!this->isFullscreen;

					if(domove==true)
						{
							this->LFSWM2_doFrameMoveEvents(e);
							this->mainClass->restackCnt=0;
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

			case UnmapNotify:
				{
					this->mainClass->mainWindowClass->LFSWM2_setClientList(this->contentWindow,false);
					this->visible=false;
					XUnmapWindow(this->mainClass->display,this->frameWindow);
					return(true);
				}
				break;

			case DestroyNotify:
				{
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
				   							cc->LFSWM2_sendCloseWindow();
				   							XUnmapWindow(this->mainClass->display, cc->frameWindow);
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
			case MapNotify:
			//	std::cerr<<"MapNotify"<<std::endl;
				break;

			case ConfigureNotify:
				break;

			case ConfigureRequest:
				{
				break;
					XRaiseWindow(this->mainClass->display,this->frameWindow);
					this->mainClass->mainWindowClass->LFSWM2_setProp(this->mainClass->rootWindow,this->mainClass->atoms.at("_NET_ACTIVE_WINDOW"),XA_WINDOW,32,&this->contentWindow,1);
					XSetInputFocus(this->mainClass->display,this->contentWindow,RevertToNone,CurrentTime);
				}
				break;

			case ConfigureRequest+1000://TODO//NEXT
				{
				//break;
					XRaiseWindow(this->mainClass->display,this->frameWindow);
					this->mainClass->mainWindowClass->LFSWM2_setProp(this->mainClass->rootWindow,this->mainClass->atoms.at("_NET_ACTIVE_WINDOW"),XA_WINDOW,32,&this->contentWindow,1);
					XSetInputFocus(this->mainClass->display,this->contentWindow,RevertToNone,CurrentTime);

					int			woff=this->mainClass->riteSideBarSize+this->mainClass->leftSideBarSize;
					int			hoff=this->mainClass->titleBarSize+this->mainClass->bottomBarSize;
					rectStruct	r;

					if(e->xconfigurerequest.value_mask==0xc && e->xconfigurerequest.send_event==0x0)
						{
							return(false);
						}
					if(e->xconfigurerequest.value_mask==0xf && e->xconfigurerequest.send_event==0x0)
						{
							return(false);
						}


					if(((e->xconfigurerequest.value_mask & CWMyframe)==CWMyframe) && (e->xconfigurerequest.window==this->contentWindow))
						{
							XWindowAttributes 	xa;

							XGetWindowAttributes(this->mainClass->display,this->frameWindow,(XWindowAttributes*)&xa);
							if(e->xconfigurerequest.send_event==true)
								{
									switch(e->xconfigurerequest.value_mask)
										{
											case 0x104:
											case 0x105:
											case 0x108:
											case 0x109:
											case 0x10c:
											case 0x10e:
											case 0x10f:
												r={e->xconfigurerequest.x,e->xconfigurerequest.y,e->xconfigurerequest.width,e->xconfigurerequest.height};
												XMoveResizeWindow(this->mainClass->display,this->frameWindow,r.x,r.y,r.w,r.h);
												this->frameWindowRect=r;
												//if((e->xconfigurerequest.value_mask & CWMyframe)==CWMyframe)
												//	XMoveWindow(this->mainClass->display,this->contentWindow,10000,10000);
												break;
										}
								}
						}
					else
						{
							if((e->xconfigurerequest.value_mask==0xc) && (e->xconfigurerequest.send_event==0x0))
								{
									rectStruct	r={e->xconfigurerequest.x,e->xconfigurerequest.y,e->xconfigurerequest.width+woff,e->xconfigurerequest.height+hoff};
									XMoveResizeWindow(this->mainClass->display,this->frameWindow,r.x,r.y,r.w,r.h);
									this->frameWindowRect=r;
									break;
								}

							if((e->xconfigurerequest.value_mask==0xf) && (e->xconfigurerequest.send_event==0x0))
								{
									rectStruct	r={e->xconfigurerequest.x,e->xconfigurerequest.y,e->xconfigurerequest.width+woff,e->xconfigurerequest.height+hoff};
									XMoveResizeWindow(this->mainClass->display,this->frameWindow,r.x,r.y,r.w,r.h);
									this->frameWindowRect=r;
								}
						}

					r.x=this->frameWindowRect.x+this->mainClass->leftSideBarSize;
					r.y=this->frameWindowRect.y+this->mainClass->titleBarSize;
					r.w=this->frameWindowRect.w-this->mainClass->leftSideBarSize+this->mainClass->riteSideBarSize;
					r.h=this->frameWindowRect.h-this->mainClass->titleBarSize-this->mainClass->bottomBarSize;
					XResizeWindow(this->mainClass->display,this->contentWindow,r.w,r.h);
					this->adjustContentWindow();
					this->mainClass->restackCnt=1;

					if(e->xconfigurerequest.detail==Below)
						{
							this->mainClass->mainWindowClass->LFSWM2_changeState(this->contentWindow,NET_WM_STATE_REMOVE,this->mainClass->atoms.at("_NET_WM_STATE_ABOVE"));
							this->mainClass->mainWindowClass->LFSWM2_changeState(this->contentWindow,NET_WM_STATE_TOGGLE,this->mainClass->atoms.at("_NET_WM_STATE_BELOW"));
							this->onBottom=!this->onBottom;
							this->mainClass->restackCnt=1;
							return(true);
							break;
						}
					return(false);
				}
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
