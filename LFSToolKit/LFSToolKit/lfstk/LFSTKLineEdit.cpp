/*
 *
 * ©K. D. Hedger. Wed  5 Aug 12:36:15 BST 2015 keithdhedger@gmail.com

 * This file (LFSTKLineEdit.cpp) is part of LFSToolKit.

 * LFSToolKit is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation,either version 3 of the License,or
 * at your option) any later version.

 * LFSToolKit is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

 * You should have received a copy of the GNU General Public License
 * along with LFSToolKit.  If not,see <http://www.gnu.org/licenses/>.
 */

#include <unistd.h>
#include <stdlib.h>

#include "lfstk/LFSTKGlobals.h"

LFSTK_lineEditClass::~LFSTK_lineEditClass()
{
	if(this->cursorColour.name!=NULL)
		{
			free(this->cursorColour.name);
			XFreeColors(this->display,this->cm,(long unsigned int*)&this->cursorColour.pixel,1,0);
		}
}

LFSTK_lineEditClass::LFSTK_lineEditClass()
{
}

/**
* Main line edit constructor.
*
* \param parentwc Main parent window class.
* \param label Displayed name.
* \param x X pos.
* \param y Y pos.
* \param w Width.
* \param h Height.
* \param gravity Button gravity.
*/
LFSTK_lineEditClass::LFSTK_lineEditClass(LFSTK_windowClass* parentwc,const char* label,int x,int y,unsigned w,unsigned h,int gravity)
{
	XSetWindowAttributes	wa;

	if(label==NULL)
		label="";

	this->LFSTK_setCommon(parentwc,label,x,y,w,h,gravity);
	this->isFocused=false;
	this->inWindow=false;

	wa.win_gravity=gravity;
	wa.bit_gravity=gravity;
	wa.save_under=true;

	this->window=XCreateWindow(this->display,this->parent,x,y,w,h,0,CopyFromParent,InputOutput,CopyFromParent,CWWinGravity|CWBitGravity,&wa);
	this->gc=XCreateGC(this->display,this->window,0,NULL);
	this->LFSTK_setFontString(this->monoFontString);
	this->wc->globalLib->LFSTK_setCairoSurface(this->display,this->window,this->visual,&this->sfc,&this->cr,w,h);
	this->LFSTK_setCairoFontData();
	XSelectInput(this->display,this->window,ButtonReleaseMask | ButtonPressMask | ExposureMask | EnterWindowMask | LeaveWindowMask|FocusChangeMask|KeyReleaseMask|SelectionClear|SelectionRequest);

	this->ml->function=&LFSTK_lib::LFSTK_gadgetEvent;
	this->ml->gadget=this;
	this->ml->type=LINEEDITGADGET;
	this->wc->LFSTK_addMappedListener(this->window,ml);

	if(strlen(label)>0)
		this->cursorPos=strlen(label);
	else
		this->cursorPos=0;
	this->buffer=label;

	this->wc->LFSTK_initDnD();
	this->gadgetAcceptsDnD=true;
	this->labelGravity=LEFT;

	LFSTK_setColourName(NORMALCOLOUR,"white");
	LFSTK_setFontColourName(NORMALCOLOUR,"black",false);
	this->charWidth=LFSTK_getTextRealWidth("X");
	this->LFSTK_setCursorColourName(this->wc->globalLib->LFSTK_getGlobalString(-1,TYPECURSORCOLOUR));
	gadgetDetails={&this->colourNames[NORMALCOLOUR],BEVELIN,NOINDICATOR,NORMALCOLOUR,0,true,{0,0,w,h},{0,0,0,0},false};
}

/**
* Clear the gadget window to the appropriate state.
*/
void LFSTK_lineEditClass::LFSTK_clearWindow()
{
	this->gadgetDetails.bevel=BEVELIN;
	this->drawLabel();
	this->drawBevel(&this->gadgetDetails.gadgetGeom,this->gadgetDetails.bevel);
	return;
}

/**
* Set this gadget to have the focus..
*/
void LFSTK_lineEditClass::LFSTK_setFocus(void)
{
	XSetInputFocus(this->display,this->window,RevertToParent,CurrentTime);
	this->isFocused=true;
	this->LFSTK_clearWindow();
}

/**
* Configure Message callback.
* \param e XButtonEvent passed from mainloop->listener.
* \return Return true if event fully handeled or false to pass it on.
*/
bool LFSTK_lineEditClass::clientMessage(XEvent *e)
{
printf("confmes from line edit\n");
	return(true);
}

