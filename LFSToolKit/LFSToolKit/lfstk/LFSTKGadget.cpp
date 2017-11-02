/*
 *
 * ©K. D. Hedger. Wed  5 Aug 13:53:44 BST 2015 kdhedger68713@gmail.com

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
#include <fcntl.h>
#include <jpeglib.h>

LFSTK_gadgetClass::~LFSTK_gadgetClass()
{
	if(this->label!=NULL)
		free(this->label);

	for(int j=NORMALCOLOUR;j<MAXCOLOURS;j++)
		{
			if(this->fontColourNames[j].name!=NULL)
				{
					free(this->fontColourNames[j].name);
					XFreeColors(this->display,this->cm,(long unsigned int*)&this->fontColourNames[j].pixel,1,0);
				}

			if(this->colourNames[j].name!=NULL)
				{
					free(this->colourNames[j].name);
					XFreeColors(this->display,this->cm,(long unsigned int*)&this->colourNames[j].pixel,1,0);
				}
		}

	if(this->fontString!=NULL)
		free(this->fontString);

	if(this->monoFontString!=NULL)
		free(this->monoFontString);

	cairo_destroy(this->cr);

	if(this->cImage!=NULL)
		cairo_surface_destroy(this->cImage);
	if(this->pattern!=NULL)
		cairo_pattern_destroy(this->pattern);

	cairo_surface_destroy(this->sfc);

	if(this->fontName!=NULL)
		free(this->fontName);

	XFreeGC(this->display,this->gc);
	XDestroyWindow(this->display,this->window);
}

LFSTK_gadgetClass::LFSTK_gadgetClass()
{
	pad=2;
}

/**
* Set the colour name for font.
* \param p Font state.
* \param colour Colour name.
* \param bool usewindow Use window colour.
* \note state is NORMALCOLOUR=0,PRELIGHTCOLOUR=1,ACTIVECOLOUR=2,INACTIVECOLOUR=3.
* \note font colour based on window or button back colour.
*/
void LFSTK_gadgetClass::LFSTK_setFontColourName(int p,const char* colour,bool usewindow)
{
	XColor			tc;
	XColor			sc;
	colourStruct	col=this->colourNames[p];

	if(this->fontColourNames[p].name!=NULL)
		free(this->fontColourNames[p].name);
	this->fontColourNames[p].name=strdup(colour);
	XAllocNamedColor(this->display,this->cm,colour,&sc,&tc);
	this->fontColourNames[p].pixel=tc.pixel;

	if(usewindow==true)
		col=this->wc->windowColourNames[0];

	if((this->autoLabelColour==true) && (p!=INACTIVECOLOUR))
		{
			if(strcmp(this->wc->globalLib->bestFontColour(col.pixel),"black")==0)
				{
					this->fontColourNames[p].RGBAColour.r=0.0;
					this->fontColourNames[p].RGBAColour.g=0.0;
					this->fontColourNames[p].RGBAColour.b=0.0;
					this->fontColourNames[p].RGBAColour.a=1.0;
				}
			else
				{
					this->fontColourNames[p].RGBAColour.r=1.0;
					this->fontColourNames[p].RGBAColour.g=1.0;
					this->fontColourNames[p].RGBAColour.b=1.0;
					this->fontColourNames[p].RGBAColour.a=1.0;
				}
		}
	else
		{
			this->fontColourNames[p].RGBAColour.r=((this->fontColourNames[p].pixel>>16) & 0xff)/256.0;
			this->fontColourNames[p].RGBAColour.g=((this->fontColourNames[p].pixel>>8) & 0xff)/256.0;
			this->fontColourNames[p].RGBAColour.b=((this->fontColourNames[p].pixel>>0) & 0xff)/256.0;
			this->fontColourNames[p].RGBAColour.a=1.0;
		}
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
* \param s Font string.
* \note eg:
* \note "sans-serif:size=8".
*/
void LFSTK_gadgetClass::LFSTK_setFontString(const char *s)
{
	if(this->fontString!=NULL)
		free(this->fontString);
	this->fontString=strdup(s);
}

/**
* Set the colour name for gadget.
* \param p Gadget state.
* \param colour Colour name.
* \note state is NORMALCOLOUR=0,PRELIGHTCOLOUR=1,ACTIVECOLOUR=2,INACTIVECOLOUR=3.
*/
void LFSTK_gadgetClass::LFSTK_setColourName(int p,const char* colour)
{
	XColor tc,sc;
	if(this->colourNames[p].name!=NULL)
		free(this->colourNames[p].name);
	this->colourNames[p].name=strdup(colour);
	XAllocNamedColor(this->display,this->cm,colour,&sc,&tc);
	this->colourNames[p].pixel=sc.pixel;

	this->colourNames[p].RGBAColour.r=((this->colourNames[p].pixel>>16) & 0xff)/256.0;
	this->colourNames[p].RGBAColour.g=((this->colourNames[p].pixel>>8) & 0xff)/256.0;
	this->colourNames[p].RGBAColour.b=((this->colourNames[p].pixel>>0) & 0xff)/256.0;
	this->colourNames[p].RGBAColour.a=1.0;
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
	return(this->colourNames[p].name);
}

//needs re vamping
void LFSTK_gadgetClass::initGadget(void)
{
	for(int j=0;j<MAXCOLOURS;j++)
		this->fontColourNames[j].name=NULL;

	for(int j=0;j<MAXCOLOURS;j++)
		this->colourNames[j].name=NULL;

	this->fontString=NULL;
	this->autoLabelColour=this->wc->autoLabelColour;

	for(int j=0;j<MAXCOLOURS;j++)
		this->LFSTK_setColourName(j,this->wc->globalLib->LFSTK_getGlobalString(j,TYPEBUTTON));

	for(int j=0;j<MAXCOLOURS;j++)
		this->LFSTK_setFontColourName(j,this->wc->globalLib->LFSTK_getGlobalString(j,TYPEFONTCOLOUR),false);

	this->LFSTK_setFontString(this->wc->globalLib->LFSTK_getGlobalString(-1,TYPEFONT));
	this->monoFontString=strdup("mono:size=12");
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
	this->display=this->wc->display;
	this->parent=this->wc->window;

	this->gadgetGeom.x=x;
	this->gadgetGeom.y=y;
	this->gadgetGeom.w=w;
	this->gadgetGeom.h=h;

	this->screen=this->wc->screen;
	this->visual=this->wc->visual;
	this->rootWindow=this->wc->rootWindow;
	this->cm=this->wc->cm;

	if(label!=NULL)
		this->label=strdup(label);
	else
		this->label=strdup("");

	this->initGadget();
 	this->blackColour=BlackPixel(this->display,this->screen);
	this->whiteColour=WhitePixel(this->display,this->screen);

	this->LFSTK_setCallBack(NULL,NULL,(void*)-1);
}

/**
* Set callback for widget.
* \param downcb Mouse down callback.
* \param releasecb Mouse up callback.
* \note Format for callback is "bool functioname(void *p,void* ud)"
* \note First param passed to callback is pointer to object.
* \note Second param passed to callback is user data.
*/
void LFSTK_gadgetClass::LFSTK_setCallBack(bool (*downcb)(void *,void*),bool (*releasecb)(void *,void*),void* ud)
{
	this->callback.pressCallback=downcb;
	this->callback.releaseCallback=releasecb;
	this->callback.userData=ud;
	this->callback.ignoreCallback=false;
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
* Clear box to colour.
* \param gadgetStruct* details
*/
void LFSTK_gadgetClass::clearBox(gadgetStruct* details)
{
	cairo_pattern_t	*patt;
	colourStruct	*usecolour;

	if(this->isActive==false)
		details->colour=&this->colourNames[INACTIVECOLOUR];

	if(this->useTile==true)
		{
			if(details->buttonTile==true)
				patt=this->pattern;
			else
				patt=this->wc->pattern;

			cairo_save(this->cr);
				cairo_reset_clip(this->cr);
				cairo_translate(this->cr,-this->gadgetGeom.x,-this->gadgetGeom.y);
				cairo_set_source(this->cr,patt);
				cairo_paint(this->cr);
			cairo_restore(this->cr);
		}
	else
		{
			cairo_save(this->cr);
				cairo_reset_clip(this->cr);
				cairo_set_source_rgba(this->cr,details->colour->RGBAColour.r,details->colour->RGBAColour.g,details->colour->RGBAColour.b,details->colour->RGBAColour.a);
				cairo_paint(this->cr);
			cairo_restore(this->cr);
		}
	XSync(this->display,false);
}

/**
* Draw bevel.
* \param geometryStruct	*geom
* \param bevelType		bevel
*/
void LFSTK_gadgetClass::drawBevel(geometryStruct* geom,bevelType bevel)
{
	cairoColor tlcolour;
	cairoColor brcolour;

	if(bevel==BEVELNONE)
		return;
	
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
		cairo_set_antialias (this->cr,CAIRO_ANTIALIAS_NONE);
		cairo_set_line_width(this->cr,1.0);
		cairo_set_source_rgba(this->cr,tlcolour.r,tlcolour.g,tlcolour.b,1.0);
		cairo_move_to(this->cr,geom->x+1,geom->h+geom->y+1);
		cairo_line_to(this->cr,geom->x+1,geom->y+1);
		cairo_line_to(this->cr,geom->x+geom->w,geom->y+1);
		cairo_stroke(this->cr);
		
		cairo_set_source_rgba(this->cr,brcolour.r,brcolour.g,brcolour.b,1.0);
		cairo_move_to(this->cr,geom->x+geom->w,geom->y+1);
		cairo_line_to(this->cr,geom->x+geom->w,geom->y+geom->h);
		cairo_line_to(this->cr,geom->x+1,geom->y+geom->h);
		cairo_stroke(this->cr);			
	cairo_restore(this->cr);
	XSync(this->display,false);
}

/**
* Draw label.
* \param p Button state.
* \note State NORMALCOLOUR=0,PRELIGHTCOLOUR=1,ACTIVECOLOUR=2,INACTIVECOLOUR=3.
* \note If this->label="--" then draw a seperator.
*/
void LFSTK_gadgetClass::drawLabel(gadgetStruct* details)
{
	int labelx=0;
	if(this->isActive==false)
		details->state=INACTIVECOLOUR;

	if(strcmp(this->label,"--")!=0)
		{
			switch(this->labelGravity)
				{
					case MENU:
						labelx=details->gadgetGeom.h+(this->pad*4);
						break;
					case LEFT:
						labelx=details->reserveSpace+this->pad;
						break;
					case CENTRE:
						labelx=((details->gadgetGeom.w-details->reserveSpace)/2)-(this->extents.width/2)+details->reserveSpace;
						break;
					case RIGHT:
						labelx=details->gadgetGeom.w-this->LFSTK_getTextWidth(this->label)-this->pad;
						break;
				}

			cairo_save(this->cr);
				cairo_select_font_face(this->cr,fontName,slant,weight);
				cairo_set_font_size(this->cr,fontSize);
				cairo_move_to(this->cr,labelx,(details->gadgetGeom.h/2)-(extents.y_bearing/2));
				cairo_set_source_rgba(this->cr,this->fontColourNames[details->state].RGBAColour.r,this->fontColourNames[details->state].RGBAColour.g,this->fontColourNames[details->state].RGBAColour.b,1.0);
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
	XSync(this->display,false);
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
* Mouse up callback.
* \param e XButtonEvent passed from mainloop->listener.
* \return Return true if event fully handeled or false to pass it on.
*/
bool LFSTK_gadgetClass::mouseUp(XButtonEvent *e)
{
	if((this->isActive==false) || (this->callback.ignoreCallback==true))
		return(true);

	if(strcmp(this->label,"--")==0)
		return(true);;


	this->gadgetDetails.colour=&this->colourNames[NORMALCOLOUR];
	this->gadgetDetails.state=NORMALCOLOUR;
	this->gadgetDetails.bevel=BEVELOUT;
	this->LFSTK_clearWindow();
	if(this->inWindow==true)
		{
			if(this->callback.releaseCallback!=NULL)
				return(this->callback.releaseCallback(this,this->callback.userData));
		}
	return(true);
}

/**
* Mouse down callback.
* \param e XButtonEvent passed from mainloop->listener.
* \return Return true if event fully handeled or false to pass it on.
*/
bool LFSTK_gadgetClass::mouseDown(XButtonEvent *e)
{
	if((this->isActive==false) || (this->callback.ignoreCallback==true))
		return(true);

	if(strcmp(this->label,"--")==0)
		return(true);;

	this->gadgetDetails.colour=&this->colourNames[ACTIVECOLOUR];
	this->gadgetDetails.state=ACTIVECOLOUR;
	this->gadgetDetails.bevel=BEVELIN;
	this->LFSTK_clearWindow();
	if(this->callback.pressCallback!=NULL)
		return(this->callback.pressCallback(this,this->callback.userData));
	return(true);
}

/**
* Mouse exit callback.
* \param e XButtonEvent passed from mainloop->listener.
* \return Return true if event fully handeled or false to pass it on.
*/
bool LFSTK_gadgetClass::mouseExit(XButtonEvent *e)
{
	if((this->isActive==false) || (this->callback.ignoreCallback==true))
		return(true);

	if(strcmp(this->label,"--")==0)
		return(true);;

	this->gadgetDetails.colour=&this->colourNames[NORMALCOLOUR];
	this->gadgetDetails.state=NORMALCOLOUR;
	this->gadgetDetails.bevel=BEVELOUT;
	this->LFSTK_clearWindow();
	this->inWindow=false;
	return(true);
}

/**
* Mouse enter callback.
* \param e XButtonEvent passed from mainloop->listener.
* \return Return true if event fully handeled or false to pass it on.
*/
bool LFSTK_gadgetClass::mouseEnter(XButtonEvent *e)
{
	if((this->isActive==false) || (this->callback.ignoreCallback==true))
		return(true);

	if(strcmp(this->label,"--")==0)
		return(true);

	this->gadgetDetails.colour=&this->colourNames[PRELIGHTCOLOUR];
	this->gadgetDetails.state=PRELIGHTCOLOUR;
	this->gadgetDetails.bevel=BEVELOUT;
	this->LFSTK_clearWindow();
	this->inWindow=true;
	return(true);
}

/**
* Mouse drag callback.
* \param e XButtonEvent passed from mainloop->listener.
* \return Return true if event fully handeled or false to pass it on.
*/
bool LFSTK_gadgetClass::mouseDrag(XMotionEvent *e)
{
	return(true);
}

void LFSTK_gadgetClass::LFSTK_resizeWindow(int w,int h)
{
	this->gadgetGeom.w=w;
	this->gadgetGeom.h=h;
	XResizeWindow(this->display,this->window,this->gadgetGeom.w,this->gadgetGeom.h);
	this->wc->setWindowGeom(0,0,this->gadgetGeom.w,this->gadgetGeom.h,WINDSETWH);
	this->LFSTK_clearWindow();
}

/**
* Key release callback.
* \param e XKeyEvent passed from mainloop->listener.
* \return Return true if event fully handeled or false to pass it on.
*/
bool LFSTK_gadgetClass::keyRelease(XKeyEvent *e)
{
	return(true);
}

/**
* Client Message callback.
* \param e XEvent passed from mainloop->listener.
* \return Return true if event fully handeled or false to pass it on.
*/
bool LFSTK_gadgetClass::clientMessage(XEvent *e)
{
	return(true);
}

/**
* Drop data.
* \param data Data drooped on gadget as string.
*/
void LFSTK_gadgetClass::LFSTK_dropData(propertyStruct* data)
{
	return;
}

/**
* Lost focus callback.
* \param e XEvent passed from mainloop->listener.
* \return Return true if event fully handeled or false to pass it on.
*/
bool LFSTK_gadgetClass::lostFocus(XEvent *e)
{
	return(true);
}

/**
* Got focus callback.
* \param e XEvent passed from mainloop->listener.
* \return Return true if event fully handeled or false to pass it on.
*/
bool LFSTK_gadgetClass::gotFocus(XEvent *e)
{
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
				//TODO//
				//xoffset=(this->gadgetGeom.w/2)-(width * ratio)/2+4;
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
			case NONE:
				xoffset=0;
				yoffset=0;
				break;
		}

	cairo_save(this->cr);
		cairo_reset_clip (this->cr);
		cairo_translate(this->cr,xoffset,yoffset);
		cairo_set_source_surface(this->cr,this->cImage,0,0);
		cairo_paint(this->cr);
	cairo_restore(this->cr);
	XSync(this->display,false);
}

/**
* Set font extents.
* \note Reset on changing label.
* \note Font string format "Sans:size=14:bold"
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
		if(strcmp(this->label,"")==0)
			cairo_text_extents(this->cr,"X|^_¸´█Á⌡",&this->extents);
		else
			cairo_text_extents(this->cr,this->label,&this->extents);
	cairo_restore(this->cr);
}

/**
* Get width of text.
* \returns test width
*/
int LFSTK_gadgetClass::LFSTK_getTextWidth(const char* text)
{
	cairo_text_extents_t returnextents;

	cairo_save(this->cr);
		cairo_select_font_face(this->cr,this->fontName,this->slant,this->weight);
		cairo_set_font_size(this->cr,this->fontSize);
		cairo_text_extents(this->cr,text,&returnextents);
	cairo_restore(this->cr);
	return(returnextents.width);
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
	return(returnextents.height);
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
	va_start(ap, fmt);
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
		cairo_text_extents(this->cr,this->label,&this->extents);
	cairo_restore(this->cr);
}

/**
* Set label.
* \param newlabel new label.
* \note Label is copied.
*/
void LFSTK_gadgetClass::LFSTK_setLabel(const char *newlabel)
{
	if(this->label!=NULL)
		free(this->label);
	this->label=strdup(newlabel);
	this->LFSTK_setCairoFontData();
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
* Get gadget geometry in global co-ords.
* \return geometry structure.
* \note Caller should free structure after use.
*/
void LFSTK_gadgetClass::LFSTK_getGlobalGeom(geometryStruct *geom)
{
	int					x,y;
	Window				child;
	XWindowAttributes	xwa;

	XTranslateCoordinates(this->display,this->window,this->rootWindow,0,0,&x,&y,&child );
	XGetWindowAttributes(this->display,this->window,&xwa);

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

	XGetWindowAttributes(this->display,this->window,&xwa);
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
	for(int j=0;j<MAXCOLOURS;j++)
		this->LFSTK_setFontColourName(j,this->wc->globalLib->LFSTK_getGlobalString(j,TYPEFONTCOLOUR),false);
	for(int j=0;j<MAXCOLOURS;j++)
		this->LFSTK_setColourName(j,this->wc->globalLib->LFSTK_getGlobalString(j,TYPEBUTTON));
	this->LFSTK_setFontString(this->wc->globalLib->LFSTK_getGlobalString(-1,TYPEFONT));
	this->autoLabelColour=this->wc->autoLabelColour;
}

/**
* Draw indicator.
* \param g Geometry Struture.
* \param state Button state.
* \param indic Indicator type CHECK=0,RADIO=1,PICTURE=2,NOINDICATOR=3.
*/
//void LFSTK_gadgetClass::drawIndicator(geometryStruct* g,int state,indicatorType indic)
void LFSTK_gadgetClass::drawIndicator(gadgetStruct* details)
{
	if(this->gadgetDetails.hasIndicator==false)
		return;

	switch(details->indic)
		{
			case CHECK:
				cairo_save(this->cr);
					cairo_reset_clip (this->cr);
					cairo_set_source_rgba(this->cr,this->colourNames[details->state].RGBAColour.r,colourNames[details->state].RGBAColour.g,colourNames[details->state].RGBAColour.b,colourNames[details->state].RGBAColour.a);
					cairo_rectangle(this->cr,details->indicatorGeom.x,details->indicatorGeom.y,details->indicatorGeom.w,details->indicatorGeom.h);
					cairo_fill(this->cr);
				cairo_restore(this->cr);
				this->drawBevel(&this->gadgetDetails.indicatorGeom,this->gadgetDetails.bevel);
				break;

			case DISCLOSURE:
				cairo_save(this->cr);
					cairo_reset_clip (this->cr);
					cairo_set_antialias (this->cr,CAIRO_ANTIALIAS_NONE);
				cairo_set_line_width(this->cr,1.0);

					cairo_set_source_rgba(this->cr,this->fontColourNames[details->state].RGBAColour.r,fontColourNames[details->state].RGBAColour.g,fontColourNames[details->state].RGBAColour.b,fontColourNames[details->state].RGBAColour.a);
					cairo_move_to(this->cr,details->indicatorGeom.x,(details->gadgetGeom.h/2)-(details->indicatorGeom.h/2));
					cairo_line_to(this->cr,details->indicatorGeom.x+details->indicatorGeom.w,details->gadgetGeom.h/2);
					cairo_line_to(this->cr,details->indicatorGeom.x,details->indicatorGeom.y+details->indicatorGeom.h);
					cairo_line_to(this->cr,details->indicatorGeom.x,(details->gadgetGeom.h/2)-(details->indicatorGeom.h/2));
					cairo_fill(this->cr);
				cairo_restore(this->cr);
				
				break;
		}
	XSync(this->display,false);
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
		cairo_set_source_rgba(this->cr,this->colourNames[state].RGBAColour.r,this->colourNames[state].RGBAColour.g,this->colourNames[state].RGBAColour.b,this->colourNames[state].RGBAColour.a);
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
	XSync(this->display,false);
}

/*! This function decompresses a JPEG image from a memory buffer and creates a
 * Cairo image surface.
 * @param data Pointer to JPEG data (i.e. the full contents of a JPEG file read
 * into this buffer).
 * @param len Length of buffer in bytes.
 * @return Returns a pointer to a cairo_surface_t structure. It should be
 * checked with cairo_surface_status() for errors.
 */
cairo_surface_t *cairo_image_surface_create_from_jpeg_mem(const unsigned char* data, size_t len)
{
	struct jpeg_decompress_struct	cinfo;
	struct jpeg_error_mgr			jerr;
	JSAMPROW						row_pointer[1];
	cairo_surface_t					*sfc;
 
   // initialize jpeg decompression structures
	cinfo.err=jpeg_std_error(&jerr);
	jpeg_create_decompress(&cinfo);
	jpeg_mem_src(&cinfo,(const unsigned char*)data, len);
	jpeg_read_header(&cinfo,true);

#if __BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__
	cinfo.out_color_space=JCS_EXT_BGRA;
#else
	cinfo.out_color_space=JCS_EXT_ARGB;
#endif

   // start decompressor
	jpeg_start_decompress(&cinfo);

   // create Cairo image surface
	sfc=cairo_image_surface_create(CAIRO_FORMAT_RGB24,cinfo.output_width,cinfo.output_height);
	if(cairo_surface_status(sfc)!=CAIRO_STATUS_SUCCESS)
		{
			jpeg_destroy_decompress(&cinfo);
			return(sfc);
		}

   // loop over all scanlines and fill Cairo image surface
	while(cinfo.output_scanline<cinfo.output_height)
		{
			row_pointer[0]=cairo_image_surface_get_data(sfc)+(cinfo.output_scanline * cairo_image_surface_get_stride(sfc));
			jpeg_read_scanlines(&cinfo,row_pointer,1);
		}

   // finish and close everything
	cairo_surface_mark_dirty(sfc);
	jpeg_finish_decompress(&cinfo);
	jpeg_destroy_decompress(&cinfo);

   // set jpeg mime data
	cairo_surface_set_mime_data(sfc,CAIRO_MIME_TYPE_JPEG,(const unsigned char*)data,len,free,(void*)data);

	return sfc;
}

/*! This function reads an JPEG image from a file an creates a Cairo image
 * surface. Internally the filesize is determined with fstat(2) and then the
 * whole data is read at once.
 * @param filename Pointer to filename of JPEG file.
 * @return Returns a pointer to a cairo_surface_t structure. It should be
 * checked with cairo_surface_status() for errors.
 * @note If the returned surface is invalid you can use errno to determine
 * further reasons. Errno is set according to fopen(3) and malloc(3). If you
 * intend to check errno you shall set it to 0 before calling this function
 * because it does not modify errno itself.
 */
cairo_surface_t *cairo_image_surface_create_from_jpeg(const char *filename)
{
	const unsigned char		*data;
	int						infile;
	struct stat				stat;
	char					magic[]="\xff\xd8\xff";

   // open input file
	if((infile=open(filename,O_RDONLY))==-1)
		return(NULL);
//		return(cairo_image_surface_create(CAIRO_FORMAT_INVALID,0,0));

   // get stat structure for file size
	if (fstat(infile,&stat)==-1)
		return(NULL);
//		return cairo_image_surface_create(CAIRO_FORMAT_INVALID, 0, 0);

   // allocate memory
	if((data=(const unsigned char*)malloc(stat.st_size))==NULL)
		return(NULL);
//		return(cairo_image_surface_create(CAIRO_FORMAT_INVALID,0,0));

   // read data
	if(read(infile,(void*)data,stat.st_size)<stat.st_size)
		return(NULL);
//		return(cairo_image_surface_create(CAIRO_FORMAT_INVALID,0,0));

	char *ptr=(char*)data;
	bool flag=true;
	for(int j=0;j<3;j++)
		if(ptr[j]!=magic[j])
			flag=false;

	close(infile);
	if(flag==false)
		{
			printf("not a jpeg\n");
			return(NULL);
		}
	return cairo_image_surface_create_from_jpeg_mem(data, stat.st_size);
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

	this->useImage=false;
	this->gotIcon=false;
	if(file==NULL)
		return(CAIRO_STATUS_FILE_NOT_FOUND);

	tempimage=cairo_image_surface_create_from_png(file);
	cs=cairo_surface_status(tempimage);
	if(cs!=CAIRO_STATUS_SUCCESS)
		{
			tempimage=cairo_image_surface_create_from_jpeg(file);
			if(tempimage==NULL)
				{
					printf("Unkown Format : %s\n",file);
					return(CAIRO_STATUS_INVALID_FORMAT);
				}
		}

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
			this->labelGravity=MENU;
			this->gadgetDetails.reserveSpace=maxHeight;
		}

	if(maxWidth>=maxHeight)
		ratio=maxHeight/height;
	else
		ratio=maxWidth/width;

	if(orient==FREE)
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

	return(cs);
}

/**
* Get gadget monitor.
* \return unsigned Monitor that gadget top left is on;
*/

int LFSTK_gadgetClass::LFSTK_gadgetOnMonitor(void)
{
	int	tx=this->gadgetGeom.x;
	int	ty=this->gadgetGeom.y;

	const geometryStruct	*g=this->wc->LFSTK_getWindowGeom();

	if(tx>=0)
		tx=(g->x)+this->gadgetGeom.x;
	else
		tx=(g->w+g->x)-abs(this->gadgetGeom.x);

	if(ty>=0)
		ty=(g->y)+this->gadgetGeom.y;
	else
		ty=(g->h+g->y)-abs(this->gadgetGeom.y);

	if(tx<0)
		tx=0;
	if(ty<0)
		ty=0;

	const monitorStruct* monitors=this->wc->LFSTK_getMonitors();

	for(int j=0;j<this->wc->LFSTK_getMonitorCount();j++)
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
	if(this->pattern!=NULL)
		{
			cairo_pattern_destroy(this->pattern);
			this->pattern=NULL;
		}
	if(path==NULL)
		{
			this->useTile=false;
			return;
		}

	cairo_surface_t	*tempimage;
	cairo_status_t	cs=CAIRO_STATUS_SUCCESS;
	tempimage=cairo_image_surface_create_from_png(path);
	cs=cairo_surface_status(tempimage);
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
* Ignore callbacks.
* \param ignore.
*/
void LFSTK_gadgetClass::LFSTK_setIgnoreCB(bool ignore)
{
	this->callback.ignoreCallback=ignore;
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

