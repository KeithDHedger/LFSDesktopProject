/*
 *
 * ©K. D. Hedger. Sun  4 Nov 19:37:12 GMT 2018 keithdhedger@gmail.com

 * This file (LFSTKMenuItem.cpp) is part of LFSToolKit.

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

LFSTK_menuItemClass::~LFSTK_menuItemClass()
{
}

/**
* Main Label constructor.
*
* \param parentwc Main parent window class.
* \param label Displayed name.
* \param x X pos.
* \param y Y pos.
* \param w Width.
* \param h Height.
* \param menu menuStruct array.
* \param gravity Label gravity
* \note optional, label gravity defaults to CENTRE.
* \note gadget gravity defaults to NorthWestGravity.
*/
LFSTK_menuItemClass::LFSTK_menuItemClass(LFSTK_toolWindowClass* parentwc,LFSTK_menuClass *mainmenu,const char* label,int x,int y,unsigned w,unsigned h,menuStruct *menu,std::vector<LFSTK_toolWindowClass*> *windows)
{
	XSetWindowAttributes	wa;

	this->LFSTK_setCommon(parentwc,label,x,y,w,h,BUTTONGRAV);

	wa.win_gravity=BUTTONGRAV;
	wa.save_under=true;
	this->window=XCreateWindow(this->display,this->parent,x,y,w,h,0,CopyFromParent,InputOutput,CopyFromParent,CWWinGravity,&wa);
	this->gc=XCreateGC(this->display,this->window,0,NULL);
	this->wc->globalLib->LFSTK_setCairoSurface(this->display,this->window,this->visual,&this->sfc,&this->cr,w,h);
	this->LFSTK_setCairoFontData();
	XSelectInput(this->display,this->window,ButtonReleaseMask | ButtonPressMask | ExposureMask | EnterWindowMask | LeaveWindowMask);

	this->style=BEVELNONE;

	this->ml->function=&LFSTK_lib::LFSTK_gadgetEvent;
	this->ml->gadget=this;
	this->ml->type=MENUITEMGADGET;
	this->wc->LFSTK_addMappedListener(this->window,ml);

	if(this->wc->globalLib->LFSTK_getUseTheme()==true)
		this->LFSTK_setTile(this->wc->globalLib->LFSTK_getGlobalString(-1,TYPEMENUITEMTILE),-1);
	else
		this->useTile=false;

	this->LFSTK_setFontColourName(NORMALCOLOUR,this->wc->globalLib->LFSTK_getGlobalString(NORMALCOLOUR,TYPEMENUITEMFONTCOLOUR),true);
	this->LFSTK_setFontString(this->wc->globalLib->LFSTK_getGlobalString(NORMALCOLOUR,TYPEMENUITEMFONT),true);
	gadgetDetails= {&this->colourNames[NORMALCOLOUR],BEVELNONE,DISCLOSURE,NORMALCOLOUR,0,true,{0,0,w,h},{(int)(w-TRIANGLESIZE-(this->pad*2)),(int)((h/2)-(TRIANGLESIZE/2)+(this->pad/2)),TRIANGLESIZE,TRIANGLESIZE},menu->hasSubMenu};
	this->LFSTK_setLabelGravity(MENU);
	this->menuData=menu;
	this->subwindows=windows;
	this->menu=mainmenu;
}

