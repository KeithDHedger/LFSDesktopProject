/*
 *
 * ©K. D. Hedger. Thu 17 Sep 11:54:58 BST 2015 keithdhedger@gmail.com

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

#include <unistd.h>

#include "lfstk/LFSTKGlobals.h"

LFSTK_toggleButtonClass::~LFSTK_toggleButtonClass()
{
	if(this->checkOn!=NULL)
		cairo_surface_destroy(this->checkOn);
	if(this->checkOff!=NULL)
		cairo_surface_destroy(this->checkOff);
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
LFSTK_toggleButtonClass::LFSTK_toggleButtonClass(LFSTK_windowClass* parentwc,const char* label,int x,int y,unsigned w,unsigned h,int gravity)
{
	XSetWindowAttributes	wa;
	char					*pathtobit;

	this->LFSTK_setCommon(parentwc,label,x,y,w,h,gravity);

	wa.win_gravity=gravity;
	wa.save_under=true;
	this->window=XCreateWindow(this->wc->app->display,this->parent,x,y,w,h,0,CopyFromParent,InputOutput,CopyFromParent,CWWinGravity,&wa);
	this->gc=XCreateGC(this->wc->app->display,this->window,0,NULL);
	this->wc->globalLib->LFSTK_setCairoSurface(this->wc->app->display,this->window,this->wc->app->visual,&this->sfc,&this->cr,w,h);
	this->LFSTK_setCairoFontData();
	XSelectInput(this->wc->app->display,this->window,this->gadgetEventMask);

	this->ml->function=&LFSTK_lib::LFSTK_gadgetEvent;
	this->ml->gadget=this;
	this->ml->type=TOGGLEGADGET;
	this->wc->LFSTK_addMappedListener(this->window,ml);

	this->boxStyle=TOGGLECHECK;

	this->toggleState=false;
	this->labelOffset=(this->gadgetGeom.h/2);
	this->LFSTK_setLabelGravity(LEFT);
	if(this->wc->globalLib->LFSTK_getUseTheme()==true)
		this->LFSTK_setTile(this->wc->globalLib->LFSTK_getGlobalString(-1,TYPEBUTTONTILE),-1);
	else
		this->useTile=false;

	this->style=BEVELNONE;

	asprintf(&pathtobit,"%s/checkon.png",this->wc->globalLib->LFSTK_getThemePath());
	if(access(pathtobit,F_OK)==0)
		{
			this->checkOn=this->wc->globalLib->LFSTK_createSurfaceFromPath(pathtobit);
			free(pathtobit);
			asprintf(&pathtobit,"%s/checkoff.png",this->wc->globalLib->LFSTK_getThemePath());
			if(access(pathtobit,F_OK)==0)
				this->checkOff=this->wc->globalLib->LFSTK_createSurfaceFromPath(pathtobit);
		}
	free(pathtobit);

	gadgetDetails={&this->wc->windowColourNames[NORMALCOLOUR],BEVELOUT,CHECK,NORMALCOLOUR,CHECKBOXSIZE,false,{0,0,w,h},{2,(int)((h/2)-(CHECKBOXSIZE/2)),CHECKBOXSIZE,CHECKBOXSIZE},true,false,true};
}

/**
* Mouse enter callback.
* \param e XButtonEvent passed from mainloop->listener.
* \return Return true if event fully handeled or false to pass it on.
*/
bool LFSTK_toggleButtonClass::mouseEnter(XButtonEvent *e)
{
	bool retval=true;

	if((this->callBacks.runTheCallback==false) || (this->isActive==false))
		return(true);

	if(this->boxStyle==TOGGLENORMAL)
		{
			this->gadgetDetails.colour=&this->newGadgetBGColours.at(PRELIGHTCOLOUR);
			this->gadgetDetails.state=PRELIGHTCOLOUR;
		}
	else
		{
			this->gadgetDetails.colour=&this->wc->windowColourNames[NORMALCOLOUR];
			this->gadgetDetails.state=PRELIGHTCOLOUR;
		}

	this->inWindow=true;

	if((this->callBacks.validCallbacks & MOUSEENTERCB) && (this->noRunCB==false))
		retval=this->callBacks.mouseEnterCallback(this,this->callBacks.mouseMoveUserData);

	LFSTK_gadgetClass::LFSTK_clearWindow();
	return(true);
}

/**
* Mouse exit callback.
* \param e XButtonEvent passed from mainloop->listener.
* \return Return true if event fully handeled or false to pass it on.
*/
bool LFSTK_toggleButtonClass::mouseExit(XButtonEvent *e)
{
	bool retval=true;

	if((this->callBacks.runTheCallback==false) || (this->isActive==false))
		return(true);

	if(this->toggleState==true)
		{
			if(this->boxStyle==TOGGLENORMAL)
				this->gadgetDetails.colour=&this->newGadgetBGColours.at(ACTIVECOLOUR);
			this->gadgetDetails.state=ACTIVECOLOUR;
			this->gadgetDetails.bevel=BEVELIN;
		}
	else
		{
			if(this->boxStyle==TOGGLENORMAL)
				this->gadgetDetails.colour=&this->newGadgetBGColours.at(NORMALCOLOUR);
			this->gadgetDetails.state=NORMALCOLOUR;
			this->gadgetDetails.bevel=BEVELOUT;
		}

	this->inWindow=false;

	if((this->callBacks.validCallbacks & MOUSEEXITCB) && (this->noRunCB==false))
		retval=this->callBacks.mouseExitCallback(this,this->callBacks.mouseMoveUserData);

	LFSTK_gadgetClass::LFSTK_clearWindow();
	return(true);
}

