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
	this->LFSTK_reParentWindow(this->wc->window,0,0);
	this->thumb->LFSTK_reParentWindow(this->wc->window,0,0);
	this->upLeft->LFSTK_reParentWindow(this->wc->window,0,0);
	this->downRight->LFSTK_reParentWindow(this->wc->window,0,0);
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
* Reset bar height.
* \param int newheight.
*/
void LFSTK_scrollBarClass::LFSTK_resetHeight(int newheight)
{
	if(this->verticalBar==false)
		{
		//TODO//
		}
	else
		{
			this->thumb->LFSTK_setLimits(-1,SCROLLBARWIDTH-1,-1,newheight-SCROLLBARWIDTH);
			this->downRight->LFSTK_moveGadget(0,newheight-SCROLLBARWIDTH);
		}
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
	char					*pathtobit;
	
	this->LFSTK_setCommon(parentwc,NULL,x,y,w,h,gravity);

	wa.win_gravity=gravity;
	wa.save_under=true;

	this->window=XCreateWindow(this->wc->app->display,this->parent,x,y,w,h,0,CopyFromParent,InputOutput,CopyFromParent,CWWinGravity|CWSaveUnder,&wa);
	this->gc=XCreateGC(this->wc->app->display,this->window,0,NULL);

	this->wc->globalLib->LFSTK_setCairoSurface(this->wc->app->display,this->window,this->wc->app->visual,&this->sfc,&this->cr,w,h);
	this->LFSTK_setCairoFontData();

	XSelectInput(this->wc->app->display,this->window,this->gadgetEventMask);
	this->ml->function=&LFSTK_lib::LFSTK_gadgetEvent;
	this->ml->gadget=this;
	this->ml->type=SCROLLBARGADGET;
	wc->LFSTK_addMappedListener(this->window,ml);


	this->newGadgetBGColours[NORMALCOLOUR]=this->LFSTK_setColour(this->wc->globalLib->LFSTK_getGlobalString(NORMALCOLOUR,TYPESBTROUGHCOLOUR));

	gadgetDetails={&this->newGadgetBGColours.at(NORMALCOLOUR),BEVELIN,NOINDICATOR,NORMALCOLOUR,0,true,{0,0,w,h},{0,0,0,0},false,false,false};

	this->verticalBar=vertical;
	if(this->verticalBar==false)
		{
//thumb
			this->thumb=new LFSTK_buttonClass(parentwc,"",0,0,h,h,NorthWestGravity);
			this->thumb->LFSTK_reParentWindow(this->window,h,0);			
			this->thumb->LFSTK_setLimits(h-1,-1,w-h,-1);
			this->thumb->LFSTK_allowYMovement(false);
//left
			this->upLeft=new LFSTK_buttonClass(parentwc,"",0,0,h,h,gravity);
			this->upLeft->LFSTK_reParentWindow(this->window,0,0);
			asprintf(&pathtobit,"%s/left.png",this->wc->globalLib->LFSTK_getThemePath());
			if(access(pathtobit,F_OK)==0)
				this->upLeft->LFSTK_setImageFromPath(pathtobit,CENTRE,true);
			else
				this->upLeft->LFSTK_setImageFromPath(LFSTKPIXMAPSDIR "/left.png",CENTRE,true);
	
			freeAndNull(&pathtobit);
//rite
			this->downRight=new LFSTK_buttonClass(parentwc,"",0,0,h,h,gravity);
			this->downRight->LFSTK_reParentWindow(this->window,w-h,0);
			asprintf(&pathtobit,"%s/rite.png",this->wc->globalLib->LFSTK_getThemePath());
			if(access(pathtobit,F_OK)==0)
				this->downRight->LFSTK_setImageFromPath(pathtobit,CENTRE,true);
			else
				this->downRight->LFSTK_setImageFromPath(LFSTKPIXMAPSDIR "/rite.png",CENTRE,true);
			freeAndNull(&pathtobit);
		}
	else
		{
//thumb
			this->thumb=new LFSTK_buttonClass(parentwc,"",0,0,w,w,NorthWestGravity);
			this->thumb->LFSTK_reParentWindow(this->window,0,0);
			this->thumb->LFSTK_setLimits(-1,w-1,-1,h-w);
			this->thumb->LFSTK_allowXMovement(false);
//up
			this->upLeft=new LFSTK_buttonClass(parentwc,"",0,0,w,w,gravity);
			this->upLeft->LFSTK_reParentWindow(this->window,0,0);
			asprintf(&pathtobit,"%s/up.png",this->wc->globalLib->LFSTK_getThemePath());
			if(access(pathtobit,F_OK)==0)
				this->upLeft->LFSTK_setImageFromPath(pathtobit,CENTRE,true);
			else
				this->upLeft->LFSTK_setImageFromPath(LFSTKPIXMAPSDIR "/up.png",CENTRE,true);
			freeAndNull(&pathtobit);
//down
			this->downRight=new LFSTK_buttonClass(parentwc,"",x,y+h-w,w,w,gravity);
			this->downRight->LFSTK_reParentWindow(this->window,0,h-w);
			asprintf(&pathtobit,"%s/down.png",this->wc->globalLib->LFSTK_getThemePath());
			if(access(pathtobit,F_OK)==0)
				this->downRight->LFSTK_setImageFromPath(pathtobit,CENTRE,true);
			else
				this->downRight->LFSTK_setImageFromPath(LFSTKPIXMAPSDIR "/down.png",CENTRE,true);
			freeAndNull(&pathtobit);
		}

	asprintf(&pathtobit,"%s/thumb.png",this->wc->globalLib->LFSTK_getThemePath());
	if(access(pathtobit,F_OK)==0)
		this->thumb->LFSTK_setImageFromPath(pathtobit,CENTRE,true);
	else
		this->thumb->LFSTK_setImageFromPath(LFSTKPIXMAPSDIR "/thumb.png",CENTRE,true);
	freeAndNull(&pathtobit);

	this->thumb->LFSTK_setCanDrag(true);
	this->thumb->LFSTK_setMouseCallBack(this->startThumbDrag,this->thumbClicked,(void*)this);
	this->upLeft->LFSTK_setMouseCallBack(NULL,this->lineUpDown,(void*)this);
	this->upLeft->LFSTK_setKeyCallBack(NULL,this->lineUpDown,(void*)this);
	this->downRight->LFSTK_setMouseCallBack(NULL,this->lineUpDown,(void*)this);
	this->downRight->LFSTK_setKeyCallBack(NULL,this->lineUpDown,(void*)this);
	this->downRight->userData=USERDATA(2);
	this->upLeft->userData=USERDATA(1);
}

