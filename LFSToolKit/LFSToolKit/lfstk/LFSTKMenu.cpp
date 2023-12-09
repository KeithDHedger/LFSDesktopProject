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
	for(int j=0;j<this->subwindows->size();j++)
		delete this->subwindows->at(j);

	delete this->subwindows;
	LFSTK_freeMenus(this->mainMenu,this->mainMenuCnt);
	freeAndNull(&this->fontName);
}

LFSTK_menuClass::LFSTK_menuClass(LFSTK_windowClass *wc,int x,int y,unsigned w,unsigned h)
{
	subwindows=new std::vector<LFSTK_toolWindowClass*>;
	subwindows->clear();
	this->parentwc=wc;
	this->x=x;
	this->y=y;
	this->w=w;
	this->h=h;
	this->fontDesc=this->parentwc->app->globalLib->LFSTK_getGlobalString(NORMALCOLOUR,TYPEMENUITEMFONT);
}

/**
* Set callback for widget.
* \param downcb Mouse down callback.
* \param releasecb Mouse up callback.
* \note Format for callback is "bool functioname(void *p,void* ud)"
* \note First param passed to callback is pointer to object.
* \note Second param passed to callback is user data.
*/
void LFSTK_menuClass::LFSTK_setMouseCallBack(bool (*downcb)(void *,void*),bool (*releasecb)(void *,void*),void* ud)
{

	if(downcb!=NULL)
		this->callBacks.validCallbacks|=MOUSEPRESSCB;
	if(releasecb!=NULL)
		this->callBacks.validCallbacks|=MOUSERELEASECB;
	this->callBacks.mousePressCallback=downcb;
	this->callBacks.mouseReleaseCallback=releasecb;
	this->callBacks.mouseUserData=ud;
}

/**
* Reset main menu size and add scroll buttons.
*/
void LFSTK_menuClass::resizeMenu(void)
{
	int					trcx;
	int					trcy;
	Window				dw;
	int					halfhite=GADGETHITE/2;
	int					sy=0;
	int					regetmenh=0;
	int					monnum;
	const monitorStruct	*m;

	this->mainMenuWindow->app->LFSTK_loadMonitorData();
	this->topMenu=0;
	regetmenh=GADGETHITE*this->mainMenuCnt;

	XTranslateCoordinates(this->parentwc->app->display,this->parentwc->window,this->parentwc->app->rootWindow,0,0,&trcx,&trcy,&dw);

	monnum=this->parentwc->LFSTK_windowOnMonitor();

	m=this->mainMenuWindow->app->LFSTK_getMonitorData(monnum);

	if((regetmenh+trcy+this->y)>m->h)
		{
			this->isScrollable=true;
			sy=halfhite;
			this->menuHeight=(m->h-(trcy+this->y));
			this->menuHeight=((this->menuHeight/GADGETHITE)*GADGETHITE);
			if(this->menuHeight<=GADGETHITE)
				return;;//TODO//
			this->mainMenuWindow->LFSTK_resizeWindow(this->w,this->menuHeight,true);

			for (std::map<int,mappedListener*>::iterator it=this->mainMenuWindow->gadgetMap.begin();it!=this->mainMenuWindow->gadgetMap.end();++it)
				{
					mappedListener	*ml=it->second;

					if((ml!=NULL) && (ml->type==MENUITEMGADGET))
						{
							ml->gadget->LFSTK_moveGadget(0,sy);
							sy+=GADGETHITE;
						}
				}

			if(this->upButton==NULL)
				this->upButton=new LFSTK_buttonClass(this->mainMenuWindow,"<<<<",0,0,this->w,halfhite);
			else
				this->upButton->LFSTK_moveGadget(0,0);
			this->upButton->LFSTK_setMouseCallBack(NULL,this->menuScroll,(void*)this);
			this->upButton->userData=USERDATA(1);
			if(this->downButton==NULL)
				this->downButton=new LFSTK_buttonClass(this->mainMenuWindow,">>>>",0,this->menuHeight-halfhite,this->w,halfhite);/////////////////>>>>>>>>>>>>>>>>>
			else
				this->downButton->LFSTK_moveGadget(0,this->menuHeight-(halfhite));
			this->downButton->LFSTK_setMouseCallBack(NULL,this->menuScroll,(void*)this);
			this->downButton->userData=USERDATA(2);
			this->maxMenusDisplayed=(this->menuHeight/GADGETHITE)+1;
		}
	else
		{
			this->menuHeight=regetmenh;
			sy=0;
			this->mainMenuWindow->LFSTK_resizeWindow(this->w,this->menuHeight,true);
			for (std::map<int,mappedListener*>::iterator it=this->mainMenuWindow->gadgetMap.begin();it!=this->mainMenuWindow->gadgetMap.end();++it)
				{
					mappedListener	*ml=it->second;

					if((ml!=NULL) && (ml->type==MENUITEMGADGET))
						{
							ml->gadget->LFSTK_moveGadget(0,sy);
							sy+=GADGETHITE;
						}
				}
			if(this->upButton!=NULL)
				this->upButton->LFSTK_moveGadget(0,-100);
			if(this->downButton!=NULL)
				this->downButton->LFSTK_moveGadget(0,-100);
		}
}

