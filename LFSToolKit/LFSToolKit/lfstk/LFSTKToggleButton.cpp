/*
 *
 * ©K. D. Hedger. Thu 17 Sep 11:54:58 BST 2015 kdhedger68713@gmail.com

 * This file (LFSTKToggleButton.cpp) is part of LFSToolKit.

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
 
#include "LFSTKGlobals.h"

LFSTK_toggleButtonClass::~LFSTK_toggleButtonClass()
{
}

LFSTK_toggleButtonClass::LFSTK_toggleButtonClass()
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
LFSTK_toggleButtonClass::LFSTK_toggleButtonClass(LFSTK_windowClass* parentwc,const char* label,int x,int y,int w,int h,int gravity)
{
	XSetWindowAttributes	wa;

	this->LFSTK_setCommon(parentwc,label,x,y,w,h,gravity);

	wa.win_gravity=gravity;
	this->window=XCreateWindow(this->display,this->parent,x,y,w,h,0,CopyFromParent,InputOutput,CopyFromParent,CWWinGravity,&wa);
	XSelectInput(this->display,this->window,ButtonReleaseMask | ButtonPressMask | ExposureMask | EnterWindowMask | LeaveWindowMask);

	this->listen.function=&LFSTK_lib::LFSTK_gadgetEvent;
	this->listen.pointer=this;
	this->listen.type=TOGGLEGADGET;

	this->wc->LFSTK_setListener(this->window,this->getListen());
	this->boxStyle=TOGGLECHECK;

	this->toggleState=false;
	this->labelOffset=(this->h/2);
	this->LFSTK_setLabelOriention(LEFT);
		if(this->wc->globalLib->LFSTK_getUseTheme()==true)
		this->LFSTK_setTile(this->wc->globalLib->LFSTK_getGlobalString(-1,TYPEBUTTONTILE),-1);
	else
		this->useTile=false;
}

/**
* Draw a togglebox.
*/
void LFSTK_toggleButtonClass::drawButton(gadgetState state)
{
	bevelType bv;
	geometryStruct	*g=new geometryStruct;

	if(this->toggleState==true)
		{
			bv=BEVELIN;
			if(state==ACTIVECOLOUR)
				bv=BEVELOUT;
		}
	else
		{
			bv=BEVELOUT;
			if(state==ACTIVECOLOUR)
				bv=BEVELIN;
		}

	if(this->boxStyle==TOGGLECHECK)
		{
			g->x=0;
			g->w=(this->h/2);
			g->h=(this->h/2);
			g->y=g->h-(g->h/2)-1;
			this->LFSTK_setLabelOriention(LEFT);
			this->labelOffset=(this->h/2);

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
					XSetForeground(this->display,this->gc,this->wc->windowColourNames[NORMALCOLOUR].pixel);
					XFillRectangle(this->display,this->window,this->gc,0,0,this->w,this->h);
				}

			this->drawBox(g,state,bv);

			g->x=2;
			g->y=g->y+2;
			g->w=g->w-4;
			g->h=g->h-4;
			if(this->toggleState==false)
				{
					if(state==ACTIVECOLOUR)
						this->drawIndicator(g,state,CHECK);
				}
			else
				{
					if(state==NORMALCOLOUR||state==PRELIGHTCOLOUR||state==INACTIVECOLOUR)
						this->drawIndicator(g,state,CHECK);
				}
		}	

	if(this->boxStyle==TOGGLENORMAL)
		{
			g->x=0;
			g->y=0;
			g->w=this->w;
			g->h=this->h;
			this->LFSTK_setLabelOriention(CENTRE);
			this->labelOffset=2;
			this->drawBox(g,state,bv);
		}

	if(this->isActive==true)
		this->LFSTK_drawLabel(NORMALCOLOUR);
	else
		this->LFSTK_drawLabel(INACTIVECOLOUR);

	delete g;
}

/**
* Clear the gadget window to the appropriate state.
*/
void LFSTK_toggleButtonClass::LFSTK_clearWindow()
{
	XSetFillStyle(this->display,this->gc,FillSolid);
	XSetClipMask(this->display,this->gc,None);

	XSetForeground(this->display,this->gc,this->wc->windowColourNames[NORMALCOLOUR].pixel);

	XFillRectangle(this->display,this->window,this->gc,0,0,this->w,this->h);

	if(this->isActive==false)
		this->drawButton(INACTIVECOLOUR);
	else
		{
			if(this->inWindow==false)
				this->drawButton(NORMALCOLOUR);
			else
				this->drawButton(PRELIGHTCOLOUR);
		}
}

/**
* Mouse enter callback.
* \param e XButtonEvent passed from mainloop->listener.
* \return Return true if event fully handeled or false to pass it on.
*/
bool LFSTK_toggleButtonClass::mouseEnter(XButtonEvent *e)
{
	if(this->isActive==false)
		{
			this->LFSTK_clearWindow();
			return(true);
		}

	this->LFSTK_clearWindow();
	this->drawButton(PRELIGHTCOLOUR);
	this->inWindow=true;
	return(true);
}

/**
* Mouse exit callback.
* \param e XButtonEvent passed from mainloop->listener.
* \return Return true if event fully handeled or false to pass it on.
*/
bool LFSTK_toggleButtonClass::mouseExit(XButtonEvent *e)
{
	if(this->isActive==false)
		{
			this->LFSTK_clearWindow();
			return(true);
		}

	this->LFSTK_clearWindow();
	this->drawButton(NORMALCOLOUR);
	this->inWindow=false;
	return(true);
}

/**
* Mouse down callback.
* \param e XButtonEvent passed from mainloop->listener.
* \return Return true if event fully handeled or false to pass it on.
*/
bool LFSTK_toggleButtonClass::mouseDown(XButtonEvent *e)
{
	if(this->isActive==false)
		{
			this->LFSTK_clearWindow();
			return(true);
		}

	this->LFSTK_clearWindow();
	this->drawButton(ACTIVECOLOUR);
	
	if(this->callback.pressCallback!=NULL)
		return(this->callback.pressCallback(this,this->callback.userData));
	return(true);
}

/**
* Mouse up callback.
* \param e XButtonEvent passed from mainloop->listener.
* \return Return true if event fully handeled or false to pass it on.
*/
bool LFSTK_toggleButtonClass::mouseUp(XButtonEvent *e)
{
	if(this->isActive==false)
		{
			this->LFSTK_clearWindow();
			return(true);
		}

	if(this->inWindow==false)
		this->LFSTK_clearWindow();
	else
		{
			this->toggleState=!this->toggleState;
			this->mouseEnter(e);
			if(this->callback.releaseCallback!=NULL)
				return(this->callback.releaseCallback(this,this->callback.userData));
		}
	return(true);
}

/**
* Set the toggle button style.
* \param drawStyle Style=TOGGLECHECK=0,TOGGLERADIO=1,TOGGLENORMAL=2.
*/
void LFSTK_toggleButtonClass::LFSTK_setToggleStyle(drawStyle ds)
{
	this->boxStyle=ds;
	if(ds==TOGGLENORMAL)
		this->labelOffset=0;
}

/**
* Set the toggle button value.
* \param bool
*/
void LFSTK_toggleButtonClass::LFSTK_setValue(bool val)
{
	this->toggleState=val;
}

/**
* Set the toggle button value.
* \param bool
*/
bool LFSTK_toggleButtonClass::LFSTK_getValue(void)
{
	return(this->toggleState);
}






