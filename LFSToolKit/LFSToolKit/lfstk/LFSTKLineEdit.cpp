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
	if(this->cursorColour.isValid==true)
		XFreeColors(this->wc->app->display,this->wc->app->cm,(long unsigned int*)&this->cursorColour.pixel,1,0);
}

LFSTK_lineEditClass::LFSTK_lineEditClass()
{
}

bool LFSTK_lineEditClass::contextCB(void *p,void* ud)
{
	LFSTK_windowClass	*lwc=static_cast<LFSTK_gadgetClass*>(p)->wc;
	LFSTK_lineEditClass	*le=static_cast<LFSTK_lineEditClass*>(ud);
	int					winnum;

	if(p!=NULL)
		{
			winnum=lwc->app->LFSTK_findWindow(lwc);
			lwc->app->windows->at(winnum).loopFlag=false;
			switch(GETUSERDATA(static_cast<LFSTK_gadgetClass*>(p)->userData))
				{
					case 1:
						le->wc->clipBuffer=le->LFSTK_getCStr();
						XSetSelectionOwner(le->wc->app->display,le->wc->LFSTK_getDnDAtom(XA_CLIPBOARD),le->wc->window,CurrentTime);
						break;
					case 2:
						le->wc->clipBuffer=le->LFSTK_getCStr();
						XSetSelectionOwner(le->wc->app->display,le->wc->LFSTK_getDnDAtom(XA_CLIPBOARD),le->wc->window,CurrentTime);
						XSync(le->wc->app->display,true);
						le->LFSTK_setBuffer("");
						break;
					case 3:
						le->getClip();
						le->LFSTK_clearWindow();
						break;
					case 4:
						le->LFSTK_setBuffer("");
						break;
				}
		}
	return(true);
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

	this->window=XCreateWindow(this->wc->app->display,this->parent,x,y,w,h,0,CopyFromParent,InputOutput,CopyFromParent,CWWinGravity|CWBitGravity,&wa);
	this->gc=XCreateGC(this->wc->app->display,this->window,0,NULL);
	this->LFSTK_setFontString(this->monoFontString);
	this->wc->globalLib->LFSTK_setCairoSurface(this->wc->app->display,this->window,this->wc->app->visual,&this->sfc,&this->cr,w,h);
	this->LFSTK_setCairoFontData();
	XSelectInput(this->wc->app->display,this->window,this->gadgetEventMask);

	this->ml->function=&LFSTK_lib::LFSTK_gadgetEvent;
	this->ml->gadget=this;
	this->ml->type=LINEEDITGADGET;
	this->wc->LFSTK_addMappedListener(this->window,ml);

	this->LFSTK_setBuffer(label);
	this->wc->LFSTK_initDnD();
	this->gadgetAcceptsDnD=true;
	this->labelGravity=LEFT;

	this->newGadgetBGColours[NORMALCOLOUR]=this->LFSTK_setColour("white");
	this->newGadgetFGColours[NORMALCOLOUR]=this->LFSTK_setColour("black");
	this->LFSTK_setCursorColourName(this->wc->globalLib->LFSTK_getGlobalString(-1,TYPECURSORCOLOUR));
	this->charWidth=LFSTK_getTextRealWidth("X");

	gadgetDetails={&this->newGadgetBGColours.at(NORMALCOLOUR),BEVELIN,NOINDICATOR,NORMALCOLOUR,0,true,{0,0,w,h},{0,0,0,0},false,false,false};	

	this->isFocused=false;
	this->inWindow=false;

	windowInitStruct		*win;
	int					sy=0;
	LFSTK_buttonClass	*btn;

	win=this->wc->app->LFSTK_getDefaultWInit();
	win->loadVars=true;
	win->x=100;
	win->y=100;
	win->w=200;
	win->h=200;
	win->wc=this->wc;
	win->windowType=this->wc->app->appAtomsHashed.at(LFSTK_UtilityClass::LFSTK_hashFromKey("_NET_WM_WINDOW_TYPE_MENU"));
	win->decorated=false;
	win->level=ABOVEALL;

	this->wc->app->LFSTK_addWindow(win,"LFSTK Context Menu");
	this->editWindow=this->wc->app->windows->back().window;
	btn=new LFSTK_buttonClass(this->editWindow,"Copy",0,sy,GADGETWIDTH,24);
	btn->userData=USERDATA(1);
	btn->LFSTK_setMouseCallBack(NULL,contextCB,USERDATA(this));
	sy+=GADGETHITE;
	btn=new LFSTK_buttonClass(this->editWindow,"Cut",0,sy,GADGETWIDTH,24);
	btn->userData=USERDATA(2);
	btn->LFSTK_setMouseCallBack(NULL,contextCB,USERDATA(this));
	sy+=GADGETHITE;
	btn=new LFSTK_buttonClass(this->editWindow,"Paste",0,sy,GADGETWIDTH,24);
	btn->userData=USERDATA(3);
	btn->LFSTK_setMouseCallBack(NULL,contextCB,USERDATA(this));
	sy+=GADGETHITE;
	btn=new LFSTK_buttonClass(this->editWindow,"Delete",0,sy,GADGETWIDTH,24);
	btn->userData=USERDATA(4);
	btn->LFSTK_setMouseCallBack(NULL,contextCB,USERDATA(this));
	sy+=GADGETHITE;
	this->editWindow->LFSTK_resizeWindow(GADGETWIDTH,sy,true);
	this->LFSTK_setContextWindow(this->editWindow);
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
	XSetInputFocus(this->wc->app->display,this->window,RevertToParent,CurrentTime);
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
* Mouse down callback.
* \param e XButtonEvent passed from mainloop->listener.
* \return Return true if event fully handeled or false to pass it on.
*/
bool LFSTK_lineEditClass::mouseDown(XButtonEvent *e)
{
	if(this->editable==false)
		return(true);
	this->startUpMDFlag=true;
	this->LFSTK_setFocus();
	if(this->isActive==false)
		this->LFSTK_clearWindow();

	return(true);
}

void LFSTK_lineEditClass::LFSTK_resizeWindow(int w,int h)
{
	this->gadgetGeom.w=w;
	this->gadgetGeom.h=h;
	this->gadgetDetails.gadgetGeom.w=this->gadgetGeom.w;
	this->gadgetDetails.gadgetGeom.h=this->gadgetGeom.h;
	XResizeWindow(this->wc->app->display,this->window,this->gadgetGeom.w,this->gadgetGeom.h);
	this->wc->globalLib->LFSTK_setCairoSurface(this->wc->app->display,this->window,this->wc->app->visual,&this->sfc,&this->cr,w,h);
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
			XUngrabKeyboard(this->wc->app->display,CurrentTime);
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
	if(this->startUpMDFlag==false)
		return(true);

	if(this->isFocused==false)
		{
			this->isFocused=true;
			this->LFSTK_clearWindow();
		}
	return(true);
}

/**
* Set the contents of the text buffer.
* \param const char *str String to set.
*/
void LFSTK_lineEditClass::LFSTK_setBuffer(std::string str)
{
	this->buffer=str;
	if(str.length()>0)
		this->cursorPos=str.length()-1;
	else
		this->cursorPos=0;
	this->setOffsetcurs(1000);
	this->setOffsetcurs(1);
	if(this->cursorPos<0)
		this->cursorPos=0;
	this->LFSTK_clearWindow();
}

/**
* Return the contents of the gadget.
* \return Return's a std::string.
* \note Don't free the returned string.
*/
std::string LFSTK_lineEditClass::LFSTK_getBuffer(void)
{
	return(this->buffer);
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
	cairo_text_extents_t		partextents;
	char					 	*data;
	char						undercurs[2]={0,0};
	char						*aftercursor;
	int						maxchars;
	cairo_text_extents_t		charextents;

	cairo_save(this->cr);
		cairo_reset_clip(this->cr);
		cairo_set_source_rgba(this->cr,this->newGadgetBGColours.at(NORMALCOLOUR).RGBAColour.r,this->newGadgetBGColours.at(NORMALCOLOUR).RGBAColour.g,this->newGadgetBGColours.at(NORMALCOLOUR).RGBAColour.b,this->newGadgetBGColours.at(NORMALCOLOUR).RGBAColour.a);
		cairo_set_operator(this->cr,CAIRO_OPERATOR_SOURCE);
		cairo_paint(this->cr);
	cairo_restore(this->cr);

	cairo_save(this->cr);
		cairo_select_font_face(this->cr,this->fontName.c_str(),this->slant,this->weight);
		cairo_set_font_size(this->cr,fontSize);
		cairo_set_source_rgba(this->cr,this->newGadgetFGColours.at(NORMALCOLOUR).RGBAColour.r,this->newGadgetFGColours.at(NORMALCOLOUR).RGBAColour.g,this->newGadgetFGColours.at(NORMALCOLOUR).RGBAColour.b,this->newGadgetFGColours.at(NORMALCOLOUR).RGBAColour.a);

		if(this->LFSTK_getTextWidth("X")<=0)
			maxchars=((this->gadgetGeom.w)/(int)((8)));
		else
			maxchars=(this->gadgetGeom.w/(int)(this->LFSTK_getTextRealWidth("X")+0.5))-1;

		if(maxchars>this->buffer.length())
			maxchars=this->buffer.length();
		startchar=this->offsetCurs;
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
			{
				if(this->cursorPos+1<this->buffer.length())
					asprintf(&aftercursor,"%s",this->buffer.substr(this->cursorPos+1,(maxchars-this->visCursorPos)).c_str());
				else
					asprintf(&aftercursor,"");
			}
//1stbit
		cairo_show_text(this->cr,data);
		cairo_text_extents(this->cr,data,&partextents);
		cairo_text_extents (this->cr,undercurs,&charextents);

		if(this->isFocused==true)
			{
				cairo_set_source_rgba(this->cr,this->cursorColour.RGBAColour.r,this->cursorColour.RGBAColour.g,this->cursorColour.RGBAColour.b,this->cursorColour.RGBAColour.a);
				cairo_rectangle(this->cr,partextents.x_advance+this->pad,yoffset-this->fontExtents.ascent,charextents.x_advance-0.5,this->fontExtents.ascent+this->fontExtents.descent);
				cairo_fill(this->cr);
				cairo_set_source_rgba(this->cr,this->newGadgetFGColours.at(NORMALCOLOUR).RGBAColour.r,this->newGadgetFGColours.at(NORMALCOLOUR).RGBAColour.g,this->newGadgetFGColours.at(NORMALCOLOUR).RGBAColour.b,this->newGadgetFGColours.at(NORMALCOLOUR).RGBAColour.a);
			}
//secondbit
		cairo_move_to(this->cr,partextents.x_advance+this->pad,yoffset);
		cairo_show_text(this->cr,undercurs);
//3rdbit
		cairo_set_source_rgba(this->cr,this->newGadgetFGColours.at(NORMALCOLOUR).RGBAColour.r,this->newGadgetFGColours.at(NORMALCOLOUR).RGBAColour.g,this->newGadgetFGColours.at(NORMALCOLOUR).RGBAColour.b,this->newGadgetFGColours.at(NORMALCOLOUR).RGBAColour.a);
		cairo_show_text(this->cr,aftercursor);

		freeAndNull(&data);
		freeAndNull(&aftercursor);
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

	selectionOwner=XGetSelectionOwner(this->wc->app->display,this->wc->LFSTK_getDnDAtom(XA_CLIPBOARD));

	if(selectionOwner==this->wc->window)
		{
			this->LFSTK_setFormatedText(this->wc->clipBuffer.c_str(),false);
			this->LFSTK_clearWindow();
			return;
		}

	if (selectionOwner!=None)
		{
			XConvertSelection(this->wc->app->display,this->wc->LFSTK_getDnDAtom(XA_CLIPBOARD),this->wc->LFSTK_getDnDAtom(XA_UTF8_STRING),this->wc->LFSTK_getDnDAtom(XA_CLIPBOARD),this->window,CurrentTime);
			if(XPending(this->wc->app->display)>-1)
				XNextEvent(this->wc->app->display,&event);

			XGetWindowProperty(this->wc->app->display,this->window,this->wc->LFSTK_getDnDAtom(XA_CLIPBOARD),0,0,False,AnyPropertyType,&type,&format,&len,&bytesLeft,&data);
			if(data!=NULL)
				{
					XFree(data);
					data=NULL;
				}

			if(bytesLeft>0)
				{
					result=XGetWindowProperty(this->wc->app->display,this->window,this->wc->LFSTK_getDnDAtom(XA_CLIPBOARD),0,bytesLeft,False,AnyPropertyType,&type,&format,&len,&dummy,&data);
					if (result==Success)
						{
							this->LFSTK_setFormatedText((const char*)data,false);
							XFree(data);
						}
				}
			XDeleteProperty(this->wc->app->display,this->window,this->wc->LFSTK_getDnDAtom(XA_CLIPBOARD));
		}
}

/**
* Get last pressed key.
* \return Return Keysym or XK_VoidSymbol if only modifier pressed.
* \note Keysyms here keysymdef.h
*/
KeySym LFSTK_lineEditClass::LFSTK_getKey(void)
{
	if(!(this->keysym_return>=XK_Shift_L) && (this->keysym_return<=XK_Hyper_R))
		return(this->keysym_return);
else
	return(XK_VoidSymbol);
}

/**
* Get last pressed modifer key.
* \return Return int.
* \note Keysyms here keysymdef.h
*/
unsigned int LFSTK_lineEditClass::LFSTK_getModifier(void)
{
	return(this->state);
}

/**
* Get last pressed keysym ( - XK_ ).
* \return const char*.
* \note DO NOT FREE.
* \note Keysyms here keysymdef.h
*/
std::string LFSTK_lineEditClass::LFSTK_getKeySym(void)
{
	return(std::string(XKeysymToString(this->keysym_return)));
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
	char	*command;

	if(this->isActive==false)
		{
			this->LFSTK_clearWindow();
			return(true);
		}
	XLookupString(e,(char*)&c,255,&keysym_return,NULL);
	if(this->isFocused==false)
		return(true);

	this->state=e->state;
	if(e->state==ControlMask)
		{
			if(keysym_return==XK_v)
				this->getClip();

			if(keysym_return==XK_c)
				{
					this->wc->clipBuffer=this->buffer;
					XSetSelectionOwner(this->wc->app->display,this->wc->LFSTK_getDnDAtom(XA_CLIPBOARD),this->wc->window,CurrentTime);
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
							this->setOffsetcurs(-1);
						}
					break;
				case XK_Delete:
					if(this->cursorPos<this->buffer.length())
						this->buffer.erase(this->cursorPos,1);
					this->setOffsetcurs(0);
					break;
				case XK_Left:
					this->setOffsetcurs(-1);
					break;
				case XK_Right:
					this->setOffsetcurs(1);
					break;
				case XK_End:
				case XK_Page_Down:
					this->setOffsetcurs(1000);
					this->setOffsetcurs(1);
					break;
				case XK_Home:
				case XK_Page_Up:
					this->offsetCurs=0;
					this->cursorPos=0;
					this->visCursorPos=0;

					break;
				case XK_Down:
				case XK_Up:
				case XK_Select ... XK_Num_Lock:
				case XK_F1 ... XK_R15:
					break;
				case XK_Return:
					if(this->callBacks.validCallbacks & KEYRELEASECB)
						return(this->callBacks.keyReleaseCallback(this,this->callBacks.keyUserData));
					break;

				default:
					if(c[0]==0)
						break;
					this->buffer.insert(this->cursorPos,1,c[0]);
					this->setOffsetcurs(1);
					break;
				}
		}

	this->LFSTK_clearWindow();

	if((this->callBacks.validCallbacks & KEYRELEASECB) && (this->callbackOnReturn==false))
		return(this->callBacks.keyReleaseCallback(this,this->callBacks.keyUserData));

	return(true);
}

