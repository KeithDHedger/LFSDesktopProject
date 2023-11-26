/*
 *
 * ©K. D. Hedger. Wed  5 Aug 13:53:44 BST 2015 keithdhedger@gmail.com

 * This file (LFSTKGadget.cpp) is part of LFSToolKit.

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

/**
* Gadget destructor.
* \note Should not be called by user, gadgets are destroyed on window close.
*/
LFSTK_gadgetClass::~LFSTK_gadgetClass()
{
	if(this->label!=NULL)
		free(this->label);

	if(this->isMapped==true)
		this->LFSTK_reParentWindow(this->wc->window,0,0);

//TODO//
	for(int j=NORMALCOLOUR;j<MAXCOLOURS;j++)
		{
			if(this->newGadgetFGColours.at(j).isValid==true)
				XFreeColors(this->wc->app->display,this->wc->app->cm,(long unsigned int*)&this->newGadgetFGColours.at(j).pixel,1,0);
			if(this->newGadgetBGColours.at(j).isValid==true)
				XFreeColors(this->wc->app->display,this->wc->app->cm,(long unsigned int*)&this->newGadgetBGColours.at(j).pixel,1,0);
		}

	if(this->fontString!=NULL)
		free(this->fontString);

	if(this->labelBGColour.isValid==true)
		XFreeColors(this->wc->app->display,this->wc->app->cm,(long unsigned int*)&this->labelBGColour.pixel,1,0);

	cairo_destroy(this->cr);

	if(this->cImage!=NULL)
		cairo_surface_destroy(this->cImage);

	if(this->pattern!=NULL)
		cairo_pattern_destroy(this->pattern);

	if(this->link!=NULL)
		cairo_surface_destroy(this->link);

	if(this->broken!=NULL)
		cairo_surface_destroy(this->broken);

	cairo_surface_destroy(this->sfc);

	if(this->fontName!=NULL)
		free(this->fontName);

	XFreeGC(this->wc->app->display,this->gc);
}

LFSTK_gadgetClass::LFSTK_gadgetClass()
{
	pad=2;
}

/**
* Get the gadget's window ID.
* \return Returns windows XID.
*/
Window LFSTK_gadgetClass::LFSTK_getWindow(void)
{
	return(this->window);
}

/**
* Set default font string.
* \param s		Font string.
* \param bool	setfontdata default false.
* \note setfontdata=true also resets text extents and other font data.
* \note eg:
* \note "sans-serif:size=8".
*/
void LFSTK_gadgetClass::LFSTK_setFontString(const char *s,bool setfontdata)
{
	if(this->fontString!=NULL)
		free(this->fontString);
	this->fontString=strdup(s);

	if(setfontdata==true)
		this->LFSTK_setCairoFontData();
}

/**
* Set the label BGcolour+alpha name .
* \param colour Colour name.
* \param double alpha.
*/
void LFSTK_gadgetClass::LFSTK_setLabelBGColour(const char* colour,double alpha)
{
	XColor tc,sc;
	XAllocNamedColor(this->wc->app->display,this->wc->app->cm,colour,&sc,&tc);

	this->labelBGColour.pixel=sc.pixel;
	this->labelBGColour.RGBAColour.r=((sc.pixel>>16) & 0xff)/256.0;
	this->labelBGColour.RGBAColour.g=((sc.pixel>>8) & 0xff)/256.0;
	this->labelBGColour.RGBAColour.b=((sc.pixel>>0) & 0xff)/256.0;
	this->labelBGColour.RGBAColour.a=alpha;
}

/**
* Get the colour name for gadget.
* \param p Gadget state.
* \return colour Const colour name.
* \note state is NORMALCOLOUR=0,PRELIGHTCOLOUR=1,ACTIVECOLOUR=2,INACTIVECOLOUR=3.
* \note Donot free returned value.
*/
const char* LFSTK_gadgetClass::LFSTK_getColourName(int p)
{
	return(newGadgetBGColours.at(p).name.c_str());
}

//needs re vamping
void LFSTK_gadgetClass::initGadget(void)
{
	this->fontString=NULL;
	this->autoLabelColour=this->wc->autoLabelColour;

	this->LFSTK_setGadgetColours(GADGETBG,this->wc->globalLib->LFSTK_getGlobalString(NORMALCOLOUR,TYPEBUTTON),
										this->wc->globalLib->LFSTK_getGlobalString(PRELIGHTCOLOUR,TYPEBUTTON),
										this->wc->globalLib->LFSTK_getGlobalString(ACTIVECOLOUR,TYPEBUTTON),
										this->wc->globalLib->LFSTK_getGlobalString(INACTIVECOLOUR,TYPEBUTTON));

	this->LFSTK_setGadgetColours(GADGETFG,this->wc->globalLib->LFSTK_getGlobalString(NORMALCOLOUR,TYPEFONTCOLOUR),
										this->wc->globalLib->LFSTK_getGlobalString(PRELIGHTCOLOUR,TYPEFONTCOLOUR),
										this->wc->globalLib->LFSTK_getGlobalString(ACTIVECOLOUR,TYPEFONTCOLOUR),
										this->wc->globalLib->LFSTK_getGlobalString(INACTIVECOLOUR,TYPEFONTCOLOUR));


	this->LFSTK_setFontString(this->wc->globalLib->LFSTK_getGlobalString(-1,TYPEFONT));
	this->monoFontString=this->wc->globalLib->LFSTK_getGlobalString(-1,TYPEMONOFONT);
	this->LFSTK_setActive(true);
	this->style=BEVELOUT;
	this->labelGravity=CENTRE;
	this->inWindow=false;
	this->labelOffset=0;
	this->gotIcon=false;
	this->iconSize=16;
	this->useTile=false;
	this->useImage=false;
	this->imageWidth=0;
	this->gadgetAcceptsDnD=false;
	this->showIndicator=true;
	this->LFSTK_setAlpha(this->wc->windowColourNames[NORMALCOLOUR].RGBAColour.a);
}

/**
* Do common setup for gadget.
* \param parentwc Main parent window class.
* \param label Displayed name.
* \param x X pos.
* \param y Y pos.
* \param w Width.
* \param h Height.
* \param gravity Button gravity.
*/
void LFSTK_gadgetClass::LFSTK_setCommon(LFSTK_windowClass* parentwc,const char* label,int x,int y,unsigned int w,unsigned int h,int gravity)
{
	this->wc=parentwc;
	this->wc->app->display=this->wc->app->display;
	this->parent=this->wc->window;

	this->gadgetGeom.x=x;
	this->gadgetGeom.y=y;
	this->gadgetGeom.w=w;
	this->gadgetGeom.h=h;

	if(label!=NULL)
		this->label=strdup(label);
	else
		this->label=strdup("");

	this->initGadget();

	this->ml=new mappedListener;
}

/**
* Set key callback for widget.
* \param downcb key down callback.
* \param releasecb key up callback.
* \note Format for callback is "bool functioname(void *p,void* ud)"
* \note First param passed to callback is pointer to object.
* \note Second param passed to callback is user data.
*/
void LFSTK_gadgetClass::LFSTK_setKeyCallBack(bool (*downcb)(void *,void*),bool (*releasecb)(void *,void*),void* ud)
{
	this->callBacks.validCallbacks=this->callBacks.validCallbacks & ALLCB & (~(KEYPRESSCB|KEYRELEASECB));
	if(downcb!=NULL)
		this->callBacks.validCallbacks|=KEYPRESSCB;
	if(releasecb!=NULL)
		this->callBacks.validCallbacks|=KEYRELEASECB;
	this->callBacks.keyPressCallback=downcb;
	this->callBacks.keyReleaseCallback=releasecb;
	this->callBacks.keyUserData=ud;
	this->callBacks.runTheCallback=true;
}

/**
* Set drop callback for widget.
* \param dropped drop callback.
* \param ud user data.
* \note Format for callback is "bool functioname(void *p,void* ud)"
* \note First param passed to callback is pointer to object.
* \note Second param passed to callback is user data.
*/
void LFSTK_gadgetClass::LFSTK_setGadgetDropCallBack(bool (*dropped)(void*,propertyStruct *data,void*),void* ud)
{
	this->callBacks.validCallbacks=this->callBacks.validCallbacks & ALLCB & (~GADGETDROPCB);
	if(dropped!=NULL)
		this->callBacks.validCallbacks|=GADGETDROPCB;
	this->callBacks.droppedGadgetCallback=dropped;
	this->callBacks.droppedUserData=ud;
	this->callBacks.runTheCallback=true;
//			this->callBacks.ignoreOrphanModKeys=true;
}

