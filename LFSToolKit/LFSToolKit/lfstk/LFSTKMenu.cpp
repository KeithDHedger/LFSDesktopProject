/*
 *
 * ©K. D. Hedger. Sun  4 Nov 19:46:07 GMT 2018 keithdhedger@gmail.com

 * This file (LFSTKMenu.cpp) is part of LFSToolKit.

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

LFSTK_menuClass::~LFSTK_menuClass()
{
	XSync(this->display,true);
	for(int j=0;j<this->subwindows->size();j++)
		delete this->subwindows->at(j);

	delete this->subwindows;
	LFSTK_freeMenus(this->mainMenu,this->mainMenuCnt);
}

LFSTK_menuClass::LFSTK_menuClass(LFSTK_windowClass *wc,int x,int y,unsigned w,unsigned h)
{
	subwindows=new std::vector<LFSTK_toolWindowClass*>;
	subwindows->clear();
	this->parentwc=wc;
	this->display=wc->display;
	this->x=x;
	this->y=y;
	this->w=w;
	this->h=h;
}

#if 0
/**
* Main menuClass constructor.
*
* \param parentwc Main parent window class.
* \param label Displayed name.
* \param x X pos.
* \param y Y pos.
* \param w Width.
* \param h Height.
* \param gravity Label gravity
* \note optional, label gravity defaults to CENTRE.
* \note gadget gravity defaults to NorthWestGravity.
*/
LFSTK_menuClass::LFSTK_menuClass(LFSTK_windowClass* parentwc,const char* label,int x,int y,unsigned w,unsigned h,int gravity,int bgrav)
{
	XSetWindowAttributes	wa;

	this->LFSTK_setCommon(parentwc,label,x,y,1,1,gravity);

	wa.win_gravity=bgrav;
	wa.save_under=true;
	this->window=XCreateWindow(this->display,this->parent,x,y,1,1,0,CopyFromParent,InputOutput,CopyFromParent,CWWinGravity,&wa);
	this->gc=XCreateGC(this->display,this->window,0,NULL);
	this->wc->globalLib->LFSTK_setCairoSurface(this->display,this->window,this->visual,&this->sfc,&this->cr,1,1);
	this->LFSTK_setCairoFontData();
	XSelectInput(this->display,this->window,ButtonReleaseMask | ButtonPressMask | ExposureMask | EnterWindowMask | LeaveWindowMask);

	this->ml->function=&LFSTK_lib::LFSTK_gadgetEvent;
	this->ml->gadget=this;
	this->ml->type=MENUGADGET;
	this->wc->LFSTK_addMappedListener(this->window,ml);

	if(this->wc->globalLib->LFSTK_getUseTheme()==true)
		this->LFSTK_setTile(this->wc->globalLib->LFSTK_getGlobalString(-1,TYPEBUTTONTILE),-1);
	else
		this->useTile=false;

	this->LFSTK_setFontColourName(0,this->wc->globalLib->LFSTK_getGlobalString(0,TYPEMENUITEMFONTCOLOUR),true);
	gadgetDetails={&this->wc->windowColourNames[NORMALCOLOUR],BEVELIN,NOINDICATOR,NORMALCOLOUR,0,true,{0,0,w,h},{0,0,0,0},false};

	this->subwindows.clear();
	this->subwindows.push_back(this->wc);
}
#endif

/**
* Set callback for widget.
* \param downcb Mouse down callback.
* \param releasecb Mouse up callback.
* \note Format for callback is "bool functioname(void *p,void* ud)"
* \note First param passed to callback is pointer to object.
* \note Second param passed to callback is user data.
*/
void LFSTK_menuClass::LFSTK_setCallBack(bool (*downcb)(void *,void*),bool (*releasecb)(void *,void*),void* ud)
{
	this->callback.pressCallback=downcb;
	this->callback.releaseCallback=releasecb;
	this->callback.userData=ud;
	this->callback.ignoreCallback=false;
}

