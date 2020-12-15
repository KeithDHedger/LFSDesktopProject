/*
 *
 * ©K. D. Hedger. Sat 12 Dec 12:33:03 GMT 2020 keithdhedger@gmail.com

 * This file (LFSTKMultiGadget.cpp) is part of LFSToolKit.

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

LFSTK_MultiGadgetClass::~LFSTK_MultiGadgetClass()
{
	for(int j=0;j<this->hitRects.size();j++)
		if(this->hitRects.at(j).gadget!=NULL)
			XReparentWindow(this->display,this->hitRects.at(j).gadget->window,this->wc->window,0,0);
}

LFSTK_MultiGadgetClass::LFSTK_MultiGadgetClass()
{
}

/**
* Main constructor.
*
* \param parentwc Main parent window class.
* \param label Displayed name.
* \param x X pos.
* \param y Y pos.
* \param w Width.
* \param h Height.
* \param gravity Button gravity.
*/
LFSTK_MultiGadgetClass::LFSTK_MultiGadgetClass(LFSTK_windowClass* parentwc,const char* label,int x,int y,unsigned w,unsigned h,int gravity)
{
	XSetWindowAttributes	wa;

	this->LFSTK_setCommon(parentwc,label,x,y,w,h,NorthWestGravity);

	wa.win_gravity=gravity;
	wa.save_under=true;

	this->window=XCreateWindow(this->display,this->parent,x,y,w,h,0,CopyFromParent,InputOutput,CopyFromParent,CWWinGravity|CWSaveUnder,&wa);
	this->gc=XCreateGC(this->display,this->window,0,NULL);

	this->wc->globalLib->LFSTK_setCairoSurface(this->display,this->window,this->visual,&this->sfc,&this->cr,w,h);
	this->LFSTK_setCairoFontData();

	XSelectInput(this->display,this->window,this->gadgetEventMask);

	this->ml->function=&LFSTK_lib::LFSTK_gadgetEvent;
	this->ml->gadget=this;
	this->ml->type=MULTIGADGET;
	wc->LFSTK_addMappedListener(this->window,ml);

	if(this->wc->globalLib->LFSTK_getUseTheme()==true)
		this->LFSTK_setTile(this->wc->globalLib->LFSTK_getGlobalString(-1,TYPEWINDOWTILE),-1);
	else
		this->useTile=false;

	gadgetDetails={&this->colourNames[NORMALCOLOUR],BEVELNONE,NOINDICATOR,NORMALCOLOUR,0,true,{0,0,w,h},{0,0,0,0},false,false,true};
	this->hitRects.clear();
}

/**
* Set hit rects and resize/move gadgets.
*
* \param std::vector<hitRect> hr
* \note must be used AFTER main window shown.
*/
void LFSTK_MultiGadgetClass::LFSTK_setHitRects(std::vector<hitRect> hr)
{
	this->hitRects=hr;
	for(int j=0;j<this->hitRects.size();j++)
		if(this->hitRects.at(j).gadget!=NULL)
			{
				this->hitRects.at(j).gadget->LFSTK_resizeWindow(this->hitRects.at(j).rect.w,this->hitRects.at(j).rect.h);
				XReparentWindow(this->display,this->hitRects.at(j).gadget->window,this->window,this->hitRects.at(j).rect.x,this->hitRects.at(j).rect.y);
				this->hitRects.at(j).gadget->parent=this->window;
			}
}

void LFSTK_MultiGadgetClass::LFSTK_updateGadget(geometryStruct oldgeom)
{
	geometryStruct			oldgadggeom;
	const geometryStruct	*wgeom=this->wc->LFSTK_getWindowGeom();

	int						diffw=wgeom->w-oldgeom.w;
	int						diffh=wgeom->h-oldgeom.h;
	int						newx;
	int						newy;

	this->LFSTK_getGeom(&oldgadggeom);
	newx=oldgadggeom.x;
	newy=oldgadggeom.y;

	int sw=oldgadggeom.w;
	int sh=oldgadggeom.h;
	if(this->stretchX==true)
		sw=oldgadggeom.w+diffw;
	if(this->stretchY==true)
		sh=oldgadggeom.h+diffh;

	this->LFSTK_resizeWindow(sw,sh);

	switch(this->lockY)
		{
			case LOCKTOTOP:
				break;
			case LOCKTOBOTTOM:
				if(this->stretchY==false)
					newy=oldgadggeom.y+diffh;;
				break;
			case LOCKTOCENTRE:
				if(this->stretchY==false)
					newy=oldgadggeom.y+(diffh/2);
				break;
			case ABSOLUTE:
				break;
		}

	switch(this->lockX)
		{
			case LOCKTOLEFT:
				break;
			case LOCKTORIGHT:
				if(this->stretchX==false)
					newx=oldgadggeom.x+diffw;
				break;
			case LOCKTOCENTRE:
				if(this->stretchX==false)
					newx=oldgadggeom.x+(diffw/2);
				break;
			case ABSOLUTE:
				break;
			}
	this->LFSTK_moveGadget(newx,newy);
	this->updateInternalGadgets(oldgadggeom);
}


void LFSTK_MultiGadgetClass::updateInternalGadgets(geometryStruct oldgadggeom)
{
	geometryStruct	newgeom;
	int				diffw;
	int				diffh;

	this->LFSTK_getGeom(&newgeom);
	diffw=newgeom.w-oldgadggeom.w;
	diffh=newgeom.h+(oldgadggeom.h/this->hitRects.size());
	diffh=(newgeom.h-oldgadggeom.h)/this->hitRects.size();

double	multiy=(double)newgeom.h+(double)((double)oldgadggeom.h/(double)this->hitRects.size());
multiy=(double)oldgadggeom.h/(double)newgeom.h;
multiy=(double)newgeom.h/(double)oldgadggeom.h;
//DEBUGFUNC("multi=%f",multiy);

	for(int j=0;j<this->hitRects.size();j++)
		{
			if(this->hitRects.at(j).gadget!=NULL)
				{
					switch(this->gadgetStretch)
						{
							case STRETCH:
								
								this->hitRects.at(j).rect.y=(int)(double)((double)this->hitRects.at(j).rect.y*multiy+0.5);
								this->hitRects.at(j).rect.h=(int)(double)((double)this->hitRects.at(j).rect.h*multiy+0.5);
								//this->hitRects.at(j).rect.y*=(multiy);
							//DEBUGFUNC("oldh=%i newh=%i diffh=%f",oldgadggeom.h,newgeom.h,multiy);
//								this->hitRects.at(j).rect.w+=diffw;
								//this->hitRects.at(j).rect.h=(this->hitRects.at(j).rect.h/multiy);
//								this->hitRects.at(j).rect.y+=diffh;
//	if(strcmp("Label Gadget",this->hitRects.at(j).gadget->LFSTK_getLabel())==0)
//		printf("->>>--x=%i\n",this->hitRects.at(j).rect.x);

								this->hitRects.at(j).gadget->LFSTK_resizeWindow(this->hitRects.at(j).rect.w,this->hitRects.at(j).rect.h);
								this->hitRects.at(j).gadget->LFSTK_moveGadget(this->hitRects.at(j).rect.x,this->hitRects.at(j).rect.y);
								//this->hitRects.at(j).gadget->LFSTK_setTile(this->wc->globalLib->LFSTK_getGlobalString(-1,TYPEWINDOWTILE),-1);
								break;
							case MOVE:
								break;
							case SPACE:
								break;
						}
				}
		}
}







