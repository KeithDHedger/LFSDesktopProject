/*
 *
 * ©K. D. Hedger. Fri 24 Jul 10:55:09 BST 2015 keithdhedger@gmail.com

 * This file (LFSTKButton.cpp) is part of LFSToolKit.

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
#include <unistd.h>
#include <stdlib.h>

#include "lfstk/LFSTKGlobals.h"

LFSTK_scrollBarClass::~LFSTK_scrollBarClass()
{
}

LFSTK_scrollBarClass::LFSTK_scrollBarClass()
{
}

/**
* Return callback user data.
* \note Returns a void* so must be cast.
* \note return value must not be freed.
*/
void* LFSTK_scrollBarClass::LFSTK_getCallbackUD()
{
	return(this->callback.userData);
}

/**
* Set buton style.
* \param s Button style.
*/
void LFSTK_scrollBarClass::LFSTK_setStyle(bevelType s)
{
	this->style=s;
}

///**
//* Ignore callbacks.
//* \param ignore.
//*/
void LFSTK_scrollBarClass::LFSTK_setIgnoreCB(bool ignore)
{
	this->callback.ignoreCallback=ignore;
}

/**
* Get the current ignore callbacks state.
*/
bool LFSTK_scrollBarClass::LFSTK_getIgnoreCB(void)
{
	return(this->callback.ignoreCallback);
}

/**
* Main button constructor.
*
* \param parentwc Main parent window class.
* \param label Displayed name.
* \param x X pos.
* \param y Y pos.
* \param w Width.
* \param h Height.
* \param gravity Button gravity.
*/
LFSTK_scrollBarClass::LFSTK_scrollBarClass(LFSTK_windowClass* parentwc,const char* label,int x,int y,unsigned w,unsigned h,int gravity)
{
	XSetWindowAttributes	wa;

	this->LFSTK_setCommon(parentwc,label,x,y,w,h,gravity);

	wa.win_gravity=gravity;
	wa.save_under=true;

	this->window=XCreateWindow(this->display,this->parent,x,y,w,h,0,CopyFromParent,InputOutput,CopyFromParent,CWWinGravity|CWSaveUnder,&wa);
	this->gc=XCreateGC(this->display,this->window,0,NULL);

	this->wc->globalLib->LFSTK_setCairoSurface(this->display,this->window,this->visual,&this->sfc,&this->cr,w,h);
	this->LFSTK_setCairoFontData();

	XSelectInput(this->display,this->window,ButtonReleaseMask | ButtonPressMask | ExposureMask | EnterWindowMask | LeaveWindowMask|ButtonMotionMask);

	this->ml->function=&LFSTK_lib::LFSTK_gadgetEvent;
	this->ml->gadget=this;
	this->ml->type=BUTTONGADGET;
	wc->LFSTK_addMappedListener(this->window,ml);

	if(this->wc->globalLib->LFSTK_getUseTheme()==true)
		this->LFSTK_setTile(this->wc->globalLib->LFSTK_getGlobalString(-1,TYPEBUTTONTILE),-1);
	else
		this->useTile=false;

	gadgetDetails={&this->colourNames[NORMALCOLOUR],BEVELOUT,NOINDICATOR,NULL,NORMALCOLOUR,0,true,{0,0,w,h},{0,0,0,0},false};
}