/**
* Show main menu window.
*/
void LFSTK_menuClass::LFSTK_showMenu(void)
{
	XEvent			event;
	Window			dw;
	int				x;
	int				y;
	unsigned int		w,h,dump;
	Window			sink;
	Window			childwindow;
	int				sinkx;
	int				sinky;
	unsigned int		buttonmask;

	this->mainMenuWindow->app->LFSTK_loadMonitorData();

	this->resizeMenu();
	if(this->upButton!=NULL)
		this->upButton->LFSTK_setIgnores(false,true);
	this->mainMenuWindow->LFSTK_showWindow(true);
	this->mainMenuWindow->LFSTK_setKeepAbove(true);

	this->mainLoop=true;

	XTranslateCoordinates(this->parentwc->app->display,this->parentwc->window,this->parentwc->app->rootWindow,this->x,this->y,&x,&y,&dw);
	this->mainMenuWindow->LFSTK_moveWindow(x,y,true);
	XSetInputFocus(this->parentwc->app->display,this->mainMenuWindow->window,RevertToNone,CurrentTime);

	if(this->upButton!=NULL)
		this->upButton->LFSTK_setIgnores(false,true);

	while(XPending(this->parentwc->app->display))
		{
			XNextEvent(this->parentwc->app->display,&event);
			mappedListener *ml=this->mainMenuWindow->LFSTK_getMappedListener(event.xany.window);

			if(ml!=NULL)
				ml->function(ml->gadget,&event,ml->type);
		}

	while(this->mainLoop==true)
		{
			if(XPending(this->parentwc->app->display))
				{
					XNextEvent(this->parentwc->app->display,&event);
				}
			else
				{
					XQueryPointer(this->parentwc->app->display,this->parentwc->app->rootWindow,&sink,&childwindow,&sinkx,&sinky,&sinkx,&sinky,&buttonmask);
					if((childwindow!=this->mainMenuWindow->window) && (buttonmask!=0))
						{
							this->mainMenuWindow->LFSTK_hideWindow();
							this->mainMenuWindow->mainLoop=false;
							this->mainLoop=false;
						}
					continue;
				}

			mappedListener *ml=this->mainMenuWindow->LFSTK_getMappedListener(event.xany.window);

			if(ml!=NULL)
				ml->function(ml->gadget,&event,ml->type);

			if(this->mainMenuWindow->LFSTK_handleWindowEvents(&event)<0)
				mainLoop=false;
	
			switch(event.type)
				{
					case ConfigureNotify:
					case Expose:
						XTranslateCoordinates(this->parentwc->app->display,this->parentwc->window,this->parentwc->app->rootWindow,this->x,this->y,&x,&y,&dw );
						this->mainMenuWindow->LFSTK_moveWindow(x,y,true);
						break;

					case FocusOut:
					case FocusIn:
						this->mainMenuWindow->LFSTK_hideWindow();
						this->mainMenuWindow->mainLoop=false;
						this->mainLoop=false;
						break;
				}
		}
}

