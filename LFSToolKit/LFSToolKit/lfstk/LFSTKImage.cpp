/*
 *
 * ©K. D. Hedger. Thu  5 Nov 16:40:48 GMT 2015 kdhedger68713@gmail.com

 * This file (LFSTKImage.cpp) is part of LFSToolKit.

 * LFSToolKit is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * at your option) any later version.

 * LFSToolKit is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

 * You should have received a copy of the GNU General Public License
 * along with LFSToolKit.  If not, see <http://www.gnu.org/licenses/>.
 */
 
#include "lfstk/LFSTKGlobals.h"

LFSTK_imageClass::~LFSTK_imageClass()
{
}

LFSTK_imageClass::LFSTK_imageClass()
{
}

void LFSTK_imageClass::LFSTK_clearWindow(void)
{
	if(this->wc->useTile==true) 
		{
			XSetTSOrigin(this->display,this->gc,0-this->gadgetGeom.x,0-this->gadgetGeom.y);
			XSetFillStyle(this->display,this->gc,FillTiled);
			XSetTile(this->display,this->gc,this->wc->tile[0]);
			XFillRectangle(this->display,this->window,this->gc,0,0,this->gadgetGeom.w,this->gadgetGeom.h);
			XSetFillStyle(this->display,this->gc,FillSolid);
		}
	else
		{
			XSetFillStyle(this->display,this->gc,FillSolid);
			XSetForeground(this->display,this->gc,this->wc->windowColourNames[NORMALCOLOUR].pixel);
			XFillRectangle(this->display,this->window,this->gc,0,0,this->gadgetGeom.w,this->gadgetGeom.h);
		}

	if(this->gotIcon==true)
		{
			XSetClipMask(this->display,this->gc,this->icon[1]);
			XSetClipOrigin(this->display,this->gc,0,0);
			XCopyArea(this->display,this->icon[0],this->window,this->gc,0,0,this->iconSize,this->iconSize,0,0);
			XSetClipMask(this->display,this->gc,None);
		}
	else if(this->useImage==true)
		{
			imlib_context_set_display(this->display);
			imlib_context_set_visual(this->visual);
			imlib_context_set_colormap(this->cm);

			imlib_context_set_drawable(this->window);
			imlib_context_set_image(this->image);
			imlib_context_set_blend(1);
			imlib_render_image_on_drawable_at_size(4,(this->gadgetGeom.h/2)-(this->imageHeight/2),this->imageWidth,this->imageHeight); 
		}
}

/**
* Main button constructor.
*
* \param parentwc Main parent window class.
* \param imagepath Path to image.
* \param x X pos.
* \param y Y pos.
* \param w Width.
* \param gravity Button gravity.
*/
LFSTK_imageClass::LFSTK_imageClass(LFSTK_windowClass* parentwc,const char* imagepath,int x,int y,int w,int gravity)
{
	XSetWindowAttributes	wa;
	mappedListener			*ml=new mappedListener;
	this->LFSTK_setCommon(parentwc,imagepath,x,y,w,w,gravity);

	wa.win_gravity=gravity;
	this->window=XCreateWindow(this->display,this->parent,x,y,w,this->gadgetGeom.h,0,CopyFromParent,InputOutput,CopyFromParent,CWWinGravity,&wa);
	XSelectInput(this->display,this->window,ButtonReleaseMask | ButtonPressMask | ExposureMask | EnterWindowMask | LeaveWindowMask);

	ml->function=&LFSTK_lib::LFSTK_gadgetEvent;
	ml->gadget=this;
	ml->type=IMAGEGADGET;
	this->wc->LFSTK_addMappedListener(this->window,ml);

	this->LFSTK_setImageFromPath(imagepath,w,this->gadgetGeom.h);
}