/**
* Set thumb position
* \param bool byvalue.
*/
void LFSTK_scrollBarClass::setState(bool byvalue,bool ignorecb)
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

	this->LFSTK_clearWindow(ignorecb);
}

/**
* Set Value and move thumb.
* \param int val New Value.
*/
void LFSTK_scrollBarClass::LFSTK_setValue(int val,bool ignore)
{
	if(this->reverse==false)
		{
			this->value=val;
			this->setState(true,ignore);
		}
	else
		{
			this->value=this->maxScale-val+this->minScale;
			this->setState(true,ignore);
		}
}

/**
* Get Value.
* \return int val.
*/
int LFSTK_scrollBarClass::LFSTK_getValue(void)
{
	if(this->reverse==false)
		return(this->value);
	return(this->maxScale-this->value+this->minScale);
}

/**
* Set allow keyboard control ( default=true ).
* \param bool allow.
*/
void LFSTK_scrollBarClass::LFSTK_setAllowKBControl(bool allow)
{
	this->allowKBControl=allow;
}

/**
* Get allow keyboard control ( default=true ).
* \return bool.
*/
bool LFSTK_scrollBarClass::LFSTK_getAllowKBControl(void)
{
	return(this->allowKBControl);
}


/**
* Set active state.
* \param nool active.
*/
void LFSTK_scrollBarClass::LFSTK_setActive(bool active)
{
	this->thumb->LFSTK_setActive(active);
	this->upLeft->LFSTK_setActive(active);
	this->downRight->LFSTK_setActive(active);
	this->thumb->LFSTK_setCanDrag(active);
	this->thumb->LFSTK_setIgnores(!active,false);
	this->isActive=active;
	if(active==true)
		{
			this->thumb->LFSTK_setMouseCallBack(this->startThumbDrag,this->thumbClicked,(void*)this);
			this->gadgetDetails.state=NORMALCOLOUR;
			this->gadgetDetails.colour=&this->newGadgetBGColours.at(NORMALCOLOUR);
		}

	this->LFSTK_clearWindow();
}

/**
* Clear the gadget window to the appropriate state.
*/
void LFSTK_scrollBarClass::LFSTK_clearWindow()
{
//this->thumb->LFSTK_setActive(this->isActive);
//this->thumb->LFSTK_setCanDrag(!this->isActive);
//if(this->startDrag=this->isActive==false)
//{
//	this->startDrag=false;
//	this->thumb->LFSTK_clearWindow();
//	//return;
//}
//	if(this->isActive==false)
//		{
//		}
	if(this->startDrag==true)
		this->setState(false);

	this->clearBox(&this->gadgetDetails);
	if(this->style!=BEVELNONE)
		this->drawBevel(&this->gadgetDetails.gadgetGeom,this->gadgetDetails.bevel);

	if(this->callBacks.validCallbacks & MOUSERELEASECB)
		this->callBacks.mouseReleaseCallback(this,this->callBacks.mouseUserData);

//reduce flickering
	if(this->startDrag==true)
		XSync(this->wc->app->display,true);
	this->thumb->LFSTK_clearWindow();
}