/**
* Set mouse enter/exit callback for widget.
* \param entercb mouse enter callback.
* \param exitcb mouse exit callback.
* \note Format for callback is "bool functioname(void *p,void* ud)"
* \note First param passed to callback is pointer to object.
* \note Second param passed to callback is user data.
*/
void LFSTK_gadgetClass::LFSTK_setMouseMoveCallBack(bool (*entercb)(LFSTK_gadgetClass*,void*),bool (*exitcb)(LFSTK_gadgetClass*,void*),void* ud)
{
	this->callBacks.validCallbacks=this->callBacks.validCallbacks & ALLCB & (~(MOUSEENTERCB|MOUSEEXITCB));
	if(entercb!=NULL)
		this->callBacks.validCallbacks|=MOUSEENTERCB;

	if(exitcb!=NULL)
		this->callBacks.validCallbacks|=MOUSEEXITCB;
	this->callBacks.mouseEnterCallback=entercb;
	this->callBacks.mouseExitCallback=exitcb;
	this->callBacks.mouseMoveUserData=ud;
	this->callBacks.runTheCallback=true;
}

/**
* Set mouse callback for widget.
* \param downcb mouse down callback.
* \param releasecb mouse up callback.
* \note Format for callback is "bool functioname(void *p,void* ud)"
* \note First param passed to callback is pointer to object.
* \note Second param passed to callback is user data.
*/
void LFSTK_gadgetClass::LFSTK_setMouseCallBack(bool (*downcb)(void*,void*),bool (*releasecb)(void*,void*),void* ud)
{
	this->callBacks.validCallbacks=this->callBacks.validCallbacks & ALLCB & (~(MOUSEPRESSCB|MOUSERELEASECB));
	if(downcb!=NULL)
		this->callBacks.validCallbacks|=MOUSEPRESSCB;
	if(releasecb!=NULL)
		this->callBacks.validCallbacks|=MOUSERELEASECB;
	this->callBacks.mousePressCallback=downcb;
	this->callBacks.mouseReleaseCallback=releasecb;
	this->callBacks.mouseUserData=ud;
	this->callBacks.runTheCallback=true;
//	this->callBacks.ignoreOrphanModKeys=true;
}

/**
* Set gadget active state.
* \param active Gadget active or not.
* \note Setting to false deactivates widget;
*/
void LFSTK_gadgetClass::LFSTK_setActive(bool active)
{
	this->isActive=active;
}

/**
* Set gadget auto colour for label.
* \param setauto Auto label colour active or not.
* \note Setting to true overrides the label colour;
* \note and tries to set it to black or white depending on background.
* \note defaults to false; 
*/
void LFSTK_gadgetClass::LFSTK_setLabelAutoColour(bool setauto)
{
	this->autoLabelColour=setauto;
}

/**
* Set whether to use gadgets tile or windows tile.
* \note if windows pixmap not available then use gadget colours.
*/
void LFSTK_gadgetClass::LFSTK_setUseWindowTile(bool usebutton)
{
	this->gadgetDetails.buttonTile=!usebutton;
}

/**
* Set background of gadget to windows pixmap.
* \note if windows pixmap not available then use gadget colours.
*/
void LFSTK_gadgetClass::LFSTK_setUseWindowPixmap(bool usepixmap)
{
	if((this->wc->px!=None) && (usepixmap==true))
		{
			XSetWindowBackgroundPixmap(this->wc->app->display,this->window,ParentRelative);
			this->gadgetDetails.useWindowPixmap=usepixmap;
		}
	else
		{
			this->gadgetDetails.useWindowPixmap=false;
			this->useTile=false;
		}
}

/**
* Clear box to colour.
* \param gadgetStruct* details
*/
void LFSTK_gadgetClass::clearBox(gadgetStruct* details)
{
	cairo_pattern_t	*patt;
	colourStruct	*usecolour;
	geometryStruct	geom;
	this->LFSTK_getGeom(&geom);

	if(details->useWindowPixmap==true)
		{
			XClearWindow(this->wc->app->display,this->window);
			return;
		}

	if(this->isActive==false)
		details->colour=&newGadgetBGColours.at(INACTIVECOLOUR);

	if(this->isTransparent==true)
		details->colour=&this->wc->windowColourNames[NORMALCOLOUR];

	if(this->useTile==true)
		{
			if(details->buttonTile==true)
					patt=this->pattern;
			else
				{
					patt=this->wc->pattern;
				}

			if(details->geomRelativeToMainWindow==true)
				this->LFSTK_getGeomWindowRelative(&geom,this->wc->window);
			
			cairo_save(this->cr);
				cairo_reset_clip(this->cr);
				if(details->geomRelativeToMainWindow==true)
					cairo_translate(this->cr,-geom.x,-geom.y);
				cairo_set_source(this->cr,patt);
				cairo_paint(this->cr);
			cairo_restore(this->cr);
		}
	else
		{
			cairo_save(this->cr);
				cairo_reset_clip(this->cr);
				cairo_set_source_rgba(this->cr,details->colour->RGBAColour.r,details->colour->RGBAColour.g,details->colour->RGBAColour.b,details->colour->RGBAColour.a);
				cairo_set_operator(this->cr,CAIRO_OPERATOR_SOURCE);
				cairo_paint(this->cr);
			cairo_restore(this->cr);
		}
}

/**
* Draw bevel.
* \param geometryStruct	*geom
* \param bevelType		bevel
*/
void LFSTK_gadgetClass::drawBevel(geometryStruct* geom,bevelType bevel)
{
/*
struct geometryStruct
{
	int			x,y;
	unsigned		w,h;
	unsigned		monitor;
};
*/
	cairoColor tlcolour;
	cairoColor brcolour;
geometryStruct geomlocal={geom->x,geom->y,geom->w,geom->h,geom->monitor};
	if(bevel==BEVELNONE)
		return;
	geomlocal.w-=this->squeezeW;
	geomlocal.h-=this->squeezeH;
	switch(bevel)
		{
			case BEVELSQUEEZEW:
			case BEVELSQUEZEH:
			case BEVELIN:
				tlcolour={0,0,0,1};
				brcolour={1,1,1,1};
				break;
			case BEVELOUT:
				tlcolour={1,1,1,1};
				brcolour={0,0,0,1};
				break;
		}
	cairo_save(this->cr);
		cairo_reset_clip (this->cr);
		cairo_set_antialias (this->cr,CAIRO_ANTIALIAS_NONE);
		cairo_set_line_width(this->cr,1.0);
		cairo_set_source_rgba(this->cr,tlcolour.r,tlcolour.g,tlcolour.b,1.0);
		cairo_set_operator(this->cr,CAIRO_OPERATOR_SOURCE);

		cairo_move_to(this->cr,geomlocal.x+1,geomlocal.h+geomlocal.y+1);
		cairo_line_to(this->cr,geomlocal.x+1,geomlocal.y+1);
		cairo_line_to(this->cr,geomlocal.x+geomlocal.w,geomlocal.y+1);

//		cairo_move_to(this->cr,geom->x+1,geom->h+geom->y+1);
//		cairo_line_to(this->cr,geom->x+1,geom->y+1);
//		cairo_line_to(this->cr,geom->x+geom->w,geom->y+1);
		cairo_stroke(this->cr);
		
		cairo_set_source_rgba(this->cr,brcolour.r,brcolour.g,brcolour.b,1.0);
		cairo_set_operator(this->cr,CAIRO_OPERATOR_SOURCE);

		cairo_move_to(this->cr,geomlocal.x+geomlocal.w,geomlocal.y+1);
		cairo_line_to(this->cr,geomlocal.x+geomlocal.w,geomlocal.y+geomlocal.h);
		cairo_line_to(this->cr,geomlocal.x+1,geomlocal.y+geomlocal.h);



//		cairo_move_to(this->cr,geom->x+geom->w,geom->y+1);
//		cairo_line_to(this->cr,geom->x+geom->w,geom->y+geom->h);
//		cairo_line_to(this->cr,geom->x+1,geom->y+geom->h);
		cairo_stroke(this->cr);			
	cairo_restore(this->cr);
}

