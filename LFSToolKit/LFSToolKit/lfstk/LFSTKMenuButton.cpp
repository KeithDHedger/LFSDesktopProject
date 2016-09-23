/*
 *
 * ©K. D. Hedger. Fri 24 Jul 10:55:09 BST 2015 kdhedger68713@gmail.com

 * This file (LFSTKMenuButton.cpp) is part of LFSToolKit.

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

void LFSTK_menuButtonClass::initMenuButton(void)
{
	this->subwc=new LFSTK_windowClass(0,0,1,1,"menu window",true,true);
	this->subwc->LFSTK_setWindowType("_NET_WM_WINDOW_TYPE_MENU");
	this->builtMenu=false;

	if(this->wc->globalLib->LFSTK_getUseTheme()==true)
		this->LFSTK_setTile(this->wc->globalLib->LFSTK_getGlobalString(-1,TYPEBUTTONTILE),-1);
	else
		this->useTile=false;
}

LFSTK_menuButtonClass::~LFSTK_menuButtonClass()
{
	delete subwc;
}

LFSTK_menuButtonClass::LFSTK_menuButtonClass()
{
}

/**
* Clear the gadget window to the appropriate state.
*/
void LFSTK_menuButtonClass::LFSTK_clearWindow()
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
	this->drawIndicator(&g,NORMALCOLOUR,DISCLOSURE);
}


