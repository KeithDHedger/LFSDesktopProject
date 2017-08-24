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

#include "lfstk/LFSTKGlobals.h"

void LFSTK_menuButtonClass::initMenuButton(void)
{
	this->subwc=new LFSTK_windowClass(0,0,1,1,"menu window",true,true);
	this->subwc->LFSTK_setWindowType("_NET_WM_WINDOW_TYPE_NORMAL");
	this->builtMenu=false;

	if(this->wc->globalLib->LFSTK_getUseTheme()==true)
		this->LFSTK_setTile(this->wc->globalLib->LFSTK_getGlobalString(-1,TYPEBUTTONTILE),-1);
	else
		this->useTile=false;
}

LFSTK_menuButtonClass::~LFSTK_menuButtonClass()
{
	if(subwc!=NULL)
		delete subwc;
	subwc=NULL;
}

LFSTK_menuButtonClass::LFSTK_menuButtonClass()
{
}

/**
* Mouse down callback.
* \param e XButtonEvent passed from mainloop->listener.
* \return Return true if event fully handeled or false to pass it on.
* \note This shows the menu items previously loaded.
*/
bool LFSTK_menuButtonClass::mouseDown(XButtonEvent *e)
{
	int						maxwidth=0;
	int						sy=0;
	int						buttonwidth=0;
	int						winhite=0;
	bool					run=true;
	XEvent					event;
	geometryStruct			g;
	Window					sink;
	Window					childwindow;
	int						sinkx;
	int						sinky;
	bool					runsub=true;
	unsigned int			buttonmask;
	LFSTK_menuButtonClass	*mb=NULL;
	LFSTK_buttonClass		*bc=NULL;
	pointStruct				point;
	const geometryStruct	*parentwindowgeom=this->wc->LFSTK_getWindowGeom();
	const geometryStruct	*subwindowgeom;
	int						labelorient;
	bool					addimagespace=false;

	if(this->isActive==false)
		return(true);

	this->gadgetDetails.colour=&this->colourNames[ACTIVECOLOUR];
	this->gadgetDetails.state=ACTIVECOLOUR;
	this->gadgetDetails.bevel=BEVELIN;
	this->LFSTK_clearWindow();

	if(this->builtMenu==false)
		{
			labelorient=LEFT;
			for(int j=0;j<this->menuCount;j++)
				{
				//TODO//
					buttonwidth=this->LFSTK_getTextWidth(menus[j].label);
					if(buttonwidth>maxwidth)
						maxwidth=buttonwidth;
					if(menus[j].imagePath!=NULL)
						{
							labelorient=MENU;
							addimagespace=true;
						}
				}
			maxwidth+=4;
			if(addimagespace==true)
				maxwidth+=this->gadgetDetails.gadgetGeom.h+(this->pad*4);
//TODO//
			for(int j=0;j<this->menuCount;j++)
				{
					if(menus[j].subMenus==NULL)
						{
							bc=new LFSTK_buttonClass(this->subwc,this->menus[j].label,0,winhite,maxwidth,24,0);
							bc->LFSTK_setCallBack(NULL,this->callback.releaseCallback,(void*)&(this->menus[j]));
							this->menus[j].bc=static_cast<LFSTK_buttonClass*>(bc);
						}
					else
						{
							mb=new LFSTK_menuButtonClass(this->subwc,this->menus[j].label,0,winhite,maxwidth,24,0);
							mb->LFSTK_setCallBack(NULL,this->callback.releaseCallback,NULL);
							mb->LFSTK_addMenus(menus[j].subMenus,menus[j].subMenuCnt);
							this->menus[j].bc=static_cast<LFSTK_buttonClass*>(mb);
						}
					winhite+=24;
					this->menus[j].bc->LFSTK_setStyle(BEVELNONE);
					if(this->wc->globalLib->LFSTK_getUseTheme()==true)
						this->menus[j].bc->LFSTK_setTile(this->wc->globalLib->LFSTK_getGlobalString(-1,TYPEMENUITEMTILE),-1);
					this->menus[j].bc->LFSTK_setFontString(this->menuItemFont);
					this->menus[j].bc->LFSTK_setCairoFontData();

					if(menus[j].imagePath!=NULL)
						this->menus[j].bc->LFSTK_setImageFromPath(menus[j].imagePath,MENU,true);
					this->menus[j].bc->LFSTK_setLabelGravity(labelorient);
				}

			this->subwc->LFSTK_resizeWindow(maxwidth,winhite,true);
			this->builtMenu=true;
		}

	this->LFSTK_getGlobalGeom(&g);
	this->subwc->LFSTK_moveWindow(g.x+g.w-TRIANGLESIZE*2,g.y+this->gadgetGeom.h-TRIANGLESIZE*2,true);
	this->subwc->LFSTK_showWindow(true);
	subwindowgeom=this->subwc->LFSTK_getWindowGeom();

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
							{
								runsub=true;
								while(runsub==true)
									{
										XQueryPointer(this->display, this->subwc->window,&sink,&childwindow,&sinkx,&sinky,&sinkx,&sinky,&buttonmask);
										if(childwindow!=0)
											runsub=false;

										if((childwindow==0) && (buttonmask != 0))
											{
												runsub=false;
												run=false;
											}
										point.x=subwindowgeom->x+sinkx;
										point.y=subwindowgeom->y+sinky;
										if(this->wc->globalLib->LFSTK_pointInRect(&point,(geometryStruct*)parentwindowgeom))
											{
												XSync(this->display,true);
												subwc->LFSTK_hideWindow();
												this->gadgetDetails.colour=&this->colourNames[NORMALCOLOUR];
												this->gadgetDetails.state=NORMALCOLOUR;
												this->LFSTK_clearWindow();
												this->inWindow=false;
												return(true);
											}
									}
								this->LFSTK_clearWindow();
							}
						break;
					case Expose:
						this->subwc->LFSTK_clearWindow();
						break;
					case ButtonRelease:
						run=false;
						break;
					default:
						break;
				}
		}

	subwc->LFSTK_hideWindow();
	return(true);
}

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
LFSTK_menuButtonClass::LFSTK_menuButtonClass(LFSTK_windowClass* parentwc,const char* label,int x,int y,unsigned w,unsigned h,int gravity)
{
	XSetWindowAttributes	wa;
	mappedListener			*ml=new mappedListener;

	this->LFSTK_setCommon(parentwc,label,x,y,w,h,gravity);

	wa.win_gravity=gravity;
	wa.save_under=true;
	this->window=XCreateWindow(this->display,this->parent,x,y,w,h,0,CopyFromParent,InputOutput,CopyFromParent,CWWinGravity,&wa);
	this->gc=XCreateGC(this->display,this->window,0,NULL);
	this->wc->globalLib->LFSTK_setCairoSurface(this->display,this->window,this->visual,&this->sfc,&this->cr,w,h);
	this->LFSTK_setCairoFontData();
	XSelectInput(this->display,this->window,ButtonReleaseMask | ButtonPressMask | ExposureMask | EnterWindowMask | LeaveWindowMask);

	this->initMenuButton();

	this->style=BEVELOUT;
	this->LFSTK_setLabelGravity(LEFT);

	ml->function=&LFSTK_lib::LFSTK_gadgetEvent;
	ml->gadget=this;
	ml->type=MENUBUTTONGADGET;
	this->wc->LFSTK_addMappedListener(this->window,ml);

	menuItemFont=this->wc->globalLib->LFSTK_getGlobalString(-1,TYPEMENUITEMFONT);
unsigned xxx=w-TRIANGLESIZE-(this->pad*2);


	gadgetDetails={&this->colourNames[NORMALCOLOUR],BEVELOUT,DISCLOSURE,NULL,NORMALCOLOUR,0,true,{0,0,w,h},{(int)(w-TRIANGLESIZE-(this->pad*2)),(int)((h/2)-(TRIANGLESIZE/2)+(this->pad/2)),TRIANGLESIZE,TRIANGLESIZE},true};
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