/**
* Draw label.
* \param p Button state.
* \note State NORMALCOLOUR=0,PRELIGHTCOLOUR=1,ACTIVECOLOUR=2,INACTIVECOLOUR=3.
* \note If this->label="--" then draw a seperator.
*/
void LFSTK_gadgetClass::drawLabel(gadgetStruct* details)
{
	int				labelx=0;
	int				labely;
	cairoColor		lcol;
	cairoColor		*colptr=&this->labelBGColour.RGBAColour;
	geometryStruct	labelrect=details->gadgetGeom;
	int				labh;
	int				laby;

	if(this->isActive==false)
		details->state=INACTIVECOLOUR;

	labely=(details->gadgetGeom.h/2)+(0.5-this->fontExtents.descent+this->fontExtents.height/2);

	if(strcmp(this->label,"--")!=0)
		{
			switch(this->labelGravity)
				{
					case MENU:
						labelx=details->gadgetGeom.h+(this->pad*4);
						break;
					case LEFT:
						if(this->useImage==true)
							labelx=details->reserveSpace+pad*2;
						else
							labelx=details->reserveSpace+(this->pad*2);
						break;
					case CENTRE:
						if(this->useImage==true)
							{
								if(this->imageGravity==RIGHT)
									labelx=((details->gadgetGeom.w-this->imageWidth)/2)-(this->textExtents.width/2);
								else
									labelx=((details->gadgetGeom.w-details->reserveSpace)/2)-(this->textExtents.width/2)+details->reserveSpace;
							}
						else
							labelx=((details->gadgetGeom.w-details->reserveSpace)/2)-(this->textExtents.width/2)+details->reserveSpace;

						if(details->hasIndicator==true)
							labelx-=(details->indicatorGeom.w/2);
						break;
					case RIGHT:
						if(this->useImage==true)
							labelx=details->gadgetGeom.w-(this->textExtents.width)-pad*2;
						else
							labelx=details->gadgetGeom.w-this->textExtents.width-this->pad*2;
						break;
					case TOOLBAR:
						labelx=((details->gadgetGeom.w)/2)-(this->textExtents.width/2);
						labelrect={labelx,(int)(details->gadgetGeom.h-(int)this->maxTextHeight-1),(unsigned int)this->textExtents.width,(unsigned int)this->LFSTK_getTextHeight(this->label)};

						labh=details->gadgetGeom.h-this->imageHeight-(this->pad*2);
						laby=details->gadgetGeom.h-(labh/2);

						labely=laby+(this->maxTextHeight/3);
						labelrect.y=details->gadgetGeom.h-(gadgetGeom.h-this->imageHeight)+this->pad*2;
						labelrect.h=labh;
						break;
				}

			cairo_save(this->cr);
				cairo_select_font_face(this->cr,fontName,slant,weight);
				cairo_set_font_size(this->cr,fontSize);

				if(this->drawLabelBG==true)
					{
						if(this->autoLabelBGColour==true)
							{
								if(strcmp(this->wc->globalLib->bestFontColour(this->newGadgetFGColours.at(details->state).pixel),"black")==0)
									{
										lcol.r=0;
										lcol.g=0;
										lcol.b=0;
										lcol.a=this->labelBGColour.RGBAColour.a;
									}
								else
									{
										lcol.r=1;
										lcol.g=1;
										lcol.b=1;
										lcol.a=this->labelBGColour.RGBAColour.a;
									}
								colptr=&lcol;
							}

						cairo_set_source_rgba(this->cr,colptr->r,colptr->g,colptr->b,colptr->a);
						cairo_set_operator(this->cr,CAIRO_OPERATOR_SOURCE);
						cairo_rectangle(this->cr,labelrect.x,labelrect.y,labelrect.w,labelrect.h);
						cairo_fill(this->cr);
					}		

				cairo_move_to(this->cr,labelx,labely);
				cairo_set_source_rgba(this->cr,this->newGadgetFGColours.at(details->state).RGBAColour.r,this->newGadgetFGColours.at(details->state).RGBAColour.g,this->newGadgetFGColours.at(details->state).RGBAColour.b,1.0);
				cairo_show_text(this->cr,this->label); 
			cairo_restore(this->cr);
		}
	else
		{
			cairoColor	tlcolour={0,0,0,1};
			cairoColor	brcolour={1,1,1,1};

			cairo_save(this->cr);
				cairo_reset_clip (this->cr);
				cairo_set_antialias (this->cr,CAIRO_ANTIALIAS_NONE);
				cairo_set_line_width(this->cr,1.0);
				cairo_set_source_rgba(this->cr,tlcolour.r,tlcolour.g,tlcolour.b,tlcolour.a);
				cairo_move_to(this->cr,0,details->gadgetGeom.h/2);
				cairo_line_to(this->cr,details->gadgetGeom.w,details->gadgetGeom.h/2);
				cairo_stroke(this->cr);

				cairo_set_source_rgba(this->cr,brcolour.r,brcolour.g,brcolour.b,brcolour.a);
				cairo_move_to(this->cr,0,details->gadgetGeom.h/2+1);
				cairo_line_to(this->cr,details->gadgetGeom.w,details->gadgetGeom.h/2+1);
				cairo_stroke(this->cr);
			cairo_restore(this->cr);
		}
}

/**
* Draw a button.
*/
void LFSTK_gadgetClass::drawGagetDetails(void)
{
	this->clearBox(&this->gadgetDetails);
	this->drawLabel(&this->gadgetDetails);
	if(this->style!=BEVELNONE)
		this->drawBevel(&this->gadgetDetails.gadgetGeom,this->gadgetDetails.bevel);
	if(this->useImage==true)
		this->drawImage();
	if(this->showIndicator==true)
		this->drawIndicator(&this->gadgetDetails);
}

/**
* Clear the gadget window to the appropriate state.
*/
void LFSTK_gadgetClass::LFSTK_clearWindow()
{
	this->drawGagetDetails();
}

/**
* Set if image can be dragged
*
* \param candrag draggable.
*/
void LFSTK_gadgetClass::LFSTK_setCanDrag(bool candrag)
{
	this->canDrag=candrag;
}

/**
* Get if image can be dragged
*
* \return Draggable.
*/
bool LFSTK_gadgetClass::LFSTK_getCanDrag(void)
{
	return(this->canDrag);
}


/**
* Private select which bevel to use.
* \param bool mousedown true bevel for mousedown, false bevel for mouse up.
*/
void LFSTK_gadgetClass::selectBevel(bool mousedown)
{
	if((this->gadgetDetails.bevel==BEVELIN) || (this->gadgetDetails.bevel==BEVELOUT))
		{
			if(mousedown==true)
				this->gadgetDetails.bevel=BEVELIN;
			else
				this->gadgetDetails.bevel=BEVELOUT;
			return;
		}
	this->gadgetDetails.bevel=BEVELNONE;
}

/**
* Mouse up callback.
* \param e XButtonEvent passed from mainloop->listener.
* \return Return true if event fully handeled or false to pass it on.
*/
bool LFSTK_gadgetClass::mouseUp(XButtonEvent *e)
{
	bool retval=true;
	this->keyEvent=NULL;
	this->mouseEvent=NULL;

//no callbacks
	if((this->callBacks.runTheCallback==false) || (this->isActive==false))
		return(true);

	if(strcmp(this->label,"--")==0)
		return(true);;

	this->gadgetDetails.colour=&this->newGadgetBGColours.at(NORMALCOLOUR);
	this->gadgetDetails.state=NORMALCOLOUR;
	this->selectBevel(false);
	this->LFSTK_clearWindow();

	if(this->inWindow==true)
		{
			this->mouseEvent=e;
			
			if(this->callBacks.validCallbacks & MOUSERELEASECB)
				retval=this->callBacks.mouseReleaseCallback(this,this->callBacks.mouseUserData);

			if(this->toParent==true)
				return(false);
		}

	return(retval);
}

/**
* Mouse down callback.
* \param e XButtonEvent passed from mainloop->listener.
* \return Return true if event fully handeled or false to pass it on.
*/
bool LFSTK_gadgetClass::mouseDown(XButtonEvent *e)
{
	bool retval=true;

	this->mouseDownX=e->x;
	this->mouseDownY=e->y;
	this->keyEvent=NULL;

//no callbacks
	if((this->callBacks.runTheCallback==false) || (this->isActive==false))
		return(true);

	if(strcmp(this->label,"--")==0)
		return(true);;

	this->gadgetDetails.colour=&this->newGadgetBGColours.at(ACTIVECOLOUR);
	this->gadgetDetails.state=ACTIVECOLOUR;
	this->selectBevel(true);
	this->LFSTK_clearWindow();
	
	if(this->callBacks.validCallbacks & MOUSEPRESSCB)
		retval=this->callBacks.mousePressCallback(this,this->callBacks.mouseUserData);

	if(this->toParent==true)
		return(false);

	return(retval);
}

