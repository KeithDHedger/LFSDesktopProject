/*
 *
 * ©K. D. Hedger. Thu  3 Jan 12:11:08 GMT 2019 keithdhedger@gmail.com

 * This file (slider.cpp) is part of LFSPanel.

 * LFSPanel is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * at your option) any later version.

 * LFSPanel is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

 * You should have received a copy of the GNU General Public License
 * along with LFSPanel.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>

#include "slider.h"

LFSTK_scrollBarClass	*vsb=NULL;
LFSTK_toolWindowClass	*scwindow=NULL;
bool					windowVisible=false;
LFSTK_toggleButtonClass	*volumeButton;
char					*iconH=NULL;
char					*iconM=NULL;
char					*iconL=NULL;

void setIcon(void)
{
	int vol=vsb->LFSTK_getValue();
	if(vol<21)
		volumeButton->LFSTK_setImageFromPath(iconL,TOOLBAR,true);
	if((vol>21) && (vol<41))
		volumeButton->LFSTK_setImageFromPath(iconM,TOOLBAR,true);
	if(vol>42)
		volumeButton->LFSTK_setImageFromPath(iconH,TOOLBAR,true);
}

bool sliderCB(void *p,void* ud)
{
	LFSTK_toggleButtonClass	*bc=static_cast<LFSTK_toggleButtonClass*>(p);
	geometryStruct			geom;
	int						x,y;
	Window					dw;

	if(p!=NULL)
		{
			if(bc->LFSTK_getValue()==1)
				{
					bc->LFSTK_getGeom(&geom);
					XTranslateCoordinates(bc->display,bc->window,bc->rootWindow,geom.x,geom.y,&x,&y,&dw);
					switch(panelGravity)
						{
							case PANELNORTH:
								scwindow->LFSTK_moveWindow(x-geom.x,y+geom.h,true);
								break;
							case PANELSOUTH:
								scwindow->LFSTK_moveWindow(x-geom.x,y-SCROLLBARWIDTH,true);
								break;
							case PANELEAST:
								scwindow->LFSTK_moveWindow(x-SCROLLBARWIDTH,y-geom.y,true);
								break;
							case PANELWEST:
								scwindow->LFSTK_moveWindow(x+geom.w,y-geom.y,true);
								break;
						}
					scwindow->LFSTK_showWindow(true);
					windowVisible=true;
				}
			else
				{
					scwindow->LFSTK_hideWindow();
					windowVisible=false;
				}
		}
	return(true);
}

bool valChanged(void *p,void* ud)
{
	LFSTK_scrollBarClass	*sb=NULL;
	char					*command;
	char					*vol;
	if(p!=NULL)
		{
			sb=static_cast<LFSTK_scrollBarClass*>(p);
			if(sb!=NULL)
				{
					asprintf(&command,"amixer set Master %i &>/dev/null",sb->LFSTK_getValue());
					system(command);
					free(command);
					vol=mainwind->globalLib->LFSTK_oneLiner("amixer get Master|tail -n1|awk '{print \"%s \" $4}'|tr -d '[]'",SLIDERLABEL);
					volumeButton->LFSTK_setLabel(vol);
					free(vol);
					volumeButton->LFSTK_clearWindow();
				}
		}
	return(true);
}

int addSlider(int x,int y,int grav,bool fromleft)
{
	int						xpos=x;
	int						ypos=y;
	int						width=0;
	int						height=0;
	int						thisgrav=grav;
	int						iconsize=16;
	char					*label=mainwind->globalLib->LFSTK_oneLiner("amixer get Master|tail -n1|awk '{print \"%s \" $4}'|tr -d '[]'",SLIDERLABEL);

	setSizes(&xpos,&ypos,&width,&height,&iconsize,&thisgrav,fromleft);
	
	volumeButton=new LFSTK_toggleButtonClass(mainwind,label,xpos,ypos,width,height,thisgrav);
	volumeButton->LFSTK_setToggleStyle(TOGGLENORMAL);
	volumeButton->LFSTK_setCallBack(NULL,sliderCB,(void*)volumeButton->LFSTK_getLabel());

	iconH=mainwind->globalLib->LFSTK_findThemedIcon(desktopTheme,"volume-high","");
	iconM=mainwind->globalLib->LFSTK_findThemedIcon(desktopTheme,"volume-medium","");
	iconL=mainwind->globalLib->LFSTK_findThemedIcon(desktopTheme,"volume-low","");

	char	*vol=mainwind->globalLib->LFSTK_oneLiner("amixer get Master|tail -n1|awk '{print $3}'");

	if((panelGravity==PANELWEST) || (panelGravity==PANELEAST))
		{
			scwindow=new LFSTK_toolWindowClass(mainwind->display,mainwind,"_NET_WM_WINDOW_TYPE_MENU",0,0,SCROLLBARWIDTH,SLIDERWIDTH,"VOL");
			vsb=new LFSTK_scrollBarClass(scwindow,true,0,0,SCROLLBARWIDTH,SLIDERWIDTH,BUTTONGRAV);
			vsb->reverse=true;
		}
	else
		{
			scwindow=new LFSTK_toolWindowClass(mainwind->display,mainwind,"_NET_WM_WINDOW_TYPE_MENU",0,0,SLIDERWIDTH,SCROLLBARWIDTH,"VOL");
			vsb=new LFSTK_scrollBarClass(scwindow,false,0,0,SLIDERWIDTH,SCROLLBARWIDTH,BUTTONGRAV);
			vsb->reverse=false;
		}

	vsb->LFSTK_setCallBack(NULL,valChanged,NULL);
	vsb->LFSTK_setScale(0,64);
	vsb->LFSTK_setValue(atoi(vol));

	setIcon();
	free(vol);
	free(label);

	return(width);
}

