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
 
#include "lfstk/LFSTKGlobals.h"

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
	mappedListener			*ml=new mappedListener;

	this->LFSTK_setCommon(parentwc,label,x,y,w,h,gravity);

	wa.win_gravity=gravity;
	wa.save_under=true;
	this->window=XCreateWindow(this->display,this->parent,x,y,w,h,0,CopyFromParent,InputOutput,CopyFromParent,CWWinGravity,&wa);
	this->gc=XCreateGC(this->display,this->window,0,NULL);
	this->wc->globalLib->LFSTK_setCairoSurface(this->display,this->window,this->visual,&this->sfc,&this->cr,w,h);
	this->LFSTK_setCairoFontData();
	XSelectInput(this->display,this->window,ButtonReleaseMask | ButtonPressMask | ExposureMask | EnterWindowMask | LeaveWindowMask);

	ml->function=&LFSTK_lib::LFSTK_gadgetEvent;
	ml->gadget=this;
	ml->type=TOGGLEGADGET;
	this->wc->LFSTK_addMappedListener(this->window,ml);

	this->boxStyle=TOGGLECHECK;

	this->toggleState=false;
	this->labelOffset=(this->gadgetGeom.h/2);
	this->LFSTK_setLabelGravity(LEFT);
	if(this->wc->globalLib->LFSTK_getUseTheme()==true)
		this->LFSTK_setTile(this->wc->globalLib->LFSTK_getGlobalString(-1,TYPEBUTTONTILE),-1);
	else
		this->useTile=false;

	this->indicGeom={0,0,CHECKBOXSIZE,CHECKBOXSIZE};
	this->style=BEVELNONE;
	gadgetDetails={&this->wc->windowColourNames[NORMALCOLOUR],BEVELOUT,CHECK,&this->indicGeom,NORMALCOLOUR,CHECKBOXSIZE,false,{0,0,w,h},{2,(h/2)-(CHECKBOXSIZE/2),CHECKBOXSIZE,CHECKBOXSIZE},true};
	this->LFSTK_setFontColourName(0,this->wc->globalLib->LFSTK_getGlobalString(0,TYPEFONTCOLOUR),true);
}

/**
* Draw a togglebox.
*/
void LFSTK_toggleButtonClass::drawButton(gadgetState state)
{
#if 0
//return;
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
			g->w=(this->gadgetGeom.h/2);
			g->h=(this->gadgetGeom.h/2);
			g->y=g->h-(g->h/2)-1;
		//	this->drawIndicator();

#if 1
			this->LFSTK_setLabelGravity(LEFT);
			this->labelOffset=(this->gadgetGeom.h/2);
//this->labelOffset=2;
			if(this->wc->useTile==true) 
				{
					XSetTSOrigin(this->display,this->gc,0-this->gadgetGeom.x,0-this->gadgetGeom.y);
					XSetFillStyle(this->display,this->gc,FillTiled);
					XSetTile(this->display,this->gc,this->wc->tile[0]);
					XFillRectangle(this->display,this->window,this->gc,0,0,this->gadgetGeom.w,this->gadgetGeom.h);
					XSetFillStyle(this->display,this->gc,FillSolid);
				}
			else
				{
					XSetFillStyle(this->display,this->gc,FillSolid);
					XSetForeground(this->display,this->gc,this->wc->windowColourNames[NORMALCOLOUR].pixel);
					XFillRectangle(this->display,this->window,this->gc,0,0,this->gadgetGeom.w,this->gadgetGeom.h);
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
#endif
		}	

	if(this->boxStyle==TOGGLENORMAL)
		{
			g->x=0;
			g->y=0;
			g->w=this->gadgetGeom.w;
			g->h=this->gadgetGeom.h;
			//this->LFSTK_setLabelGravity(CENTRE);
			//this->labelOffset=2;
			this->drawBox(g,state,bv);
		}

	if(this->isActive==true)
		this->LFSTK_drawLabel(NORMALCOLOUR);
	else
		this->LFSTK_drawLabel(INACTIVECOLOUR);

	delete g;
#endif
}