/**
* Mouse enter callback.
* \param e XButtonEvent passed from mainloop->listener.
* \return Return true if event fully handeled or false to pass it on.
*/
bool LFSTK_lineEditClass::mouseEnter(XButtonEvent *e)
{
	return(true);
}

/**
* Mouse down callback.
* \param e XButtonEvent passed from mainloop->listener.
* \return Return true if event fully handeled or false to pass it on.
*/
bool LFSTK_lineEditClass::mouseDown(XButtonEvent *e)
{
	if(this->isActive==false)
		{
			this->LFSTK_clearWindow();
			return(true);
		}

	return(true);
}

void LFSTK_lineEditClass::LFSTK_resizeWindow(int w,int h)
{
	this->gadgetGeom.w=w-(pad*2);
	this->gadgetGeom.h=h-(pad*2);
	XResizeWindow(this->display,this->window,this->gadgetGeom.w,this->gadgetGeom.h);
	this->LFSTK_clearWindow();
}

/**
* Lost focus callback.
* \param e XEvent passed from mainloop->listener.
* \return Return true if event fully handeled or false to pass it on.
*/
bool LFSTK_lineEditClass::lostFocus(XEvent *e)
{
	if(this->isFocused==true)
		{
			XUngrabKeyboard(this->display,CurrentTime);
			this->isFocused=false;
			this->LFSTK_clearWindow();
		}
	return(true);
}

/**
* Got focus callback.
* \param e XEvent passed from mainloop->listener.
* \return Return true if event fully handeled or false to pass it on.
*/
bool LFSTK_lineEditClass::gotFocus(XEvent *e)
{
	if(this->isFocused==false)
		{
			XGrabKeyboard(this->display,this->window,true,GrabModeAsync,GrabModeAsync,CurrentTime);
			this->isFocused=true;
			XSetInputFocus(this->display,this->window,RevertToParent,CurrentTime);
			this->LFSTK_clearWindow();
		}
	return(true);
}

/**
* Set the contents of the text buffer.
* \param const char *str String to set.
*/
void LFSTK_lineEditClass::LFSTK_setBuffer(const char *str)
{
	const char	*bufferstr=str;

	if(bufferstr==NULL)
		bufferstr="";
	this->buffer=bufferstr;
	this->cursorPos=strlen(bufferstr);
	this->LFSTK_clearWindow();
}

/**
* Return the contents of the gadget.
* \return Return's a std::string.
* \note Don't free the returned string.
*/
const std::string* LFSTK_lineEditClass::LFSTK_getBuffer(void)
{
	return(const_cast<std::string*>(&(this->buffer)));
}

/**
* Return the contents of the gadget.
* \return Return's a c str.
* \note Don't free the returned string.
*/
const char* LFSTK_lineEditClass::LFSTK_getCStr(void)
{
	return(this->buffer.c_str());
}


void LFSTK_lineEditClass::drawLabel(void)
{
	int						startchar=0;
	double					yoffset=0;
	cairo_text_extents_t	partextents;
	char				 	*data;
	char					undercurs[2]={0,0};
	char					*aftercursor;
	int						maxchars;
	cairo_text_extents_t	charextents;

	cairo_save(this->cr);
		cairo_reset_clip(this->cr);
		cairo_set_source_rgba(this->cr,this->colourNames[NORMALCOLOUR].RGBAColour.r,this->colourNames[NORMALCOLOUR].RGBAColour.g,this->colourNames[NORMALCOLOUR].RGBAColour.b,this->colourNames[NORMALCOLOUR].RGBAColour.a);
		cairo_paint(this->cr);
	cairo_restore(this->cr);

	cairo_save(this->cr);
		cairo_select_font_face(this->cr,this->fontName,this->slant,this->weight);
		cairo_set_font_size(this->cr,fontSize);
		cairo_set_source_rgba(this->cr,0.0,0,0,1.0);

		if(this->LFSTK_getTextWidth("X")<=0)
			maxchars=((this->gadgetGeom.w)/(int)((8)));
		else
			maxchars=(this->gadgetGeom.w/(int)(this->LFSTK_getTextRealWidth("X")+0.5))-1;

		if(maxchars>this->buffer.length())
			maxchars=this->buffer.length();

		startchar=(this->cursorPos)-maxchars;
		yoffset=(this->gadgetDetails.gadgetGeom.h/2)-(this->textExtents.y_bearing/2);
		cairo_move_to(this->cr,this->pad,yoffset);

		if(startchar<0)
			startchar=0;

		asprintf(&data,"%s",this->buffer.substr(startchar,this->cursorPos-startchar).c_str());
		undercurs[0]=this->buffer.c_str()[this->cursorPos];
		if(undercurs[0]==0)
			undercurs[0]=' ';

		if(this->cursorPos-startchar>=maxchars)
			asprintf(&aftercursor,"");
		else
			asprintf(&aftercursor,"%s",this->buffer.substr(this->cursorPos+1,(maxchars-this->cursorPos-startchar)).c_str());

//1stbit
		cairo_show_text(this->cr,data);
		cairo_text_extents(this->cr,data,&partextents);
		cairo_text_extents (this->cr,undercurs,&charextents);

		if(this->isFocused==true)
			{

				cairo_set_source_rgba(this->cr,this->cursorColour.RGBAColour.r,this->cursorColour.RGBAColour.g,this->cursorColour.RGBAColour.b,this->cursorColour.RGBAColour.a);
				cairo_rectangle(this->cr,partextents.x_advance+0.5,yoffset-this->fontExtents.ascent,charextents.x_advance-0.5,this->fontExtents.ascent+this->fontExtents.descent);
				cairo_fill(this->cr);
			}
		cairo_set_source_rgba(this->cr,1.0,1.0,1.0,1.0);
//secondbit
		cairo_move_to(this->cr,partextents.x_advance,yoffset);
		cairo_show_text(this->cr,undercurs);
//3rdbit
		cairo_set_source_rgba(this->cr,0.0,0.0,0.0,1.0);
		cairo_show_text(this->cr,aftercursor);

		free(data);
		free(aftercursor);
	cairo_restore(this->cr);
}