/**
* Mouse down callback.
* \param e XButtonEvent passed from mainloop->listener.
* \return Return true if event fully handeled or false to pass it on.
* \note This shows the menu items previously loaded.
*/
bool LFSTK_menuButtonClass::mouseDown(XButtonEvent *e)
{
	LFSTK_buttonClass		*bc;
	geometryStruct			*g;
	int						maxwid=0;
	XEvent					event;
	bool					run=true;
	int						testwid=0;
	int						addto;
	int						sy;
	fontStruct				*tfont;
	const char				*itemfont;
	LFSTK_menuButtonClass	*mb=NULL;
	const monitorStruct 	*mons;
	int 					xpos;
	int 					ypos;
	int						maxiconsize=0;

	geometryStruct	geom={0,0,this->w,this->h};

	if(this->isActive==false)
		{
			this->LFSTK_clearWindow();
			return(true);
		}

	this->drawBox(&geom,ACTIVECOLOUR,this->getActiveBevel());
	this->LFSTK_drawLabel(ACTIVECOLOUR);
	this->drawIndicator(&geom,ACTIVECOLOUR,DISCLOSURE);
	g=this->LFSTK_getGeom();

	if(this->builtMenu==false)
		{
			itemfont=this->wc->globalLib->LFSTK_getGlobalString(-1,TYPEMENUITEMFONT);
			tfont=this->wc->globalLib->LFSTK_loadFont(this->display,this->screen,itemfont);
			maxiconsize=0;
			for(int j=0;j<this->menuCount;j++)
				{
					testwid=this->wc->globalLib->LFSTK_getTextwidth(this->display,(XftFont*)(tfont->data),this->menus[j].label);
					if((this->menus[j].useIcon==true) || (this->menus[j].useImage==true))
						{
							if(maxiconsize<this->menus[j].iconSize)
								maxiconsize=this->menus[j].iconSize;
							testwid+=maxiconsize;
						}
					if(testwid>maxwid)
						maxwid=testwid;
				}

			addto=tfont->ascent+tfont->descent+8;
			maxwid+=maxiconsize+4;
			subwc->LFSTK_resizeWindow(maxwid,this->menuCount*addto,true);
			sy=0;

			for(int j=0;j<this->menuCount;j++)
				{
					if(this->menus[j].subMenus==NULL)
						{
							bc=new LFSTK_buttonClass(subwc,this->menus[j].label,0,sy,maxwid,addto,0);
							this->menus[j].bc=bc;
							bc->LFSTK_setLabelOriention(LEFT);
							bc->LFSTK_setCallBack(NULL,this->callback.releaseCallback,(void*)&(this->menus[j]));
							bc->LFSTK_setStyle(BEVELNONE);
							bc->LFSTK_setFontString(itemfont);
							bc->LFSTK_setLabelAutoColour(this->autoLabelColour);
							if(this->wc->globalLib->LFSTK_getUseTheme()==true)
								bc->LFSTK_setTile(this->wc->globalLib->LFSTK_getGlobalString(-1,TYPEMENUITEMTILE),-1);

							if(this->menus[j].useIcon==true)
								bc->LFSTK_setIcon(this->menus[j].icon[0],this->menus[j].icon[1],this->menus[j].iconSize);
							else if(this->menus[j].useImage==true)
								bc->LFSTK_setScaledImage(this->menus[j].image,this->menus[j].imageWidth,this->menus[j].imageHeight);
							for(int j=0;j<MAXCOLOURS;j++)
								{
									bc->LFSTK_setColourName(j,this->wc->globalLib->LFSTK_getGlobalString(j,TYPEMENUITEM));
									bc->LFSTK_setFontColourName(j,this->wc->globalLib->LFSTK_getGlobalString(j,TYPEMENUITEMFONTCOLOUR));
								}
						}
					else
						{
							mb=new LFSTK_menuButtonClass(subwc,this->menus[j].label,0,sy,maxwid,addto,0);
							mb->LFSTK_setStyle(BEVELNONE);
							mb->LFSTK_setFontString(itemfont);
							mb->LFSTK_setLabelAutoColour(this->autoLabelColour);

							if(this->wc->globalLib->LFSTK_getUseTheme()==true)
								mb->LFSTK_setTile(this->wc->globalLib->LFSTK_getGlobalString(-1,TYPEMENUITEMTILE),-1);
							for(int k=0;k<MAXCOLOURS;k++)
								{
									mb->LFSTK_setColourName(k,this->wc->globalLib->LFSTK_getGlobalString(k,TYPEMENUITEM));
									mb->LFSTK_setFontColourName(k,this->wc->globalLib->LFSTK_getGlobalString(k,TYPEMENUITEMFONTCOLOUR));
								}

							this->menus[j].bc=static_cast<LFSTK_buttonClass*>(mb);
							mb->LFSTK_addMenus(this->menus[j].subMenus,this->menus[j].subMenuCnt);
							mb->LFSTK_setCallBack(NULL,this->callback.releaseCallback,(void*)&(this->menus[j]));
							if(this->menus[j].useIcon==true)
								mb->LFSTK_setIcon(this->menus[j].icon[0],this->menus[j].icon[1],this->menus[j].iconSize);
							else if(this->menus[j].useImage==true)
								mb->LFSTK_setImage(this->menus[j].image,this->menus[j].imageWidth,this->menus[j].imageHeight);

							mb->isSubmenu=true;
						}
					sy+=addto;
				}
			this->builtMenu=true;
		}

	xpos=g->x;
	ypos=g->y+this->h;
	subwc->LFSTK_moveWindow(xpos,ypos,true);

	mons=this->wc->LFSTK_getMonitorData(this->LFSTK_gadgetOnMonitor());
	geometryStruct *subwcg=subwc->LFSTK_getGeom();

	if(xpos+subwcg->w>mons->x+mons->w)
		xpos=(mons->x+mons->w)-subwcg->w;

	if(xpos<mons->x)
		xpos=mons->x;

	if(ypos+subwcg->h>mons->y+mons->h)
		ypos=(mons->y+mons->h)-subwcg->h;

	subwc->LFSTK_moveWindow(xpos,ypos,true);
	subwc->LFSTK_showWindow(true);

	while (run==true)
		{
			XNextEvent(subwc->display,&event);
			mappedListener *ml=subwc->LFSTK_getMappedListener(event.xany.window);
			if(ml!=NULL)
				ml->function(ml->gadget,&event,ml->type);
			switch(event.type)
				{
				case LeaveNotify:
					if(event.xany.window==subwc->window)
					run=false;
					break;
				case Expose:
				break;
				case ButtonRelease:
					run=false;
					break;
				default:
					break;
				}
		}

	subwc->LFSTK_hideWindow();
	delete g;
	delete subwcg;
	return(true);
}
	