/**
* Clear the gadget window to the appropriate state.
* \note ignores callback.
*/
void LFSTK_scrollBarClass::LFSTK_clearWindow(bool ignorecb)
{
	if(ignorecb==false)
		{
			if(this->startDrag==true)
				this->setState(false);

			this->clearBox(&this->gadgetDetails);
			if(this->style!=BEVELNONE)
				this->drawBevel(&this->gadgetDetails.gadgetGeom,this->gadgetDetails.bevel);
			if(this->callBacks.validCallbacks & MOUSERELEASECB)
				this->callBacks.mouseReleaseCallback(this,this->callBacks.mouseUserData);

			this->thumb->LFSTK_clearWindow();
		}
	else
		{
			this->clearBox(&this->gadgetDetails);
			if(this->style!=BEVELNONE)
				this->drawBevel(&this->gadgetDetails.gadgetGeom,this->gadgetDetails.bevel);
			this->thumb->LFSTK_clearWindow();
		}
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

bool LFSTK_scrollBarClass::pageUpDown(void *object,void* userdata)
{
	LFSTK_buttonClass		*button=static_cast<LFSTK_buttonClass*>(object);
	LFSTK_scrollBarClass	*sbar=static_cast<LFSTK_scrollBarClass*>(userdata);

	sbar->startDrag=false;

	if((long)button->userData==1)
		sbar->value-=sbar->pageScroll;

	if((long)button->userData==2)
		sbar->value+=sbar->pageScroll;

	sbar->setState(true);
	return(true);
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
* Key up callback.
* \param e XButtonEvent passed from mainloop->listener.
* \return Return true if event fully handeled or false to pass it on.
*/
bool LFSTK_scrollBarClass::keyRelease(XKeyEvent *e)
{
	char	c[255];
	KeySym	keysym_return;

	if(this->LFSTK_getAllowKBControl()==false)
		return(true);

	XLookupString(e,(char*)&c,255,&keysym_return,NULL);
	switch(keysym_return)
		{
			case XK_Prior:
				this->LFSTK_scrollByPage(true);
				break;
			case XK_Next:
				this->LFSTK_scrollByPage(false);
				break;
			case XK_Up:
				this->LFSTK_scrollByLine(true);
				break;
			case XK_Down:
				this->LFSTK_scrollByLine(false);
				break;
		}
	this->setState(true);
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

	if((this->callBacks.runTheCallback==false) || (this->isActive==false))
		return(true);

	if(this->inWindow==true)
		{
			this->thumb->LFSTK_getGeom(&geom);
			this->thumb->LFSTK_getLimits(&rect);

			switch(e->button)
				{
					case Button1:
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
						break;
					case Button4:
						this->value-=this->pageScroll;
						break;
					case Button5:
						this->value+=this->pageScroll;
						break;
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
			if(sb->callBacks.validCallbacks & MOUSERELEASECB)
				sb->callBacks.mouseReleaseCallback(sb,sb->callBacks.mouseUserData);

			sb->startDrag=false;
		}
	return(true);
}

bool LFSTK_scrollBarClass::startThumbDrag(void *object,void* userdata)
{
	LFSTK_scrollBarClass	*sb=NULL;
//	if(sb->isActive==false)
//		return(true);
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
	this->inWindow=false;
	this->startDrag=false;
	return(true);
}

bool LFSTK_scrollBarClass::mouseDown(XButtonEvent *e)
{
	if(((this->callBacks.runTheCallback==false) || (this->isActive==false)) || ((this->callBacks.validCallbacks & MOUSEPRESSCB)==0))
		return(true);

	this->inWindow=true;
	this->startDrag=false;
	return(true);
}

/**
* Programatically press up/left down/right button.
* \param bool true= scroll up/left, false=down/right.
*/
void LFSTK_scrollBarClass::LFSTK_scrollByLine(bool up)
{
	if(up==true)
		this->lineUpDown(this->upLeft,this);
	else
		this->lineUpDown(this->downRight,this);
	return;
}

/**
* Programatically press pageup/left pagedown/right button.
* \param bool true= scroll up/left, false=down/right.
*/
void LFSTK_scrollBarClass::LFSTK_scrollByPage(bool up)
{
	if(up==true)
		this->pageUpDown(this->upLeft,this);
	else
		this->pageUpDown(this->downRight,this);
	return;
}



