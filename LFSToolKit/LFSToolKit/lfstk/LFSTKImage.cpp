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
			cairo_save(this->cr);
				cairo_translate(this->cr,0,0);
				//cairo_scale(this->cr,this->scaleX,this->scaleY);
				cairo_set_source_surface(this->cr,this->cImage,0,0);
				cairo_paint(this->cr);
			cairo_restore(this->cr);
		}
}

#if 0
/**
* Set image and render with cairo.
* \param file Path to image file.
* \param w,h Size of image.
*/
cairo_status_t LFSTK_imageClass::LFSTK_setImageFromPathX(const char *file,int w,int h)
{
	cairo_status_t cs=CAIRO_STATUS_SUCCESS;

	this->sfc=cairo_xlib_surface_create(this->display,this->window,this->visual,w,h);
	cairo_xlib_surface_set_size(sfc,w,h);
	this->cr=cairo_create(sfc);
	this->cImage=cairo_image_surface_create_from_png(file);
	cs=cairo_surface_status (this->cImage);
	if(cs==CAIRO_STATUS_SUCCESS)
		{
			this->scaleX=(float)((float)this->gadgetGeom.w/(float)cairo_image_surface_get_width(this->cImage));
			this->scaleY=(float)((float)this->gadgetGeom.h/(float)cairo_image_surface_get_height(this->cImage));
			this->useImage=true;
		}
	else	
		printf("File %s error - Error:%s\n",file,cairo_status_to_string(cs));
	return(cs);
}
#endif

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

	this->LFSTK_setImageFromPath(imagepath,AUTO);
}
