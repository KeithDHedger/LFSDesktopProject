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
* \param x X pos.
* \param y Y pos.
* \param w Width.
* \param h Height.
* \param menu menuStruct array.
* \note optional, label gravity defaults to LEFT.
* \note gadget gravity defaults to NorthWestGravity.
*/
LFSTK_menuItemClass::LFSTK_menuItemClass(LFSTK_toolWindowClass* parentwc,LFSTK_menuClass *mainmenu,int x,int y,unsigned w,unsigned h,menuStruct *menu,int labelgrav)
{
	XSetWindowAttributes	wa;

	this->LFSTK_setCommon(parentwc,menu->label,x,y,w,h,BUTTONGRAV);

	wa.win_gravity=BUTTONGRAV;
	wa.save_under=true;
	this->window=XCreateWindow(this->wc->app->display,this->parent,x,y,w,h,0,CopyFromParent,InputOutput,CopyFromParent,CWWinGravity,&wa);
	this->gc=XCreateGC(this->wc->app->display,this->window,0,NULL);
	this->wc->globalLib->LFSTK_setCairoSurface(this->wc->app->display,this->window,this->wc->app->visual,&this->sfc,&this->cr,w,h);
	this->LFSTK_setCairoFontData();
	XSelectInput(this->wc->app->display,this->window,ButtonReleaseMask | ButtonPressMask | ExposureMask | EnterWindowMask | LeaveWindowMask);

	this->style=BEVELNONE;

	this->ml->function=&LFSTK_lib::LFSTK_gadgetEvent;
	this->ml->gadget=this;
	this->ml->type=MENUITEMGADGET;
	this->wc->LFSTK_addMappedListener(this->window,ml);

	if(this->wc->globalLib->LFSTK_getUseTheme()==true)
		this->LFSTK_setTile(this->wc->globalLib->LFSTK_getGlobalString(-1,TYPEMENUITEMTILE),-1);
	else
		this->useTile=false;

	this->LFSTK_setGadgetColours(GADGETBG,this->wc->globalLib->LFSTK_getGlobalString(NORMALCOLOUR,TYPEMENUITEM),
										this->wc->globalLib->LFSTK_getGlobalString(PRELIGHTCOLOUR,TYPEMENUITEM),
										this->wc->globalLib->LFSTK_getGlobalString(ACTIVECOLOUR,TYPEMENUITEM),
										this->wc->globalLib->LFSTK_getGlobalString(INACTIVECOLOUR,TYPEMENUITEM));

	this->LFSTK_setGadgetColours(GADGETFG,this->wc->globalLib->LFSTK_getGlobalString(NORMALCOLOUR,TYPEMENUITEMFONTCOLOUR),
										this->wc->globalLib->LFSTK_getGlobalString(PRELIGHTCOLOUR,TYPEMENUITEMFONTCOLOUR),
										this->wc->globalLib->LFSTK_getGlobalString(ACTIVECOLOUR,TYPEMENUITEMFONTCOLOUR),
										this->wc->globalLib->LFSTK_getGlobalString(INACTIVECOLOUR,TYPEMENUITEMFONTCOLOUR));

	gadgetDetails= {&this->newGadgetBGColours.at(NORMALCOLOUR),BEVELNONE,DISCLOSURE,NORMALCOLOUR,0,true,{0,0,w,h},{(int)(w-TRIANGLESIZE-(this->pad*2)),(int)((h/2)-(TRIANGLESIZE/2)+(this->pad/2)),TRIANGLESIZE,TRIANGLESIZE},menu->hasSubMenu,false,true};
	this->LFSTK_setLabelGravity(labelgrav);
	this->menuData=menu;
	this->menu=mainmenu;
	this->x=x;
	this->y=y;
	this->w=w;
	this->h=h;
}