/**
* Show main menu window.
*/
void LFSTK_menuClass::LFSTK_showMenu(void)
{
	XEvent	event;
	Window dw;
	int x;
	int y;
	unsigned int w,h,dump;

	this->mainMenuWindow->LFSTK_showWindow(true);
	this->mainLoop=true;

	XTranslateCoordinates(this->display,this->parentwc->window,this->parentwc->rootWindow,this->x,this->y,&x,&y,&dw);
	this->mainMenuWindow->LFSTK_moveWindow(x,y,true);
	XSetInputFocus(this->mainMenuWindow->display,this->mainMenuWindow->window,RevertToNone,CurrentTime);
	XSync(display,false);

	while(XPending(this->display))
		{
			XNextEvent(this->mainMenuWindow->display,&event);
			mappedListener *ml=this->mainMenuWindow->LFSTK_getMappedListener(event.xany.window);

			if(ml!=NULL)
				ml->function(ml->gadget,&event,ml->type);
		}

	while(this->mainLoop==true)
		{
			XNextEvent(this->mainMenuWindow->display,&event);
			mappedListener *ml=this->mainMenuWindow->LFSTK_getMappedListener(event.xany.window);

			if(ml!=NULL)
				ml->function(ml->gadget,&event,ml->type);

			if(this->mainMenuWindow->LFSTK_handleWindowEvents(&event)<0)
				mainLoop=false;

			switch(event.type)
				{
					case ConfigureNotify:
					case Expose:
						XTranslateCoordinates(this->display,this->parentwc->window,this->parentwc->rootWindow,this->x,this->y, &x, &y, &dw );
						this->mainMenuWindow->LFSTK_moveWindow(x,y,true);
						break;

					case FocusOut:
						this->mainMenuWindow->LFSTK_hideWindow();
						this->mainMenuWindow->mainLoop=false;
						this->mainLoop=false;
						break;
				}
		}
}

/**
* Add main menus to class.
* \param menuStruct **menus, Pointer to array of prefilled menuStruct's.
* \param int menucnt, Number of main menus.
* \note Menu class owns array DON'T free.
*/
void LFSTK_menuClass::LFSTK_addMainMenus(menuStruct **menus,int menucnt)
{
	LFSTK_menuItemClass		*label;
	int						sy=0;

	this->mainMenuCnt=menucnt;
	this->mainMenu=menus;
	this->mainMenuWindow=new LFSTK_toolWindowClass(this->display,this->parentwc,"_NET_WM_WINDOW_TYPE_MENU",this->x,this->y,GADGETWIDTH,GADGETHITE*this->mainMenuCnt,"menu window");
	for(int j=0;j<this->mainMenuCnt;j++)
		{
			label=new LFSTK_menuItemClass(this->mainMenuWindow,this,this->mainMenu[j]->label,0,sy,GADGETWIDTH,GADGETHITE,this->mainMenu[j],this->subwindows);
			label->LFSTK_setCallBack(this->callback.pressCallback,this->callback.releaseCallback,(void*)(this->mainMenu[j]->userData));
			if(this->mainMenu[j]->imageType==FILETHUMB)
				label->LFSTK_setImageFromPath(this->mainMenu[j]->data.imagePath,MENU,true);
			if(this->mainMenu[j]->imageType==CAIROTHUMB)
				label->LFSTK_setImageFromSurface(this->mainMenu[j]->data.surface,MENU,true);
			sy+=GADGETHITE;
		}

	this->subwindows->push_back(this->mainMenuWindow);
}

void LFSTK_menuClass::LFSTK_freeMenus(menuStruct **menus,int menucnt)
{
	for(int j=0;j<menucnt;j++)
		{
			if(menus[j]->imageType==CAIROTHUMB)
				cairo_surface_destroy(menus[j]->data.surface);
			free(menus[j]->label);
			if(menus[j]->hasSubMenu==true)
				{
					this->LFSTK_freeMenus(menus[j]->subMenus,menus[j]->subMenuCnt);
				}
			delete menus[j];
		}
	delete[] menus;
}




