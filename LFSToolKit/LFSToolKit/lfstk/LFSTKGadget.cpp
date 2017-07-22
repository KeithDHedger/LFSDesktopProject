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
	this->labelGravity=CENTRE;
	this->inWindow=false;
	this->autoLabelColour=this->wc->autoLabelColour;
	this->labelOffset=0;
	this->gotIcon=false;
	this->iconSize=16;
	this->freeOnDelete=false;
	this->useTile=false;
	this->useImage=false;
	this->image=NULL;
	this->imageWidth=0;

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

	this->gadgetGeom.x=x;
	this->gadgetGeom.y=y;
	this->gadgetGeom.w=w;
	this->gadgetGeom.h=h;

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
	gadgetState		drawcolour=INACTIVECOLOUR;
	geometryStruct	g={0,0,this->gadgetGeom.w,this->gadgetGeom.h};

	if(this->isActive==true)
		drawcolour=NORMALCOLOUR;

	this->drawBox(&g,drawcolour,this->style);

	if(this->useImage==true)
		this->drawImage();

	this->LFSTK_drawLabel(drawcolour);
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
	if(this->useImage==true)
		this->drawImage();
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
	geometryStruct	g={0,0,this->gadgetGeom.w,this->gadgetGeom.h};

	if(this->isActive==false)
		{
			this->LFSTK_clearWindow();
			return(true);
		}

	this->drawBox(&g,PRELIGHTCOLOUR,this->style);
	this->LFSTK_drawLabel(PRELIGHTCOLOUR);
	if(this->useImage==true)
		this->drawImage();
	XSync(this->display,false);
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
			case AUTO:
				//TODO//
				//xoffset=(maxWidth/2)-(width * ratio)/2+4;
				break;
			case LEFT:
				xoffset=4;
				break;
		}

	cairo_save(this->cr);
		cairo_reset_clip (this->cr);
		cairo_translate(this->cr,xoffset,yoffset);
		cairo_set_source_surface(this->cr,this->cImage,0,0);
		cairo_paint(this->cr);
	cairo_restore(this->cr);
}

