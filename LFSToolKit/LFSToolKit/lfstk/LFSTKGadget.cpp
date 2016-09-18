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

#include "LFSTKGlobals.h"

LFSTK_gadgetClass::~LFSTK_gadgetClass()
{
	if(this->label!=NULL)
		free(this->label);
	for(int j=NORMALCOLOUR;j<MAXCOLOURS;j++)
		{
			if(this->fontColourNames[j].name!=NULL)
				free(this->fontColourNames[j].name);
			if(this->colourNames[j].name!=NULL)
				free(this->colourNames[j].name);
		}
	if(this->fontString!=NULL)
		free(this->fontString);

	if(this->freeOnDelete==true)
		imlib_free_pixmap_and_mask(this->icon[0]);

	if(this->image!=NULL)
		{
			imlib_context_set_image(this->image);
			imlib_free_image();
		}

	if(this->scaledImage!=NULL)
		{
			imlib_context_set_image(this->scaledImage);
			imlib_free_image();
		}

	XftColorFree(this->display,this->visual,this->cm,&(this->blackXftColour));
	XftColorFree(this->display,this->visual,this->cm,&(this->whiteXftColour));
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
* \note state is NORMALCOLOUR=0,PRELIGHTCOLOUR=1,ACTIVECOLOUR=2,INACTIVECOLOUR=3.
*/
void LFSTK_gadgetClass::LFSTK_setFontColourName(int p,const char* colour)
{
	this->fontColourNames[p].name=strdup(colour);
	XftColorAllocName(this->display,this->visual,this->cm,colour,&(this->fontColourNames[p].xftcol));
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
	this->font=this->wc->globalLib->LFSTK_loadFont(this->display,this->screen,s);
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

	for(int j=0;j<MAXCOLOURS;j++)
		this->LFSTK_setColourName(j,this->wc->globalLib->LFSTK_getGlobalString(j,TYPEBUTTON));

	for(int j=0;j<MAXCOLOURS;j++)
		this->LFSTK_setFontColourName(j,this->wc->globalLib->LFSTK_getGlobalString(j,TYPEFONTCOLOUR));

	this->LFSTK_setFontString(this->wc->globalLib->LFSTK_getGlobalString(-1,TYPEFONT));
	this->LFSTK_setActive(true);
	this->LFSTK_setLabelAutoColour(false);
	this->style=BEVELOUT;
	this->labelOrientation=CENTRE;
	this->inWindow=false;
	this->autoLabelColour=this->wc->autoLabelColour;
	this->labelOffset=0;
	this->gotIcon=false;
	this->iconSize=16;
	this->freeOnDelete=false;
	this->useTile=false;
	this->useImage=false;
	this->image=NULL;
	this->scaledImage=NULL;
	this->gadgetAcceptsDnD=false;
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

	this->x=x;
	this->y=y;
	this->w=w;
	this->h=h;

	this->geom.x=x;
	this->geom.y=y;
	this->geom.w=w;
	this->geom.h=h;

	this->screen=this->wc->screen;
	this->visual=this->wc->visual;
	this->rootWindow=this->wc->rootWindow;
	this->cm=this->wc->cm;
	this->gc=this->wc->gc;

	if(label!=NULL)
		this->label=strdup(label);
	this->initGadget();
 	this->blackColour=BlackPixel(this->display,this->screen);
	this->whiteColour=WhitePixel(this->display,this->screen);
	XftColorAllocName(this->display,this->visual,this->cm,"black",&(this->blackXftColour));
	XftColorAllocName(this->display,this->visual,this->cm,"white",&(this->whiteXftColour));

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

#if 0
/**
* Set callback for widget.
* \param downcb Mouse down callback.
* \param releasecb Mouse up callback.
* \note Format for callback is "bool functioname(void *class,void *object,void* userdata)"
* \note First param passed to callback is pointer to object.
* \note Second param passed to callback is user data.
*/
void LFSTK_gadgetClass::LFSTK_setCallBack(bool (*downcb)(void*,void *,void*),bool (*releasecb)(void*,void *,void*),void* ud)
{
	this->callback.pressCallback=downcb;
	this->callback.releaseCallback=releasecb;
	this->callback.userData=ud;
	this->callback.ignoreCallback=false;
}

#endif

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
* Clear the gadget window to the appropriate state.
*/
void LFSTK_gadgetClass::LFSTK_clearWindow()
{
	geometryStruct g={0,0,this->w,this->h};

	if(this->isActive==true)
		{
			this->drawBox(&g,NORMALCOLOUR,this->style);
			this->LFSTK_drawLabel(NORMALCOLOUR);
		}
	else
		{
			this->drawBox(&g,INACTIVECOLOUR,this->style);
			this->LFSTK_drawLabel(INACTIVECOLOUR);
		}
}

/**
* Mouse up callback.
* \param e XButtonEvent passed from mainloop->listener.
* \return Return true if event fully handeled or false to pass it on.
*/
bool LFSTK_gadgetClass::mouseUp(XButtonEvent *e)
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
			this->mouseEnter(e);
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
	if(this->isActive==false)
		{
			this->LFSTK_clearWindow();
			return(true);
		}

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
	if(this->isActive==false)
		{
			this->LFSTK_clearWindow();
			return(true);
		}

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
	geometryStruct	g={0,0,this->w,this->h};

	if(this->isActive==false)
		{
			this->LFSTK_clearWindow();
			return(true);
		}

	this->drawBox(&g,PRELIGHTCOLOUR,this->style);
	this->LFSTK_drawLabel(PRELIGHTCOLOUR);
	this->inWindow=true;
	return(true);
}

void LFSTK_gadgetClass::LFSTK_resizeWindow(int w,int h)
{
	this->w=w;
	this->h=h;
	XResizeWindow(this->display,this->window,w,h);
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
* Draw string.
* \param font Font to use..
* \param x Xpos.
* \param y Ypos.
* \param col Colour.
* \param s String to draw.
*/
void LFSTK_gadgetClass::drawString(XftFont* font,int x,int y,int state,const char *s)
{
	XftDrawChange(this->wc->draw,this->window);
	if(this->autoLabelColour==true)
		{
			if(strcmp(this->wc->globalLib->bestFontColour(this->colourNames[state].pixel),"black")==0)
				XftDrawStringUtf8(this->wc->draw,&(this->blackXftColour),font,x,y,(XftChar8 *)s,strlen(s));
			else
				XftDrawStringUtf8(this->wc->draw,&(this->whiteXftColour),font,x,y,(XftChar8 *)s,strlen(s));
		}
	else
		XftDrawStringUtf8(this->wc->draw,&(this->fontColourNames[state].xftcol),font,x,y,(XftChar8 *)s,strlen(s));
}

/**
* Draw label.
* \param p Button state.
* \note State NORMALCOLOUR=0,PRELIGHTCOLOUR=1,ACTIVECOLOUR=2,INACTIVECOLOUR=3.
* \note If this->label="--" then dra a seperator.
*/
void LFSTK_gadgetClass::LFSTK_drawLabel(int state)
{
	if(strcmp(this->label,"--")!=0)
		{
			switch(this->labelOrientation)
				{
					case LEFT:
						this->drawString((XftFont*)(this->font->data),2+this->labelOffset,(this->h/2)+((this->font->ascent-2)/2),state,this->label);
						break;
					case RIGHT:
						this->drawString((XftFont*)(this->font->data),this->w-2-(this->wc->globalLib->LFSTK_getTextwidth(this->display,(XftFont*)(this->font->data),this->label))+this->labelOffset,(this->h/2)+((this->font->ascent-2)/2),state,this->label);
						break;
					default://centre
						this->drawString((XftFont*)(this->font->data),(this->w/2)-(this->wc->globalLib->LFSTK_getTextwidth(this->display,(XftFont*)this->font->data,this->label)/2)+this->labelOffset,(this->h/2)+((this->font->ascent-2)/2),state,this->label);
						break;
				}

			if(this->gotIcon==true)
				{
					XSetClipMask(this->display,this->gc,this->icon[1]);
					XSetClipOrigin(this->display,this->gc,4,(this->h/2)-(this->iconSize/2));
					XCopyArea(this->display,this->icon[0],this->window,this->gc,0,0,this->iconSize,this->iconSize,4,(this->h/2)-(this->iconSize/2));
					XSetClipMask(this->display,this->gc,None);
				}
			else if(this->useImage==true)
				{
					imlib_context_set_display(this->display);
					imlib_context_set_visual(this->visual);
					imlib_context_set_colormap(this->cm);
					if(this->scaledImage==NULL)
						{
							imlib_context_set_image(this->image);
							this->scaledImage=imlib_create_cropped_scaled_image(0,0,imlib_image_get_width(),imlib_image_get_height(),this->imageWidth,this->imageHeight);
						}
					imlib_context_set_drawable(this->window);
					imlib_context_set_image(this->scaledImage);
					imlib_render_image_on_drawable(4,(this->h/2)-(this->imageHeight/2)); 
				}
		}
	else
		{
			XSetClipMask(this->display,this->gc,None);
			XSetFillStyle(this->display,this->gc,FillSolid);
			XSetForeground(this->display,this->gc,this->blackColour);
			XDrawLine(this->display,this->window,this->gc,this->x,this->h/2,this->x+this->w,this->h/2);
			XSetForeground(this->display,this->gc,this->whiteColour);
			XDrawLine(this->display,this->window,this->gc,this->x,(this->h/2)+1,this->x+this->w,(this->h/2)+1);
		}
}

void LFSTK_gadgetClass::LFSTK_setLabel(const char *newlabel)
{
	if(this->label!=NULL)
		free(this->label);
	this->label=strdup(newlabel);
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
void LFSTK_gadgetClass::LFSTK_setLabelOriention(int orient)
{
	this->labelOrientation=orient;
}

/**
* Get gadget geometry.
* \return geometry structure.
* \note Caller should free structure after use.
*/
geometryStruct *LFSTK_gadgetClass::LFSTK_getGeom(void)
{
	geometryStruct		*g=new geometryStruct;
	XWindowAttributes	xwa;
	int					x,y;
	Window				child;

	XTranslateCoordinates(this->display,this->window,this->rootWindow,0,0,&x,&y,&child );
	XGetWindowAttributes(this->display,this->window,&xwa);

	g->x=x;
	g->y=y;
	g->w=xwa.width;
	g->h=xwa.height;

	return(g);
}

/**
* Get gadget geometry.
* \param geometry structure.
*/
void LFSTK_gadgetClass::LFSTK_getGeom(geometryStruct *geom)
{
	XWindowAttributes	xwa;
	int					x,y;
	Window				child;

	XTranslateCoordinates(this->display,this->window,this->rootWindow,0,0,&geom->x,&geom->y,&child );
	XGetWindowAttributes(this->display,this->window,&xwa);

	geom->w=xwa.width;
	geom->h=xwa.height;
}

/**
* Reload global colours from main window.
*/
void LFSTK_gadgetClass::LFSTK_reloadColours(void)
{
	for(int j=0;j<MAXCOLOURS;j++)
		this->LFSTK_setFontColourName(j,this->wc->globalLib->LFSTK_getGlobalString(j,TYPEFONTCOLOUR));
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
void LFSTK_gadgetClass::drawIndicator(geometryStruct* g,int state,indicatorType indic)
{
	int sx,sy;

	switch(indic)
		{
			case CHECK:
				XSetForeground(this->display,this->gc,this->blackColour);
				XDrawLine(this->display,this->window,this->gc,g->x,g->y+1,g->x+g->w-1,g->y+g->h);
				XDrawLine(this->display,this->window,this->gc,g->x+1,g->y+g->h,g->x+g->w,g->y+1);

				XSetForeground(this->display,this->gc,this->whiteColour);
				XDrawLine(this->display,this->window,this->gc,g->x,g->y,g->x+g->w,g->y+g->h);
				XDrawLine(this->display,this->window,this->gc,g->x,g->y+g->h,g->x+g->w,g->y);
				break;

			case DISCLOSURE:
				if(strcmp(this->wc->globalLib->bestFontColour(this->colourNames[state].pixel),"black")==0)
					XSetForeground(this->display,this->gc,this->blackColour);
				else
					XSetForeground(this->display,this->gc,this->whiteColour);
				sx=g->x+g->w-DISCLOSURESIZE;
				sy=g->y+(g->h/2)-(DISCLOSURESIZE/2);
				for(int j=0;j<DISCLOSURESIZE/2;j++)
					XDrawLine(this->display,this->window,this->gc,sx+j,sy+j,sx+j,sy+DISCLOSURESIZE-j);
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
	int tlcolour;
	int brcolour;

	switch(bevel)
		{
			case BEVELIN:
				tlcolour=this->blackColour;
				brcolour=this->whiteColour;
				break;
			case BEVELOUT:
				tlcolour=this->whiteColour;
				brcolour=this->blackColour;
				break;
			case BEVELNONE:
				tlcolour=this->colourNames[state].pixel;
				brcolour=this->colourNames[state].pixel;
				break;
		}

	if(this->useTile==true)
		{
			XSetTSOrigin(this->display,this->gc,0-this->x,0-this->y);
			XSetFillStyle(this->display,this->gc,FillTiled);
			XSetTile(this->display,this->gc,this->tile[0]);
			XFillRectangle(this->display,this->window,this->gc,g->x,g->y,g->w,g->h);
			XSetFillStyle(this->display,this->gc,FillSolid);
		}
	else
		{
			XSetFillStyle(this->display,this->gc,FillSolid);
			XSetClipMask(this->display,this->gc,None);
			XSetForeground(this->display,this->gc,this->colourNames[state].pixel);
			XFillRectangle(this->display,this->window,this->gc,g->x,g->y,g->w,g->h);
		}

	if(bevel!=BEVELNONE)
		{
//draw bottom rite
			XSetForeground(this->display,this->gc,brcolour);
			XDrawLine(this->display,this->window,this->gc,g->x,g->y+g->h-1,g->x+g->w-1,g->y+g->h-1);
			XDrawLine(this->display,this->window,this->gc,g->x+g->w-1,g->y+g->h-1,g->x+g->w-1,g->y);

//draw top left
			XSetForeground(this->display,this->gc,tlcolour);
			XDrawLine(this->display,this->window,this->gc,g->x,g->y,g->x,g->y+g->h-1);
			XDrawLine(this->display,this->window,this->gc,g->x,g->y,g->x+g->w-1,g->y);
		}
}

/**
* Set Icon.
* \param image Pixmap.
* \param mask Pixmap mask.
* \note Pixmap and mask are owned by caller.
*/
void LFSTK_gadgetClass::LFSTK_setIcon(Pixmap image,Pixmap mask,int size)
{
	this->icon[0]=image;
	this->icon[1]=mask;
	this->gotIcon=true;
	this->iconSize=size;
	this->labelOffset=this->iconSize+4;
	this->freeOnDelete=false;
}

/**
* Set Image.
* \param image Imlib_Image.
* \param w,h dimensions.
* \note Image is owned by caller.
*/
void LFSTK_gadgetClass::LFSTK_setImage(Imlib_Image image,int w,int h)
{
	this->image=image;
	this->useImage=true;
	this->imageWidth=w;
	this->imageHeight=h;
	this->labelOffset=w+4;
	this->freeOnDelete=false;
}

/**
* Set Icon.
* \param file Path to image file.
* \param size Size of image.
*/
void LFSTK_gadgetClass::LFSTK_setIconFromPath(const char *file,int size)
{
	Imlib_Image	image=NULL;

	image=imlib_load_image_immediately_without_cache(file);
	if(image!=NULL)
		{
			this->iconSize=size;
			imlib_context_set_display(this->wc->display);
			imlib_context_set_visual(this->wc->visual);
			imlib_context_set_colormap(this->wc->cm);
			imlib_context_set_drawable(this->wc->window);
			imlib_context_set_image(image);
			imlib_render_pixmaps_for_whole_image_at_size(&this->icon[0],&this->icon[1],this->iconSize,this->iconSize);
			imlib_free_image();
			this->gotIcon=true;
			this->labelOffset=this->iconSize+4;
			this->freeOnDelete=true;
		}
	else
		{
			this->gotIcon=false;
			this->freeOnDelete=false;
		}
}

/**
* Set image and render with imlib2.
* \param file Path to image file.
* \param w,h Size of image.
*/
void LFSTK_gadgetClass::LFSTK_setImageFromPath(const char *file,int w,int h)
{		
	if(file==NULL)
		{
			this->useImage=false;
		}
	else
		{
			this->image=NULL;

			this->image=imlib_load_image_immediately_without_cache(file);
			if(image!=NULL)
				{
					this->imageWidth=w;
					this->imageHeight=h;
					this->useImage=true;
					this->labelOffset=w+4;
				}
			else
				this->useImage=false;
		}
}

/**
* Get gadget monitor.
* \return unsigned Monitor that gadget top left is on;
*/

int LFSTK_gadgetClass::LFSTK_gadgetOnMonitor(void)
{
	int	tx=this->x;
	int	ty=this->y;

	geometryStruct	*g=this->wc->LFSTK_getGeom();

	if(tx>=0)
		tx=(g->x)+this->x;
	else
		tx=(g->w+g->x)-abs(this->x);

	if(ty>=0)
		ty=(g->y)+this->y;
	else
		ty=(g->h+g->y)-abs(this->y);
	free(g);

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
	if(this->useTile==true)
		{
			this->useTile=false;
			imlib_free_pixmap_and_mask(this->tile[0]);
		}
	if(this->wc->globalLib->LFSTK_setPixmapsFromPath(this->display,this->visual,this->cm,this->window,path,&this->tile[0],&this->tile[1],size)==true)
		this->useTile=true;
	else
		this->useTile=false;
}