/**
* Scroll menus.
*/
bool LFSTK_menuClass::menuScroll(void *object,void* userdata)
{
	LFSTK_buttonClass	*button=static_cast<LFSTK_buttonClass*>(object);
	LFSTK_menuClass		*menuc=static_cast<LFSTK_menuClass*>(userdata);
	geometryStruct		geom;
	int					offset;
	int					cnt;

	if((long)button->userData==1)
		{
			cnt=0;
			menuc->topMenu-=(menuc->maxMenusDisplayed-2);
			offset=menuc->topMenu;

			if(menuc->downButton!=NULL)
				menuc->downButton->LFSTK_setIgnores(true,true);

			if(menuc->topMenu<0)
				{
					menuc->topMenu=0;
					offset=0;
					if(menuc->upButton!=NULL)
						menuc->upButton->LFSTK_setIgnores(false,true);
				}

			for (std::map<int,mappedListener*>::iterator it=menuc->mainMenuWindow->gadgetMap.begin();it!=menuc->mainMenuWindow->gadgetMap.end();++it)
				{
					mappedListener	*ml=it->second;
					if((ml!=NULL) && (ml->type==MENUITEMGADGET))
						{
							ml->gadget->LFSTK_getGeom(&geom);
							ml->gadget->LFSTK_moveGadget(geom.x,cnt+(GADGETHITE/2));
							cnt+=GADGETHITE;
						}
				}

			for (std::map<int,mappedListener*>::iterator it=menuc->mainMenuWindow->gadgetMap.begin();it!=menuc->mainMenuWindow->gadgetMap.end();++it)
				{
					mappedListener	*ml=it->second;
					if((ml!=NULL) && (ml->type==MENUITEMGADGET))// && (ml->gadget==gadget))
						{
							ml->gadget->LFSTK_getGeom(&geom);
							geom.y=geom.y-((offset)*GADGETHITE);
							ml->gadget->LFSTK_moveGadget(geom.x,geom.y);
						}
				}
			return(true);
		}

	if((long)button->userData==2)
		{
			menuc->topMenu+=(menuc->maxMenusDisplayed-2);
			offset=menuc->topMenu;
			cnt=0;

			if(menuc->upButton!=NULL)
				menuc->upButton->LFSTK_setIgnores(true,true);

			if((menuc->topMenu+(menuc->maxMenusDisplayed-2))>menuc->mainMenuCnt)
				{
					menuc->topMenu=menuc->mainMenuCnt-(menuc->maxMenusDisplayed-2);
					offset=menuc->topMenu;
					if(menuc->downButton!=NULL)
						menuc->downButton->LFSTK_setIgnores(false,true);
				}

			for (std::map<int,mappedListener*>::iterator it=menuc->mainMenuWindow->gadgetMap.begin();it!=menuc->mainMenuWindow->gadgetMap.end();++it)
				{
					mappedListener	*ml=it->second;
					if((ml!=NULL) && (ml->type==MENUITEMGADGET))
						{
							ml->gadget->LFSTK_getGeom(&geom);
							ml->gadget->LFSTK_moveGadget(geom.x,cnt+(GADGETHITE/2));
							cnt+=GADGETHITE;
						}
				}

			for (std::map<int,mappedListener*>::iterator it=menuc->mainMenuWindow->gadgetMap.begin();it!=menuc->mainMenuWindow->gadgetMap.end();++it)
				{
					mappedListener	*ml=it->second;
					if((ml!=NULL) && (ml->type==MENUITEMGADGET))
						{
							ml->gadget->LFSTK_getGeom(&geom);
							geom.y=geom.y-((offset)*GADGETHITE);
							ml->gadget->LFSTK_moveGadget(geom.x,geom.y);
						}
				}
			return(true);
		}
	return(true);
}

