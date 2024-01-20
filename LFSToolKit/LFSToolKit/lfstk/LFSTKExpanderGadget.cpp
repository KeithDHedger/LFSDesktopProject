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

LFSTK_ExpanderGadgetClass::~LFSTK_ExpanderGadgetClass()
{
	for(int j=0;j<this->hitRects.size();j++)
		if(this->hitRects.at(j).gadget!=NULL)
			XReparentWindow(this->wc->app->display,this->hitRects.at(j).gadget->window,this->wc->window,0,0);
}

LFSTK_ExpanderGadgetClass::LFSTK_ExpanderGadgetClass()
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
LFSTK_ExpanderGadgetClass::LFSTK_ExpanderGadgetClass(LFSTK_windowClass* parentwc,const char* label,int x,int y,unsigned w,unsigned h,int gravity)
{
	XSetWindowAttributes	wa;

	this->LFSTK_setCommon(parentwc,label,x,y,w,h,NorthWestGravity);

	wa.win_gravity=gravity;
	wa.save_under=true;

	this->window=XCreateWindow(this->wc->app->display,this->parent,x,y,w,h,0,CopyFromParent,InputOutput,CopyFromParent,CWWinGravity|CWSaveUnder,&wa);
	this->gc=XCreateGC(this->wc->app->display,this->window,0,NULL);

	this->wc->globalLib->LFSTK_setCairoSurface(this->wc->app->display,this->window,this->wc->app->visual,&this->sfc,&this->cr,w,h);
	this->LFSTK_setCairoFontData();

	XSelectInput(this->wc->app->display,this->window,this->gadgetEventMask);

	this->ml->function=&LFSTK_lib::LFSTK_gadgetEvent;
	this->ml->gadget=this;
	this->ml->type=MULTIGADGET;
	this->isTransparent=true;

	wc->LFSTK_addMappedListener(this->window,ml);

	if(this->wc->globalLib->LFSTK_getUseTheme()==true)
		this->LFSTK_setTile(this->wc->globalLib->LFSTK_getGlobalString(-1,TYPEWINDOWTILE),-1);
	else
		this->useTile=false;

	gadgetDetails={&this->newGadgetBGColours.at(PRELIGHTCOLOUR),BEVELNONE,NOINDICATOR,PRELIGHTCOLOUR,0,true,{0,0,w,h},{0,0,0,0},false,false,true};

	this->hitRects.clear();
}

/**
* Set hit rects.
*
* \param std::vector<hitRect> hr
*/
void LFSTK_ExpanderGadgetClass::LFSTK_setHitRects(std::vector<hitRect> hr)
{
	this->hitRects=hr;
}

/**
* Private Set hit rects and resize/move gadgets.
* \param bool rezizeandmove true=resize and move gadgets.
*/
void LFSTK_ExpanderGadgetClass::justifyHitRects(bool rezizeandmove)
{
	geometryStruct	newgeom;
	int				sx=0;
	int				gapsize=0;

	this->LFSTK_getGeom(&newgeom);

	if(this->gadgetStretch==SPACESPREADX)
		{
			int totalw=0;
			int gaps=this->hitRects.size()+1;
			for(int k=0;k<this->hitRects.size();k++)
				totalw+=this->hitRects.at(k).rect.w;
			gapsize=(newgeom.w-totalw)/gaps;
			sx=gapsize;
		}

	if(this->gadgetStretch==SPACESPREADY)
		{
			int totalh=0;
			int gaps=this->hitRects.size()+1;
			for(int k=0;k<this->hitRects.size();k++)
				totalh+=this->hitRects.at(k).rect.h;
			gapsize=(newgeom.h-totalh)/gaps;
			sx=gapsize;
		}

	if(this->gadgetStretch==SPACERIGHT)
		sx=newgeom.w;

	for(int j=0;j<this->hitRects.size();j++)
		if(this->hitRects.at(j).gadget!=NULL)
			{
				switch(this->gadgetStretch)
					{
						case SPACESPREADX:
							this->hitRects.at(j).rect.x=sx;
							sx+=this->hitRects.at(j).rect.w+gapsize;
							break;
						case SPACESPREADY:
							this->hitRects.at(j).rect.y=sx;
							sx+=this->hitRects.at(j).rect.h+gapsize;
							break;
						case SPACELEFT:
							this->hitRects.at(j).rect.x=sx;
							sx+=this->hitRects.at(j).rect.w+spacePad;
							break;
						case SPACERIGHT:	
							sx-=(this->hitRects.at(this->hitRects.size()-j-1).rect.w+spacePad);
							this->hitRects.at(this->hitRects.size()-j-1).rect.x=sx;
							break;
					}
				if(rezizeandmove==true)
					{
						this->hitRects.at(j).gadget->LFSTK_resizeWindow(this->hitRects.at(j).rect.w,this->hitRects.at(j).rect.h);
						this->hitRects.at(j).gadget->LFSTK_moveGadget(this->hitRects.at(j).rect.x,this->hitRects.at(j).rect.y);
					}
			}
}

/**
* Set hit rects and resize/move gadgets.
* \note Not normally used by user.
* \note Called from expose event from main window.
*/
void LFSTK_ExpanderGadgetClass::LFSTK_resetGadgets(void)
{
	this->justifyHitRects(true);

	for(int j=0;j<this->hitRects.size();j++)
		if(this->hitRects.at(j).gadget!=NULL)
			{
				XReparentWindow(this->wc->app->display,this->hitRects.at(j).gadget->window,this->window,this->hitRects.at(j).rect.x,this->hitRects.at(j).rect.y);
				this->hitRects.at(j).gadget->parent=this->window;
			}
}

