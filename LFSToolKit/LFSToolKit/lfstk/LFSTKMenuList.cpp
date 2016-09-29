/*
 *
 * ©K. D. Hedger. Fri 23 Sep 15:31:35 BST 2016 kdhedger68713@gmail.com

 * This file (LFSTKMenuList.cpp) is part of LFSToolKit.

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
#include <sys/stat.h>
#include <sys/types.h>
#include "LFSTKGlobals.h"

LFSTK_menuListClass::~LFSTK_menuListClass(void)
{
}

/**
* Main menulist constructor.
*
* \param parentwc Main parent window class.
* \param label Displayed name.
* \param x X pos.
* \param y Y pos.
* \param w Width.
* \param h Height.
* \param gravity Button gravity.
*/
LFSTK_menuListClass::LFSTK_menuListClass(LFSTK_windowClass* parentwc,const char* label,int x,int y,int w,int h,int gravity)
{
	XSetWindowAttributes	wa;
	mappedListener			*ml=new mappedListener;
	this->LFSTK_setCommon(parentwc,label,x,y,w,h,gravity);

	wa.win_gravity=gravity;
	this->window=XCreateWindow(this->display,this->parent,x,y,w,h,0,CopyFromParent,InputOutput,CopyFromParent,CWWinGravity,&wa);
	XSelectInput(this->display,this->window,ButtonReleaseMask | ButtonPressMask | ExposureMask | EnterWindowMask | LeaveWindowMask);

	ml->function=&LFSTK_lib::LFSTK_gadgetEvent;
	ml->gadget=this;
	ml->type=MENULISTGADGET;
	this->wc->LFSTK_addMappedListener(this->window,ml);

	if(this->wc->globalLib->LFSTK_getUseTheme()==true)
		this->LFSTK_setTile(this->wc->globalLib->LFSTK_getGlobalString(-1,TYPEBUTTONTILE),-1);
	else
		this->useTile=false;

	this->listStrings=NULL;
	this->listImages=NULL;
	this->listCnt=0;
	this->listImageCnt=0;
	this->menuWindowSize=new geometryStruct;
	this->menuWindowSize->x=0;
	this->menuWindowSize->y=0;
	this->menuWindowSize->w=0;
	this->menuWindowSize->h=0;
}

/**
* Mouse down callback.
* \param e XButtonEvent passed from mainloop->listener.
* \return Return true if event fully handeled or false to pass it on.
*/
bool LFSTK_menuListClass::mouseDown(XButtonEvent *e)
{
	LFSTK_windowClass	*subwc;
	int 				xpos;
	int 				ypos;
	const monitorStruct	*mons;
	XEvent				event;
	bool				run=true;

	Window				root_return;
	Window				child_return;
	int					win_x_return;
	int					win_y_return;
	unsigned			mask_return;
	pointStruct			point;

	geometryStruct		*g;
	g=this->LFSTK_getGeom();

	if(this->isActive==false)
		{
			this->LFSTK_clearWindow();
			return(true);
		}

	this->drawBox(g,ACTIVECOLOUR,this->getActiveBevel());
	this->LFSTK_drawLabel(ACTIVECOLOUR);

	if(this->callback.pressCallback!=NULL)
		return(this->callback.pressCallback(this,this->callback.userData));

	subwc=new LFSTK_windowClass(0,0,this->menuWindowSize->w,FNAVBUTTONHITE*this->listCnt,"menu window",true,true);
	subwc->LFSTK_setWindowType("_NET_WM_WINDOW_TYPE_MENU");
	xpos=g->x;
	ypos=g->y+this->h;
	subwc->LFSTK_moveWindow(0,0,true);

	mons=this->wc->LFSTK_getMonitorData(this->LFSTK_gadgetOnMonitor());
	geometryStruct *subwcg=subwc->LFSTK_getGeom();

	if(xpos+subwcg->w>mons->x+mons->w)
		xpos=(mons->x+mons->w)-subwcg->w;

	if(xpos<mons->x)
		xpos=mons->x;

	if(ypos+subwcg->h>mons->y+mons->h)
		ypos=(mons->y+mons->h)-subwcg->h;

	subwc->LFSTK_moveWindow(xpos,ypos,true);
	this->menuWindowSize->x=xpos;
	this->menuWindowSize->y=ypos;

	LFSTK_buttonClass	*label;
	for(int j=0;j<this->listCnt;j++)
		{
			label=new LFSTK_buttonClass(subwc,this->listStrings[j],0,j*FNAVBUTTONHITE,this->menuWindowSize->w,FNAVBUTTONHITE,NorthWestGravity);
		}
	subwc->LFSTK_showWindow(true);
	run=true;
	while (run==true)
		{
			XNextEvent(subwc->display,&event);
			
			mappedListener *ml=subwc->LFSTK_getMappedListener(event.xany.window);
			if(ml!=NULL)
				{
					ml->function(ml->gadget,&event,ml->type);
					continue;
				}

			switch(event.type)
				{
					case LeaveNotify:
						{
							if(event.xany.window==subwc->window)
								{
									mask_return=0;
									while(mask_return==0)
										{
										    XQueryPointer(subwc->display,subwc->window,&root_return, &child_return,&point.x,&point.y,&win_x_return,&win_y_return,&mask_return);
											if(this->wc->globalLib->LFSTK_pointInRect(&point,this->menuWindowSize)==true)
												break;
										}
									while(mask_return!=0)
										{
											XQueryPointer(subwc->display,subwc->window,&root_return, &child_return,&point.x,&point.y,&win_x_return,&win_y_return,&mask_return);
											if(subwc->window==child_return)
												break;
										}
									if(this->wc->globalLib->LFSTK_pointInRect(&point,this->menuWindowSize)==true)
										continue;
									run=false;
								}
						}
					break;
				//case Expose:
				//	subwc->LFSTK_clearWindow();
				//	break;
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
* Set primary menu list.
* \param char **list.
*/
void LFSTK_menuListClass::LFSTK_addMenuList(char **list,unsigned cnt)
{
	const char	*bigstring=NULL;
	unsigned	maxlen=0;

	if(this->listStrings!=NULL)
		{
			for(int j=0;j<this->listCnt;j++)
				free(this->listStrings[j]);
			delete this->listStrings;
		}
	this->listCnt=cnt;
	this->listStrings=new char*[cnt];
	for(int j=0;j<cnt;j++)
		{
			this->listStrings[j]=strdup(list[j]);
			if(strlen(this->listStrings[j])>maxlen)
				{
					maxlen=strlen(this->listStrings[j]);
					bigstring=this->listStrings[j];
				}
		}
	this->LFSTK_getFontGeom(this->menuWindowSize,bigstring);
	this->menuWindowSize->h=this->menuWindowSize->h*cnt;
//	printf("%i %i %i %i\n",this->menuWindowSize->x,this->menuWindowSize->y,this->menuWindowSize->w,this->menuWindowSize->h*cnt);
}



