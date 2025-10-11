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
	int	menucnt=0;

	this->mainClass=mainclass;
	//this->picFormat=XRenderFindStandardFormat(this->mainClass->display,PictStandardRGB24);
	//this->pa.subwindow_mode=IncludeInferiors;
	while(menuItemlabels[menucnt]!=NULL)
		menuNames.push_back(menuItemlabels[menucnt++]);
}

LFSWM2_clientClass::~LFSWM2_clientClass(void)
{
	XWindowAttributes	x_window_attrs;

//	if(this->windowHints!=NULL)
//	this->mainClass->mainWindowClass->LFSWM2_freeHints(this->windowHints);
	this->mainClass->LFSWM2_freeHints(&this->windowHints);
	this->mainClass->mainWindowClass->LFSWM2_setClientList(this->contentWindow,false);

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
}

void LFSWM2_clientClass::LFSWM2_setWindowName(void)
{
	char					*namex=NULL;
	XTextProperty		p;
	long unsigned int	nitems_return;

	this->name="";

	namex=(char*)this->mainClass->mainWindowClass->LFSWM2_getProp(this->contentWindow,this->mainClass->atomshashed.at(LFSTK_UtilityClass::LFSTK_hashFromKey("_NET_WM_NAME")),this->mainClass->atomshashed.at(LFSTK_UtilityClass::LFSTK_hashFromKey("UTF8_STRING")),&nitems_return);
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
							this->nameIsUTF=false;
						}
					XFree(p.value);
				}
		}

	if(namex!=NULL)
		free(namex);
}