/**
* Mouse exit callback.
* \param e XButtonEvent passed from mainloop->listener.
* \return Return true if event fully handeled or false to pass it on.
*/
bool LFSTK_gadgetClass::mouseExit(XButtonEvent *e)
{
	bool retval=true;

	this->keyEvent=NULL;

//no callbacks
	if((this->callBacks.runTheCallback==false) || (this->isActive==false) )
		return(true);

	if(strcmp(this->label,"--")==0)
		return(true);;

	this->gadgetDetails.colour=&this->newGadgetBGColours.at(NORMALCOLOUR);
	this->gadgetDetails.state=NORMALCOLOUR;
	this->LFSTK_clearWindow();
	this->inWindow=false;

	if((this->callBacks.validCallbacks & MOUSEEXITCB) && (this->noRunCB==false))
		retval=this->callBacks.mouseExitCallback(this,this->callBacks.mouseMoveUserData);

	if(this->toParent==true)
		return(false);

	return(true);
}

/**
* Mouse enter callback.
* \param e XButtonEvent passed from mainloop->listener.
* \return Return true if event fully handeled or false to pass it on.
*/
bool LFSTK_gadgetClass::mouseEnter(XButtonEvent *e)
{
	bool retval=true;

	this->keyEvent=NULL;

//no callbacks
	if((this->callBacks.runTheCallback==false) || (this->isActive==false) )
		return(true);

	if(strcmp(this->label,"--")==0)
		return(true);

	this->gadgetDetails.colour=&this->newGadgetBGColours.at(PRELIGHTCOLOUR);
	this->gadgetDetails.state=PRELIGHTCOLOUR;
	this->LFSTK_clearWindow();
	this->inWindow=true;

	if((this->callBacks.validCallbacks & MOUSEENTERCB) && (this->noRunCB==false))
		retval=this->callBacks.mouseEnterCallback(this,this->callBacks.mouseMoveUserData);

	if(this->toParent==true)
		return(false);

	return(true);
}

/**
* Allow movement in X axis.
* \param bool True=Allow movemen in X axis.
*/
void LFSTK_gadgetClass::LFSTK_allowXMovement(bool confine)
{
	this->allowX=confine;
}

/**
* Allow movement in Y axis.
* \param bool True=Allow movemen in Y axis.
*/
void LFSTK_gadgetClass::LFSTK_allowYMovement(bool confine)
{
	this->allowY=confine;
}

/**
* Mouse drag callback.
* \param e XButtonEvent passed from mainloop->listener.
* \return Return true if event fully handeled or false to pass it on.
*/
bool LFSTK_gadgetClass::mouseDrag(XMotionEvent *e)
{
	this->keyEvent=NULL;
	if(this->canDrag==true)
		{
			if(this->allowX==true)
				{
					if((this->gadgetGeom.x+e->x-this->mouseDownX>this->minX) && (this->gadgetGeom.x+e->x-this->mouseDownX+this->gadgetGeom.w<=this->maxX))
						this->gadgetGeom.x+=e->x-this->mouseDownX;
				}
			if(this->allowY==true)
				{
					if((this->gadgetGeom.y+e->y-this->mouseDownY>this->minY) && (this->gadgetGeom.y+e->y-this->mouseDownY+this->gadgetGeom.h<=this->maxY))
						this->gadgetGeom.y+=e->y-this->mouseDownY;
				}
			if(this->snap>1)
				{
					this->gadgetGeom.x=(this->gadgetGeom.x/this->snap)*this->snap;
					this->gadgetGeom.y=(this->gadgetGeom.y/this->snap)*this->snap;
				}
			this->LFSTK_moveGadget(this->gadgetGeom.x,this->gadgetGeom.y);
			this->wc->app->isDragging=true;
		}
	return(true);
}

void LFSTK_gadgetClass::LFSTK_resizeWindow(int w,int h)
{
	this->gadgetGeom.w=w;
	this->gadgetGeom.h=h;
	this->gadgetDetails.gadgetGeom.w=w;
	this->gadgetDetails.gadgetGeom.h=h;
	XResizeWindow(this->wc->app->display,this->window,this->gadgetGeom.w,this->gadgetGeom.h);
	this->wc->globalLib->LFSTK_setCairoSurface(this->wc->app->display,this->window,this->wc->app->visual,&this->sfc,&this->cr,w,h);

	this->LFSTK_clearWindow();
}

/**
* Client Message callback.
* \param e XEvent passed from mainloop->listener.
* \return Return true if event fully handeled or false to pass it on.
*/
bool LFSTK_gadgetClass::clientMessage(XEvent *e)
{
	this->keyEvent=NULL;
	return(true);
}

/**
* Selection Request callback.
* \param e XEvent passed from mainloop->listener.
* \return Return true if event fully handeled or false to pass it on.
*/
bool LFSTK_gadgetClass::selectionRequest(XSelectionRequestEvent *e)
{
	this->keyEvent=NULL;
	return(true);
}

/**
* Drop data.
* \param data Data drooped on gadget as string.
*/
void LFSTK_gadgetClass::LFSTK_dropData(propertyStruct* data)
{
	if(this->callBacks.validCallbacks & GADGETDROPCB)
		this->callBacks.droppedGadgetCallback(this,data,USERDATA(this->callBacks.droppedUserData));
	return;
}

/**
* Lost focus callback.
* \param e XEvent passed from mainloop->listener.
* \return Return true if event fully handeled or false to pass it on.
*/
bool LFSTK_gadgetClass::lostFocus(XEvent *e)
{
	this->keyEvent=NULL;
	return(true);
}

/**
* Got focus callback.
* \param e XEvent passed from mainloop->listener.
* \return Return true if event fully handeled or false to pass it on.
*/
bool LFSTK_gadgetClass::gotFocus(XEvent *e)
{
	this->keyEvent=NULL;
	return(true);
}

/**
* Draw Scaled image.
*/
void LFSTK_gadgetClass::drawImage()
{
	int		yoffset=0;
	int		xoffset=0;

	if(strcmp(this->label,"--")==0)
		return;

	yoffset=(this->gadgetGeom.h/2)-(this->imageHeight/2);
	switch(this->imageGravity)
		{
			case MENU:
				xoffset=this->pad*2;
				break;
			case LEFT:
				xoffset=this->pad;
				break;
			case CENTRE:
				xoffset=(this->gadgetGeom.w/2)-(this->imageWidth/2);
				break;
			case RIGHT:
				xoffset=this->gadgetGeom.w-this->imageWidth-this->pad;
				break;
			case PRESERVEASPECT:
				xoffset=(this->gadgetGeom.w/2)-(this->imageWidth/2);
				yoffset=(this->gadgetGeom.h/2)-(this->imageHeight/2);
				break;
			case NONE:
				xoffset=0;
				yoffset=0;
				break;
			case TOOLBAR:
				xoffset=(this->gadgetGeom.w/2)-(this->imageWidth/2);
				yoffset=this->pad*2;
				break;
		}

	cairo_save(this->cr);
		cairo_reset_clip (this->cr);
		cairo_translate(this->cr,xoffset,yoffset);
		cairo_set_source_surface(this->cr,this->cImage,0,0);
		cairo_set_operator(this->cr,CAIRO_OPERATOR_OVER);
		cairo_paint_with_alpha(this->cr,this->alpha);

		if(this->gadgetDetails.showLink==true)
			{
				cairo_save(this->cr);
					cairo_translate(this->cr,this->imageWidth-16,this->imageHeight-16);
					cairo_set_source_surface(this->cr,this->link,0,0);
					cairo_paint_with_alpha(this->cr,this->alpha);
				cairo_restore(this->cr);
				
			}
		if(this->gadgetDetails.showBroken==true)
			{
				cairo_translate(this->cr,0,this->imageHeight-16);
				cairo_set_source_surface(this->cr,this->broken,0,0);
				cairo_paint_with_alpha(this->cr,this->alpha);
			}
	cairo_restore(this->cr);
}

