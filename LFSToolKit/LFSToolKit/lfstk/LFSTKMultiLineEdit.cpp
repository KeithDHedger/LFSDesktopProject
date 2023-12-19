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
#include <stdio.h>
#include <string.h>

#include <math.h>
#include <cairo.h>

#include <vector>

#include "lfstk/LFSTKGlobals.h"

LFSTK_multiLineEditClass::~LFSTK_multiLineEditClass()
{
	if(!this->lines.empty())
		{
			for (int i=0; i<lines.size(); i++)
				{
					freeAndNull(&lines.at(i)->line);
					delete lines.at(i);
				}
			this->lines.clear();
		}
}

LFSTK_multiLineEditClass::LFSTK_multiLineEditClass()
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
LFSTK_multiLineEditClass::LFSTK_multiLineEditClass(LFSTK_windowClass* parentwc,const char* label,int x,int y,unsigned w,unsigned h,int gravity)
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
	XSelectInput(this->wc->app->display,this->window,this->gadgetEventMask|SelectionClear|SelectionRequest);

	this->ml->function=&LFSTK_lib::LFSTK_gadgetEvent;
	this->ml->gadget=this;
	this->ml->type=MULTILINEGADGET;
	this->wc->LFSTK_addMappedListener(this->window,ml);

	if(strlen(label)>0)
		this->cursorPos=strlen(label);
	else
		this->cursorPos=0;
	this->buffer=label;

	this->wc->LFSTK_initDnD();
	this->gadgetAcceptsDnD=true;
	this->labelGravity=LEFT;


	this->newGadgetBGColours[NORMALCOLOUR]=this->LFSTK_setColour("white");
	this->newGadgetFGColours[NORMALCOLOUR]=this->LFSTK_setColour("black");
	this->LFSTK_setCursorColourName(this->wc->globalLib->LFSTK_getGlobalString(-1,TYPECURSORCOLOUR));

	gadgetDetails={&this->newGadgetBGColours.at(NORMALCOLOUR),BEVELIN,NOINDICATOR,NORMALCOLOUR,0,true,{0,0,w,h},{0,0,0,0},false,false,false};

	this->topLine=0;
	this->setDisplayLines();
}

/**
* Clear the gadget window to the appropriate state.
*/
void LFSTK_multiLineEditClass::LFSTK_clearWindow()
{
	this->gadgetDetails.bevel=BEVELIN;
	this->drawText();
	this->drawBevel(&this->gadgetDetails.gadgetGeom,this->gadgetDetails.bevel);
	return;
}

/**
* Set this gadget to have the focus.
*/
void LFSTK_multiLineEditClass::LFSTK_setFocus(void)
{
	this->isFocused=true;
	this->setDisplayLines();
}

/**
* Update the text.
*/
void LFSTK_multiLineEditClass::LFSTK_upDateText(void)
{
	this->setDisplayLines();
}

void LFSTK_multiLineEditClass::LFSTK_resizeWindow(int w,int h)
{
	this->gadgetGeom.w=w-(pad*2);
	this->gadgetGeom.h=h-(pad*2);
	XResizeWindow(this->wc->app->display,this->window,this->gadgetGeom.w,this->gadgetGeom.h);
	this->LFSTK_clearWindow();
}

/**
* Lost focus callback.
* \param e XEvent passed from mainloop->listener.
* \return Return true if event fully handeled or false to pass it on.
*/
bool LFSTK_multiLineEditClass::lostFocus(XEvent *e)
{
	if(this->isFocused==true)
		{
			XUngrabKeyboard(this->wc->app->display,CurrentTime);
			this->isFocused=false;
		}
	this->setDisplayLines();
	return(true);
}

/**
* Got focus callback.
* \param e XEvent passed from mainloop->listener.
* \return Return true if event fully handeled or false to pass it on.
*/
bool LFSTK_multiLineEditClass::gotFocus(XEvent *e)
{
	if(this->startUpMDFlag==false)
		return(true);

	if(this->isFocused==false)
		this->isFocused=true;

	this->setDisplayLines();
	return(true);
}

/**
* Set the contents of the text buffer.
* \param const char *str String to set.
*/
void LFSTK_multiLineEditClass::LFSTK_setBuffer(const char *str)
{
	const char	*bufferstr=str;

	if(bufferstr==NULL)
		bufferstr="";
	this->buffer=bufferstr;
	this->cursorPos=strlen(bufferstr);
	this->LFSTK_upDateText();
	this->LFSTK_clearWindow();
}

/**
* Return the contents of the gadget.
* \return Return's a std::string.
*/
std::string LFSTK_multiLineEditClass::LFSTK_getBuffer(void)
{
	return(this->buffer);
}

