/*
 *
 * ©K. D. Hedger. Fri 28 Aug 19:17:00 BST 2015 keithdhedger@gmail.com

 * This file (LFSTKLabel.cpp) is part of LFSToolKit.

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

LFSTK_labelClass::~LFSTK_labelClass()
{
}

/**
* Main Label constructor.
*
* \param parentwc Main parent window class.
* \param label Displayed name.
* \param x X pos.
* \param y Y pos.
* \param w Width.
* \param h Height.
* \param gravity Label gravity
* \note optional, label gravity defaults to CENTRE.
* \note gadget gravity defaults to NorthWestGravity.
*/
LFSTK_labelClass::LFSTK_labelClass(LFSTK_windowClass* parentwc,const char* label,int x,int y,unsigned w,unsigned h,int gravity,int bgrav)
{
	XSetWindowAttributes	wa;

	this->LFSTK_setCommon(parentwc,label,x,y,w,h,gravity);

	wa.win_gravity=bgrav;
	wa.save_under=true;
	wa.border_pixel=0;
	wa.background_pixel=0;

	XVisualInfo vinfo;
	XMatchVisualInfo(this->wc->app->display, DefaultScreen(this->wc->app->display), 32, TrueColor, &vinfo);

	this->visual=vinfo.visual;
	this->cmap=XCreateColormap(this->wc->app->display,DefaultRootWindow(this->wc->app->display),this->visual,AllocNone);
	wa.colormap=this->cmap;

	this->window=XCreateWindow(this->wc->app->display,this->parent,x,y,w,h,0,vinfo.depth,InputOutput,this->visual,(CWColormap | CWBorderPixel| CWBackPixel |CWWinGravity),&wa);

	this->gc=XCreateGC(this->wc->app->display,this->window,0,NULL);
	this->wc->globalLib->LFSTK_setCairoSurface(this->wc->app->display,this->window,this->visual,&this->sfc,&this->cr,w,h);
	this->LFSTK_setCairoFontData();
	XSelectInput(this->wc->app->display,this->window,ButtonReleaseMask | ButtonPressMask | ExposureMask | EnterWindowMask | LeaveWindowMask);

	this->style=BEVELNONE;

	this->ml->function=&LFSTK_lib::LFSTK_gadgetEvent;
	this->ml->gadget=this;
	this->ml->type=LABELGADGET;
	this->gadgetType=LABELGADGET;
	this->wc->LFSTK_addMappedListener(this->window,ml);

	if(this->wc->globalLib->LFSTK_getUseTheme()==true)
		this->LFSTK_setTile(this->wc->globalLib->LFSTK_getGlobalString(-1,TYPEWINDOWTILE),-1);
	else
		this->useTile=false;

/*
struct gadgetStruct
{
	colourStruct			*colour=NULL;
	bevelType			bevel=BEVELNONE;
	indicatorType		indic=NOINDICATOR;
	int					state=NORMALCOLOUR;
	int					reserveSpace=0;
	bool					buttonTile=false;
	geometryStruct		gadgetGeom={0,0,1,1};
	geometryStruct		indicatorGeom={0,0,1,1};
	bool					hasIndicator=false;
	bool					useWindowPixmap=false;
	bool					geomRelativeToMainWindow=false;
	bool					showLink=false;
	bool					showBroken=false;
};
*/
	gadgetDetails={&this->wc->windowColourNames[NORMALCOLOUR],BEVELNONE,NOINDICATOR,NORMALCOLOUR,0,false,{0,0,w,h},{0,0,0,0},false,false,true};
	this->LFSTK_setLabelGravity(gravity);
//	fprintf(stderr,"w=%i\n",gadgetGeom.w);
//this->gadgetDetails.gadgetGeom.w=1000;
//this->gadgetDetails.gadgetGeom.h=h;

}
