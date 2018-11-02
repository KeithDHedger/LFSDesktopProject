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

	gadgetDetails={&this->colourNames[NORMALCOLOUR],BEVELOUT,NOINDICATOR,NORMALCOLOUR,0,true,{0,0,w,h},{0,0,0,0},false};

	this->verticalBar=vertical;

	if(this->verticalBar==false)
		{
//thumb
			this->thumb=new LFSTK_buttonClass(parentwc,"",x+h,y,h,h,gravity);
			this->thumb->LFSTK_setLimits(x+h-1,-1,x+w-h,-1);
			this->thumb->LFSTK_allowYMovement(false);
//left
			this->upLeft=new LFSTK_buttonClass(parentwc,"",x,y,h,h,gravity);
			this->upLeft->LFSTK_setImageFromPath(LFSTKPIXMAPSDIR "/left.png",CENTRE,true);
//rite
			this->downRight=new LFSTK_buttonClass(parentwc,"",x+w-h,y,h,h,gravity);
			this->downRight->LFSTK_setImageFromPath(LFSTKPIXMAPSDIR "/rite.png",CENTRE,true);
		}
	else
		{
//thumb
			this->thumb=new LFSTK_buttonClass(parentwc,"",x,y+w,w,w,gravity);
			this->thumb->LFSTK_setLimits(-1,y+w-1,-1,y+h-w);
			this->thumb->LFSTK_allowXMovement(false);
//up
			this->upLeft=new LFSTK_buttonClass(parentwc,"",x,y,w,w,gravity);
			this->upLeft->LFSTK_setImageFromPath(LFSTKPIXMAPSDIR "/up.png",CENTRE,true);
//down
			this->downRight=new LFSTK_buttonClass(parentwc,"",x,y+h-w,w,w,gravity);
			this->downRight->LFSTK_setImageFromPath(LFSTKPIXMAPSDIR "/down.png",CENTRE,true);
		}

	this->thumb->LFSTK_setCanDrag(true);
	this->thumb->LFSTK_setCallBack(this->startThumbDrag,this->thumbClicked,(void*)this);
	this->upLeft->LFSTK_setCallBack(NULL,this->lineUpDown,(void*)this);
	this->downRight->LFSTK_setCallBack(NULL,this->lineUpDown,(void*)this);
	this->downRight->userData=(void*)2;
	this->upLeft->userData=(void*)1;
}

/**
* Set thumb position
* \param bool byvalue.
*/
void LFSTK_scrollBarClass::setState(bool byvalue)
{
	geometryStruct	geom;
	rectStruct		rect;
	int				diff;

	this->thumb->LFSTK_getGeom(&geom);
	this->thumb->LFSTK_getLimits(&rect);

	if(byvalue==true)
		{
			if(this->verticalBar==false)
				{
					geom.x=rect.x+(int)((this->value*this->scale)+0.5);
					if(this->value<this->minScale)
						{
							geom.x=rect.x+1;
							this->value=this->minScale;
						}
					if(this->value>this->maxScale)
						{
							geom.x=rect.w-geom.h;
							this->value=this->maxScale;
						}
					if(geom.x==rect.x)
						geom.x++;
				}
			else
				{
					geom.y=rect.y+(int)((this->value*this->scale)+0.5);
					if(this->value<this->minScale)
						{
							geom.y=rect.y+1;
							this->value=this->minScale;
						}
					if(this->value>this->maxScale)
						{
							geom.y=rect.h-geom.w;
							this->value=this->maxScale;
						}
					if(geom.y==rect.y)
						geom.y++;
				}

			this->thumb->LFSTK_moveGadget(geom.x,geom.y);
		}
	else
		{
			if(this->verticalBar==false)
				diff=geom.x-1-rect.x;
			else
				diff=geom.y-1-rect.y;

			this->value=(int)(double)((diff/this->scale)+0.5);

			if(this->value<this->minScale)
				this->value=this->minScale;
			if(this->value>this->maxScale)
				this->value=this->maxScale;
			return;
		}

	this->LFSTK_clearWindow();
}