/**
* Set contents to the clipboard.
*/
void LFSTK_lineEditClass::getClip(void)
{
	Window			selectionOwner;
	unsigned char	*data=NULL;
	Atom			type;
	int				format,result;
	unsigned long	len=0,bytesLeft=0,dummy=0;
	bool			run=true;
	XEvent			event;

	selectionOwner=XGetSelectionOwner(this->display,this->wc->LFSTK_getDnDAtom(XA_CLIPBOARD));
//TODO//HACK to paste into self!!
//printf("selectionOwner=%i this->wc->window=%i\n",selectionOwner,this->wc->window);

	if(selectionOwner==this->wc->window)
		{
			//this->buffer=this->buffer+this->wc->clipBuffer;
			this->LFSTK_setFormatedText(this->wc->clipBuffer.c_str(),false);
			this->LFSTK_clearWindow();
			return;
		}

	if (selectionOwner!=None)
		{
			XConvertSelection(this->display,this->wc->LFSTK_getDnDAtom(XA_CLIPBOARD),this->wc->LFSTK_getDnDAtom(XA_UTF8_STRING),this->wc->LFSTK_getDnDAtom(XA_CLIPBOARD),this->window,CurrentTime);
			XFlush(this->display);

			while (run==true)
				{
					XNextEvent(this->display,&event);
					switch(event.type)
						{
							case SelectionNotify:
								if(event.xselection.requestor==this->window)
									run=false;
								break;
						}
				}

			XGetWindowProperty(this->display,this->window,this->wc->LFSTK_getDnDAtom(XA_CLIPBOARD),0,0,False,AnyPropertyType,&type,&format,&len,&bytesLeft,&data);
			if(data!=NULL)
				{
					XFree(data);
					data=NULL;
				}

			if(bytesLeft>0)
				{
					result=XGetWindowProperty(this->display,this->window,this->wc->LFSTK_getDnDAtom(XA_CLIPBOARD),0,bytesLeft,False,AnyPropertyType,&type,&format,&len,&dummy,&data);
					if (result==Success)
						{
							this->LFSTK_setFormatedText((const char*)data,false);
							XFree(data);
						}
				}
			XDeleteProperty(this->display,this->window,this->wc->LFSTK_getDnDAtom(XA_CLIPBOARD));
		}
}