/**
* Clear the gadget window to the appropriate state.
*/
void LFSTK_toggleButtonClass::LFSTK_clearWindowxx()
{
#if 0
	if(this->toggleState==true)
		{
			this->style=BEVELIN;
		}
	else
		{
			this->style=BEVELOUT;
		}

	if(this->boxStyle==TOGGLENORMAL)
		{
			LFSTK_gadgetClass::LFSTK_clearWindow();
			return;
		}

	if(this->boxStyle==TOGGLECHECK)
		{
			//this->clearBox(&this->wc->windowColourNames[NORMALCOLOUR],BEVELOUT);
			this->LFSTK_drawLabel(NORMALCOLOUR);
		}
return;
	XSetFillStyle(this->display,this->gc,FillSolid);
	XSetClipMask(this->display,this->gc,None);

	XSetForeground(this->display,this->gc,this->wc->windowColourNames[NORMALCOLOUR].pixel);

	XFillRectangle(this->display,this->window,this->gc,0,0,this->gadgetGeom.w,this->gadgetGeom.h);

	if(this->isActive==false)
		this->drawButton(INACTIVECOLOUR);
	else
		{
			if(this->inWindow==false)
				this->drawButton(NORMALCOLOUR);
			else
				this->drawButton(PRELIGHTCOLOUR);
		}
#endif
}

/**
* Mouse enter callback.
* \param e XButtonEvent passed from mainloop->listener.
* \return Return true if event fully handeled or false to pass it on.
*/
bool LFSTK_toggleButtonClass::mouseEnter(XButtonEvent *e)
{
	if(this->boxStyle==TOGGLENORMAL)
		{
			this->gadgetDetails.colour=&this->colourNames[PRELIGHTCOLOUR];
			this->gadgetDetails.state=PRELIGHTCOLOUR;
			if(this->toggleState==true)
				this->gadgetDetails.bevel=BEVELIN;
			else
				this->gadgetDetails.bevel=BEVELOUT;
		}
	else
		{
			this->gadgetDetails.indic=CHECK;
			this->gadgetDetails.colour=&this->wc->windowColourNames[NORMALCOLOUR];
			this->gadgetDetails.state=PRELIGHTCOLOUR;
			if(this->toggleState==true)
				this->gadgetDetails.bevel=BEVELIN;
			else
				this->gadgetDetails.bevel=BEVELOUT;
		}

	this->inWindow=true;
	XSync(this->display,false);
	LFSTK_gadgetClass::LFSTK_clearWindow();
	return(true);
//
//
//	if(this->boxStyle==TOGGLENORMAL)
//		{
//			LFSTK_gadgetClass::mouseEnter(e);
//			return(true);
//		}
//	if(this->boxStyle==TOGGLENORMAL)
//		{
//			if(this->toggleState==true)
//				this->style=BEVELIN;
//			else
//				this->style=BEVELOUT;
//			LFSTK_gadgetClass::mouseEnter(e);
//			return(true);
//		}
//
//return(true);
//	if(this->isActive==false)
//		{
//			this->LFSTK_clearWindow();
//			return(true);
//		}
//
//	this->LFSTK_clearWindow();
//	this->drawButton(PRELIGHTCOLOUR);
//	this->inWindow=true;
//	return(true);
}

