/*
 *
 * ©K. D. Hedger. Fri 28 Aug 19:17:00 BST 2015 kdhedger68713@gmail.com

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

//#include "LFSTKLabel.h"
#include "LFSTKGlobals.h"

LFSTK_labelClass::~LFSTK_labelClass()
{
}

LFSTK_labelClass::LFSTK_labelClass()
{
}

/**
* Clear the gadget window to the appropriate state.
* \note Label background is set to the window normal colour unless the label is set as inactive,
* \note in which case the label's inactive colour is used.
*/
void LFSTK_labelClass::LFSTK_clearWindow(void)
{
	if(this->wc->useTile==true)
		{
			XSetTSOrigin(this->display,this->gc,0-this->x,0-this->y);
			XSetFillStyle(this->display,this->gc,FillTiled);
			XSetTile(this->display,this->gc,this->wc->tile[0]);
			XFillRectangle(this->display,this->window,this->gc,0,0,this->w,this->h);
			XSetFillStyle(this->display,this->gc,FillSolid);
		}
	else
		{
			XSetFillStyle(this->display,this->gc,FillSolid);
			XSetClipMask(this->display,this->gc,None);
			if(this->isActive==true)
				XSetForeground(this->display,this->gc,this->wc->windowColourNames[NORMALCOLOUR].pixel);
			else
				XSetForeground(this->display,this->gc,this->colourNames[INACTIVECOLOUR].pixel);
			XFillRectangle(this->display,this->window,this->gc,0,0,this->w,this->h);
		}

	if(this->isActive==true)
		this->LFSTK_drawLabel(NORMALCOLOUR);
	else
		this->LFSTK_drawLabel(INACTIVECOLOUR);
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
* \param gravity Button gravity.
*/
LFSTK_labelClass::LFSTK_labelClass(LFSTK_windowClass* parentwc,const char* label,int x,int y,int w,int h,int gravity)
{
	XSetWindowAttributes	wa;

	this->LFSTK_setCommon(parentwc,label,x,y,w,h,gravity);

	wa.win_gravity=gravity;
	this->window=XCreateWindow(this->display,this->parent,x,y,w,h,0,CopyFromParent,InputOutput,CopyFromParent,CWWinGravity,&wa);
	XSelectInput(this->display,this->window,ButtonReleaseMask | ButtonPressMask | ExposureMask | EnterWindowMask | LeaveWindowMask);

	this->style=BEVELNONE;

	this->listen.function=&LFSTK_lib::LFSTK_gadgetEvent;
	this->listen.pointer=this;
	this->listen.type=LABELGADGET;
	this->wc->LFSTK_setListener(this->window,this->getListen());

	this->LFSTK_setActive(true);
	this->LFSTK_setFontColourName(NORMALCOLOUR,this->wc->globalLib->LFSTK_getGlobalString(NORMALCOLOUR,TYPEFONTCOLOUR));
	this->LFSTK_setColourName(NORMALCOLOUR,this->wc->globalLib->LFSTK_getGlobalString(NORMALCOLOUR,TYPEWINDOW));

	if(this->wc->globalLib->LFSTK_getUseTheme()==true)
		this->LFSTK_setTile(this->wc->globalLib->LFSTK_getGlobalString(-1,TYPEWINDOWTILE),-1);
	else
		this->useTile=false;

}
