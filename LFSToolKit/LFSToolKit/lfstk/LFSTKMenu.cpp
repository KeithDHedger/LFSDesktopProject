/*
 *
 * ©K. D. Hedger. Sun  4 Nov 19:46:07 GMT 2018 keithdhedger@gmail.com

 * This file (LFSTKMenu.cpp) is part of LFSToolKit.

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

LFSTK_menuClass::~LFSTK_menuClass()
{
}

/**
* Main menuClass constructor.
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
LFSTK_menuClass::LFSTK_menuClass(LFSTK_windowClass* parentwc,const char* label,int x,int y,unsigned w,unsigned h,int gravity,int bgrav)
{
	XSetWindowAttributes	wa;

	this->LFSTK_setCommon(parentwc,label,x,y,w,h,gravity);

	wa.win_gravity=bgrav;
	wa.save_under=true;
	this->window=XCreateWindow(this->display,this->parent,x,y,w,h,0,CopyFromParent,InputOutput,CopyFromParent,CWWinGravity,&wa);
	this->gc=XCreateGC(this->display,this->window,0,NULL);
	this->wc->globalLib->LFSTK_setCairoSurface(this->display,this->window,this->visual,&this->sfc,&this->cr,w,h);
	this->LFSTK_setCairoFontData();
	XSelectInput(this->display,this->window,ButtonReleaseMask | ButtonPressMask | ExposureMask | EnterWindowMask | LeaveWindowMask);

	//this->style=BEVELNONE;

	this->ml->function=&LFSTK_lib::LFSTK_gadgetEvent;
	this->ml->gadget=this;
	this->ml->type=MENUGADGET;
	this->wc->LFSTK_addMappedListener(this->window,ml);

	if(this->wc->globalLib->LFSTK_getUseTheme()==true)
		this->LFSTK_setTile(this->wc->globalLib->LFSTK_getGlobalString(-1,TYPEBUTTONTILE),-1);
	else
		this->useTile=false;

	this->LFSTK_setFontColourName(0,this->wc->globalLib->LFSTK_getGlobalString(0,TYPEMENUITEMFONTCOLOUR),true);
	gadgetDetails={&this->wc->windowColourNames[NORMALCOLOUR],BEVELIN,NOINDICATOR,NORMALCOLOUR,0,true,{0,0,w,h},{0,0,0,0},false};
//	gadgetDetails={&this->colourNames[NORMALCOLOUR],BEVELOUT,NOINDICATOR,NORMALCOLOUR,0,true,{0,0,w,h},{0,0,0,0},false};

//	this->LFSTK_setLabelGravity(gravity);
}

