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

LFSTK_buttonClass::~LFSTK_buttonClass()
{
}

LFSTK_buttonClass::LFSTK_buttonClass()
{
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
LFSTK_buttonClass::LFSTK_buttonClass(LFSTK_windowClass* parentwc,const char* label,int x,int y,unsigned w,unsigned h,int gravity)
{
	XSetWindowAttributes	wa;

	this->LFSTK_setCommon(parentwc,label,x,y,w,h,gravity);

	wa.win_gravity=gravity;
	wa.save_under=true;

	this->window=XCreateWindow(this->wc->app->display,this->parent,x,y,w,h,0,CopyFromParent,InputOutput,CopyFromParent,CWWinGravity|CWSaveUnder,&wa);
	this->gc=XCreateGC(this->wc->app->display,this->window,0,NULL);

	this->wc->globalLib->LFSTK_setCairoSurface(this->wc->app->display,this->window,this->wc->app->visual,&this->sfc,&this->cr,w,h);
	this->LFSTK_setCairoFontData();

	XSelectInput(this->wc->app->display,this->window,this->gadgetEventMask);

	this->ml->function=&LFSTK_lib::LFSTK_gadgetEvent;
	this->ml->gadget=this;
	this->ml->type=BUTTONGADGET;
	wc->LFSTK_addMappedListener(this->window,ml);

	if(this->wc->globalLib->LFSTK_getUseTheme()==true)
		this->LFSTK_setTile(this->wc->globalLib->LFSTK_getGlobalString(-1,TYPEBUTTONTILE),-1);
	else
		this->useTile=false;

	if(strcmp(this->label,"--")==0)
		gadgetDetails={&this->newGadgetBGColours.at(NORMALCOLOUR),BEVELOUT,NOINDICATOR,NORMALCOLOUR,0,true,{0,0,w,h},{0,0,0,0},false,false,true};
	else
		gadgetDetails={&this->newGadgetBGColours.at(NORMALCOLOUR),BEVELOUT,NOINDICATOR,NORMALCOLOUR,0,true,{0,0,w,h},{0,0,0,0},false,false,false};
}

/**
* Key release callback.
* \param e XKeyEvent passed from mainloop->listener.
* \return Return true if event fully handeled or false to pass it on.
*/
bool LFSTK_buttonClass::keyRelease(XKeyEvent *e)
{
	bool retval=false;

	if(this->callBacks.validCallbacks & KEYRELEASECB)
		retval=this->callBacks.keyReleaseCallback(this,this->callBacks.keyUserData);

	if(this->toParent==true)
		return(false);

	return(retval);
}

/**
* Key press callback.
* \param e XKeyEvent passed from mainloop->listener.
* \return Return true if event fully handeled or false to pass it on.
*/
bool LFSTK_buttonClass::keyPress(XKeyEvent *e)
{
	bool retval=false;

	if(this->callBacks.validCallbacks & KEYPRESSCB)
		retval=this->callBacks.keyPressCallback(this,this->callBacks.keyUserData);

	if(this->toParent==true)
		return(false);

	return(retval);
}