void LFSWM2_clientClass::drawMousePressed(Window id,Pixmap pm,controlData data)
{
	std::string		cn=data.controlName+"-pressed";
	long unsigned	col=(0xff000000|(~this->mainClass->frameFG->pixel&0xffffff));

	if(this->mainClass->useTheme==true)
		{
			XSetClipMask(this->mainClass->display,this->frameGC,this->mainClass->mainWindowClass->theme.masks[LFSTK_UtilityClass::LFSTK_hashFromKey(cn)]);
			XSetClipOrigin(this->mainClass->display,this->frameGC,0,0);
			XCopyArea(this->mainClass->display,this->mainClass->mainWindowClass->theme.pixmaps[LFSTK_UtilityClass::LFSTK_hashFromKey(cn)],id,this->frameGC,0,0,this->mainClass->mainWindowClass->theme.partsWidth[LFSTK_UtilityClass::LFSTK_hashFromKey(cn)],this->mainClass->mainWindowClass->theme.partsHeight[LFSTK_UtilityClass::LFSTK_hashFromKey(cn)],0,0);
			XShapeCombineMask(this->mainClass->display,id,ShapeBounding,0,0,this->mainClass->mainWindowClass->theme.masks[LFSTK_UtilityClass::LFSTK_hashFromKey(cn)],ShapeSet);
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
			XSetClipMask(this->mainClass->display,this->frameGC,this->mainClass->mainWindowClass->theme.masks[LFSTK_UtilityClass::LFSTK_hashFromKey(cn)]);
			XSetClipOrigin(this->mainClass->display,this->frameGC,0,0);
			XCopyArea(this->mainClass->display,this->mainClass->mainWindowClass->theme.pixmaps[LFSTK_UtilityClass::LFSTK_hashFromKey(cn)],id,this->frameGC,0,0,this->mainClass->mainWindowClass->theme.partsWidth[LFSTK_UtilityClass::LFSTK_hashFromKey(cn)],this->mainClass->mainWindowClass->theme.partsHeight[LFSTK_UtilityClass::LFSTK_hashFromKey(cn)],0,0);
			XShapeCombineMask(this->mainClass->display,id,ShapeBounding,0,0,this->mainClass->mainWindowClass->theme.masks[LFSTK_UtilityClass::LFSTK_hashFromKey(cn)],ShapeSet);
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
			XSetClipMask(this->mainClass->display,this->frameGC,this->mainClass->mainWindowClass->theme.masks[LFSTK_UtilityClass::LFSTK_hashFromKey(cn)]);
			XSetClipOrigin(this->mainClass->display,this->frameGC,0,0);
			XCopyArea(this->mainClass->display,this->mainClass->mainWindowClass->theme.pixmaps[LFSTK_UtilityClass::LFSTK_hashFromKey(cn)],id,this->frameGC,0,0,this->mainClass->mainWindowClass->theme.partsWidth[LFSTK_UtilityClass::LFSTK_hashFromKey(cn)],this->mainClass->mainWindowClass->theme.partsHeight[LFSTK_UtilityClass::LFSTK_hashFromKey(cn)],0,0);
			XShapeCombineMask(this->mainClass->display,id,ShapeBounding,0,0,this->mainClass->mainWindowClass->theme.masks[LFSTK_UtilityClass::LFSTK_hashFromKey(cn)],ShapeSet);
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
//this->mainClass->mainWindowClass->LFSWM2_windowClass::LFSWM2_removeProp(this->contentWindow,LFSTK_UtilityClass::LFSTK_hashFromKey("_NET_FRAME_EXTENTS"));
	em.type=ClientMessage;
	em.window=this->contentWindow;
	em.message_type=this->mainClass->atomshashed.at(LFSTK_UtilityClass::LFSTK_hashFromKey("WM_PROTOCOLS"));
	em.format=32;
	em.data.l[0]=this->mainClass->atomshashed.at(LFSTK_UtilityClass::LFSTK_hashFromKey("WM_DELETE_WINDOW"));;
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

	if(this->windowType==UTILITYWINDOW)
		{
			this->frameWindowRect=this->mainClass->mainWindowClass->LFSWM2_getWindowRect(this->frameWindow,this->mainClass->rootWindow);
			return;
		}

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
	if(this->windowType==UTILITYWINDOW)
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

//					switch(this->resizeMode)
//						{
//							case FASTRESIZE:
//								XMoveWindow(this->mainClass->display,this->contentWindow,this->frameWindowRect.w+10,0);
//								break;
//							case LIVERESIZE:
//								break;
//							case SIZERESIZE:
//								break;
////							case SCALERESIZE:
////								{
////									this->currentRootPixmap=XCreatePixmap(this->mainClass->display,this->contentWindow,this->contentWindowRect.w,this->contentWindowRect.h,this->mainClass->depth);
////									XSetClipMask(this->mainClass->display,this->mainClass->mainGC,None);
////									XSetClipOrigin(this->mainClass->display, this->mainClass->mainGC,0,0);
////									this->resizeImage=XGetImage(this->mainClass->display,this->contentWindow,0,0,this->contentWindowRect.w,this->contentWindowRect.h,AllPlanes,ZPixmap);
////
////									XSetClipMask(this->mainClass->display,this->mainClass->mainGC,None);
////									XSetClipOrigin(this->mainClass->display,this->mainClass->mainGC,0,0);
////									XPutImage(this->mainClass->display,this->currentRootPixmap, this->mainClass->mainGC,this->resizeImage,0,0,0,0,this->contentWindowRect.w,this->contentWindowRect.h);
////
////									this->ow=this->contentWindowRect.w;
////									this->oh=this->contentWindowRect.h;								
////									this->resizeWindow=XCreateSimpleWindow(this->mainClass->display,this->frameWindow,this->mainClass->leftSideBarSize,this->mainClass->titleBarSize,this->contentWindowRect.w,this->contentWindowRect.h,BORDER_WIDTH,this->mainClass->frameFG->pixel,this->mainClass->frameBG->pixel);
////									XMoveWindow(this->mainClass->display,this->contentWindow,this->mainClass->displayWidth+10,0);
////									XMapWindow(this->mainClass->display,this->resizeWindow);
////									XRaiseWindow(this->mainClass->display,this->resizeWindow);
////									primaryPicture=XRenderCreatePicture(this->mainClass->display,this->currentRootPixmap,picFormat,CPSubwindowMode,&pa);
////									windowPicture=XRenderCreatePicture(this->mainClass->display,this->resizeWindow,picFormat,CPSubwindowMode,&pa);
////
////									XRenderComposite(this->mainClass->display,PictOpSrc,primaryPicture,0,windowPicture,0,0,0,0,0,0,this->ow,this->oh);
////								}
////								break;
//						}
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
				//if(this->currentRootPixmap!=None)
				//	XFreePixmap(this->mainClass->display,this->currentRootPixmap);
				//if(this->primaryPicture!=None)
				//	XRenderFreePicture(this->mainClass->display,this->primaryPicture);
				//if(this->windowPicture!=None)
				//	XRenderFreePicture(this->mainClass->display,this->windowPicture);
				//if(this->resizeImage!=NULL)
				//	XDestroyImage(this->resizeImage);

				//this->currentRootPixmap=None;
				//this->primaryPicture=None;
				//this->windowPicture=None;
				//this->resizeImage=None;

				XMoveResizeWindow(this->mainClass->display,this->resizeWindow,this->mainClass->displayWidth+10,0,1,1);
				this->setWindowRects(true);
				break;

			case MotionNotify:
				if(buttonDown==true)
					{
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

//								if(this->resizeMode==SCALERESIZE)
//									{
//										XResizeWindow(this->mainClass->display,this->resizeWindow,r.w-contentwadjust,r.h-contenthadjust);
//										double x_scale=(double)ow /((double)this->frameWindowRect.w-(double)(this->mainClass->leftSideBarSize+this->mainClass->riteSideBarSize));
//										double y_scale=(double)oh /((double)this->frameWindowRect.h-(double)(this->mainClass->titleBarSize+this->mainClass->bottomBarSize));
//										XTransform transform_matrix1=
//											{{
//												{XDoubleToFixed(x_scale),XDoubleToFixed(0),XDoubleToFixed(0)},
//												{XDoubleToFixed(0),XDoubleToFixed(y_scale),XDoubleToFixed(0)},
//												{XDoubleToFixed(0),XDoubleToFixed(0),XDoubleToFixed(1.0)}  
//											}};
//										XResizeWindow(this->mainClass->display,this->contentWindow,r.w-contentwadjust,r.h-contenthadjust);
//										XRenderSetPictureTransform(this->mainClass->display,primaryPicture,&transform_matrix1);
//										XRenderComposite(this->mainClass->display,PictOpSrc,primaryPicture,0,windowPicture,0,0,0,0,0,0,r.w-contentwadjust,r.h-contenthadjust);
//
//										this->steps=0;
//										break;
//									}

								//if(this->resizeMode==LIVERESIZE)
									{
										this->contentWindowRect={this->mainClass->leftSideBarSize,this->mainClass->titleBarSize,r.w-contentwadjust,r.h-contenthadjust};
										XResizeWindow(this->mainClass->display,this->contentWindow,r.w-contentwadjust,r.h-contenthadjust);
									}
//								else
//									{
//										XMoveWindow(this->mainClass->display,this->contentWindow,this->frameWindowRect.w+10,0);
//									}

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
//	if(this->isBorderless==true)
//if((this->isBorderless==true) || (this->windowType==UTILITYWINDOW))
//		return;
if(this->windowType==UTILITYWINDOW)
{
	if(useframerect==true)
		{
			XResizeWindow(this->mainClass->display,this->contentWindow,this->frameWindowRect.w,this->frameWindowRect.h);
			this->contentWindowRect={this->contentWindowRect.x,this->contentWindowRect.y,this->frameWindowRect.w,this->frameWindowRect.h};
		}
	else
		{
			XResizeWindow(this->mainClass->display,this->contentWindow,w,h);
			this->contentWindowRect={this->contentWindowRect.x,this->contentWindowRect.y,w,h};
		}
	return;
}
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
			this->mainClass->mainWindowClass->LFSWM2_removeProp(this->contentWindow,this->mainClass->atomshashed.at(LFSTK_UtilityClass::LFSTK_hashFromKey("_NET_WM_STATE_MAXIMIZED_HORZ")));
			this->mainClass->mainWindowClass->LFSWM2_removeProp(this->contentWindow,this->mainClass->atomshashed.at(LFSTK_UtilityClass::LFSTK_hashFromKey("_NET_WM_STATE_MAXIMIZED_VERT")));
			this->isMaximized=false;
		}

	this->mainClass->mainWindowClass->LFSWM2_removeProp(this->contentWindow,this->mainClass->atomshashed.at(LFSTK_UtilityClass::LFSTK_hashFromKey("_NET_WM_STATE_HIDDEN")));
	this->isShaded=false;
	this->isMinimized=false;
	this->mainClass->mainWindowClass->LFSWM2_setWindowState(this->contentWindow,NormalState);
	this->mainClass->mainWindowClass->LFSWM2_reloadWindowState(this->contentWindow);
}

void LFSWM2_clientClass::LFSWM2_maxWindow(void)
{
	if(this->isMaximized==true)
		{
			this->mainClass->mainWindowClass->LFSWM2_removeProp(this->contentWindow,this->mainClass->atomshashed.at(LFSTK_UtilityClass::LFSTK_hashFromKey("_NET_WM_STATE_MAXIMIZED_HORZ")));
			this->mainClass->mainWindowClass->LFSWM2_removeProp(this->contentWindow,this->mainClass->atomshashed.at(LFSTK_UtilityClass::LFSTK_hashFromKey("_NET_WM_STATE_MAXIMIZED_VERT")));
		}
	else
		{
			this->mainClass->mainWindowClass->LFSWM2_addState(this->contentWindow,this->mainClass->atomshashed.at(LFSTK_UtilityClass::LFSTK_hashFromKey("_NET_WM_STATE_MAXIMIZED_HORZ")));
			this->mainClass->mainWindowClass->LFSWM2_addState(this->contentWindow,this->mainClass->atomshashed.at(LFSTK_UtilityClass::LFSTK_hashFromKey("_NET_WM_STATE_MAXIMIZED_VERT")));
		}
	this->mainClass->mainWindowClass->LFSWM2_reloadWindowState(this->contentWindow);
}

void LFSWM2_clientClass::LFSWM2_minWindow(void)
{
	this->mainClass->mainWindowClass->LFSWM2_addState(this->contentWindow,this->mainClass->atomshashed.at(LFSTK_UtilityClass::LFSTK_hashFromKey("_NET_WM_STATE_HIDDEN")));
	this->isMinimized=true;
	this->mainClass->mainWindowClass->LFSWM2_setWindowState(this->contentWindow,IconicState);
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
	int					stringnum=-1;
	bool					usealt=false;
	LFSWM2_clientClass	*cc=static_cast<LFSWM2_clientClass*>(ud);
	std::string			comp=static_cast<LFSTK_gadgetClass*>(p)->LFSTK_getLabel();

	if(LFSTK_UtilityClass::LFSTK_strStr(comp,cc->menuNames.at(MAXLABEL)).empty()==false)
		{
			cc->LFSWM2_maxWindow();
			usealt=cc->isMaximized;
			stringnum=MAXLABEL;
		}

	if(LFSTK_UtilityClass::LFSTK_strStr(comp,cc->menuNames.at(MINLABEL)).empty()==false)
		cc->LFSWM2_minWindow();

	if(LFSTK_UtilityClass::LFSTK_strStr(comp,cc->menuNames.at(SHADELABEL)).empty()==false)
		{
			cc->LFSWM2_shadeWindow();
			usealt=cc->isShaded;
			stringnum=SHADELABEL;
		}

	if(LFSTK_UtilityClass::LFSTK_strStr(comp,cc->menuNames.at(FSLABEL)).empty()==false)
		{
			cc->LFSWM2_fullscreenWindow(true,true);
			cc->mainClass->mainWindowClass->LFSWM2_addState(cc->contentWindow,cc->mainClass->atomshashed.at(LFSTK_UtilityClass::LFSTK_hashFromKey("_NET_WM_STATE_FULLSCREEN")));
			XSetInputFocus(cc->mainClass->display,cc->contentWindow,RevertToNone,CurrentTime);
		}

	if(LFSTK_UtilityClass::LFSTK_strStr(comp,cc->menuNames.at(ONTOPLABEL)).empty()==false)
		{
			if(cc->onTop==false)
				{
					XSetInputFocus(cc->mainClass->display,cc->contentWindow,RevertToNone,CurrentTime);
					cc->isActive=true;
					cc->mainClass->mainWindowClass->LFSWM2_setProp(cc->mainClass->rootWindow,cc->mainClass->atomshashed.at(LFSTK_UtilityClass::LFSTK_hashFromKey("_NET_ACTIVE_WINDOW")),XA_WINDOW,32,&cc->contentWindow,1);
					cc->mainClass->mainWindowClass->LFSWM2_addState(cc->contentWindow,cc->mainClass->atomshashed.at(LFSTK_UtilityClass::LFSTK_hashFromKey("_NET_WM_STATE_ABOVE")));
					cc->mainClass->mainWindowClass->LFSWM2_removeProp(cc->contentWindow,cc->mainClass->atomshashed.at(LFSTK_UtilityClass::LFSTK_hashFromKey("_NET_WM_STATE_BELOW")));
					cc->mainClass->mainEventClass->LFSWM2_shuffle(cc->contentWindow);
				}
			else
				{
					XSetInputFocus(cc->mainClass->display,None,RevertToNone,CurrentTime);
					cc->mainClass->mainWindowClass->LFSWM2_removeProp(cc->contentWindow,cc->mainClass->atomshashed.at(LFSTK_UtilityClass::LFSTK_hashFromKey("_NET_WM_STATE_ABOVE")));
					cc->mainClass->mainWindowClass->LFSWM2_removeProp(cc->mainClass->rootWindow,cc->mainClass->atomshashed.at(LFSTK_UtilityClass::LFSTK_hashFromKey("_NET_ACTIVE_WINDOW")));
					cc->mainClass->mainEventClass->LFSWM2_shuffle(None);
					cc->mainClass->mainWindowClass->LFSWM2_refreshFrame(cc);
				}
			
			cc->mainClass->mainEventClass->LFSWM2_restack();
			usealt=!cc->onTop;
			stringnum=ONTOPLABEL;
		}

	if(LFSTK_UtilityClass::LFSTK_strStr(comp,cc->menuNames.at(ONBOTTOMLABEL)).empty()==false)
		{
			if(cc->onBottom==false)
				{
					cc->mainClass->mainWindowClass->LFSWM2_addState(cc->contentWindow,cc->mainClass->atomshashed.at(LFSTK_UtilityClass::LFSTK_hashFromKey("_NET_WM_STATE_BELOW")));
					cc->mainClass->mainWindowClass->LFSWM2_removeProp(cc->contentWindow,cc->mainClass->atomshashed.at(LFSTK_UtilityClass::LFSTK_hashFromKey("_NET_WM_STATE_ABOVE")));
					cc->mainClass->mainEventClass->LFSWM2_shuffle(None);
				}
			else
				{
					cc->mainClass->mainWindowClass->LFSWM2_removeProp(cc->contentWindow,cc->mainClass->atomshashed.at(LFSTK_UtilityClass::LFSTK_hashFromKey("_NET_WM_STATE_BELOW")));
					cc->mainClass->mainEventClass->LFSWM2_shuffle(None);
				}

			cc->mainClass->mainEventClass->LFSWM2_restack();
			usealt=!cc->onBottom;
			stringnum=ONBOTTOMLABEL;
			cc->mainClass->mainEventClass->LFSWM2_restack();
		}

	if(LFSTK_UtilityClass::LFSTK_strStr(comp,cc->menuNames.at(ONALLDESKSLABEL)).empty()==false)
		{
			cc->visibleOnAllDesks=!cc->visibleOnAllDesks;
			if(cc->visibleOnAllDesks==true)
				{
					cc->mainClass->mainWindowClass->LFSWM2_addState(cc->contentWindow,cc->mainClass->atomshashed.at(LFSTK_UtilityClass::LFSTK_hashFromKey("_NET_WM_STATE_STICKY")));
				}
			else
				cc->mainClass->mainWindowClass->LFSWM2_removeProp(cc->contentWindow,cc->mainClass->atomshashed.at(LFSTK_UtilityClass::LFSTK_hashFromKey("_NET_WM_STATE_STICKY")));
			cc->mainClass->mainEventClass->LFSWM2_restack();
			usealt=cc->visibleOnAllDesks;
			stringnum=ONALLDESKSLABEL;
		}

	if(static_cast<LFSTK_gadgetClass*>(p)->LFSTK_getLabel().compare("Close")==0)
		cc->LFSWM2_sendCloseWindow();
	//if(static_cast<LFSTK_gadgetClass*>(p)->LFSTK_getLabel().compare("Fast Resize")==0)
	//	cc->resizeMode=FASTRESIZE;
	//if(static_cast<LFSTK_gadgetClass*>(p)->LFSTK_getLabel().compare("Live Resize")==0)
	//	cc->resizeMode=LIVERESIZE;

	if(static_cast<LFSTK_gadgetClass*>(p)->LFSTK_getLabel().compare("About")==0)
		system("lfsabout &");

#ifdef __DEBUG__
	if(static_cast<LFSTK_gadgetClass*>(p)->LFSTK_getLabel().compare("DEBUG")==0)
		{
			fprintf(stderr,"\ncontentWindow=%p\n",cc->contentWindow);
			fprintf(stderr,"frameWindow=%p\n",cc->frameWindow);
			fprintf(stderr,"name=%s\n",cc->name.c_str());
			fprintf(stderr,"visibleOnAllDesks=%s\n",cc->mainClass->DEBUG_printBool(cc->visibleOnAllDesks));
			fprintf(stderr,"onTop=%s\n",cc->mainClass->DEBUG_printBool(cc->onTop));
			fprintf(stderr,"onBottom=%s\n",cc->mainClass->DEBUG_printBool(cc->onBottom));
			fprintf(stderr,"isWithdrawn=%s\n",cc->mainClass->DEBUG_printBool(cc->isWithdrawn));
			fprintf(stderr,"isBorderless=%s\n",cc->mainClass->DEBUG_printBool(cc->isBorderless));
			const char	*types="_NET_WM_NAME _NET_WM_WINDOW_TYPE WM_CLASS WM_NAME _NET_WM_STATE _NET_WM_DESKTOP _NET_FRAME_EXTENTS WM_STATE WM_NORMAL_HINTS ";
			char			*command;
			asprintf(&command,"xprop -id %p %s\n",cc->contentWindow,types);
			fprintf(stderr,"Atoms:\n");
			system(command);
			fprintf(stderr,"using command:\n%s\n",command);
			free(command);
		}
#endif

	static_cast<LFSTK_gadgetClass*>(p)->wc->LFSTK_hideWindow();
	if(stringnum!=-1)
		{
			if(usealt==true)
				cc->menuNames.at(stringnum)=altMenuitemlabels[stringnum];
			else
				cc->menuNames.at(stringnum)=menuItemlabels[stringnum];
		}
	return(true);	
}

void LFSWM2_clientClass::showWMMenu(int x,int y)
{
	LFSTK_applicationClass	*apc=NULL;
	LFSTK_windowClass		*wc=NULL;
	LFSTK_menuClass			*menu=NULL;

	apc=new LFSTK_applicationClass();
	apc->LFSTK_addWindow(NULL,"");
	wc=apc->mainWindow;

	menu=new LFSTK_menuClass(wc,x,y,1,1);

	wc->LFSTK_resizeWindow(1,1,true);
	wc->LFSTK_hideWindow();

	menu->LFSTK_setMouseCallBack(NULL,wmCB,(void*)0x888);

#ifdef __DEBUG__
	infoDataStruct	**mms=new infoDataStruct*[NUMOFMENUS+1];
#else
	infoDataStruct	**mms=new infoDataStruct*[NUMOFMENUS];
#endif
	for (int j=0;j<NUMOFMENUS;j++)
		{
			mms[j]=new infoDataStruct;
			mms[j]->label=this->menuNames.at(j);
			mms[j]->hasSubMenu=false;
			mms[j]->subMenus=NULL;
			mms[j]->userData=(void*)this;
			mms[j]->imageType=NOTHUMB;
		}

#ifdef __DEBUG__
	mms[NUMOFMENUS]=new infoDataStruct;
	mms[NUMOFMENUS]->label="DEBUG";
	mms[NUMOFMENUS]->hasSubMenu=false;
	mms[NUMOFMENUS]->subMenus=NULL;
	mms[NUMOFMENUS]->userData=(void*)this;
	mms[NUMOFMENUS]->imageType=NOTHUMB;
	menu->LFSTK_addMainMenus(mms,NUMOFMENUS+1);
#else
	menu->LFSTK_addMainMenus(mms,NUMOFMENUS);
#endif

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
							this->mainClass->mainWindowClass->LFSWM2_addState(this->contentWindow,this->mainClass->atomshashed.at(LFSTK_UtilityClass::LFSTK_hashFromKey("_NET_WM_STATE_FULLSCREEN")));
							XMoveResizeWindow(this->mainClass->display,this->frameWindow,mg.x,mg.y,mg.w,mg.h);
							XMoveResizeWindow(this->mainClass->display,this->contentWindow,0,0,mg.w,mg.h);
						}
				}
		}
	else
		{
			this->mainClass->mainWindowClass->LFSWM2_removeProp(this->contentWindow,this->mainClass->atomshashed.at(LFSTK_UtilityClass::LFSTK_hashFromKey("_NET_WM_STATE_FULLSCREEN")));
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
//if(this->windowType==UTILITYWINDOW)
//	return;
	this->contentWindowRect=this->mainClass->mainWindowClass->LFSWM2_getWindowRect(this->contentWindow,this->mainClass->rootWindow);
	this->frameWindowRect=this->mainClass->mainWindowClass->LFSWM2_getWindowRect(this->frameWindow,this->mainClass->rootWindow);

	if(resize==true)
		this->LFSWM2_resizeControls();
}