/**
* Mouse down callback.
* \param e XButtonEvent passed from mainloop->listener.
* \return Return true if event fully handeled or false to pass it on.
*/
bool LFSTK_toggleButtonClass::mouseDown(XButtonEvent *e)
{
	if((this->callBacks.runTheCallback==false) || (this->isActive==false))
		return(true);

	if(this->boxStyle==TOGGLENORMAL)
		{
			this->gadgetDetails.colour=&this->newGadgetBGColours.at(ACTIVECOLOUR);
			this->gadgetDetails.state=ACTIVECOLOUR;
		}
	else
		{
			this->gadgetDetails.colour=&this->wc->windowColourNames[NORMALCOLOUR];
			this->gadgetDetails.state=ACTIVECOLOUR;
		}

	if(this->toggleState==true)
		this->gadgetDetails.bevel=BEVELOUT;
	else
		this->gadgetDetails.bevel=BEVELIN;

	LFSTK_gadgetClass::LFSTK_clearWindow();

	if(this->callBacks.validCallbacks & MOUSEPRESSCB)
		return(this->callBacks.mousePressCallback(this,this->callBacks.mouseUserData));

	return(true);
}

/**
* Mouse up callback.
* \param e XButtonEvent passed from mainloop->listener.
* \return Return true if event fully handeled or false to pass it on.
*/
bool LFSTK_toggleButtonClass::mouseUp(XButtonEvent *e)
{
	gadgetState col=NORMALCOLOUR;

	if((this->callBacks.runTheCallback==false) || (this->isActive==false))
		return(true);

	if(this->inWindow==true)
		{
			this->toggleState=!this->toggleState;
			col=PRELIGHTCOLOUR;
		}

	if(this->boxStyle==TOGGLECHECK)
		{
			if(this->toggleState==true)
				{
					if(this->useImage==true)
						this->cImage=this->checkOn;
					this->gadgetDetails.state=ACTIVECOLOUR;
				}
			else
				{
					if(this->useImage==true)
						this->cImage=this->checkOff;
					this->gadgetDetails.state=NORMALCOLOUR;
				}
		}

	LFSTK_gadgetClass::LFSTK_clearWindow();

	if(this->inWindow==true)
		{
			if(this->callBacks.validCallbacks & MOUSERELEASECB)
				return(this->callBacks.mouseReleaseCallback(this,this->callBacks.mouseUserData));
		}
	return(true);
}

/**
* Set the toggle button style.
* \param drawStyle Style=TOGGLECHECK=0,TOGGLERADIO=1,TOGGLENORMAL=2.
* \note TOGGLERADIO not implemented yet.
*/
void LFSTK_toggleButtonClass::LFSTK_setToggleStyle(drawStyle ds)
{
	this->boxStyle=ds;
	if(ds==TOGGLENORMAL)
		{
			this->labelOffset=0;
			this->style=BEVELOUT;
			this->LFSTK_setLabelGravity(CENTRE);

			gadgetDetails.hasIndicator=false;
			gadgetDetails.bevel=BEVELOUT;
			gadgetDetails.colour=&this->newGadgetBGColours.at(NORMALCOLOUR);
			gadgetDetails.reserveSpace=0;
			gadgetDetails.buttonTile=true;
			this->showIndicator=false;
			if(this->useImage==true)
				{
					this->cImage=NULL;
					this->useImage=false;
				}
			this->gadgetDetails.geomRelativeToMainWindow=false;
		}
	else
		{
			this->style=BEVELNONE;
			this->LFSTK_setLabelGravity(LEFT);
			gadgetDetails.hasIndicator=true;
			gadgetDetails.buttonTile=false;
			this->showIndicator=true;
			if(this->useImage==true)
				this->cImage=this->checkOff;
			this->gadgetDetails.geomRelativeToMainWindow=true;
		}
}

/**
* Set the toggle button value.
* \param bool
*/
void LFSTK_toggleButtonClass::LFSTK_setValue(bool val)
{
	this->toggleState=val;
	if(this->toggleState==true)
		{
		
			this->gadgetDetails.bevel=BEVELIN;
			this->gadgetDetails.state=ACTIVECOLOUR;
			if((this->useImage==true) && (this->boxStyle!=TOGGLENORMAL))
				this->cImage=this->checkOn;
		}
	else
		{
			this->gadgetDetails.bevel=BEVELOUT;
			this->gadgetDetails.state=NORMALCOLOUR;
			if((this->useImage==true) && (this->boxStyle!=TOGGLENORMAL))
				this->cImage=this->checkOff;
		}

	this->LFSTK_clearWindow();
}

/**
* Set the toggle button value.
* \param bool
*/
bool LFSTK_toggleButtonClass::LFSTK_getValue(void)
{
	return(this->toggleState);
}