/**
* Mouse exit callback.
* \param e XButtonEvent passed from mainloop->listener.
* \return Return true if event fully handeled or false to pass it on.
*/
bool LFSTK_toggleButtonClass::mouseExit(XButtonEvent *e)
{
	if(this->boxStyle==TOGGLENORMAL)
		{
			this->gadgetDetails.colour=&this->colourNames[NORMALCOLOUR];
			this->gadgetDetails.state=NORMALCOLOUR;
			if(this->toggleState==true)
				this->gadgetDetails.bevel=BEVELIN;
			else
				this->gadgetDetails.bevel=BEVELOUT;
		}
	else
		{
			this->gadgetDetails.indic=CHECK;
			this->gadgetDetails.colour=&this->wc->windowColourNames[NORMALCOLOUR];
			this->gadgetDetails.state=NORMALCOLOUR;
			if(this->toggleState==true)
				this->gadgetDetails.bevel=BEVELIN;
			else
				this->gadgetDetails.bevel=BEVELOUT;
		}

	this->inWindow=false;
	XSync(this->display,false);
	LFSTK_gadgetClass::LFSTK_clearWindow();
	return(true);
//
////	if(this->boxStyle==TOGGLENORMAL)
////		{
////			if(this->toggleState==true)
////				this->style=BEVELIN;
////			else
////				this->style=BEVELOUT;
////			LFSTK_gadgetClass::mouseEnter(e);
////			return(true);
////		}
////return(true);
//	if(this->boxStyle==TOGGLENORMAL)
//		{
//			LFSTK_gadgetClass::mouseExit(e);
//			return(true);
//		}
//	if(this->isActive==false)
//		{
//			this->LFSTK_clearWindow();
//			return(true);
//		}
//
//	this->LFSTK_clearWindow();
//	this->drawButton(NORMALCOLOUR);
//	this->inWindow=false;
//	return(true);
}

/**
* Mouse down callback.
* \param e XButtonEvent passed from mainloop->listener.
* \return Return true if event fully handeled or false to pass it on.
*/
bool LFSTK_toggleButtonClass::mouseDown(XButtonEvent *e)
{
	if(this->boxStyle==TOGGLENORMAL)
		{
			this->gadgetDetails.colour=&this->colourNames[ACTIVECOLOUR];
			this->gadgetDetails.state=ACTIVECOLOUR;
			if(this->toggleState==true)
				this->gadgetDetails.bevel=BEVELOUT;
			else
				this->gadgetDetails.bevel=BEVELIN;
		}
	else
		{
			this->gadgetDetails.colour=&this->wc->windowColourNames[NORMALCOLOUR];
			this->gadgetDetails.state=ACTIVECOLOUR;
			if(this->toggleState==true)
				this->gadgetDetails.bevel=BEVELIN;
			else
				this->gadgetDetails.bevel=BEVELOUT;
		}

	XSync(this->display,false);
	LFSTK_gadgetClass::LFSTK_clearWindow();
	if(this->callback.pressCallback!=NULL)
		return(this->callback.pressCallback(this,this->callback.userData));
	return(true);
//
//return(true);
//	if(this->isActive==false)
//		{
//			this->LFSTK_clearWindow();
//			return(true);
//		}
//
//	this->drawButton(ACTIVECOLOUR);
//	XSync(this->display,false);
//
//	if(this->callback.pressCallback!=NULL)
//		return(this->callback.pressCallback(this,this->callback.userData));
//	return(true);
}