/**
* Set hit rects and resize/move gadgets.
* \note Not normally used by user.
* \note Called from ConfigureNotify event from main window.
*/
void LFSTK_ExpanderGadgetClass::LFSTK_updateGadget(geometryStruct oldgeom)
{
	geometryStruct			oldgadggeom;
	const geometryStruct		*wgeom=this->wc->LFSTK_getWindowGeom();

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

	//this->wc->app->globalLib->LFSTK_setCairoSurface(this->wc->app->display,this->wc->window,this->wc->visual,&this->wc->sfc,&this->wc->cr,sw,sh);
	//this->wc->app->globalLib->LFSTK_setCairoSurface(this->wc->app->display,this->window,this->visual,&this->sfc,&this->cr,sw,sh);
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
	//this->wc->LFSTK_clearWindow();
//void LFSTK_lib::LFSTK_setCairoSurface(Display *display,Window window,Visual *visual,cairo_surface_t **sfc,cairo_t **cr,int width,int height)

}

/**
* Private updateInternalGadgets.
* \param geometryStruct oldgadggeom.
* \note Move not really useful
*/
void LFSTK_ExpanderGadgetClass::updateInternalGadgets(geometryStruct oldgadggeom)
{
	geometryStruct	newgeom;

	this->LFSTK_getGeom(&newgeom);

	double	multiy=(double)newgeom.h/(double)oldgadggeom.h;
	double	multix=(double)newgeom.w/(double)oldgadggeom.w;

	for(int j=0;j<this->hitRects.size();j++)
		{
			if(this->hitRects.at(j).gadget!=NULL)
				{
					switch(this->gadgetStretch)
						{
							case STRETCH:
								{
									this->hitRects.at(j).rect.y=(int)(double)((double)this->hitRects.at(j).rect.y*multiy+0.5);
									this->hitRects.at(j).rect.h=(int)(double)((double)this->hitRects.at(j).rect.h*multiy+0.5);
									this->hitRects.at(j).rect.x=(int)(double)((double)this->hitRects.at(j).rect.x*multix+0.5);
									this->hitRects.at(j).rect.w=(int)(double)((double)this->hitRects.at(j).rect.w*multix+0.5);

									this->hitRects.at(j).gadget->LFSTK_resizeWindow(this->hitRects.at(j).rect.w,this->hitRects.at(j).rect.h);
									this->hitRects.at(j).gadget->LFSTK_moveGadget(this->hitRects.at(j).rect.x,this->hitRects.at(j).rect.y);
									mappedListener	*ml=this->hitRects.at(j).gadget->wc->LFSTK_getMappedListener(this->hitRects.at(j).gadget->window);
									if(ml->type==LISTGADGET)
										{
											LFSTK_listGadgetClass	*list=static_cast<LFSTK_listGadgetClass*>(this->hitRects.at(j).gadget);
											list->LFSTK_resetListHeight(this->hitRects.at(j).rect.h);
											list->LFSTK_updateList();

											for(int i=0;i<list->maxShowing;i++)
												{
													list->labelsArray->at(i)->LFSTK_resizeWindow(this->hitRects.at(j).rect.w-SCROLLBARWIDTH-1,list->labelsArray->at(i)->gadgetDetails.gadgetGeom.h);
													list->labelsArray->at(i)->LFSTK_moveGadget(1,1+LABELHITE*i);
												}
											list->scrollBar->LFSTK_resizeWindow(SCROLLBARWIDTH,this->hitRects.at(j).rect.h);
											list->scrollBar->LFSTK_moveGadget(this->hitRects.at(j).rect.w-SCROLLBARWIDTH,0);
										}
								}
								break;

							case MOVE://TODO/MOVE list
								{
									double	perc=(double)this->hitRects.at(j).rect.x/(double)oldgadggeom.w;
									this->hitRects.at(j).rect.x=(int)(((double)newgeom.w*perc)+0.5);
									perc=(double)this->hitRects.at(j).rect.y/(double)oldgadggeom.h;
									this->hitRects.at(j).rect.y=(int)(((double)newgeom.h*perc)+0.5);
									this->hitRects.at(j).gadget->LFSTK_moveGadget(this->hitRects.at(j).rect.x,this->hitRects.at(j).rect.y);
								}
								break;

							case SPACESPREADX:
							case SPACESPREADY:
							case SPACELEFT:
							case SPACERIGHT:
								this->justifyHitRects(true);
								break;

							default:
								break;
						}
				}
		}
}

/**
* Drop data.
* \param data Data droped on gadget as string.
*/
void LFSTK_ExpanderGadgetClass::LFSTK_dropData(propertyStruct* data)
{
	for(int j=0;j<this->hitRects.size();j++)
		{
			if((this->hitRects.at(j).gadget!=NULL) && (this->hitRects.at(j).gadget->gadgetAcceptsDnD==true))
				{
					pointStruct point={this->wc->droppedData.x,this->wc->droppedData.y};
					geometryStruct geom;
		
					this->hitRects.at(j).gadget->LFSTK_getGeomWindowRelative(&geom,this->wc->app->rootWindow);
					if(this->wc->globalLib->LFSTK_pointInRect(&point,&geom)==true)
						this->hitRects.at(j).gadget->LFSTK_dropData(data);
				}
		}
}