/**
* Mouse exit callback.
* \param e XButtonEvent passed from mainloop->listener.
* \return Return true if event fully handeled or false to pass it on.
*/
bool LFSTK_menuItemClass::mouseExit(XButtonEvent *e)
{
	if((this->callBacks.runTheCallback==false) || (this->isActive==false))
		return(true);

	if(strcmp(this->label,"--")==0)
		return(true);;

	this->gadgetDetails.colour=&this->newGadgetBGColours.at(NORMALCOLOUR);
	this->gadgetDetails.state=NORMALCOLOUR;
	this->gadgetDetails.bevel=BEVELOUT;
	this->LFSTK_clearWindow();
	this->inWindow=false;
	return(true);
}

/**
* Reset main menu size and add scroll buttons.
*/
void LFSTK_menuItemClass::resizeMenu(int newsize)
{
	int	halfhite=GADGETHITE/2;
	int	sy=0;

	this->topMenu=0;
	this->menuHeight=((newsize/GADGETHITE)*GADGETHITE)+GADGETHITE;
	this->maxMenusDisplayed=(this->menuHeight/GADGETHITE)+1;
	this->subwc->LFSTK_resizeWindow(this->w,this->menuHeight,true);
	if(this->isScrollable==true)
		sy=halfhite;

	for (std::map<int,mappedListener*>::iterator it=this->subwc->gadgetMap.begin();it!=this->subwc->gadgetMap.end();++it)
		{
			mappedListener	*ml=it->second;

			if((ml!=NULL) && (ml->type==MENUITEMGADGET))
				{
					ml->gadget->LFSTK_moveGadget(0,sy);
					sy+=GADGETHITE;
				}
		}

	if(this->upButton==NULL)
		this->upButton=new LFSTK_buttonClass(this->subwc,"<<<<",0,0,this->w,halfhite);
	else
		this->upButton->LFSTK_moveGadget(0,0);
	this->upButton->LFSTK_setMouseCallBack(NULL,this->menuItemScroll,(void*)this);
	this->upButton->userData=USERDATA(1);
	if(this->downButton==NULL)
		this->downButton=new LFSTK_buttonClass(this->subwc,">>>>",0,this->menuHeight-halfhite,this->w,halfhite);
	else
		this->downButton->LFSTK_moveGadget(0,this->menuHeight-halfhite);
	this->downButton->LFSTK_setMouseCallBack(NULL,this->menuItemScroll,(void*)this);
	this->downButton->userData=USERDATA(2);
	this->maxMenusDisplayed=(this->menuHeight/GADGETHITE)+1;

	if(this->isScrollable==false)
		{
			if(this->upButton!=NULL)
				this->upButton->LFSTK_moveGadget(0,-100);
			if(this->downButton!=NULL)
				this->downButton->LFSTK_moveGadget(0,-100);
		}
}