/**
* Set font extents.
* \note Reset on changing label.
* \note Font string format "Sans:size=14:bold"
* \note gadget->extents.height=height of label.
* \note gadget->maxTextHeight max height of any text.
*/
void LFSTK_gadgetClass::LFSTK_setCairoFontData(void)
{
	char	*string=strdup(this->fontString);
	char	*str;
	bool	found=false;
	int		labelwidth=0;
	int		labelx=0;

	this->weight=CAIRO_FONT_WEIGHT_NORMAL;
	this->slant=CAIRO_FONT_SLANT_NORMAL;
	this->fontSize=10;

	if(this->fontName!=NULL)
		free(this->fontName);
	this->fontName=strdup("Sans");

	str=strtok(string,":");
	while(1)
		{
			found=false;
			if(str==NULL)
				break;
			if(strcasecmp(str,"bold")==0)
				{
					this->weight=CAIRO_FONT_WEIGHT_BOLD;
					found=true;
				}
			if(strcasecmp(str,"italic")==0)
				{
					this->slant=CAIRO_FONT_SLANT_ITALIC;
					found=true;
				}
			if(strcasestr(str,"size=")!=NULL)
				{
					this->fontSize=atoi(&str[5]);
					found=true;
				}

			if(found==false)
				{
					if(this->fontName!=NULL)
						free(this->fontName);
					this->fontName=strdup(str);
				}
			str=strtok(NULL,":");
		}
	free(string);

	cairo_save(this->cr);
		cairo_select_font_face(this->cr,this->fontName,this->slant,this->weight);
		cairo_set_font_size(this->cr,this->fontSize);
		cairo_font_extents(this->cr,&this->fontExtents);
		if(strlen(this->label)>0)
			cairo_text_extents(this->cr,this->label,&this->textExtents);
		else
			cairo_text_extents(this->cr,"X",&this->textExtents);
		this->maxTextHeight=this->fontExtents.descent+this->fontExtents.ascent;
	cairo_restore(this->cr);
}

/**
* Get width of text.
* \returns text width as double.
*/
double LFSTK_gadgetClass::LFSTK_getTextRealWidth(const char* text)
{
	cairo_text_extents_t returnextents;

	cairo_save(this->cr);
		cairo_select_font_face(this->cr,this->fontName,this->slant,this->weight);
		cairo_set_font_size(this->cr,this->fontSize);
		cairo_text_extents(this->cr,text,&returnextents);
	cairo_restore(this->cr);
	return(returnextents.x_advance);
}

/**
* Get width of text.
* \returns text width rounded down to int.
*/
int LFSTK_gadgetClass::LFSTK_getTextWidth(const char* text)
{
	cairo_text_extents_t returnextents;

	cairo_save(this->cr);
		cairo_select_font_face(this->cr,this->fontName,this->slant,this->weight);
		cairo_set_font_size(this->cr,this->fontSize);
		cairo_text_extents(this->cr,text,&returnextents);
	cairo_restore(this->cr);
	return((int)returnextents.x_advance);
}

/**
* Get height of text.
* \returns text height
*/
int LFSTK_gadgetClass::LFSTK_getTextHeight(const char* text)
{
	cairo_text_extents_t returnextents;

	cairo_save(this->cr);
		cairo_select_font_face(this->cr,this->fontName,this->slant,this->weight);
		cairo_set_font_size(this->cr,this->fontSize);
		cairo_text_extents(this->cr,text,&returnextents);
	cairo_restore(this->cr);
	return((int)(long)returnextents.height);
}

/**
* Set font parts.
* \note Reset on changing label.
* \ Format string = n=font name, I=italic,i=no italic.
* \ Format string = B=bold, b=no bold, s=font size.
*/
void LFSTK_gadgetClass::LFSTK_setCairoFontDataParts(const char* fmt,...)
{
	va_list	ap;
	va_start(ap,fmt);
	while (*fmt)
		{
			switch(*fmt)
				{
					case 'n':
						if(this->fontName!=NULL)
							free(this->fontName);
						this->fontName=strdup(va_arg(ap,char*));
						break;
					case 'I':
						this->slant=CAIRO_FONT_SLANT_ITALIC;
						break;
					case 'i':
						this->slant=CAIRO_FONT_SLANT_NORMAL;
						break;
					case 'B':
						this->weight=CAIRO_FONT_WEIGHT_BOLD;
						break;
					case 'b':
						this->weight=CAIRO_FONT_WEIGHT_NORMAL;
						break;
					case 's':
						this->fontSize=va_arg(ap,int);
						break;
				}
			fmt++;
		}
	va_end(ap);

	cairo_save(this->cr);
		cairo_select_font_face(this->cr,this->fontName,this->slant,this->weight);
		cairo_set_font_size(this->cr,this->fontSize);
		cairo_font_extents(this->cr,&this->fontExtents);
		cairo_text_extents(this->cr,this->label,&this->textExtents);
		this->maxTextHeight=this->fontExtents.descent+this->fontExtents.ascent;
	cairo_restore(this->cr);
}

/**
* Set label.
* \param newlabel new label.
* \note Label is copied.
*/
void LFSTK_gadgetClass::LFSTK_setLabel(const char *newlabel,bool clearwindow)
{
	if(this->label!=NULL)
		free(this->label);
	this->label=strdup(newlabel);
	this->LFSTK_setCairoFontData();
	if(clearwindow==true)
		this->LFSTK_clearWindow();
}

/**
* Get the label.
* \return char* Returned string must not be freed.
*/
const char *LFSTK_gadgetClass::LFSTK_getLabel(void)
{
	return(this->label);
}

/**
* Set label orientation.
* \param o.
* \note o=LEFT=0,CENTRE=1,RIGHT=2.
*/
void LFSTK_gadgetClass::LFSTK_setLabelGravity(int orient)
{
	this->labelGravity=orient;
}

/**
* Get gadget geometry relative to window.
* \param geometry structure.
* \param Window win geometry relative to window 'win'.
*/
void LFSTK_gadgetClass::LFSTK_getGeomWindowRelative(geometryStruct *geom,Window win)
{
	int					x,y;
	Window				child;
	XWindowAttributes	xwa;

	XTranslateCoordinates(this->wc->app->display,this->window,win,0,0,&x,&y,&child );
	XGetWindowAttributes(this->wc->app->display,this->window,&xwa);

	geom->x=x;
	geom->y=y;
	geom->w=xwa.width;
	geom->h=xwa.height;
}

/**
* Get gadget geometry in local co-ords.
* \param geometry structure.
*/
void LFSTK_gadgetClass::LFSTK_getGeom(geometryStruct *geom)
{
	XWindowAttributes	xwa;

	XGetWindowAttributes(this->wc->app->display,this->window,&xwa);
	geom->x=xwa.x;
	geom->y=xwa.y;
	geom->w=xwa.width;
	geom->h=xwa.height;
}

/**
* Reload global colours from main window.
*/
void LFSTK_gadgetClass::LFSTK_reloadColours(void)
{
//TODO//
	for(int j=NORMALCOLOUR;j<MAXCOLOURS;j++)
		{
			if(this->newGadgetFGColours.at(j).isValid==true)
				XFreeColors(this->wc->app->display,this->wc->app->cm,(long unsigned int*)&this->newGadgetFGColours.at(j).pixel,1,0);
			if(this->newGadgetBGColours.at(j).isValid==true)
				XFreeColors(this->wc->app->display,this->wc->app->cm,(long unsigned int*)&this->newGadgetBGColours.at(j).pixel,1,0);
		}

	this->LFSTK_setGadgetColours(GADGETBG,this->wc->globalLib->LFSTK_getGlobalString(NORMALCOLOUR,TYPEBUTTON),
										this->wc->globalLib->LFSTK_getGlobalString(PRELIGHTCOLOUR,TYPEBUTTON),
										this->wc->globalLib->LFSTK_getGlobalString(ACTIVECOLOUR,TYPEBUTTON),
										this->wc->globalLib->LFSTK_getGlobalString(INACTIVECOLOUR,TYPEBUTTON));

	this->LFSTK_setGadgetColours(GADGETFG,this->wc->globalLib->LFSTK_getGlobalString(NORMALCOLOUR,TYPEFONTCOLOUR),
										this->wc->globalLib->LFSTK_getGlobalString(PRELIGHTCOLOUR,TYPEFONTCOLOUR),
										this->wc->globalLib->LFSTK_getGlobalString(ACTIVECOLOUR,TYPEFONTCOLOUR),
										this->wc->globalLib->LFSTK_getGlobalString(INACTIVECOLOUR,TYPEFONTCOLOUR));

	this->LFSTK_setFontString(this->wc->globalLib->LFSTK_getGlobalString(-1,TYPEFONT));
	this->autoLabelColour=this->wc->autoLabelColour;
}