void LFSWM2_clientClass::LFSWM2_showWindow(bool checkstate)
{
	if((checkstate==true) && ((this->isWithdrawn==true) || (this->isHidden==true)))
		return;

	if(this->windowType==UTILITYWINDOW)
		XMoveWindow(this->mainClass->display,this->contentWindow,0,0);

	this->contentWindowRect=this->mainClass->mainWindowClass->LFSWM2_getWindowRect(this->contentWindow,this->mainClass->rootWindow);
	this->visible=true;
//	if(this->isWithdrawn==true)
		this->mainClass->mainWindowClass->LFSWM2_setWindowState(this->contentWindow,NormalState);
	this->isWithdrawn=false;
	this->isHidden=false;
	this->isMinimized=false;
	//this->mainClass->mainWindowClass->LFSWM2_removeProp(this->contentWindow,this->mainClass->atomshashed.at(LFSTK_UtilityClass::LFSTK_hashFromKey("_NET_WM_STATE_HIDDEN")));
	XMapWindow(this->mainClass->display,this->frameWindow);
	//fprintf(stderr,">>>>>>>>>>>>>>>\n");
}

void LFSWM2_clientClass::LFSWM2_hideWindow(bool withdraw)
{
	this->visible=false;
	//XUnmapWindow(this->mainClass->display,this->contentWindow);
	if(withdraw==true)
		{
			this->holdFrameRect=this->frameWindowRect;
			this->isWithdrawn=true;
			this->mainClass->mainWindowClass->LFSWM2_setWindowState(this->contentWindow,WithdrawnState);
		}
	XUnmapWindow(this->mainClass->display,this->frameWindow);
}

