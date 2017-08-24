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
	XFreePixmap(this->display,shape);
	cairo_surface_destroy(this->shapesfc);
	cairo_destroy(this->shapecr);
}

LFSTK_imageClass::LFSTK_imageClass()
{
}

/**
* Mouse down callback.
* \param e XButtonEvent passed from mainloop->listener.
* \return Return true if event fully handeled or false to pass it on.
*/
bool LFSTK_imageClass::mouseDown(XButtonEvent *e)
{
	this->mouseDownX=e->x;
	this->mouseDownY=e->y;
	return(true);
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
			if(this->callback.releaseCallback!=NULL)
				return(this->callback.releaseCallback(this,this->callback.userData));
		}
	return(true);
}

/**
* Mouse drag callback.
* \param e XMotionEvent passed from mainloop->listener.
* \return Return true if event fully handeled or false to pass it on.
*/
bool LFSTK_imageClass::mouseDrag(XMotionEvent *e)
{
	if(this->canDrag==true)
		{
			this->gadgetGeom.x+=e->x-this->mouseDownX;
			this->gadgetGeom.y+=e->y-this->mouseDownY;
			if(this->snap>1)
				{
					this->gadgetGeom.x=(this->gadgetGeom.x/this->snap)*this->snap;
					this->gadgetGeom.y=(this->gadgetGeom.y/this->snap)*this->snap;
				}
			XMoveWindow(this->display,this->window,this->gadgetGeom.x,this->gadgetGeom.y);
		}
	return(true);
}

/**
* Set if image can be dragged
*
* \param candrag draggable.
*/
void LFSTK_imageClass::LFSTK_setCanDrag(bool candrag)
{
	this->canDrag=candrag;
}

/**
* Get if image can be dragged
*
* \return Draggable.
*/
bool LFSTK_imageClass::LFSTK_getCanDrag(void)
{
	return(this->canDrag);
}

/**
* Set snap to grid size
*
* \param int sze.
*/
void LFSTK_imageClass::LFSTK_snapSize(int sze)
{
	this->snap=sze;
}

/**
* Clear the image window ans set shape.
*/
void LFSTK_imageClass::LFSTK_clearWindow(void)
{
	cairo_save(this->cr);
		cairo_reset_clip(this->cr);
		cairo_set_source_surface(this->cr,this->cImage,0,0);
		cairo_paint(this->cr);
	cairo_restore(this->cr);

	cairo_save(this->shapecr);
		cairo_reset_clip(this->shapecr);
		cairo_set_operator(this->shapecr,CAIRO_OPERATOR_CLEAR);
		cairo_rectangle(this->shapecr,0,0,this->gadgetGeom.w,this->gadgetGeom.h);
		cairo_fill(this->shapecr);
		cairo_set_operator(this->shapecr,CAIRO_OPERATOR_OVER);
		cairo_mask_surface(this->shapecr,this->cImage,0,0);
	cairo_restore(this->shapecr);

	XShapeCombineMask(this->display,this->window,ShapeBounding,0,0,cairo_xlib_surface_get_drawable(shapesfc),ShapeSet);
	XFlush(this->display);
	XSync(this->display,false);
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
	mappedListener			*ml=new mappedListener;
	this->LFSTK_setCommon(parentwc,imagepath,x,y,w,h,gravity);

	wa.win_gravity=gravity;
	wa.save_under=true;
	this->window=XCreateWindow(this->display,this->parent,x,y,w,this->gadgetGeom.h,0,CopyFromParent,InputOutput,CopyFromParent,CWWinGravity|CWSaveUnder,&wa);
	this->gc=XCreateGC(this->display,this->window,0,NULL);
	this->wc->globalLib->LFSTK_setCairoSurface(this->display,this->window,this->visual,&this->sfc,&this->cr,w,h);
	this->LFSTK_setCairoFontData();
	XSelectInput(this->display,this->window,ButtonPressMask|ButtonReleaseMask|ExposureMask|ButtonMotionMask);

	ml->function=&LFSTK_lib::LFSTK_gadgetEvent;
	ml->gadget=this;
	ml->type=IMAGEGADGET;
	this->wc->LFSTK_addMappedListener(this->window,ml);

	this->LFSTK_setImageFromPath(imagepath,gravity,scale);
	this->useTile=this->wc->useTile;
	gadgetDetails={&this->wc->windowColourNames[NORMALCOLOUR],BEVELNONE,NOINDICATOR,NULL,NORMALCOLOUR,0,false,{0,0,w,h},{0,0,0,0},false};

	this->shape=XCreatePixmap(this->display,this->window,w,h,1);
	this->shapesfc=cairo_xlib_surface_create_for_bitmap(this->display,this->shape,DefaultScreenOfDisplay(this->display),w,h);
	this->shapecr=cairo_create(shapesfc);
}
