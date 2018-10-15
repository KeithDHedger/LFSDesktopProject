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
* Set buton style.
* \param s Button style.
*/
void LFSTK_scrollBarClass::LFSTK_setStyle(bevelType s)
{
	this->style=s;
}

/**
* Main button constructor.
*
* \param parentwc Main parent window class.
* \param vertical True=Vertical bar, else horizontal.
* \param x X pos.
* \param y Y pos.
* \param w Width.
* \param h Height.
* \param gravity Button gravity.
*/
LFSTK_scrollBarClass::LFSTK_scrollBarClass(LFSTK_windowClass* parentwc,bool vertical,int x,int y,unsigned w,unsigned h,int gravity)
{
	XSetWindowAttributes	wa;

	this->LFSTK_setCommon(parentwc,NULL,x,y,w,h,gravity);

	wa.win_gravity=gravity;
	wa.save_under=true;

	this->window=XCreateWindow(this->display,this->parent,x,y,w,h,0,CopyFromParent,InputOutput,CopyFromParent,CWWinGravity|CWSaveUnder,&wa);
	this->gc=XCreateGC(this->display,this->window,0,NULL);

	this->wc->globalLib->LFSTK_setCairoSurface(this->display,this->window,this->visual,&this->sfc,&this->cr,w,h);
	this->LFSTK_setCairoFontData();

	XSelectInput(this->display,this->window,ButtonReleaseMask | ButtonPressMask | ExposureMask | EnterWindowMask | LeaveWindowMask|ButtonMotionMask);

	this->ml->function=&LFSTK_lib::LFSTK_gadgetEvent;
	this->ml->gadget=this;
	this->ml->type=SCROLLBARGADGET;
	wc->LFSTK_addMappedListener(this->window,ml);

//	if(this->wc->globalLib->LFSTK_getUseTheme()==true)
//		this->LFSTK_setTile(this->wc->globalLib->LFSTK_getGlobalString(-1,TYPEBUTTONTILE),-1);
//	else
//		this->useTile=false;

	gadgetDetails={&this->colourNames[NORMALCOLOUR],BEVELOUT,NOINDICATOR,NULL,NORMALCOLOUR,0,true,{0,0,w,h},{0,0,0,0},false};

	this->verticalBar=vertical;

	if(this->verticalBar==false)
		{
//thumb
			this->thumb=new LFSTK_buttonClass(parentwc,"",x+h,y,h,h,gravity);
			this->thumb->LFSTK_setCanDrag(true);
			this->thumb->LFSTK_setLimits(x+h,-1,x+w-h,-1);
			this->thumb->LFSTK_allowYMovement(false);
			this->thumb->LFSTK_setCallBack(NULL,this->thumbClicked,(void*)this);
//up
			this->upLeft=new LFSTK_buttonClass(parentwc,"",x,y,h,h,gravity);
			this->upLeft->LFSTK_setCallBack(NULL,this->lineUpDown,(void*)this);
			this->upLeft->userData=(void*)1;
//down
			this->downRight=new LFSTK_buttonClass(parentwc,"",x+w-h,y,h,h,gravity);
			this->downRight->LFSTK_setCallBack(NULL,this->lineUpDown,(void*)this);
			this->downRight->userData=(void*)2;
		}
	else
		{
//thumb
			this->thumb=new LFSTK_buttonClass(parentwc,"",x,y+w,w,w,gravity);
			this->thumb->LFSTK_setCanDrag(true);
			this->thumb->LFSTK_setLimits(-1,y+w,-1,y+h-w);
			this->thumb->LFSTK_allowXMovement(false);
			this->thumb->LFSTK_setCallBack(NULL,this->thumbClicked,(void*)this);
//up
			this->upLeft=new LFSTK_buttonClass(parentwc,"",x,y,w,w,gravity);
			this->upLeft->LFSTK_setCallBack(NULL,this->lineUpDown,(void*)this);
			this->upLeft->userData=(void*)1;
//down
			this->downRight=new LFSTK_buttonClass(parentwc,"",x,y+h-w,w,w,gravity);
			this->downRight->LFSTK_setCallBack(NULL,this->lineUpDown,(void*)this);
			this->downRight->userData=(void*)2;
		}
}

/**
* Clear the gadget window to the appropriate state.
*/
void LFSTK_scrollBarClass::LFSTK_clearWindow()
{
	geometryStruct	geom;

	this->thumb->LFSTK_getGeom(&geom);
	if(this->verticalBar==false)
		this->value=(int)(((geom.x-this->gadgetGeom.x-this->gadgetGeom.h+1)/this->scale)+0.5);
	else
		this->value=(int)(((geom.y-this->gadgetGeom.y-this->gadgetGeom.w)/this->scale)+0.5);

	this->clearBox(&this->gadgetDetails);

	if(this->noCallback==false)
		if(this->callback.releaseCallback!=NULL)
			this->callback.releaseCallback(this,this->callback.userData);
	this->thumb->LFSTK_clearWindow();
	this->noCallback=false;
}

