/*
 *
 * ©K. D. Hedger. Wed  5 Aug 12:36:15 BST 2015 kdhedger68713@gmail.com

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

#include "LFSTKGlobals.h"

LFSTK_lineEditClass::~LFSTK_lineEditClass()
{
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
LFSTK_lineEditClass::LFSTK_lineEditClass(LFSTK_windowClass* parentwc,const char* label,int x,int y,int w,int h,int gravity)
{
	XSetWindowAttributes	wa;
	mappedListener			*ml=new mappedListener;

	this->LFSTK_setCommon(parentwc,label,x,y,w,h,gravity);
	this->isFocused=false;
	this->inWindow=false;
	wa.win_gravity=gravity;
	wa.bit_gravity=gravity;

	this->window=XCreateWindow(this->display,this->parent,x+pad,y+pad,w-(pad*2),h-(pad*2),0,CopyFromParent,InputOutput,CopyFromParent,CWWinGravity|CWBitGravity,&wa);
	XSelectInput(this->display,this->window,ButtonReleaseMask | ButtonPressMask | ExposureMask | EnterWindowMask | LeaveWindowMask|FocusChangeMask|KeyReleaseMask);

	ml->function=&LFSTK_lib::LFSTK_gadgetEvent;
	ml->gadget=this;
	ml->type=LINEEDITGADGET;
	this->wc->LFSTK_addMappedListener(this->window,ml);

	if(strlen(label)>0)
		this->cursorPos=strlen(label);
	else
		this->cursorPos=0;

	this->buffer=label;

	this->wc->LFSTK_initDnD();
	this->gadgetAcceptsDnD=true;
}

/**
* Clear the gadget window to the appropriate state.
*/
void LFSTK_lineEditClass::LFSTK_clearWindow()
{
	XSetFillStyle(this->display,this->gc,FillSolid);
	XSetClipMask(this->display,this->gc,None);

	if(this->isActive==true)
		XSetForeground(this->display,this->gc,whiteColour);
	else
		XSetForeground(this->display,this->gc,this->colourNames[INACTIVECOLOUR].pixel);
	XFillRectangle(this->display,this->window,this->gc,0,0,this->w,this->h);

	XSetForeground(this->display,this->gc,this->blackColour);
	XDrawLine(this->display,this->window,this->gc,0,this->h-1,0,0);
	XDrawLine(this->display,this->window,this->gc,0,0,this->w-1,0);
	XSetForeground(this->display,this->gc,this->whiteColour);
	XDrawLine(this->display,this->window,this->gc,0,this->h-1,this->w-1,this->h-1);
	XDrawLine(this->display,this->window,this->gc,this->w-1,this->h-1,this->w-1,0);

	this->drawLabel();
}

/**
* Set this gadget to have the focus..
*/
void LFSTK_lineEditClass::LFSTK_setFocus(void)
{
	XGrabKeyboard(this->display,this->window,true,GrabModeAsync,GrabModeAsync,CurrentTime);
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
	if(this->isActive==false)
		{
			this->LFSTK_clearWindow();
			return(true);
		}
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
	this->w=w-(pad*2);
	this->h=h-(pad*2);
	XResizeWindow(this->display,this->window,this->w,this->h);
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
			this->inWindow=false;
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
			this->inWindow=true;
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
	this->buffer=str;
	this->cursorPos=strlen(str);
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

void LFSTK_lineEditClass::drawLabel(void)
{
	int startchar=0;
	int	len=this->buffer.length();
	int curpos;

	if(this->wc->globalLib->LFSTK_getTextwidth(this->display,(XftFont*)(this->font->data),this->buffer.c_str())<this->w-4)
		{
			startchar=0;
			len=this->buffer.length();
			curpos=this->wc->globalLib->LFSTK_getTextwidth(this->display,(XftFont*)(this->font->data),this->buffer.substr(startchar,this->cursorPos).c_str());
		}
	else if(this->wc->globalLib->LFSTK_getTextwidth(this->display,(XftFont*)(this->font->data),this->buffer.substr(0,this->cursorPos).c_str())>this->w-4)
		{
			startchar=0;
			len=this->cursorPos;
			while(this->wc->globalLib->LFSTK_getTextwidth(this->display,(XftFont*)(this->font->data),this->buffer.substr(startchar,len).c_str())>this->w-4)
				{
					startchar++;
					len--;
				}
			curpos=this->wc->globalLib->LFSTK_getTextwidth(this->display,(XftFont*)(this->font->data),this->buffer.substr(startchar,len).c_str());
		}
	else if(this->wc->globalLib->LFSTK_getTextwidth(this->display,(XftFont*)(this->font->data),this->buffer.substr(0,len).c_str())>this->w-4)
		{
			startchar=0;
			len=this->buffer.length();
			while(this->wc->globalLib->LFSTK_getTextwidth(this->display,(XftFont*)(this->font->data),this->buffer.substr(startchar,len).c_str())>this->w-4)
				{
					len--;
				}
			curpos=this->wc->globalLib->LFSTK_getTextwidth(this->display,(XftFont*)(this->font->data),this->buffer.substr(startchar,this->cursorPos).c_str());
		}

//	this->drawString((XftFont*)(this->font->data),2,(this->h/2)+((this->wc->font->ascent-2)/2),NORMALCOLOUR,this->buffer.substr(startchar,len).c_str());
	XftDrawChange(this->wc->draw,this->window);
	XftDrawStringUtf8(this->wc->draw,&(this->blackXftColour),(XftFont*)(this->font->data),2,(this->h/2)+((this->wc->font->ascent-2)/2),(XftChar8 *)this->buffer.substr(startchar,len).c_str(),len);

	if(this->isFocused==true)
		{
			XSetForeground(this->display,this->gc,this->blackColour);
			XDrawLine(this->display,this->window,this->gc,2+curpos,3,2+curpos,this->h-3);
		}
}

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
							this->buffer.insert(this->cursorPos,(char*)data);
							this->cursorPos+=strlen((char*)data);
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
*/
bool LFSTK_lineEditClass::keyRelease(XKeyEvent *e)
{
	char	c[255];
	KeySym	keysym_return;

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
		}
	else
		{
			switch(keysym_return)
				{
				case XK_BackSpace:
					if(this->cursorPos>0)
						{
							this->buffer.erase(this->cursorPos-1,1);
							this->cursorPos--;
						}
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
					break;
				case XK_Down:
				case XK_Up:
				case XK_Select ... XK_Num_Lock:
				case XK_F1 ... XK_R15:
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
		this->LFSTK_setBuffer((const char*)data->data);

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
			this->LFSTK_setBuffer(ret);
			free(ret);
			free(d);
		}
}