/**
* Return the contents of the gadget.
* \return Return's a c str.
* \note Don't free the returned string.
*/
const char* LFSTK_multiLineEditClass::LFSTK_getCStr(void)
{
	return(this->buffer.c_str());
}

void LFSTK_multiLineEditClass::drawText(void)
{
	int						cursorwidth;
	int						startchar=0;
	int						len=this->cursorPos;
	const char				*curs="";
	double					yoffset=0;
	cairo_text_extents_t		partextents;
	cairo_text_extents_t		charextents;
	int						offline=0;
	long						maxlines=0;
	int						calcmax=0;
	int						linewithcursor=0;
	int						linenum=0;

	cairo_save(this->cr);
		cairo_reset_clip(this->cr);
		cairo_set_source_rgba(this->cr,this->newGadgetBGColours.at(NORMALCOLOUR).RGBAColour.r,this->newGadgetBGColours.at(NORMALCOLOUR).RGBAColour.g,this->newGadgetBGColours.at(NORMALCOLOUR).RGBAColour.b,this->newGadgetBGColours.at(NORMALCOLOUR).RGBAColour.a);
		cairo_paint(this->cr);
	cairo_restore(this->cr);
	
	cairo_save(this->cr);
		cairo_select_font_face(this->cr,fontName,slant,weight);
		cairo_set_font_size(this->cr,fontSize);
		cairo_set_source_rgba(this->cr,this->newGadgetFGColours.at(NORMALCOLOUR).RGBAColour.r,this->newGadgetFGColours.at(NORMALCOLOUR).RGBAColour.g,this->newGadgetFGColours.at(NORMALCOLOUR).RGBAColour.b,this->newGadgetFGColours.at(NORMALCOLOUR).RGBAColour.a);

		if(this->isFocused==true)
			{
				for(int j=0;j<this->lines.size();j++)
					{
						if(calcmax+this->textExtents.height<this->gadgetGeom.h-this->textExtents.height)
							{
								calcmax+=this->textExtents.height;
								maxlines++;
							}
							
						if(lines.at(j)->cursorPos!=-1)
							linewithcursor=j;
					}
				if(linewithcursor>maxlines)
					offline=linewithcursor-maxlines;

				if(offline<this->topLine)
					offline=this->topLine;

				if(linewithcursor<offline)
					offline=linewithcursor;
				this->topLine=offline;
			}
		else
			offline=this->topLine;

		for (int j=0+offline;j<this->lines.size();j++)
			{
				yoffset=this->lines.at(linenum)->ypos;
				if(yoffset>this->gadgetGeom.h)
					continue;
				cairo_move_to(this->cr,this->pad,yoffset);
				linenum++;
//do cursor
				if(lines.at(j)->cursorPos!=-1)
					{
						char 		*data;
						std::string	undercursstr="  ";
						char			*aftercursor;
						
						asprintf(&data,"%s",lines.at(j)->line);
						undercursstr[0]=data[lines.at(j)->cursorPos];
						undercursstr[1]=0;
						if(undercursstr.at(0)==0)
							undercursstr[0]=' ';

						data[lines.at(j)->cursorPos]=0;
						aftercursor=&data[lines.at(j)->cursorPos+1];
//1stbit
						cairo_show_text(this->cr,data);
						cairo_text_extents (this->cr,data,&partextents);
						cairo_text_extents (this->cr,undercursstr.c_str(),&charextents);
						cairo_set_source_rgba(this->cr,this->cursorColour.RGBAColour.r,this->cursorColour.RGBAColour.g,this->cursorColour.RGBAColour.b,this->cursorColour.RGBAColour.a);

						cairo_rectangle(this->cr,partextents.x_advance+0.5+this->pad,yoffset-this->fontExtents.ascent,charextents.x_advance-0.5,this->fontExtents.ascent+this->fontExtents.descent);
						cairo_fill(this->cr);
//secondbit
						cairo_move_to(this->cr,partextents.x_advance+this->pad,yoffset);
						cairo_set_source_rgba(this->cr,this->newGadgetFGColours.at(NORMALCOLOUR).RGBAColour.r,this->newGadgetFGColours.at(NORMALCOLOUR).RGBAColour.g,this->newGadgetFGColours.at(NORMALCOLOUR).RGBAColour.b,this->newGadgetFGColours.at(NORMALCOLOUR).RGBAColour.a);
						cairo_show_text(this->cr,undercursstr.c_str());
//3rdbit
						cairo_set_source_rgba(this->cr,this->newGadgetFGColours.at(NORMALCOLOUR).RGBAColour.r,this->newGadgetFGColours.at(NORMALCOLOUR).RGBAColour.g,this->newGadgetFGColours.at(NORMALCOLOUR).RGBAColour.b,this->newGadgetFGColours.at(NORMALCOLOUR).RGBAColour.a);
						if(lines.at(j)->cursorPos<strlen(lines.at(j)->line))
							cairo_show_text(this->cr,aftercursor);

						freeAndNull(&data);
					}
				else
					{
						cairo_set_source_rgba(this->cr,this->newGadgetFGColours.at(NORMALCOLOUR).RGBAColour.r,this->newGadgetFGColours.at(NORMALCOLOUR).RGBAColour.g,this->newGadgetFGColours.at(NORMALCOLOUR).RGBAColour.b,this->newGadgetFGColours.at(NORMALCOLOUR).RGBAColour.a);
						cairo_show_text(this->cr,lines.at(j)->line);
					}

				if(this->doHighlight==true)
					this->highLightText();
			}
	cairo_restore(this->cr);
}