/**
* Add main menus to class.
* \param menuStruct **menus, Pointer to array of prefilled menuStruct's.
* \param int menucnt, Number of main menus.
* \note Menu class owns array DON'T free.
*/
void LFSTK_menuClass::LFSTK_addMainMenus(menuStruct **menus,int menucnt)
{
	LFSTK_menuItemClass	*label;
	int					sy=0;
	double				txtwid=0;
	double				maxtxtwid=0;
	int					finaltxtwid;
	int					gotsubmenu=8;
	int					gotthumb=LEFT;
	int					winshrink=0;
	int					hite=GADGETHITE;

	if(this->mainMenu!=NULL)
		{
			for(int j=0;j<this->subwindows->size();j++)
				delete this->subwindows->at(j);
			this->subwindows->clear();
			LFSTK_freeMenus(this->mainMenu,this->mainMenuCnt);
		}

	this->mainMenuCnt=menucnt;
	this->mainMenu=menus;

	for(int j=0;j<menucnt;j++)
		{
			txtwid=this->LFSTK_getTextWidthForFont(this->mainMenu[j]->label);
			if(txtwid>maxtxtwid)
				maxtxtwid=txtwid;
			if(this->mainMenu[j]->hasSubMenu==true)
				gotsubmenu=GADGETHITE;
			if(this->mainMenu[j]->imageType!=NOTHUMB)
				gotthumb=MENU;
			if(strcmp(this->mainMenu[j]->label,"--")==0)
				winshrink+=(GADGETHITE-SEPARATORHITE);
		}

	if(gotthumb==MENU)
		maxtxtwid+=GADGETHITE+gotsubmenu;
	else
		maxtxtwid+=gotsubmenu;

	windowInitStruct	*win;

	win=new windowInitStruct;//TODO//
	win->app=this->parentwc->app;
	win->windowName="LFSTKMenu";
	win->className="LFSTKMenu";
	win->appName="LFSTKApplication";
	win->loadVars=true;
	win->x=this->x;
	win->y=this->y;
	win->w=maxtxtwid;
	win->h=GADGETHITE*this->mainMenuCnt-winshrink;
	if((this->menuHeight>0) && (win->h>this->menuHeight))
	{
		win->h=((this->menuHeight/GADGETHITE)*GADGETHITE)+GADGETHITE;
		this->menuHeight=win->h;
		this->isScrollable=true;
	}
	else
		this->isScrollable=false;
	win->wc=this->parentwc;
	//win->windowType="_NET_WM_WINDOW_TYPE_MENU";
	win->windowType=this->parentwc->app->appAtomsHashed.at(this->parentwc->app->globalLib->prefs.LFSTK_hashFromKey("_NET_WM_WINDOW_TYPE_MENU"));

	win->decorated=false;
	win->overRide=true;
	win->level=ABOVEALL;
	if(this->isScrollable==true)
		{
			win->h+=hite;
			sy=hite/2;
		}
	this->mainMenuWindow=new LFSTK_toolWindowClass(win,this->parentwc->app);
	delete win;

	for(int j=0;j<this->mainMenuCnt;j++)
		{
			hite=GADGETHITE;
			label=new LFSTK_menuItemClass(this->mainMenuWindow,this,0,sy,maxtxtwid,hite,this->mainMenu[j],gotthumb);
			label->LFSTK_setMouseCallBack(this->callBacks.mousePressCallback,this->callBacks.mouseReleaseCallback,(void*)(this->mainMenu[j]->userData));
			if(this->mainMenu[j]->imageType==FILETHUMB)
				label->LFSTK_setImageFromPath(this->mainMenu[j]->data.imagePath,MENU,true);
			if(this->mainMenu[j]->imageType==CAIROTHUMB)
				label->LFSTK_setImageFromSurface(this->mainMenu[j]->data.surface,MENU,true);
			sy+=hite;
		}

	if(this->isScrollable==true)
		{
			this->upButton=new LFSTK_buttonClass(this->mainMenuWindow,"<<<<",0,0,maxtxtwid,hite/2);
			this->upButton->LFSTK_setMouseCallBack(NULL,this->menuScroll,(void*)this);
			this->upButton->userData=USERDATA(1);
			this->downButton=new LFSTK_buttonClass(this->mainMenuWindow,">>>>",0,this->menuHeight+(hite/2),maxtxtwid,hite/2);
			this->downButton->LFSTK_setMouseCallBack(NULL,this->menuScroll,(void*)this);
			this->downButton->userData=USERDATA(2);
			this->maxMenusDisplayed=(this->menuHeight/GADGETHITE)+1;
		}
	this->w=maxtxtwid;
	this->subwindows->push_back(this->mainMenuWindow);
}

void LFSTK_menuClass::LFSTK_freeMenus(menuStruct **menus,int menucnt)
{
	for(int j=0;j<menucnt;j++)
		{
			if(menus[j]->imageType==CAIROTHUMB)
				cairo_surface_destroy(menus[j]->data.surface);
			if(menus[j]->imageType==FILETHUMB)
				freeAndNull(&menus[j]->data.imagePath);
			if(menus[j]->label!=NULL)
				freeAndNull(&menus[j]->label);
			if(menus[j]->hasSubMenu==true)
				{
					this->LFSTK_freeMenus(menus[j]->subMenus,menus[j]->subMenuCnt);
				}
			delete menus[j];
		}
	delete[] menus;
	this->mainMenu=NULL;
	this->mainMenuCnt=0;
}

/**
* Return text width.
* \param const char *text.
* \return int text width rounded to nearest int.
*/
int	LFSTK_menuClass::LFSTK_getTextWidthForFont(const char *text)
{
	cairo_text_extents_t	returnextents;
	cairo_t					*cr=NULL;
	cairo_surface_t			*sfc=NULL;
	char					*string=strdup(this->fontDesc);
	char					*str;
	bool					found=false;

	weight=CAIRO_FONT_WEIGHT_NORMAL;
	slant=CAIRO_FONT_SLANT_NORMAL;
	fontSize=10;

	if(this->fontName!=NULL)
		freeAndNull(&this->fontName);

	this->fontName=strdup("sans");
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
						freeAndNull(&this->fontName);
					this->fontName=strdup(str);
				}
			str=strtok(NULL,":");
		}
	freeAndNull(&string);

	this->parentwc->globalLib->LFSTK_setCairoSurface(this->parentwc->app->display,this->parentwc->window,this->parentwc->app->visual,&sfc,&cr,1,1);
	cairo_select_font_face(cr,this->fontName,this->slant,this->weight);
	cairo_set_font_size(cr,this->fontSize);
	cairo_text_extents(cr,text,&returnextents);

	cairo_destroy(cr);
	cairo_surface_destroy(sfc);
	return((int)(returnextents.x_advance+0.5));
}