/**
* Draw indicator.
* \param g Geometry Struture.
* \param state Button state.
* \param indic Indicator type CHECK=0,RADIO=1,PICTURE=2,NOINDICATOR=3.
*/
void LFSTK_gadgetClass::drawIndicator(gadgetStruct* details)
{
	if(this->gadgetDetails.hasIndicator==false)
		return;

	switch(details->indic)
		{
			case CHECK:
				cairo_save(this->cr);
					cairo_reset_clip (this->cr);
					cairo_set_source_rgba(this->cr,this->newGadgetBGColours.at(details->state).RGBAColour.r,this->newGadgetBGColours.at(details->state).RGBAColour.g,this->newGadgetBGColours.at(details->state).RGBAColour.b,this->newGadgetBGColours.at(details->state).RGBAColour.a);
					cairo_rectangle(this->cr,details->indicatorGeom.x,details->indicatorGeom.y,details->indicatorGeom.w,details->indicatorGeom.h);
					cairo_fill(this->cr);

					if((static_cast<LFSTK_toggleButtonClass*>(this))->LFSTK_getValue()==false)
						{
							cairo_translate(this->cr,details->indicatorGeom.x,details->indicatorGeom.y);
							if((static_cast<LFSTK_toggleButtonClass*>(this))->checkOff!=NULL)
								{
									cairo_set_source_surface(this->cr,(static_cast<LFSTK_toggleButtonClass*>(this))->checkOff,0,0);
									cairo_paint_with_alpha(this->cr,this->alpha);
								}
						}
					else
						{
							if((static_cast<LFSTK_toggleButtonClass*>(this))->checkOn!=NULL)
								{
									cairo_translate(this->cr,details->indicatorGeom.x,details->indicatorGeom.y);
									cairo_set_source_surface(this->cr,(static_cast<LFSTK_toggleButtonClass*>(this))->checkOn,0,0);
									cairo_paint_with_alpha(this->cr,this->alpha);
								}
						}
				cairo_restore(this->cr);
				this->drawBevel(&this->gadgetDetails.indicatorGeom,this->gadgetDetails.bevel);
				break;

			case DISCLOSURE:
				cairo_save(this->cr);
					cairo_reset_clip (this->cr);
					cairo_set_antialias (this->cr,CAIRO_ANTIALIAS_NONE);
				cairo_set_line_width(this->cr,1.0);
					cairo_set_source_rgba(this->cr,this->newGadgetFGColours.at(details->state).RGBAColour.r,this->newGadgetFGColours.at(details->state).RGBAColour.g,this->newGadgetFGColours.at(details->state).RGBAColour.b,this->newGadgetFGColours.at(details->state).RGBAColour.a);
					cairo_move_to(this->cr,details->indicatorGeom.x,(details->gadgetGeom.h/2)-(details->indicatorGeom.h/2));
					cairo_line_to(this->cr,details->indicatorGeom.x+details->indicatorGeom.w,details->gadgetGeom.h/2);
					cairo_line_to(this->cr,details->indicatorGeom.x,details->indicatorGeom.y+details->indicatorGeom.h);
					cairo_line_to(this->cr,details->indicatorGeom.x,(details->gadgetGeom.h/2)-(details->indicatorGeom.h/2));
					cairo_fill(this->cr);
				cairo_restore(this->cr);
				
				break;
		}
}

/**
* Get Active bevel.
* \return bevelType Active bevel.
*/
bevelType LFSTK_gadgetClass::getActiveBevel(void)
{
	switch(this->style)
		{
			case BEVELIN:
				return(BEVELOUT);
				break;
			case BEVELOUT:
				return(BEVELIN);
				break;
		}
	return(BEVELNONE);
}

/**
* Draw box.
* \param g Geometry Struture.
* \param state Gadget state.
* \param bevel Bevel type.
* \note If bevel=BEVELNONE doesn't draw bevel sic.
*/
void LFSTK_gadgetClass::drawBox(geometryStruct* g,gadgetState state,bevelType bevel)
{
	cairoColor tlcolour;
	cairoColor brcolour;
	
	switch(bevel)
		{
			case BEVELIN:
				tlcolour={0,0,0,1};
				brcolour={1,1,1,1};
				break;
			case BEVELOUT:
				tlcolour={1,1,1,1};
				brcolour={0,0,0,1};
				break;
		}
	cairo_save(this->cr);
		cairo_reset_clip (this->cr);
		cairo_set_source_rgba(this->cr,this->newGadgetBGColours.at(state).RGBAColour.r,this->newGadgetBGColours.at(state).RGBAColour.g,this->newGadgetBGColours.at(state).RGBAColour.b,this->newGadgetBGColours.at(state).RGBAColour.a);
		cairo_paint(this->cr);

		if(bevel!=BEVELNONE)
			{
				cairo_set_source_rgba(this->cr,tlcolour.r,tlcolour.g,tlcolour.b,tlcolour.a);
				cairo_move_to(this->cr,0,g->h);
				cairo_line_to(this->cr,0,0);
				cairo_line_to(this->cr,g->w-1,0);
				cairo_stroke(this->cr);
				cairo_set_source_rgba(this->cr,brcolour.r,brcolour.g,brcolour.b,brcolour.a);
				cairo_move_to(this->cr,g->w,0);
				cairo_line_to(this->cr,g->w,g->h);
				cairo_line_to(this->cr,0,g->h);
				cairo_stroke(this->cr);			
			}
	cairo_restore(this->cr);
}

/**
* Set image and render with cairo.
* \param file Path to image file.
* \param orient orientation of image ( LEFT,CENTRE,RIGHT ).
* \param scale scale type for image.
*/
void LFSTK_gadgetClass::LFSTK_setImageFromSurface(cairo_surface_t *sfc,int orient,bool scale)
{
	cairo_status_t	cs=CAIRO_STATUS_SUCCESS;
	cairo_t			*tcr;
	float			scaleX=1.0;
	float			scaleY=1.0;
	int				txtx;
	float			maxWidth;
	float			maxHeight;
	float			ratio;
	float			width;
	float			height;
	char			*suffix=NULL;

	if(sfc==NULL)
		return;

	width=cairo_image_surface_get_width(sfc);
	height=cairo_image_surface_get_height(sfc);

	if(orient!=MENU)
		{
			txtx=this->LFSTK_getTextWidth(this->label);
			maxWidth=this->gadgetGeom.w-txtx-8;
			maxHeight=this->gadgetGeom.h-8;
		}
	else
		{
			maxWidth=this->gadgetGeom.h-(pad*4);
			maxHeight=maxWidth;
			this->gadgetDetails.reserveSpace=maxHeight;
		}

	if(orient==TOOLBAR)
		{
			maxWidth=this->gadgetGeom.w-(this->pad*2);
			maxHeight=this->gadgetGeom.h-8-(this->textExtents.height+(this->pad*2));
		}

	if(maxWidth>=maxHeight)
		ratio=maxHeight/height;
	else
		ratio=maxWidth/width;

	if(orient==PRESERVEASPECT)
		{
			if(width>=height)
				ratio=maxWidth/width;
			else
				ratio=maxHeight/height;
		}

	this->imageWidth=width*ratio;
	this->imageHeight=height*ratio;

	this->useImage=true;
	this->imageGravity=orient;
	this->labelGravity=orient;

	if(scale==false)
		{
			this->imageWidth=this->gadgetGeom.w;
			this->imageHeight=this->gadgetGeom.h;
			ratio=1.0;
		}

	if(this->cImage!=NULL)
		cairo_surface_destroy(this->cImage);
	this->cImage=cairo_surface_create_similar_image(sfc,cairo_image_surface_get_format(sfc),this->imageWidth,this->imageHeight);
	tcr=cairo_create(this->cImage);
	cairo_reset_clip(tcr);
	cairo_scale(tcr,ratio,ratio);
	cairo_set_source_surface(tcr,sfc,0,0);
	cairo_paint(tcr);

	this->gadgetDetails.reserveSpace=this->imageWidth;
	
	cairo_destroy(tcr);

	if(link==NULL)
		{
			this->link=cairo_image_surface_create_from_png (DATADIR "/Pixmaps/symbolic-link.png");
			this->broken=cairo_image_surface_create_from_png (DATADIR "/Pixmaps/unreadable.png");
		}
}