/**
* Mouse up callback.
* \param e XButtonEvent passed from mainloop->listener.
* \return Return true if event fully handeled or false to pass it on.
*/
bool LFSTK_menuButtonClass::mouseUp(XButtonEvent *e)
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
			if((this->callback.releaseCallback!=NULL) && ((this->callback.ignoreCallback==false)))
				return(this->callback.releaseCallback(this,this->callback.userData));
		}
	return(true);
}

/**
* Mouse exit callback.
* \param e XButtonEvent passed from mainloop->listener.
* \return Return true if event fully handeled or false to pass it on.
*/
bool LFSTK_menuButtonClass::mouseExit(XButtonEvent *e)
{
	this->LFSTK_clearWindow();
	this->inWindow=false;
	return(true);
}

/**
* Mouse enter callback.
* \param e XButtonEvent passed from mainloop->listener.
* \return Return true if event fully handeled or false to pass it on.
*/
bool LFSTK_menuButtonClass::mouseEnter(XButtonEvent *e)
{
	geometryStruct	g={0,0,this->w,this->h};

	if(this->isActive==false)
		{
			this->LFSTK_clearWindow();
			return(true);
		}

	this->drawBox(&g,PRELIGHTCOLOUR,this->style);
	this->LFSTK_drawLabel(PRELIGHTCOLOUR);
	this->drawIndicator(&g,PRELIGHTCOLOUR,DISCLOSURE);}

/**
* Add array of menu items to gadget.
* \param menus Pointer to array of menuItemStruct's.
* \param cnt Length of array.
* \note It is up to the caller to set the menuItemStruct and free it and any data it contains when finished.
*/
void LFSTK_menuButtonClass::LFSTK_addMenus(menuItemStruct* menus,int cnt)
{
	this->menus=menus;
	this->menuCount=cnt;
}

/**
* Main menu button constructor.
*
* \param parentwc Main parent window class.
* \param label Displayed name.
* \param x X pos.
* \param y Y pos.
* \param w Width.
* \param h Height.
* \param gravity Button gravity.
*/
LFSTK_menuButtonClass::LFSTK_menuButtonClass(LFSTK_windowClass* parentwc,const char* label,int x,int y,int w,int h,int gravity)
{
	XSetWindowAttributes	wa;
	mappedListener			*ml=new mappedListener;

	this->LFSTK_setCommon(parentwc,label,x,y,w,h,gravity);

	wa.win_gravity=gravity;

	this->window=XCreateWindow(this->display,this->parent,x,y,w,h,0,CopyFromParent,InputOutput,CopyFromParent,CWWinGravity,&wa);
	XSelectInput(this->display,this->window,ButtonReleaseMask | ButtonPressMask | ExposureMask | EnterWindowMask | LeaveWindowMask);

	this->initMenuButton();

	this->style=BEVELOUT;
	this->LFSTK_setLabelOriention(LEFT);

	ml->function=&LFSTK_lib::LFSTK_gadgetEvent;
	ml->gadget=this;
	ml->type=MENUBUTTONGADGET;
	this->wc->LFSTK_addMappedListener(this->window,ml);

	this->isSubmenu=false;
}

/**
* Force an update of menu's for dynamic menu.
* \note It is up to the caller to set the menuItemStruct and free it and any data it contains when finished.
*/
void LFSTK_menuButtonClass::LFSTK_updateMenus(menuItemStruct* menus,int cnt)
{
	this->builtMenu=false;
	this->menus=menus;
	this->menuCount=cnt;
}