/**
* Draw label.
* \param p Button state.
* \note State NORMALCOLOUR=0,PRELIGHTCOLOUR=1,ACTIVECOLOUR=2,INACTIVECOLOUR=3.
* \note If this->label="--" then dra a seperator.
*/
void LFSTK_gadgetClass::LFSTK_drawLabel(int state)
{
	int	labelwidth=0;
	int labelx=0;

	labelwidth=this->wc->globalLib->LFSTK_getTextwidth(this->display,(XftFont*)this->font->data,this->label);
	if(strcmp(this->label,"--")!=0)
		{
			switch(this->labelGravity)
				{
					case LEFT:
						if(this->useImage==true)
							labelx=this->imageWidth+8;
						else
							labelx=this->labelOffset+2;
						this->drawString((XftFont*)(this->font->data),labelx,(this->gadgetGeom.h/2)+((this->font->ascent-2)/2),state,this->label);	
						break;
					case RIGHT:
						labelx=this->gadgetGeom.w-labelwidth-2;			
						this->drawString((XftFont*)(this->font->data),labelx,(this->gadgetGeom.h/2)+((this->font->ascent-2)/2),state,this->label);	
						break;
					case MENU:
						if(this->useImage==true)
							labelx=this->imageWidth+8;
						else
							labelx=this->gadgetGeom.h+1;
						this->drawString((XftFont*)(this->font->data),labelx,(this->gadgetGeom.h/2)+((this->font->ascent-2)/2),state,this->label);
						break;
					default://centre
						labelx=((this->gadgetGeom.w-this->imageWidth)/2)-(labelwidth/2)+this->imageWidth;
						this->drawString((XftFont*)(this->font->data),labelx,(this->gadgetGeom.h/2)+((this->font->ascent-2)/2),state,this->label);
						break;
				}

//			if(this->gotIcon==true)
//				{
//					XSetClipMask(this->display,this->gc,this->icon[1]);
//					XSetClipOrigin(this->display,this->gc,4,(this->gadgetGeom.h/2)-(this->iconSize/2));
//					XCopyArea(this->display,this->icon[0],this->window,this->gc,0,0,this->iconSize,this->iconSize,4,(this->gadgetGeom.h/2)-(this->iconSize/2));
//					XSetClipMask(this->display,this->gc,None);
//				}
		}
	else
		{
			XSetClipMask(this->display,this->gc,None);
			XSetFillStyle(this->display,this->gc,FillSolid);
			XSetForeground(this->display,this->gc,this->blackColour);
			XDrawLine(this->display,this->window,this->gc,this->gadgetGeom.x,this->gadgetGeom.h/2,this->gadgetGeom.x+this->gadgetGeom.w,this->gadgetGeom.h/2);
			XSetForeground(this->display,this->gc,this->whiteColour);
			XDrawLine(this->display,this->window,this->gc,this->gadgetGeom.x,(this->gadgetGeom.h/2)+1,this->gadgetGeom.x+this->gadgetGeom.w,(this->gadgetGeom.h/2)+1);
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
				if(state==PRELIGHTCOLOUR)
//				if(strcmp(this->wc->globalLib->bestFontColour(this->colourNames[state].pixel),"black")==0)
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
			XSetTSOrigin(this->display,this->gc,0-this->gadgetGeom.x,0-this->gadgetGeom.y);
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
		return(cairo_image_surface_create(CAIRO_FORMAT_INVALID,0,0));

   // get stat structure for file size
	if (fstat(infile,&stat)==-1)
		return cairo_image_surface_create(CAIRO_FORMAT_INVALID, 0, 0);

   // allocate memory
	if((data=(const unsigned char*)malloc(stat.st_size))==NULL)
		return(cairo_image_surface_create(CAIRO_FORMAT_INVALID,0,0));

   // read data
	if(read(infile,(void*)data,stat.st_size)<stat.st_size)
		return(cairo_image_surface_create(CAIRO_FORMAT_INVALID,0,0));

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
* \param w,h Size of image.
*/
cairo_status_t LFSTK_gadgetClass::LFSTK_setImageFromPath(const char *file,int grav)
{
	cairo_status_t	cs=CAIRO_STATUS_SUCCESS;
	cairo_surface_t	*tempimage;
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

	this->sfc=cairo_xlib_surface_create(this->display,this->window,this->visual,this->gadgetGeom.w,this->gadgetGeom.h);
	this->cr=cairo_create(sfc);

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

	txtx=this->wc->globalLib->LFSTK_getTextwidth(this->display,(XftFont*)(this->font->data),this->label);
	maxWidth=this->gadgetGeom.w-txtx-8;
	maxHeight=this->gadgetGeom.h-8;
	width=cairo_image_surface_get_width(tempimage);
	height=cairo_image_surface_get_height(tempimage);

	if(maxWidth>maxHeight)
		ratio=maxHeight/height;
	else
		ratio=maxWidth/width;

	this->imageWidth=width*ratio;
	this->imageHeight=height*ratio;

	this->useImage=true;
	this->imageGravity=grav;
	this->labelOffset=this->imageWidth;

	this->cImage=cairo_surface_create_similar_image(tempimage,cairo_image_surface_get_format(tempimage),this->imageWidth,this->imageHeight);
	tcr=cairo_create(this->cImage);
	cairo_reset_clip(tcr);
	cairo_scale(tcr,ratio,ratio);
	cairo_set_source_surface(tcr,tempimage,0,0);
	cairo_paint(tcr);

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

/**
* Get font width and height.
* \param geometryStruct *geom;
* \param const char *label;
*/

void LFSTK_gadgetClass::LFSTK_getFontGeom(geometryStruct *geom,const char *label)
{
	const char		*itemfont;
	fontStruct		*tfont;

	geom->x=0;
	geom->y=0;
	itemfont=this->wc->globalLib->LFSTK_getGlobalString(-1,TYPEMENUITEMFONT);
	tfont=this->wc->globalLib->LFSTK_loadFont(this->display,this->screen,itemfont);
	geom->w=this->wc->globalLib->LFSTK_getTextwidth(this->display,(XftFont*)(tfont->data),label);
	geom->h=tfont->ascent+tfont->descent+8;
}