/**
* Set image and render with cairo.
* \param file Path to image file.
* \param orient orientation of image ( LEFT,CENTRE,RIGHT ).
* \param scale scale type for image.
*/
cairo_status_t LFSTK_gadgetClass::LFSTK_setImageFromPath(const char *file,int orient,bool scale)
{
	cairo_status_t	cs=CAIRO_STATUS_SUCCESS;
	cairo_surface_t	*tempimage=NULL;
	cairo_t			*tcr;
	float			scaleX=1.0;
	float			scaleY=1.0;
	int				txtx;
	float			maxWidth;
	float			maxHeight;
	float			ratio;
	float			width;
	float			height;
	char			*suffix=NULL;

	if((file==NULL) || (this->imagePath.compare(file)==0))
		return(CAIRO_STATUS_SUCCESS);

	this->useImage=false;
	this->gotIcon=false;
	if(file==NULL)
		return(CAIRO_STATUS_FILE_NOT_FOUND);

	suffix=strrchr((char*)file,'.');
	if((suffix!=NULL) && (strcasecmp(suffix,".png")==0))
		{
			tempimage=cairo_image_surface_create_from_png(file);
			cs=cairo_surface_status(tempimage);
		}
	else
		cs=CAIRO_STATUS_INVALID_FORMAT;
	
	if(cs!=CAIRO_STATUS_SUCCESS)
		{
			tempimage=this->wc->globalLib->LFSTK_cairo_image_surface_create_from_jpeg(file);
			if(tempimage==NULL)
				{
					printf("Unkown Format : %s\n",file);
					return(CAIRO_STATUS_INVALID_FORMAT);
				}
		}
	this->imagePath=file;
	width=cairo_image_surface_get_width(tempimage);
	height=cairo_image_surface_get_height(tempimage);

	if(orient!=MENU)
		{
			txtx=this->LFSTK_getTextWidth(this->label);
			maxWidth=this->gadgetGeom.w-txtx-8;
			maxHeight=this->gadgetGeom.h-8;
		}
	else
		{
			maxWidth=this->gadgetGeom.h-(pad*4);
			maxHeight=maxWidth;
			this->gadgetDetails.reserveSpace=maxHeight;
		}

	if(orient==TOOLBAR)
		{
			maxWidth=this->gadgetGeom.w-(this->pad*2);
			maxHeight=this->gadgetGeom.h-8-(this->textExtents.height+(this->pad*2));
		}

	if(maxWidth>=maxHeight)
		ratio=maxHeight/height;
	else
		ratio=maxWidth/width;

	if(orient==PRESERVEASPECT)
		{
			if(width>=height)
				ratio=maxWidth/width;
			else
				ratio=maxHeight/height;
		}

	this->imageWidth=width*ratio;
	this->imageHeight=height*ratio;

	this->useImage=true;
	this->imageGravity=orient;
	this->labelGravity=orient;

	if(scale==false)
		{
			this->imageWidth=this->gadgetGeom.w;
			this->imageHeight=this->gadgetGeom.h;
			ratio=1.0;
		}

	if(this->cImage!=NULL)
		cairo_surface_destroy(this->cImage);

	this->cImage=cairo_surface_create_similar_image(tempimage,cairo_image_surface_get_format(tempimage),this->imageWidth,this->imageHeight);
	tcr=cairo_create(this->cImage);
	cairo_reset_clip(tcr);
	cairo_scale(tcr,ratio,ratio);
	cairo_set_source_surface(tcr,tempimage,0,0);
	cairo_paint(tcr);
	this->gadgetDetails.reserveSpace=this->imageWidth;
	
	cairo_destroy(tcr);
	cairo_surface_destroy(tempimage);

	if(this->link==NULL)
		{
			this->link=cairo_image_surface_create_from_png (DATADIR "/Pixmaps/symbolic-link.png");
			this->broken=cairo_image_surface_create_from_png (DATADIR "/Pixmaps/unreadable.png");
		}

	return(cs);
}

/**
* Get gadget monitor.
* \return unsigned Monitor that gadget top left is on;
*/

int LFSTK_gadgetClass::LFSTK_gadgetOnWhichMonitor(void)
{
	geometryStruct	g;
	int				tx=this->gadgetGeom.x;
	int				ty=this->gadgetGeom.y;

	this->LFSTK_getGeomWindowRelative(&g,this->wc->app->rootWindow);

	if(tx>=0)
		tx=(g.x)+this->gadgetGeom.x;
	else
		tx=(g.w+g.x)-abs(this->gadgetGeom.x);

	if(ty>=0)
		ty=(g.y)+this->gadgetGeom.y;
	else
		ty=(g.h+g.y)-abs(this->gadgetGeom.y);

	if(tx<0)
		tx=0;
	if(ty<0)
		ty=0;

	const monitorStruct* monitors=this->wc->app->LFSTK_getMonitors();

	for(int j=0;j<this->wc->app->LFSTK_getMonitorCount();j++)
		{
			if((tx>=monitors[j].x) && (tx<(monitors[j].x+monitors[j].w)) && (ty>=monitors[j].y) && (ty<(monitors[j].y+monitors[j].h)))
				return(j);
		}
	return(-1);
}

/**
* Set gadget background tile.
* \param path Path to image file.
* \param size Size of image or -1.
*/
void LFSTK_gadgetClass::LFSTK_setTile(const char *path,int size)
{
	cairo_surface_t	*tempimage;
	cairo_status_t	cs=CAIRO_STATUS_SUCCESS;
	char				*suffix=NULL;

	if(this->pattern!=NULL)
		{
			cairo_pattern_destroy(this->pattern);
			this->pattern=NULL;
		}

	if((path==NULL) || (access(path,F_OK)!=0))
		{
			this->useTile=false;
			return;
		}

	suffix=strrchr((char*)path,'.');
	if((suffix!=NULL) && (strcasecmp(suffix,".png")==0))
		{
			tempimage=cairo_image_surface_create_from_png(path);
			cs=cairo_surface_status(tempimage);
		}
	else
		cs=CAIRO_STATUS_INVALID_FORMAT;

	if(cs==CAIRO_STATUS_SUCCESS)
		{
			this->pattern=cairo_pattern_create_for_surface(tempimage);
			cairo_surface_destroy(tempimage);
			cairo_pattern_set_extend(pattern,CAIRO_EXTEND_REPEAT);
			this->useTile=true;
		}
	else
		this->useTile=false;
}

/**
* Ignore orphaned mod keys and callbacks.
* \param bool runcb true=run callback,false=don't run.
* \param bool ignoreorphanmod ignore mod keys without 'normal' key.
*/
void LFSTK_gadgetClass::LFSTK_setIgnores(bool runcb,bool ignoreorphanmod)
{
	this->callBacks.ignoreOrphanModKeys=ignoreorphanmod;
	this->callBacks.runTheCallback=runcb;
}

/**
* Set show indicator.
* \param show.
* \note Shows/Hides indicator on gadgets that have one.
* \note Defaults to true, no effect on gadgets that have no indicator.
*/
void LFSTK_gadgetClass::LFSTK_setShowIndicator(bool show)
{
	this->showIndicator=show;
}

/**
* Set gadget size.
* \param int width.
* \param int height.
*/
void LFSTK_gadgetClass::LFSTK_setGadgetSize(int width,int height)
{
	XResizeWindow(this->wc->app->display,this->window,width,height);
	this->gadgetDetails.gadgetGeom.w=width;
	this->gadgetDetails.gadgetGeom.h=height;
	this->gadgetGeom.w=width;
	this->gadgetGeom.h=height;
	this->wc->globalLib->LFSTK_setCairoSurface(this->wc->app->display,this->window,this->visual,&this->sfc,&this->cr,width,height);
}

/**
* Set snap to grid size
*
* \param int sze.
*/
void LFSTK_gadgetClass::LFSTK_snapSize(int sze)
{
	this->snap=sze;
}

/**
* Set alpha of image
*
* \param double alpha.
*/
void LFSTK_gadgetClass::LFSTK_setAlpha(double alph)
{
	this->alpha=alph;
}

/**
* Show gadget
*/
void LFSTK_gadgetClass::LFSTK_showGadget(void)
{
	XMapWindow(this->wc->app->display,this->window);
	this->isMapped=true;
}