/**
* Clear the gadget window to the appropriate state.
*/
void LFSTK_scrollBarClass::LFSTK_clearWindow()
{
	if(this->startDrag==true)
		this->setState(false);

	this->clearBox(&this->gadgetDetails);
	if(this->callback.releaseCallback!=NULL)
		this->callback.releaseCallback(this,this->callback.userData);

//reduce flickering
	if(this->startDrag==true)
		XSync(this->display,true);
	this->thumb->LFSTK_clearWindow();
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
	this->maxValue=this->maxScale-this->minScale+1;
	if(this->verticalBar==false)
		this->scale=(this->gadgetGeom.w-(this->gadgetGeom.h*3))/this->maxValue;
	else
		this->scale=(this->gadgetGeom.h-(this->gadgetGeom.w*3))/this->maxValue;
}

/**
* Set the line scroll value.
* \param int line jump.
*/
void LFSTK_scrollBarClass::LFSTK_setLineScroll(int line)
{
	this->lineScroll=line;
}

/**
* Set the page scroll value.
* \param int page jump.
*/
void LFSTK_scrollBarClass::LFSTK_setPageScroll(int line)
{
	this->pageScroll=line;
}

bool LFSTK_scrollBarClass::lineUpDown(void *object,void* userdata)
{
	LFSTK_buttonClass		*button=static_cast<LFSTK_buttonClass*>(object);
	LFSTK_scrollBarClass	*sbar=static_cast<LFSTK_scrollBarClass*>(userdata);

	sbar->startDrag=false;

	if((long)button->userData==1)
		sbar->value-=sbar->lineScroll;

	if((long)button->userData==2)
		sbar->value+=sbar->lineScroll;

	sbar->setState(true);
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
	rectStruct		rect;

	this->mouseDownX=e->x;
	this->mouseDownY=e->y;

	if((this->isActive==false) || (this->callback.ignoreCallback==true))
		return(true);

	if(strcmp(this->label,"--")==0)
		return(true);;

	if(this->inWindow==true)
		{
			this->thumb->LFSTK_getGeom(&geom);
			this->thumb->LFSTK_getLimits(&rect);

			if(this->verticalBar==false)
				{
					if((this->mouseDownX+geom.w)<geom.x-rect.y)
						this->value-=this->pageScroll;
					else
						this->value+=this->pageScroll;
				}
			else
				{
					if((this->mouseDownY-geom.h)<geom.y-rect.y)
						this->value-=this->pageScroll;
					else
						this->value+=this->pageScroll;
				}
			this->setState(true);
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
				sb->callback.releaseCallback(sb,sb->callback.userData);
			sb->startDrag=false;
		}
	return(true);
}

bool LFSTK_scrollBarClass::startThumbDrag(void *object,void* userdata)
{
	LFSTK_scrollBarClass	*sb=NULL;
	if(userdata!=NULL)
		{
			sb=static_cast<LFSTK_scrollBarClass*>(userdata);
			sb->startDrag=true;
		}
	return(true);
}

/**
* Mouse enter callback.
* \param e XButtonEvent passed from mainloop->listener.
* \return Return true if event fully handeled or false to pass it on.
*/
bool LFSTK_scrollBarClass::mouseEnter(XButtonEvent *e)
{
	//this->noCallback=true;????
	this->inWindow=true;
	this->startDrag=false;
	return(true);
}

/**
* Mouse exit callback.
* \param e XButtonEvent passed from mainloop->listener.
* \return Return true if event fully handeled or false to pass it on.
*/
bool LFSTK_scrollBarClass::mouseExit(XButtonEvent *e)
{
	//this->noCallback=true;????
	this->inWindow=false;
	this->startDrag=false;
	return(true);
}

bool LFSTK_scrollBarClass::mouseDown(XButtonEvent *e)
{

	if((this->isActive==false) || (this->callback.ignoreCallback==true))
		return(true);

	this->inWindow=true;
	this->startDrag=false;
	return(true);
}




