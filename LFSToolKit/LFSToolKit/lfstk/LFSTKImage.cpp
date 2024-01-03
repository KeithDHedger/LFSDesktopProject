/*
 *
 * ©K. D. Hedger. Thu  5 Nov 16:40:48 GMT 2015 keithdhedger@gmail.com

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
	cairo_surface_destroy(this->shapesfc);
	cairo_destroy(this->shapecr);
	XFreePixmap(this->wc->app->display,this->shape);
}

LFSTK_imageClass::LFSTK_imageClass()
{
}

/**
* Mouse up callback.
* \param e XButtonEvent passed from mainloop->listener.
* \return Return true if event fully handeled or false to pass it on.
*/
bool LFSTK_imageClass::mouseUp(XButtonEvent *e)
{
	pointStruct 	pt;

	pt.x=this->gadgetGeom.x+e->x;
	pt.y=this->gadgetGeom.y+e->y;

	this->LFSTK_clearWindow();

	if(this->wc->globalLib->LFSTK_pointInRect(&pt,&(this->gadgetGeom))==true)
		{
			if(this->callBacks.validCallbacks & MOUSERELEASECB)
				return(this->callBacks.mouseReleaseCallback(this,this->callBacks.mouseUserData));
		}
	return(true);
}

/**
* Clear the image window and set shape.
*/
void LFSTK_imageClass::LFSTK_clearWindow(void)
{
	int xoffset;
	int yoffset;

	if(this->useImage==false)
		{
			cairo_save(this->shapecr);
				cairo_reset_clip(this->shapecr);
				cairo_set_operator(this->shapecr,CAIRO_OPERATOR_CLEAR);
				cairo_rectangle(this->shapecr,0,0,this->gadgetGeom.w,this->gadgetGeom.h);
				cairo_fill(this->shapecr);
			cairo_restore(this->shapecr);

			XShapeCombineMask(this->wc->app->display,this->window,ShapeBounding,0,0,cairo_xlib_surface_get_drawable(shapesfc),ShapeSet);
			return;
		}

	xoffset=(this->gadgetGeom.w/2)-(this->imageWidth/2);
	yoffset=(this->gadgetGeom.h/2)-(this->imageHeight/2);

	cairo_save(this->cr);
		cairo_set_source_rgba (this->cr,1.0,1.0,1.0,this->alpha);
		cairo_rectangle(this->cr,0,0,this->gadgetGeom.w,this->gadgetGeom.h);
		cairo_fill(this->cr);
	cairo_restore(this->cr);

	cairo_save(this->shapecr);
		cairo_reset_clip(this->shapecr);
		cairo_set_source_surface(this->cr,this->cImage,xoffset,yoffset);
		cairo_set_operator(this->cr,CAIRO_OPERATOR_SOURCE);
		cairo_paint_with_alpha(this->cr,this->alpha);
		cairo_set_operator(this->shapecr,CAIRO_OPERATOR_CLEAR);
		cairo_rectangle(this->shapecr,0,0,this->gadgetGeom.w,this->gadgetGeom.h);
		cairo_fill(this->shapecr);
		cairo_set_operator(this->shapecr,CAIRO_OPERATOR_OVER);
		cairo_mask_surface(this->shapecr,this->cImage,xoffset,yoffset);
	cairo_restore(this->shapecr);

	XShapeCombineMask(this->wc->app->display,this->window,ShapeBounding,0,0,cairo_xlib_surface_get_drawable(shapesfc),ShapeSet);
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
LFSTK_imageClass::LFSTK_imageClass(LFSTK_windowClass* parentwc,const char* imagepath,int x,int y,unsigned w,unsigned h,int gravity,bool scale)
{
	XSetWindowAttributes	wa;

	this->LFSTK_setCommon(parentwc,imagepath,x,y,w,h,gravity);

	wa.win_gravity=gravity;
	wa.save_under=true;
	//this->window=XCreateWindow(this->wc->app->display,this->parent,x,y,w,this->gadgetGeom.h,0,CopyFromParent,InputOutput,CopyFromParent,CWWinGravity|CWSaveUnder,&wa);

	this->window=XCreateWindow(this->wc->app->display,this->parent,x,y,w,this->gadgetGeom.h,0,CopyFromParent,InputOutput,CopyFromParent,CWWinGravity|CWSaveUnder,&wa);

	this->gc=XCreateGC(this->wc->app->display,this->window,0,NULL);
	this->wc->globalLib->LFSTK_setCairoSurface(this->wc->app->display,this->window,this->wc->app->visual,&this->sfc,&this->cr,w,h);
	this->LFSTK_setCairoFontData();
	XSelectInput(this->wc->app->display,this->window,ButtonPressMask|ButtonReleaseMask|ExposureMask|ButtonMotionMask);

	this->ml->function=&LFSTK_lib::LFSTK_gadgetEvent;
	this->ml->gadget=this;
	this->ml->type=IMAGEGADGET;
	this->wc->LFSTK_addMappedListener(this->window,ml);
	if(imagepath!=NULL)
		this->LFSTK_setImageFromPath(std::string(imagepath),gravity,scale);

	this->useTile=this->wc->useTile;
	this->gadgetDetails={&this->wc->windowColourNames[NORMALCOLOUR],BEVELNONE,NOINDICATOR,NORMALCOLOUR,0,false,{0,0,w,h},{0,0,0,0},false,false,false};

	this->shape=XCreatePixmap(this->wc->app->display,this->window,w,h,1);
	this->shapesfc=cairo_xlib_surface_create_for_bitmap(this->wc->app->display,this->shape,DefaultScreenOfDisplay(this->wc->app->display),w,h);
	this->shapecr=cairo_create(this->shapesfc);
}