/**
* Mouse up callback.
* \param e XButtonEvent passed from mainloop->listener.
* \return Return true if event fully handeled or false to pass it on.
*/
bool LFSTK_toggleButtonClass::mouseUp(XButtonEvent *e)
{
	gadgetState col=NORMALCOLOUR;

	if(this->inWindow==true)
		{
			this->toggleState=!this->toggleState;
			col=PRELIGHTCOLOUR;
		}

	if(this->boxStyle==TOGGLENORMAL)
		{
			this->gadgetDetails.colour=&this->colourNames[col];
			this->gadgetDetails.state=col;
			if(this->toggleState==true)
				this->gadgetDetails.bevel=BEVELIN;
			else
				this->gadgetDetails.bevel=BEVELOUT;
		}
	else
		{
			this->gadgetDetails.colour=&this->wc->windowColourNames[NORMALCOLOUR];
			this->gadgetDetails.state=col;
			if(this->toggleState==true)
				this->gadgetDetails.bevel=BEVELIN;
			else
				this->gadgetDetails.bevel=BEVELOUT;
		}

	LFSTK_gadgetClass::LFSTK_clearWindow();
	XSync(this->display,false);

	if(this->inWindow==true)
		{
			if(this->callback.releaseCallback!=NULL)
				return(this->callback.releaseCallback(this,this->callback.userData));
		}
	return(true);
//
//
//
//	if(this->boxStyle==TOGGLENORMAL)
//		{
//			LFSTK_gadgetClass::mouseUp(e);
//			return(true);
//		}
//	this->state=NORMALCOLOUR;
//
//	if(this->isActive==false)
//		{
//			this->LFSTK_clearWindow();
//			return(true);
//		}
//
//	if(this->inWindow==false)
//		this->LFSTK_clearWindow();
//	else
//		{
//			this->toggleState=!this->toggleState;
//			this->mouseEnter(e);
//			if(this->callback.releaseCallback!=NULL)
//				return(this->callback.releaseCallback(this,this->callback.userData));
//		}
//	return(true);
}
//	gadgetDetails={&this->gadgetGeom,&this->wc->windowColourNames[NORMALCOLOUR],BEVELOUT,CHECK,&this->indicGeom,NORMALCOLOUR,h-4,false,false,{0,0,w,h},{2,2,8,8},true};

/**
* Set the toggle button style.
* \param drawStyle Style=TOGGLECHECK=0,TOGGLERADIO=1,TOGGLENORMAL=2.
*/
void LFSTK_toggleButtonClass::LFSTK_setToggleStyle(drawStyle ds)
{
	this->boxStyle=ds;
	if(ds==TOGGLENORMAL)
		{
			this->labelOffset=0;
			this->style=BEVELOUT;
			this->LFSTK_setLabelGravity(CENTRE);
		//	this->LFSTK_setFontColourName(0,this->wc->globalLib->LFSTK_getGlobalString(0,TYPEFONTCOLOUR),true);
		//	this->LFSTK_setFontColourName(0,this->wc->globalLib->LFSTK_getGlobalString(0,TYPEFONTCOLOUR),false);

//	for(int j=0;j<MAXCOLOURS;j++)
//		this->fontColourNames[j].name=NULL;
//
//	for(int j=0;j<MAXCOLOURS;j++)
//		this->colourNames[j].name=NULL;
//
//	this->autoLabelColour=this->wc->autoLabelColour;
//	for(int j=0;j<MAXCOLOURS;j++)
//		this->LFSTK_setColourName(j,this->wc->globalLib->LFSTK_getGlobalString(j,TYPEBUTTON));
//
//	for(int j=0;j<MAXCOLOURS;j++)
//		this->LFSTK_setFontColourName(j,this->wc->globalLib->LFSTK_getGlobalString(j,TYPEFONTCOLOUR),false);



			gadgetDetails.hasIndicator=false;
			gadgetDetails.bevel=BEVELOUT;
			gadgetDetails.colour=&this->colourNames[NORMALCOLOUR];
			gadgetDetails.reserveSpace=0;
			gadgetDetails.buttonTile=true;
			//gadgetDetails={&this->gadgetGeom,&this->colourNames[NORMALCOLOUR],BEVELOUT,NOINDICATOR,NULL,NORMALCOLOUR,0,false,false};
		}
	else
		{
			this->style=BEVELNONE;
			this->LFSTK_setLabelGravity(LEFT);
			gadgetDetails.hasIndicator=true;
			gadgetDetails.buttonTile=false;
//			gadgetDetails.butt=false;
			//gadgetDetails={&this->gadgetGeom,&this->wc->windowColourNames[NORMALCOLOUR],BEVELNONE,CHECK,&this->indicGeom,NORMALCOLOUR,this->gadgetGeom.h,false,false};
		}
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