/**
* Mouse enter callback.
* \param e XButtonEvent passed from mainloop->listener.
* \return Return true if event fully handeled or false to pass it on.
*/
bool LFSTK_menuItemClass::mouseEnter(XButtonEvent *e)
{
	double				txtwid=0;
	double				maxtxtwid=0;
	int					finaltxtwid;
	int					gotsubmenu=this->pad*4;
	int					gotthumb=LEFT;
	int					winshrink=0;
	int					hite=GADGETHITE;
	Window				sink;
	Window				childwindow;
	int					sinkx;
	int					sinky;
	unsigned int			buttonmask;
	int					trcx;
	int					trcy;
	Window				dw;
	const monitorStruct	*m;
	int					monnum;

	if((this->callBacks.runTheCallback==false) || (this->isActive==false))
		return(true);

	if(strcmp(this->label,"--")==0)
		return(true);

	//this->gadgetDetails.colour=&this->colourNames[PRELIGHTCOLOUR];
	this->gadgetDetails.colour=&this->newGadgetBGColours.at(PRELIGHTCOLOUR);
	this->gadgetDetails.state=PRELIGHTCOLOUR;
	this->gadgetDetails.bevel=BEVELOUT;
	this->LFSTK_clearWindow();
	this->inWindow=true;

	if(this->menuData->hasSubMenu==true)
		{
			LFSTK_menuItemClass	*label;
			int		sy=0;
			XEvent	event;
			XEvent	lookevent;
			bool	gotlooked=false;

			if(this->subwc==NULL)
				{
					for(int j=0;j<this->menuData->subMenuCnt;j++)
						{
							txtwid=this->menu->LFSTK_getTextWidthForFont(this->menuData->subMenus[j]->label);

							if(txtwid>maxtxtwid)
								maxtxtwid=txtwid;
							if(this->menuData->subMenus[j]->hasSubMenu==true)
								gotsubmenu=GADGETHITE;
							if(this->menuData->subMenus[j]->imageType!=NOTHUMB)
								gotthumb=MENU;
						}
					if(gotthumb==MENU)
						maxtxtwid+=GADGETHITE+gotsubmenu;
					else
						maxtxtwid+=gotsubmenu;
					this->w=maxtxtwid;
					this->subwc=new LFSTK_toolWindowClass(this->wc->app->display,this->wc,"_NET_WM_WINDOW_TYPE_MENU",this->gadgetGeom.x,this->gadgetGeom.y,maxtxtwid,GADGETHITE*this->menuData->subMenuCnt-winshrink,"menu window",this->wc->app);

					for(int j=0; j<this->menuData->subMenuCnt; j++)
						{
							hite=GADGETHITE;

							label=new LFSTK_menuItemClass(this->subwc,this->menu,0,sy,maxtxtwid,hite,this->menuData->subMenus[j],gotthumb);
							label->LFSTK_setMouseCallBack(this->callBacks.mousePressCallback,this->callBacks.mouseReleaseCallback,this->menuData->subMenus[j]->userData);
							if(this->menuData->subMenus[j]->imageType==FILETHUMB)
								label->LFSTK_setImageFromPath(this->menuData->subMenus[j]->data.imagePath,MENU,true);
							if(this->menuData->subMenus[j]->imageType==CAIROTHUMB)
								label->LFSTK_setImageFromSurface(this->menuData->subMenus[j]->data.surface,MENU,true);
							sy+=hite;
						}
					this->menu->subwindows->push_back(this->subwc);
				}


			if(this->subwc!=NULL)
				{
					monnum=this->wc->LFSTK_windowOnMonitor();
					m=this->subwc->app->LFSTK_getMonitorData(monnum);

					if((this->menuData->subMenuCnt*GADGETHITE)>m->h-e->y_root)
						{
							this->isScrollable=true;
							this->resizeMenu((m->h-e->y_root)-GADGETHITE);
							if(this->upButton!=NULL)
								this->upButton->LFSTK_setIgnores(false,true);
						}
					else
						{
							this->isScrollable=false;
							this->resizeMenu((this->menuData->subMenuCnt*GADGETHITE)-GADGETHITE);
						}
				}
			this->subwc->mainLoop=true;
			this->subwc->LFSTK_showWindow(true);
			this->subwc->LFSTK_setKeepAbove(true);

			while(this->subwc->mainLoop==true)
				{
					if(XPending(this->wc->app->display))
						{
							XNextEvent(this->wc->app->display,&event);
						}
					else
						{
							XQueryPointer(this->wc->app->display,this->subwc->app->rootWindow,&sink,&childwindow,&sinkx,&sinky,&sinkx,&sinky,&buttonmask);
							if((childwindow!=this->subwc->window) && (buttonmask!=0))
								{
									for(int j=0;j<this->menu->subwindows->size();j++)
										{
											this->menu->subwindows->at(j)->LFSTK_hideWindow();
											this->menu->subwindows->at(j)->mainLoop=false;
										}
									this->menu->mainMenuWindow->LFSTK_hideWindow();
									this->menu->mainLoop=false;
								}
							continue;
						}

					mappedListener *ml=this->subwc->LFSTK_getMappedListener(event.xany.window);
					switch(event.type)
						{
							case FocusOut:
								for(int j=0;j<this->menu->subwindows->size();j++)
									{
										this->menu->subwindows->at(j)->LFSTK_hideWindow();
										this->menu->subwindows->at(j)->mainLoop=false;
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
								if(ml->type==BUTTONGADGET)
									continue;

								for(int j=0;j<this->menu->subwindows->size();j++)
									{
										this->menu->subwindows->at(j)->LFSTK_hideWindow();
										this->menu->subwindows->at(j)->mainLoop=false;
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
										XTranslateCoordinates(this->subwc->app->display,this->wc->window,this->subwc->app->rootWindow,0,0,&x,&y,&dw );
										this->subwc->LFSTK_moveWindow(x+this->gadgetGeom.w,y+this->gadgetGeom.y,true);
									}
								break;
						}

					gotlooked=XCheckTypedWindowEvent(this->wc->app->display,this->subwc->window,EnterNotify,&lookevent);
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

					gotlooked=XCheckTypedWindowEvent(this->wc->app->display,this->subwc->window,LeaveNotify,&lookevent);
					if(gotlooked==true)
						{
							gotlooked=XCheckTypedWindowEvent(this->wc->app->display,this->wc->window,EnterNotify,&lookevent);
							if(gotlooked==true)
								{
									gotlooked=XCheckTypedWindowEvent(this->wc->app->display,this->window,EnterNotify,&lookevent);
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

/**
* Scroll menus.
*/
bool LFSTK_menuItemClass::menuItemScroll(void *object,void* userdata)
{
	LFSTK_buttonClass	*button=static_cast<LFSTK_buttonClass*>(object);
	LFSTK_menuItemClass	*menuc=static_cast<LFSTK_menuItemClass*>(userdata);
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

			for (std::map<int,mappedListener*>::iterator it=menuc->subwc->gadgetMap.begin();it!=menuc->subwc->gadgetMap.end();++it)
				{
					mappedListener	*ml=it->second;
					if((ml!=NULL) && (ml->type==MENUITEMGADGET))
						{
							ml->gadget->LFSTK_getGeom(&geom);
							ml->gadget->LFSTK_moveGadget(0,cnt+(GADGETHITE/2));
							cnt+=GADGETHITE;
						}
				}

			for (std::map<int,mappedListener*>::iterator it=menuc->subwc->gadgetMap.begin();it!=menuc->subwc->gadgetMap.end();++it)
				{
					mappedListener	*ml=it->second;
					if((ml!=NULL) && (ml->type==MENUITEMGADGET))// && (ml->gadget==gadget))
						{
							ml->gadget->LFSTK_getGeom(&geom);
							geom.y=geom.y-((offset)*GADGETHITE);
							ml->gadget->LFSTK_moveGadget(0,geom.y);
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

			if((menuc->topMenu+(menuc->maxMenusDisplayed-2))>menuc->menuData->subMenuCnt)
				{
					menuc->topMenu=menuc->menuData->subMenuCnt-(menuc->maxMenusDisplayed-2);
					offset=menuc->topMenu;
					if(menuc->downButton!=NULL)
						menuc->downButton->LFSTK_setIgnores(false,true);
				}

			for (std::map<int,mappedListener*>::iterator it=menuc->subwc->gadgetMap.begin();it!=menuc->subwc->gadgetMap.end();++it)
				{
					mappedListener	*ml=it->second;
					if((ml!=NULL) && (ml->type==MENUITEMGADGET))// && (ml->gadget==gadget))
						{
							ml->gadget->LFSTK_getGeom(&geom);
							ml->gadget->LFSTK_moveGadget(0,cnt+(GADGETHITE/2));
							cnt+=GADGETHITE;
						}
				}

			for (std::map<int,mappedListener*>::iterator it=menuc->subwc->gadgetMap.begin();it!=menuc->subwc->gadgetMap.end();++it)
				{
					mappedListener	*ml=it->second;
					if((ml!=NULL) && (ml->type==MENUITEMGADGET))// && (ml->gadget==gadget))
						{
							ml->gadget->LFSTK_getGeom(&geom);
							geom.y=geom.y-((offset)*GADGETHITE);
							ml->gadget->LFSTK_moveGadget(0,geom.y);
						}
				}
			return(true);
		}
	return(true);
}