/**
* Set the scale range
* \param int min
* \param int max
*/
void LFSTK_scrollBarClass::LFSTK_setScale(double min,double max)
{
	this->minScale=min;
	this->maxScale=max;
	this->maxValue=this->maxScale-this->minScale;
	if(this->verticalBar==false)
		this->scale=(this->gadgetGeom.w-this->gadgetGeom.h-this->gadgetGeom.h-this->gadgetGeom.h)/(max-min);
	else
		this->scale=(this->gadgetGeom.h-this->gadgetGeom.w-this->gadgetGeom.w-this->gadgetGeom.w)/(max-min);
}

/**
* Set the line up value.
* \param int line jump.
*/
void LFSTK_scrollBarClass::LFSTK_setScrollLine(int line)
{
	this->lineScroll=line;
}

bool LFSTK_scrollBarClass::lineUpDown(void *object,void* userdata)
{
	int						addval=0;
	geometryStruct			geom;
	rectStruct				rect;

	LFSTK_buttonClass		*button=static_cast<LFSTK_buttonClass*>(object);
	LFSTK_scrollBarClass	*sbar=static_cast<LFSTK_scrollBarClass*>(userdata);

	sbar->thumb->LFSTK_getGeom(&geom);
	sbar->thumb->LFSTK_getLimits(&rect);

	if((long)button->userData==1)
		addval=-sbar->lineScroll;

	if((long)button->userData==2)
		addval=sbar->lineScroll;

	if(sbar->verticalBar==false)
		{
			geom.x+=addval;

			if(geom.x<rect.x)
				geom.x=rect.x+1;
			if(geom.x+geom.w>rect.w)
				geom.x=rect.w-geom.w-1;
		}
	else
		{
			geom.y+=addval;

			if(geom.y<rect.y)
				geom.y=rect.y+1;
			if(geom.y+geom.h>rect.h)
				geom.y=rect.h-geom.h;
		}
	sbar->noCallback=false;
	sbar->thumb->LFSTK_moveGadget(geom.x,geom.y);
	return(true);
}

/**
* Mouse up callback.
* \param e XButtonEvent passed from mainloop->listener.
* \return Return true if event fully handeled or false to pass it on.
*/
bool LFSTK_scrollBarClass::mouseUp(XButtonEvent *e)
{
	geometryStruct	geom;
	int				addval=0;
	rectStruct		rect;

	this->mouseDownX=e->x;
	this->mouseDownY=e->y;

	if((this->isActive==false) || (this->callback.ignoreCallback==true))
		return(true);

	if(strcmp(this->label,"--")==0)
		return(true);;

	if(this->inWindow==true)
		{
			this->noCallback=false;
			this->thumb->LFSTK_getGeom(&geom);
			this->thumb->LFSTK_getLimits(&rect);
			addval=(int)((this->maxValue/this->troughScroll)*this->scale);

//fprintf(stderr,"addval=%i\n",addval);

			if(this->verticalBar==false)
				{
					if(this->mouseDownX<(geom.x-rect.x+geom.h))
						geom.x-=addval;
					else
						geom.x+=addval;

					if(geom.x<rect.x)
						geom.x=rect.x+1;
					if(geom.x+geom.w>rect.w)
						geom.x=rect.w-geom.w-1;
				}
			else
				{
					if(this->mouseDownY<(geom.y-rect.y+geom.w))
						geom.y-=addval;
					else
						geom.y+=addval;

					if(geom.y<rect.y)
						geom.y=rect.y+1;
					if(geom.y+geom.h>rect.h)
						geom.y=rect.h-geom.h-1;
				}
			this->thumb->LFSTK_moveGadget(geom.x,geom.y);
		}
	return(true);
}

bool LFSTK_scrollBarClass::thumbClicked(void *object,void* userdata)
{
	LFSTK_scrollBarClass	*sb=NULL;
	if(userdata!=NULL)
		{
			sb=static_cast<LFSTK_scrollBarClass*>(userdata);
			if(sb->callback.releaseCallback!=NULL)
				return(sb->callback.releaseCallback(sb,sb->callback.userData));
		}
}

/**
* Mouse enter callback.
* \param e XButtonEvent passed from mainloop->listener.
* \return Return true if event fully handeled or false to pass it on.
*/
bool LFSTK_scrollBarClass::mouseEnter(XButtonEvent *e)
{
	this->noCallback=true;
	this->inWindow=true;
	return(true);
}
//
/**
* Mouse exit callback.
* \param e XButtonEvent passed from mainloop->listener.
* \return Return true if event fully handeled or false to pass it on.
*/
bool LFSTK_scrollBarClass::mouseExit(XButtonEvent *e)
{
	this->noCallback=true;
	this->inWindow=false;
	return(true);
}