/**
* Set contents to the clipboard.
*/
void LFSTK_multiLineEditClass::LFSTK_addHighLights(int x,int y,int len,cairoColor col)
{
	this->highLights.push_back({x,y,len,col});
	this->doHighlight=true;
}

/**
* Set contents to the clipboard.
*/
void LFSTK_multiLineEditClass::highLightText(void)
{
	double				yoffset;
	int					xoffset;
	int					len;
	cairo_text_extents_t	charextents;

	cairo_text_extents(this->cr,"X",&charextents);

	for(int j=0;j<this->highLights.size();j++)
		{
			xoffset=this->highLights.at(j).sx*(charextents.x_advance)+this->pad;
			yoffset=this->lines.at(this->highLights.at(j).sy)->ypos-this->lines.at(this->highLights.at(j).sy)->height+this->pad;
			len=this->highLights.at(j).len*(charextents.x_advance+0.5);
			int hite=this->lines.at(this->highLights.at(j).sy)->height;

			cairo_save(this->cr);
				cairo_reset_clip(this->cr);
				cairo_set_source_rgba(this->cr,this->highLights.at(j).colour.r,this->highLights.at(j).colour.g,this->highLights.at(j).colour.b,this->highLights.at(j).colour.a);
				cairo_rectangle(this->cr,xoffset,yoffset,len,hite);
				cairo_fill(this->cr);
			cairo_restore(this->cr);
		}
}

/**
* Set contents to the clipboard.
*/
void LFSTK_multiLineEditClass::getClip(void)
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

			while (run==true)
				{
					XNextEvent(this->wc->app->display,&event);
					switch(event.type)
						{
							case SelectionNotify:
								if(event.xselection.requestor==this->window)
									run=false;
								break;
						}
				}

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
* Key release callback.
* \param e XEvent passed from mainloop->listener.
* \return Return true if event fully handeled or false to pass it on.
* \note button down callback is used for "Return" key callback.
* \note button up callback is used for key callback.
* \note Keysyms here keysymdef.h
*/
bool LFSTK_multiLineEditClass::keyRelease(XKeyEvent *e)
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
					this->buffer.insert(this->cursorPos,4,' ');
					this->cursorPos+=4;

					break;
				case XK_BackSpace:
					if(this->cursorPos>0)
						{
							this->buffer.erase(this->cursorPos-1,1);
							this->cursorPos--;
						}
					break;
				case XK_Delete:
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
					while((this->cursorPos<this->buffer.length()) && (this->buffer[this->cursorPos]!='\n'))
						this->cursorPos++;
					break;
				case XK_Page_Down:
					this->cursorPos=this->buffer.length();
					break;
				case XK_Home:
					while((this->cursorPos>0) && (this->buffer[this->cursorPos]!='\n'))
						this->cursorPos--;
					if(this->cursorPos!=0)
						this->cursorPos++;
					break;
				case XK_Page_Up:
					this->cursorPos=0;
					break;
				case XK_Down:
					if(this->cursorPos>this->buffer.length())
						break;
					do
						this->cursorPos++;
					while((this->cursorPos<this->buffer.length()) && (this->buffer[this->cursorPos]!='\n'));
					if(this->cursorPos>this->buffer.length())
						this->cursorPos=this->buffer.length();
					break;
				case XK_Up:
					if(this->cursorPos==0)
						break;
					do
						this->cursorPos--;
					while((this->cursorPos>0) && (this->buffer[this->cursorPos]!='\n'));
					break;
				case XK_Select ... XK_Num_Lock:
				case XK_F1 ... XK_R15:
					break;
				case XK_Return:
					this->buffer.insert(this->cursorPos,1,'\n');
					this->cursorPos++;
					if(this->callBacks.validCallbacks & KEYRELEASECB)
						{
							this->setDisplayLines();
							this->LFSTK_clearWindow();
							return(this->callBacks.keyReleaseCallback(this,this->callBacks.keyUserData));
						}
					break;

				default:
					if(c[0]==0)
						break;
					this->buffer.insert(this->cursorPos,1,c[0]);
					this->cursorPos++;
					break;
				}
		}

	this->setDisplayLines();
	this->LFSTK_clearWindow();

	if((this->callBacks.validCallbacks & KEYRELEASECB) && (this->callbackOnReturn==false))
		return(this->callBacks.keyReleaseCallback(this,this->callBacks.keyUserData));

	return(true);
}