/**
* Mouse exit callback.
* \param e XButtonEvent passed from mainloop->listener.
* \return Return true if event fully handeled or false to pass it on.
*/
bool LFSTK_menuItemClass::mouseExit(XButtonEvent *e)
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
bool LFSTK_menuItemClass::mouseEnter(XButtonEvent *e)
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

	if(this->menuData->hasSubMenu==true)
		{
			LFSTK_menuItemClass	*label;
			int					sy=0;
			XEvent	event;
			XEvent	lookevent;
			bool	gotlooked=false;
	
			if(this->subwc==NULL)
				{
					this->subwc=new LFSTK_toolWindowClass(this->display,this->wc,"_NET_WM_WINDOW_TYPE_MENU",this->gadgetGeom.x,this->gadgetGeom.y,GADGETWIDTH,GADGETHITE*this->menuData->subMenuCnt,"menu window");
					for(int j=0; j<this->menuData->subMenuCnt; j++)
						{
							label=new LFSTK_menuItemClass(this->subwc,this->menu,this->menuData->subMenus[j]->label,0,sy,GADGETWIDTH,GADGETHITE,this->menuData->subMenus[j],this->subwindows);
							label->LFSTK_setCallBack(this->callback.pressCallback,this->callback.releaseCallback,this->menuData->subMenus[j]->userData);
							if(this->menuData->subMenus[j]->imageType==FILETHUMB)
								label->LFSTK_setImageFromPath(this->menuData->subMenus[j]->data.imagePath,MENU,true);
							if(this->menuData->subMenus[j]->imageType==CAIROTHUMB)
								label->LFSTK_setImageFromSurface(this->menuData->subMenus[j]->data.surface,MENU,true);
							sy+=GADGETHITE;
						}
					this->menu->subwindows->push_back(this->subwc);
				}
			this->subwc->mainLoop=true;
			this->subwc->LFSTK_showWindow(true);
 
			while(this->subwc->mainLoop==true)
				{
					XNextEvent(this->display,&event);
					mappedListener *ml=this->subwc->LFSTK_getMappedListener(event.xany.window);
					switch(event.type)
						{
							case FocusOut:
								for(int j=0;j<this->subwindows->size();j++)
									{
										this->subwindows->at(j)->LFSTK_hideWindow();
										this->subwindows->at(j)->mainLoop=false;
									}
								this->menu->mainMenuWindow->LFSTK_hideWindow();
								this->menu->mainLoop=false;
								continue;
								break;
						}

					if(ml!=NULL)
						ml->function(ml->gadget,&event,ml->type);

					if(this->subwc->LFSTK_handleWindowEvents(&event)<0)
						this->subwc->mainLoop=false;

					switch(event.type)
						{
							case ButtonRelease:
								for(int j=0;j<this->subwindows->size();j++)
									{
										this->subwindows->at(j)->LFSTK_hideWindow();
										this->subwindows->at(j)->mainLoop=false;
									}
								this->menu->mainMenuWindow->LFSTK_hideWindow();
								this->menu->mainLoop=false;
								continue;
								break;
							case Expose:
								if (event.xexpose.count==0)
									{
										Window dw;
										int x;
										int y;
										unsigned int w,h,dump;
										XTranslateCoordinates(this->subwc->display,this->wc->window,this->subwc->rootWindow, 0, 0, &x, &y, &dw );
										this->subwc->LFSTK_moveWindow(x+GADGETWIDTH,y+this->gadgetGeom.y,true);
									}
								break;
						}


					gotlooked=XCheckTypedWindowEvent(this->display,this->subwc->window,EnterNotify,&lookevent);
					if(gotlooked==false)
						{
							switch(event.type)
								{
									case LeaveNotify:
										if(event.xcrossing.window==this->window)
											{
												this->mouseExit(&event.xbutton);
												this->subwc->LFSTK_hideWindow();
												this->subwc->mainLoop=false;
											}
										break;
								}
						}

					gotlooked=XCheckTypedWindowEvent(this->display,this->subwc->window,LeaveNotify,&lookevent);
					if(gotlooked==true)
						{
							gotlooked=XCheckTypedWindowEvent(this->display,this->wc->window,EnterNotify,&lookevent);
							if(gotlooked==true)
								{
									gotlooked=XCheckTypedWindowEvent(this->display,this->window,EnterNotify,&lookevent);
									if(gotlooked==false)
										{
											this->subwc->LFSTK_hideWindow();
											this->subwc->mainLoop=false;
											this->mouseExit(&event.xbutton);
										}
								}
						}
				}
		}
	return(true);
}