/**
* Key release callback.
* \param e XEvent passed from mainloop->listener.
* \return Return true if event fully handeled or false to pass it on.
* \note button down callback is used for "Return" key callback.
* \note button up callback is used for key callback.
* \note Keysyms here keysymdef.h
*/
bool LFSTK_lineEditClass::keyRelease(XKeyEvent *e)
{
	char	c[255];
	KeySym	keysym_return;
	char	*command;

	if(this->isActive==false)
		{
			this->LFSTK_clearWindow();
			return(true);
		}

	XLookupString(e,(char*)&c,255,&keysym_return,NULL);

	if(this->isFocused==false)
		return(true);

	if(e->state==ControlMask)
		{
			if(keysym_return==XK_v)
				this->getClip();
//TODO//
			if(keysym_return==XK_c)
				{
					this->wc->clipBuffer=this->buffer;
					XSetSelectionOwner(this->display,this->wc->LFSTK_getDnDAtom(XA_CLIPBOARD),this->wc->window,CurrentTime);
				}

			if(keysym_return==XK_Delete)
				{
					this->LFSTK_setBuffer("");
				}
		}
	else
		{
			switch(keysym_return)
				{
				case XK_Tab:
					this->buffer.insert(this->cursorPos,8,' ');
					this->cursorPos+=8;

					break;
				case XK_BackSpace:
					if(this->cursorPos>0)
						{
							this->buffer.erase(this->cursorPos-1,1);
							this->cursorPos--;
						}
					break;
				case XK_Delete:
					if(this->cursorPos<this->buffer.length())
						this->buffer.erase(this->cursorPos,1);
					break;
				case XK_Left:
					if(this->cursorPos>0)
						this->cursorPos--;
					break;
				case XK_Right:
					if(this->cursorPos<this->buffer.length())
						this->cursorPos++;
					break;
				case XK_End:
				case XK_Page_Down:
					this->cursorPos=this->buffer.length();
					break;
				case XK_Home:
				case XK_Page_Up:
					this->cursorPos=0;
					//this->drawLabel();
					break;
				case XK_Down:
				case XK_Up:
				case XK_Select ... XK_Num_Lock:
				case XK_F1 ... XK_R15:
					break;
				case XK_Return:
					if(this->callback.pressCallback!=NULL)
						return(this->callback.pressCallback(this,this->callback.userData));
					break;

				default:
					if(c[0]==0)
						break;
					this->buffer.insert(this->cursorPos,1,c[0]);
					this->cursorPos++;
					break;
				}
		}

	this->LFSTK_clearWindow();
	if(this->callback.releaseCallback!=NULL)
		return(this->callback.releaseCallback(this,this->callback.userData));

	return(true);
}

/**
* Drop data.
* \param data Data drooped on gadget as string.
*/
void LFSTK_lineEditClass::LFSTK_dropData(propertyStruct* data)
{
	int	endl;

	if(strcasecmp(data->mimeType,"text/plain")==0)
		this->LFSTK_setFormatedText((const char*)data->data,true);

	if(strcasecmp(data->mimeType,"text/uri-list")==0)
		{
			char	*d;
			char	*ret;
			asprintf(&d,"%s",(const char*)data->data);
			endl=strlen(d)-1;
			while ((endl >= 0) && (isspace(d[endl])) )
				{
					d[endl]=0;
					endl--;
				}
			ret=this->wc->globalLib->LFSTK_oneLiner("echo -n \"%s\"|sed 's|^file://||;s|%%20| |g'",d);
			this->LFSTK_setFormatedText((const char*)ret,true);
			free(ret);
			free(d);
		}
}

/**
* Set formated txt.
* \param const char* Text to be formated.
* \param bool true=Replace contents, false=insert.
*/
void  LFSTK_lineEditClass::LFSTK_setFormatedText(const char *txt,bool replace)
{
	std::string	formtxt="";
	int			len=0;

	for(int j=0;j<strlen(txt);j++)
		{
			switch(txt[j])
				{
					case '\t':
						formtxt.append("        ");
						len+=8;
						break;
					case '\n':
					case 0:
					case '\r':
						break;
					default:
						formtxt.append(1,txt[j]);
						len++;
						break;
				}
		}

	if(replace==true)
		{
			this->buffer=formtxt;
			this->cursorPos=this->buffer.length();
			this->LFSTK_clearWindow();
		}
	else
		{
			this->buffer.insert(this->cursorPos,formtxt);
			this->cursorPos+=len;
			this->LFSTK_clearWindow();
		}
}

/**
* Set the colour name for cursor.
* \param colour Colour name.
*/
void LFSTK_lineEditClass::LFSTK_setCursorColourName(const char* colour)
{
	XColor			tc;
	XColor			sc;

	if(this->cursorColour.name!=NULL)
		free(this->cursorColour.name);
	this->cursorColour.name=strdup(colour);
	XAllocNamedColor(this->display,this->cm,colour,&sc,&tc);
	this->cursorColour.pixel=tc.pixel;

	this->cursorColour.RGBAColour.r=((this->cursorColour.pixel>>16) & 0xff)/256.0;
	this->cursorColour.RGBAColour.g=((this->cursorColour.pixel>>8) & 0xff)/256.0;
	this->cursorColour.RGBAColour.b=((this->cursorColour.pixel>>0) & 0xff)/256.0;
	this->cursorColour.RGBAColour.a=0.8;
}