void LFSWM2_clientClass::LFSWM2_resizeControls(void)
{
	if(this->isBorderless==true)
		return;

	XResizeWindow(this->mainClass->display,this->bottomDragger,this->frameWindowRect.w,this->dragSpacing);
	XResizeWindow(this->mainClass->display,this->leftSideDragger,this->leftSideDragWidth,this->frameWindowRect.h-(this->dragSpacing*2));
	XResizeWindow(this->mainClass->display,this->rightSideDragger,this->riteSideDragWidth,this->frameWindowRect.h-(this->dragSpacing*2));
}

void LFSWM2_clientClass::LFSWM2_setWMState()
{
	if(this->LFSWM2_getWindowState()==WithdrawnState)
		this->LFSWM2_hideWindow(true);
}

void LFSWM2_clientClass::LFSWM2_setNetWMState(XEvent *e)
{
	long unsigned int	n;
	Atom					*states;

	this->onBottom=false;
	this->onTop=false;
	this->visibleOnAllDesks=false;

	states=(Atom*)this->mainClass->mainWindowClass->LFSWM2_getFullProp(e->xproperty.window,this->mainClass->atomshashed.at(LFSTK_UtilityClass::LFSTK_hashFromKey("_NET_WM_STATE")),XA_ATOM,32,&n);

	if(n>0)
		{
			for(long unsigned int g=0;g<n;g++)
				{
					//this->mainClass->DEBUG_printAtom(states[g]);
					this->mainClass->mainWindowClass->LFSWM2_changeState(e->xproperty.window,NET_WM_STATE_ADD,states[g]);
				}
		}

	if(!this->mainClass->mainWindowClass->LFSWM2_hasState(e->xproperty.window,this->mainClass->atomshashed.at(LFSTK_UtilityClass::LFSTK_hashFromKey("_NET_WM_STATE_HIDDEN"))))
		{
			this->visible=true;
			this->LFSWM2_showWindow();
		}

	if(this->mainClass->mainWindowClass->LFSWM2_hasState(e->xproperty.window,this->mainClass->atomshashed.at(LFSTK_UtilityClass::LFSTK_hashFromKey("_NET_WM_STATE_STICKY"))))
		{
			this->visibleOnAllDesks=true;
		}

	if(this->mainClass->mainWindowClass->LFSWM2_hasState(e->xproperty.window,this->mainClass->atomshashed.at(LFSTK_UtilityClass::LFSTK_hashFromKey("_NET_WM_STATE_BELOW"))))
		{
			this->onBottom=true;
			this->onTop=false;
			XLowerWindow(this->mainClass->display,this->contentWindow);
		}

	if(this->mainClass->mainWindowClass->LFSWM2_hasState(e->xproperty.window,this->mainClass->atomshashed.at(LFSTK_UtilityClass::LFSTK_hashFromKey("_NET_WM_STATE_ABOVE"))))
		{
			this->onBottom=false;
			this->onTop=true;
			XRaiseWindow(this->mainClass->display,this->contentWindow);
			this->mainClass->mainWindowClass->LFSWM2_setProp(this->mainClass->rootWindow,this->mainClass->atomshashed.at(LFSTK_UtilityClass::LFSTK_hashFromKey("_NET_ACTIVE_WINDOW")),XA_WINDOW,32,&this->contentWindow,1);
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
							LFSWM2_clientClass	*rcc;
							bool					donemove=false;
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
									this->mainClass->LFSWM2_setCurrentDesktop(this->onDesk,true,true);
									this->mainClass->mainWindowClass->LFSWM2_setProp(this->contentWindow,this->mainClass->atomshashed.at(LFSTK_UtilityClass::LFSTK_hashFromKey("_NET_WM_DESKTOP")),XA_CARDINAL,32,&this->onDesk,1);
									XWarpPointer(this->mainClass->display,None,None,0,0,0,0,this->mainClass->displayWidth-50,0);
									donemove=true;
								}
							else if((ee.xbutton.x_root>this->mainClass->displayWidth-20) && (direction==1))
								{
									this->onDesk=this->mainClass->LFSWM2_getHigherDesktop(this->onDesk);
									this->mainClass->LFSWM2_setCurrentDesktop(this->onDesk,true,true);
									this->mainClass->mainWindowClass->LFSWM2_setProp(this->contentWindow,this->mainClass->atomshashed.at(LFSTK_UtilityClass::LFSTK_hashFromKey("_NET_WM_DESKTOP")),XA_CARDINAL,32,&this->onDesk,1);
									XWarpPointer(this->mainClass->display,None,None,0,0,0,0,(-(this->mainClass->displayWidth)+50),0);
									donemove=true;
								}
							if(donemove==true)
								{
									XSync(this->mainClass->display,true);
									this->mainClass->mainEventClass->LFSWM2_restack();
									for(long unsigned j=0;j<this->mainClass->mainWindowClass->windowIDList.size();j++)
										{
											rcc=this->mainClass->mainWindowClass->LFSWM2_getClientClass(this->mainClass->mainWindowClass->windowIDList.at(j));
											if(rcc!=NULL)
												this->mainClass->mainWindowClass->LFSWM2_refreshFrame(rcc,NULL);
										}
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
//	if(this->isBorderless==true)
	if(this->windowType==UTILITYWINDOW)
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
//if(e->xany.window==0x3400006)
//{
//	//cc=this->mainClass->mainWindowClass->LFSWM2_getClientClass(e.xkey.window);
//	if((this!=NULL) && (this->isMinimized==true))
//		{
//			fprintf(stderr,">>>>>>>e.xany.window==0x3400006 cc->isMinimized==true cc->isVisible=%i\n",this->visible);
//			//cc->LFSWM2_showWindow(false);
//		}
//}
	switch(e->type)
		{
			case ButtonPress:
				//fprintf(stderr,"ButtonPress\n");
				if((e->xbutton.state&(this->mainClass->modKeys))!=(this->mainClass->modKeys))//TODO//???windows key for now used to move window wihout restacking
					{
						if(this->isActive==false)
							{
								this->mainClass->mainEventClass->LFSWM2_shuffle(this->contentWindow);
								this->mainClass->mainEventClass->LFSWM2_restack();
								this->isActive=true;
							}
					}
				break;

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

			case MapRequest:
				//std::cerr<<"client MapRequest"<<std::endl;
				this->mainClass->mainWindowClass->LFSWM2_removeProp(this->contentWindow,this->mainClass->atomshashed.at(LFSTK_UtilityClass::LFSTK_hashFromKey("_NET_WM_STATE_HIDDEN")));
				this->LFSWM2_showWindow(true);
				this->mainClass->mainEventClass->noRestack=false;
				break;

			case UnmapNotify:
				//fprintf(stderr,"UnmapNotify from client\n");
				this->LFSWM2_hideWindow(true);
				this->mainClass->mainEventClass->noRestack=false;
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
				if(this->holdFrameRect.w==-1)
					return(false);
				this->frameWindowRect=this->holdFrameRect;
				XMoveWindow(this->mainClass->display,this->frameWindow,this->frameWindowRect.x,this->frameWindowRect.y);
				XRaiseWindow(this->mainClass->display,this->frameWindow);
				this->mainClass->mainWindowClass->LFSWM2_setProp(this->mainClass->rootWindow,this->mainClass->atomshashed.at(LFSTK_UtilityClass::LFSTK_hashFromKey("_NET_ACTIVE_WINDOW")),XA_WINDOW,32,&this->contentWindow,1);
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

if(this->windowType==UTILITYWINDOW)
	return;

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
			this->mainClass->mainWindowClass->LFSWM2_setProp(this->mainClass->rootWindow,this->mainClass->atomshashed.at(LFSTK_UtilityClass::LFSTK_hashFromKey("_NET_ACTIVE_WINDOW")),XA_WINDOW,32,(void*)&this->contentWindow,1);
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
	if((this->isBorderless==false) && (this->windowType!=UTILITYWINDOW))
		{
			unsigned long v[10]={(unsigned long)this->mainClass->leftSideBarSize,(unsigned long)this->mainClass->riteSideBarSize,(unsigned long)this->mainClass->titleBarSize,(unsigned long)this->mainClass->bottomBarSize,0,0,0,0,0};
			this->mainClass->mainWindowClass->LFSWM2_setProp(this->contentWindow,this->mainClass->atomshashed.at(LFSTK_UtilityClass::LFSTK_hashFromKey("_NET_FRAME_EXTENTS")),XA_CARDINAL,32,&v,4);
		}
	else
		{
			unsigned long v[10]={0,0,0,0,0,0,0,0,0};
			this->mainClass->mainWindowClass->LFSWM2_setProp(this->contentWindow,this->mainClass->atomshashed.at(LFSTK_UtilityClass::LFSTK_hashFromKey("_NET_FRAME_EXTENTS")),XA_CARDINAL,32,&v,4);
		}
}

long LFSWM2_clientClass::LFSWM2_getWindowState(void)
{
	unsigned long	nitems,bytesafter;
	unsigned char	*prop;
	Atom				actualtype;
	int				actualformat;
	long				state=NormalState;

	if(XGetWindowProperty(this->mainClass->display,this->contentWindow,this->mainClass->atomshashed.at(LFSTK_UtilityClass::LFSTK_hashFromKey("WM_STATE")),0L,2L,False,this->mainClass->atomshashed.at(LFSTK_UtilityClass::LFSTK_hashFromKey("WM_STATE")),&actualtype,&actualformat,&nitems,&bytesafter,&prop)==Success)
		{
			if (nitems>0)
				state=((long *)prop)[0];
			XFree(prop);
		}
	return(state);
}