/**
* Drop data.
* \param data Data drooped on gadget as string.
*/
void LFSTK_multiLineEditClass::LFSTK_dropData(propertyStruct* data)
{
	int	endl;

	if(strcasecmp(data->mimeType,"text/plain")==0)
		this->LFSTK_setFormatedText((const char*)data->data,true);

	if(strcasecmp(data->mimeType,"text/uri-list")==0)
		{
			char	*ret;
			ret=this->wc->app->globalLib->LFSTK_cleanString((const char*)data->data);
			this->LFSTK_setFormatedText((const char*)ret,true);
			freeAndNull(&ret);
		}

	if(this->callBacks.validCallbacks & GADGETDROPCB)
		this->callBacks.droppedGadgetCallback(this,data,USERDATA(this->callBacks.droppedUserData));
}

/**
* Set up lines for display.
*/
void  LFSTK_multiLineEditClass::setDisplayLines(void)
{
	cairo_text_extents_t		extents;
	char						data[4096]={0,};
	int						len=0;
	geometryStruct			geom;
	lineStruct				*newline;
	int						cursorpos=-1;
	bool						donecurs=false;
	double					sy;

	this->LFSTK_getGeom(&geom);

	for (int i=0; i<lines.size(); i++)
		{
			freeAndNull(&lines.at(i)->line);
			delete lines.at(i);
		}
	this->lines.clear();

	cairo_select_font_face(this->cr,fontName,slant,weight);
	cairo_set_font_size(this->cr,fontSize);
	cairo_text_extents(this->cr,"X~!_^",&extents);
	sy=0;

	for(int j=0;j<this->buffer.length();j++)
		{
			if((this->cursorPos==j) && (this->isFocused==true))
				{
					cursorpos=len;
					donecurs=true;
				}

			data[len]=this->buffer.c_str()[j];
			data[len+1]=0;
			if(((extents.x_advance)>geom.w) || (data[len]=='\n'))
				{
					if(data[len]!='\n')
						j--;
					data[len++]=0;
					newline=new lineStruct;
					asprintf(&newline->line,"%s",data);
					newline->cursorPos=cursorpos;
					newline->xpos=this->pad;
					newline->width=0;
					newline->height=extents.height;
					sy+=extents.height+0.5;
					newline->ypos=sy;
					lines.push_back(newline);
					data[0]=0;
					len=0;
					cursorpos=-1;
				}
			else
				{
					len++;
					data[len]=0;
				}
		}
//last line
	if(strlen(data)>0)
		{
			newline=new lineStruct;
			asprintf(&newline->line,"%s",data);
			if((donecurs==false) && (this->isFocused==true))
				{
					cursorpos=strlen(newline->line);
					donecurs=true;
				}
			newline->cursorPos=cursorpos;
			newline->xpos=this->pad;
			newline->width=0;
			newline->height=extents.height;
			sy+=extents.height+0.5;
			newline->ypos=sy;
			lines.push_back(newline);
		}
//special case last line is newline
	if((donecurs==false) && (this->isFocused==true))
		{
			newline=new lineStruct;
			asprintf(&newline->line," ");
			newline->cursorPos=0;
			lines.push_back(newline);
		}
	this->LFSTK_clearWindow();
}

/**
* Set formated txt.
* \param const char* Text to be formated.
* \param bool true=Replace contents, false=insert.
*/
void  LFSTK_multiLineEditClass::LFSTK_setFormatedText(const char *txt,bool replace)
{
	std::string	formtxt="";
	int			len=0;

	for(int j=0;j<strlen(txt);j++)
		{
			switch(txt[j])
				{
					case '\t':
						formtxt.append("    ");
						len+=4;
						break;
						break;
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

	this->setDisplayLines();
}