/**
* Drop data.
* \param data Data dropped on gadget as string.
*/
void LFSTK_lineEditClass::LFSTK_dropData(propertyStruct* data)
{
	int	endl;
	if(strcasecmp(data->mimeType,"text/plain")==0)
		this->LFSTK_setFormatedText((const char*)data->data,true);

	if(strcasecmp(data->mimeType,"text/uri-list")==0)
		{
			std::string ret;
			ret=this->wc->app->globalLib->LFSTK_cleanString(std::string((const char*)data->data));
			this->LFSTK_setFormatedText(ret,true);
		}

	if(this->callBacks.validCallbacks & GADGETDROPCB)
		this->callBacks.droppedGadgetCallback(this,data,USERDATA(this->callBacks.droppedUserData));
}

/**
* Set formated txt.
* \param const char* Text to be formated.
* \param bool true=Replace contents, false=insert.
*/
void  LFSTK_lineEditClass::LFSTK_setFormatedText(std::string txt,bool replace)//TODO//
{
	std::string	formtxt="";
	int			len=0;

	for(int j=0;j<txt.length();j++)
		{
			switch(txt.at(j))
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
* Get the colour name for cursor.
* \return std::string Colour name.
*/
std::string LFSTK_lineEditClass::LFSTK_getCursorColourName(void)
{
	return(this->cursorColour.name);
}

/**
* Set the colour name for cursor.
* \param colour Colour name.
*/
void LFSTK_lineEditClass::LFSTK_setCursorColourName(const char* colour)
{
	XColor			tc;
	XColor			sc;

	this->cursorColour.name=colour;
	XAllocNamedColor(this->wc->app->display,this->wc->app->cm,colour,&sc,&tc);
	this->cursorColour.pixel=tc.pixel;

	this->cursorColour.RGBAColour.r=((this->cursorColour.pixel>>16) & 0xff)/256.0;
	this->cursorColour.RGBAColour.g=((this->cursorColour.pixel>>8) & 0xff)/256.0;
	this->cursorColour.RGBAColour.b=((this->cursorColour.pixel>>0) & 0xff)/256.0;
	this->cursorColour.RGBAColour.a=0.8;
}

void LFSTK_lineEditClass::setOffsetcurs(int step)
{
	unsigned	maxchars=0;
	unsigned	realmaxchars=0;

	if(this->LFSTK_getTextWidth("X")<=0)
		maxchars=((this->gadgetGeom.w)/(int)((8)));
	else
		maxchars=(this->gadgetGeom.w/(int)(this->LFSTK_getTextRealWidth("X")+0.5))-1;

	realmaxchars=maxchars;
	if(maxchars>this->buffer.length()-1)
		maxchars=this->buffer.length()-1;

	if((this->cursorPos>=this->buffer.length()) && (step==1))
		return;

	this->visCursorPos+=step;
	this->cursorPos+=step;
	if(this->visCursorPos<0)
		{
			this->visCursorPos=0;
			this->offsetCurs--;
		}

	if(this->visCursorPos>maxchars)
		{
			this->visCursorPos=maxchars;
			if(this->buffer.length()>realmaxchars)
				this->offsetCurs++;
		}

	if(this->offsetCurs<0)
		this->offsetCurs=0;

	if(this->cursorPos<0)
		this->cursorPos=0;

	if(this->cursorPos>this->buffer.length())
		{
			this->cursorPos=this->buffer.length()-1;
			this->offsetCurs=this->buffer.length()-maxchars-1;
			this->visCursorPos=maxchars;
		}
}

/**
* Set only run callback when return key pressed ( defualt=true ).
* \param bool
*/
void LFSTK_lineEditClass::LFSTK_setCallbackOnReturn(bool onreturn)
{
	this->callbackOnReturn=onreturn;
}

/**
* Return whether to only run callback on return pressed.
* \return bool
*/
bool LFSTK_lineEditClass::LFSTK_getCallbackOnReturn(void)
{
	return(this->callbackOnReturn);
}

/**
* Set whether editbox is editable.
* \param bool
*/
void LFSTK_lineEditClass::LFSTK_setEditable(bool canedit)
{
	this->editable=canedit;
}