/**
* Hide gadget
*/
void LFSTK_gadgetClass::LFSTK_hideGadget(void)
{
	XUnmapWindow(this->wc->app->display,this->window);
	this->isMapped=false;
}

/**
* Set context window for gadget.
* \param LFSTK_windowClass *wc.
*/
void LFSTK_gadgetClass::LFSTK_setContextWindow(LFSTK_windowClass *wc)
{
	this->contextWC=wc;
}

/**
* Get context window for gadget.
* \return LFSTK_windowClass*.
*/
LFSTK_windowClass* LFSTK_gadgetClass::LFSTK_getContextWindow(void)
{
	return(this->contextWC);
}

/**
* Move gadget.
* \param x New X.
* \param y New Y.
* \param tellx Inform X (default=true).
*/
void LFSTK_gadgetClass::LFSTK_moveGadget(int x,int y)
{
	this->gadgetGeom.x=x;
	this->gadgetGeom.y=y;
	XMoveWindow(this->wc->app->display,this->window,x,y);
}

/**
* Set BG colour for label
* \param double r Red componant.
* \param double g Green componant.
* \param double b blue componant.
* \param double a Alpha componant.
* 
* \note The Alpha componant is also used for label autocolour.
*/
void LFSTK_gadgetClass::LFSTK_setLabelBGColour(double r,double g,double b,double a)
{
	this->labelBGColour.RGBAColour.r=r;
	this->labelBGColour.RGBAColour.g=g;
	this->labelBGColour.RGBAColour.b=b;
	this->labelBGColour.RGBAColour.a=a;
}

/**
* Set movement limts.
* \param int minx.
* \param int miny.
* \param int maxx.
* \param int maxy.
* \note To set no limit use -1.
*/
void LFSTK_gadgetClass::LFSTK_setLimits(int minx,int miny,int maxx,int maxy)
{
	this->minX=minx;
	this->minY=miny;
	this->maxX=maxx;
	this->maxY=maxy;
}

/**
* Get movement limts.
* \param *rectStruct.
* \note rect->x=minX, rect->w=this->maxX
* \note rect->y=minY, rect->h=this->maxY
*/
void LFSTK_gadgetClass::LFSTK_getLimits(rectStruct *rect)
{
	rect->x=this->minX;
	rect->w=this->maxX;
	rect->y=this->minY;
	rect->h=this->maxY;
}

/**
* Manually set indicator.
* \param int
*\note type is one of CHECK,RADIO,PICTURE,DISCLOSURE,NOINDICATOR.
*/
void LFSTK_gadgetClass::LFSTK_setIndicator(indicatorType indictype)
{
	unsigned int indicsize;

	this->gadgetDetails.indic=indictype;
	if(indictype!=NOINDICATOR)
		{
			switch(indictype)
				{
					case CHECK:
						indicsize=CHECKBOXSIZE;
						break;
					case DISCLOSURE:
						indicsize=TRIANGLESIZE;
						break;
				}
			this->gadgetDetails.hasIndicator=true;
			this->gadgetDetails.indicatorGeom={(int)(this->gadgetGeom.w-indicsize-(this->pad*2)),(int)((this->gadgetGeom.h/2)-(indicsize/2)+(this->pad/2)),indicsize,indicsize};
		}
	else
		{
			this->gadgetDetails.hasIndicator=false;
			this->gadgetDetails.indicatorGeom={0,0,0,0};
		}
}

/**
* Re Parent the gadget window.
* \param Window win New parent window.
* \param int newx,int newy New position.
*/
void LFSTK_gadgetClass::LFSTK_reParentWindow(Window win,int newx,int newy)
{
	XMapWindow(this->wc->app->display,win);
	XMapWindow(this->wc->app->display,this->window);
	XReparentWindow(this->wc->app->display,this->window,win,newx,newy);
	this->parent=win;
}

/**
* Set callbacks for gadget.
* \param callbackStruct cbs.
*/
void LFSTK_gadgetClass::LFSTK_setCallBacks(callbackStruct cbs)
{
	this->callBacks=cbs;
}

/**
* Set gadget style.
* \param s Gadget style.
*/
void LFSTK_gadgetClass::LFSTK_setStyle(bevelType s)
{
	this->style=s;
}

/**
* Set a colour name.
* \param colour Colour name.
* \note Alpha of colour is set to alpha of window normal, unless specifically set.
* \return A populated colourStruct.
*/
colourStruct LFSTK_gadgetClass::LFSTK_setColour(std::string colour)
{
	XColor		tc,sc;
	std::string	str=colour;
	int			alphaint=-1;
	colourStruct	cs;

	cs.name=colour;

	if(cs.name.at(0)=='#')
		{
			if(cs.name.length()>7)
				{
					alphaint=std::stoi (cs.name.substr(1,2),nullptr,16);
					cs.name.erase(cs.name.begin()+1,cs.name.begin()+3);
				}
		}

	if(XAllocNamedColor(this->wc->app->display,this->wc->app->cm,cs.name.c_str(),&sc,&tc)!=0)
		{
			cs.pixel=sc.pixel;
			cs.RGBAColour.r=((cs.pixel>>16) & 0xff)/256.0;
			cs.RGBAColour.g=((cs.pixel>>8) & 0xff)/256.0;
			cs.RGBAColour.b=((cs.pixel>>0) & 0xff)/256.0;

			if(alphaint!=-1)
				cs.RGBAColour.a=alphaint/256.0;
			else
				cs.RGBAColour.a=this->wc->windowColourNames[NORMALCOLOUR].RGBAColour.a;
			cs.isValid=true;
		}
	else
		cs.isValid=false;

	return(cs);
}

/**
* Set gadget colours.
* \param gadgetColourType Colour type GADGETBG GADGETFG.
* \param std::string normal Colour name (eg "grey", "#80ff00ff").
* \param std::string prelight		"
* \param std::string active			"
* \param std::string inactive		"
*/
void LFSTK_gadgetClass::LFSTK_setGadgetColours(gadgetColourType type,std::string normal,std::string prelight,std::string active,std::string inactive)
{
	colourStruct	cs;

	if(type==GADGETBG)
		{
			this->newGadgetBGColours[NORMALCOLOUR]=this->LFSTK_setColour(normal);
			this->newGadgetBGColours[PRELIGHTCOLOUR]=this->LFSTK_setColour(prelight);
			this->newGadgetBGColours[ACTIVECOLOUR]=this->LFSTK_setColour(active);
			this->newGadgetBGColours[INACTIVECOLOUR]=this->LFSTK_setColour(inactive);
		}
	else
		{

			this->newGadgetFGColours[INACTIVECOLOUR]=this->LFSTK_setColour(inactive);

			if(this->autoLabelColour==false)
				{
					this->newGadgetFGColours[NORMALCOLOUR]=this->LFSTK_setColour(normal);
					this->newGadgetFGColours[PRELIGHTCOLOUR]=this->LFSTK_setColour(prelight);
					this->newGadgetFGColours[ACTIVECOLOUR]=this->LFSTK_setColour(active);
				}
			else
				{
					for(int j=0;j<3;j++)
						{
							if(strcmp(this->wc->globalLib->bestFontColour(this->newGadgetBGColours.at(j).pixel),"black")==0)
								this->newGadgetFGColours[j]=this->LFSTK_setColour("black");
							else
								this->newGadgetFGColours[j]=this->LFSTK_setColour("white");
						}
				}
		}
}

/**
* Set pair of gadget colours bg/fg.
* \param gadgetState state NORMALCOLOUR etc.
* \param std::string back Colour BG name (eg "grey", "#80ff00ff").
* \param std::string fore Colour FG name (eg "grey", "#80ff00ff").
* \note will set auto colour FG if set.
*/
void LFSTK_gadgetClass::LFSTK_setGadgetColourPair(gadgetState state,std::string back,std::string fore)
{
	this->newGadgetBGColours[state]=this->LFSTK_setColour(back);
	if(this->autoLabelColour==false)
		this->newGadgetFGColours[state]=this->LFSTK_setColour(fore);
	else
		{
			if(strcmp(this->wc->globalLib->bestFontColour(this->newGadgetBGColours.at(state).pixel),"black")==0)
				this->newGadgetFGColours[state]=this->LFSTK_setColour("black");
			else
				this->newGadgetFGColours[state]=this->LFSTK_setColour("white");
		}
}