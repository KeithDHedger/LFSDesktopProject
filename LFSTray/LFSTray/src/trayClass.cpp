/*
 *
 * ©K. D. Hedger. Tue 28 Jan 12:51:01 GMT 2025 keithdhedger@gmail.com

 * This file (trayClass.cpp) is part of LFSTray.

 * LFSTray is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.

 * LFSTray is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

 * You should have received a copy of the GNU General Public License
 * along with LFSTray.  If not, see <http://www.gnu.org/licenses/>.
*/

#include "globals.h"

LFSTray_trayClass::LFSTray_trayClass(LFSTK_applicationClass *app)
{
	this->apc=app;
	this->embedClass=new LFSTray_embedClass(this);
}

LFSTray_trayClass::~LFSTray_trayClass()
{
	delete this->embedClass;
}

void LFSTray_trayClass::setTrayAtoms(void)
{
	std::string aname;

	aname="_NET_SYSTEM_TRAY_S"+std::to_string(this->apc->screen);
	this->_NET_SYSTEM_TRAY_SCREEN=XInternAtom(this->apc->display,aname.c_str(),False);

	aname="MANAGER";
	this->MANAGER=XInternAtom(this->apc->display,aname.c_str(),False);

	aname="_NET_SYSTEM_TRAY_OPCODE";
	this->_NET_SYSTEM_TRAY_OPCODE=XInternAtom(this->apc->display,aname.c_str(),False);

	aname="_NET_SYSTEM_TRAY_MESSAGE_DATA";
	this->_NET_SYSTEM_TRAY_MESSAGE_DATA=XInternAtom(this->apc->display,aname.c_str(),False);

	aname="_NET_WM_NAME";
	this->_NET_WM_NAME=XInternAtom(this->apc->display,aname.c_str(),False);

	aname="UTF8_STRING";
	this->UTF8_STRING=XInternAtom(this->apc->display,aname.c_str(),False);

	aname="_NET_WM_PID";
	this->_NET_WM_PID=XInternAtom(this->apc->display,aname.c_str(),False);


	aname="_XEMBED";
	this->_XEMBED=XInternAtom(this->apc->display,aname.c_str(),False);
}

void* LFSTray_trayClass::getProperty(Window w,Atom prop,Atom type,long unsigned int *nitems_return)
{
	void				*prop_return=NULL;
	Atom				actual_type_return=0;
	int				actual_format_return=0;
	unsigned long	bytes_after_return=0;
	int				result=-1;

	result=XGetWindowProperty(this->apc->display,w,prop,0L,32,false,type, &actual_type_return, &actual_format_return,nitems_return, &bytes_after_return, (unsigned char**)&prop_return);
	if(result!=Success||bytes_after_return>0)
		{
			XFree(prop_return);
			prop_return=NULL;
			return(NULL);
		}

	return(prop_return);
}

std::string LFSTray_trayClass::getWindowName(Window win)
{
	char					*namex=NULL;
	XTextProperty		p;
	long unsigned int	nitems_return;
	std::string			name;
	name="";

	namex=(char*)this->getProperty(win,this->_NET_WM_NAME,this->UTF8_STRING, &nitems_return);
	if(namex!=NULL)
		{
			name=namex;
			free(namex);
		}
	else
		{
			if(XGetWMName(this->apc->display,win, &p)!=0)
				{
					char		**v=NULL;
					int		n=0;

					XmbTextPropertyToTextList(this->apc->display, &p, &v, &n);
					if(n>0)
						name=v[0];

					if(v!=NULL)
						XFreeStringList(v);
				}
		}

	return(name);
}

void LFSTray_trayClass::resetWindow(void)
{
	const monitorStruct *mons=this->apc->LFSTK_getMonitors();
	int					w=1;
	int					h=1;
	int					x=-1000;
	int					y=-1000;

	if(this->iconList.size()==0)
		{
			this->apc->mainWindow->LFSTK_moveResizeWindow(-1000,-1000,1,1,true);
			return;
		}

	if(this->gravity==N || this->gravity==S)
		this->vertical=false;
	if(this->gravity==E || this->gravity==W)
		this->vertical=true;

	if(this->vertical==false)
		{
			w=this->iconList.size()*this->iconSize;
			h=this->iconSize;		
		}
	else
		{
			w=this->iconSize;
			h=this->iconList.size()*this->iconSize;
		}

	switch(this->gravity)
		{
			case N:
				x=(mons[this->onMonitor].x+(mons[this->onMonitor].w)/2)-((this->iconList.size()*this->iconSize)/2);
				y=mons[this->onMonitor].y;
				break;
			case S:
				x=(mons[this->onMonitor].x+(mons[this->onMonitor].w)/2)-((this->iconList.size()*this->iconSize)/2);
				y=mons[this->onMonitor].y+mons[this->onMonitor].h-this->iconSize;
				break;
			case E:
				x=mons[this->onMonitor].x+mons[this->onMonitor].w-this->iconSize;
				y=(mons[this->onMonitor].y+(mons[this->onMonitor].h)/2)-((this->iconList.size()*this->iconSize)/2);
				break;
			case W:
				x=mons[this->onMonitor].x;
				y=(mons[this->onMonitor].y+(mons[this->onMonitor].h)/2)-((this->iconList.size()*this->iconSize)/2);
				break;
			case NW:
				x=mons[this->onMonitor].x;
				y=mons[this->onMonitor].y;
				break;
			case NE:
				if(this->vertical==false)
					{
						x=mons[this->onMonitor].x+mons[this->onMonitor].w-this->iconList.size()*this->iconSize;
						y=mons[this->onMonitor].y;
					}
				else
					{
						x=mons[this->onMonitor].x+mons[this->onMonitor].w-this->iconSize;
						y=mons[this->onMonitor].y;
					}
				break;
			case SW:
				if(this->vertical==false)
					{
						x=mons[this->onMonitor].x;
						y=mons[this->onMonitor].y+mons[this->onMonitor].h-this->iconSize;
					}
				else
					{
						x=mons[this->onMonitor].x;
						y=mons[this->onMonitor].y+mons[this->onMonitor].h-this->iconList.size()*this->iconSize;
					}
				break;
			case SE:
				if(this->vertical==false)
					{
						x=mons[this->onMonitor].x+mons[this->onMonitor].w-this->iconList.size()*this->iconSize;
						y=mons[this->onMonitor].y+mons[this->onMonitor].h-this->iconSize;
					}
				else
					{
						x=mons[this->onMonitor].x+mons[this->onMonitor].w-this->iconSize;
						y=mons[this->onMonitor].y+mons[this->onMonitor].h-this->iconList.size()*this->iconSize;
					}
				break;
		}
	this->apc->mainWindow->LFSTK_moveResizeWindow(x,y,w,h,true);
	this->x=x;
	this->y=y;
}

void	 LFSTray_trayClass::setStacking(int stackit)
{
	this->apc->mainWindow->LFSTK_setKeepAbove(false);
	this->apc->mainWindow->LFSTK_setKeepBelow(false);
	switch(stackit)
		{
			case ABOVEALL:
				this->apc->mainWindow->LFSTK_setKeepAbove(true);
				break;
			case BELOWALL:
				this->apc->mainWindow->LFSTK_setKeepBelow(true);
				break;
			case NORMAL:
				this->apc->mainWindow->LFSTK_setKeepAbove(false);
				this->apc->mainWindow->LFSTK_setKeepBelow(false);
			break;
		}
}
