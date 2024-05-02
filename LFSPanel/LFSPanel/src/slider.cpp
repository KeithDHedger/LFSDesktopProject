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
#include <alsa/asoundlib.h>

#include "globals.h"

LFSTK_scrollBarClass	*vsb=NULL;
LFSTK_windowClass	*scwindow=NULL;
bool					windowVisible=false;
LFSTK_toggleButtonClass	*volumeButton;
std::string			iconH;
std::string			iconM;
std::string			iconL;
std::string			iconZ;
int					oldVolVal=-1;
char					label[32];

void setLabel(void)
{
	int	value=(int)(((double)vsb->LFSTK_getValue()/64.0)*100.0);
	sprintf(label,"Vol %i%%",value);
	volumeButton->LFSTK_setLabel(label);
	setIcon();
	volumeButton->LFSTK_clearWindow();
}

int getAlsaVolume(bool setvol,int volume)
{
	long					value=-1;
	snd_mixer_t				*handle;
	snd_mixer_selem_id_t	*sid;
	snd_mixer_elem_t		*elem;
	const char				*card="default";
	const char				*selem_name="Master";

	snd_mixer_open(&handle,0);
	snd_mixer_attach(handle,card);
	snd_mixer_selem_register(handle,NULL,NULL);
	snd_mixer_load(handle);

	snd_mixer_selem_id_malloc(&sid);
	snd_mixer_selem_id_set_index(sid,0);
	snd_mixer_selem_id_set_name(sid,selem_name);
	elem=snd_mixer_find_selem(handle,sid);

	if(setvol==true)
	   snd_mixer_selem_set_playback_volume_all(elem,volume);
	else
		snd_mixer_selem_get_playback_volume(elem,SND_MIXER_SCHN_UNKNOWN,&value);

	snd_mixer_detach(handle,card);
	snd_mixer_close(handle);
	snd_mixer_selem_id_free(sid);

	return(value);
}

void setIcon(void)
{
	int vol=vsb->LFSTK_getValue();

	if((iconZ.length()>0) && (vol<16))
		volumeButton->LFSTK_setImageFromPath(iconZ,TOOLBAR,true);

	if((iconL.length()>0) && (vol>=16) && (vol<32))
		volumeButton->LFSTK_setImageFromPath(iconL,TOOLBAR,true);

	if((iconM.length()>0) && (vol>=32) && (vol<48))
		volumeButton->LFSTK_setImageFromPath(iconM,TOOLBAR,true);

	if((iconH.length()>0) && (vol>=48))
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
					bc->LFSTK_getGeomWindowRelative(&geom,apc->rootWindow);
					switch(panelGravity)
						{
							case PANELNORTH:
								scwindow->LFSTK_moveWindow(geom.x,geom.y+geom.h,true);
								break;
							case PANELSOUTH:
								scwindow->LFSTK_moveWindow(geom.x,geom.y-SCROLLBARWIDTH,true);
								break;
							case PANELEAST:
								scwindow->LFSTK_moveWindow(geom.x-SCROLLBARWIDTH,geom.y,true);
								break;
							case PANELWEST:
								scwindow->LFSTK_moveWindow(geom.x+geom.w,geom.y,true);
								break;
						}
					scwindow->LFSTK_setKeepAbove(true);
					scwindow->LFSTK_showWindow(true);
					scwindow->LFSTK_redrawAllGadgets();
					apc->windows->at(apc->LFSTK_findWindow(scwindow)).showing=true;
				}
			else
				{
					scwindow->LFSTK_hideWindow();
					apc->windows->at(apc->LFSTK_findWindow(scwindow)).showing=false;
				}
			bc->LFSTK_clearWindow();
		}
	return(true);
}

bool valChanged(void *p,void* ud)
{
	LFSTK_scrollBarClass	*sb=NULL;
	char					*command;
	char					*vol;
	int						volume=-1;

	if(p!=NULL)
		{
			sb=static_cast<LFSTK_scrollBarClass*>(p);
			if(sb!=NULL)
				{
					getAlsaVolume(true,sb->LFSTK_getValue());
					setLabel();
				}
		}
	return(true);
}

void updateSlider(void)
{
	int		volume;

	volume=getAlsaVolume(false,-1);
	if(oldVolVal!=volume)
		{
			vsb->LFSTK_setValue(volume);
			oldVolVal=vsb->LFSTK_getValue();
			setLabel();
		}							
}

int addSlider(int x,int y,int grav,bool fromleft)
{
	int					xpos=x;
	int					ypos=y;
	int					width=0;
	int					height=0;
	int					thisgrav=grav;
	int					iconsize=16;
	windowInitStruct		*win;
	int					w,h;
	char					*vol;
	bool					direction=false;
	char					*label=strdup(mainwind->globalLib->LFSTK_oneLiner("amixer get Master|tail -n1|awk '{print \"%s \" $4}'|tr -d '[]'",SLIDERLABEL).c_str());//TODO//
	getAlsaVolume(false,-1);
	setSizes(&xpos,&ypos,&width,&height,&iconsize,&thisgrav,fromleft);
	
	volumeButton=new LFSTK_toggleButtonClass(mainwind,label,xpos,ypos,width,height,thisgrav);
	volumeButton->LFSTK_setToggleStyle(TOGGLENORMAL);
	volumeButton->LFSTK_setMouseCallBack(NULL,sliderCB,(void*)volumeButton->LFSTK_getLabel().c_str());

	setGadgetDetails(volumeButton);//TODO//

	iconH=mainwind->globalLib->LFSTK_findThemedIcon(desktopTheme,"volume-high","");
	iconM=mainwind->globalLib->LFSTK_findThemedIcon(desktopTheme,"volume-medium","");
	iconL=mainwind->globalLib->LFSTK_findThemedIcon(desktopTheme,"volume-low","");
	iconZ=mainwind->globalLib->LFSTK_findThemedIcon(desktopTheme,"volume-zero","");

	vol=strdup(mainwind->globalLib->LFSTK_oneLiner("amixer get Master|tail -n1|awk '{print $3}'").c_str());//TODO//
	
	win=new windowInitStruct;
	win->x=100;
	win->y=100;
	win->app=apc;
	win->windowType=apc->appAtomsHashed.at(LFSTK_UtilityClass::LFSTK_hashFromKey("_NET_WM_WINDOW_TYPE_TOOL"));

	if((panelGravity==PANELWEST) || (panelGravity==PANELEAST))
		{
			w=16;
			h=100;
			direction=true;
		}
	else
		{
			w=100;
			h=16;
			direction=false;
		}

	//win->x=100;
	//win->y=100;
	//w=w;
	//h=h;
	win->w=w;
	win->h=h;
	win->app=apc;
	apc->LFSTK_addToolWindow(win);
	scwindow=apc->windows->back().window;

//	win->w=w;
//	win->h=h;
//	apc->LFSTK_addToolWindow(win);
//	scwindow=apc->windows->back().window;

	vsb=new LFSTK_scrollBarClass(scwindow,direction,0,0,w,h,BUTTONGRAV);
	
	vsb->LFSTK_setMouseCallBack(NULL,valChanged,NULL);
	vsb->LFSTK_setScale(0,64);

	vsb->LFSTK_setValue(atoi(vol));
	vsb->reverse=direction;
	setIcon();
	free(vol);
	free(label);

	return(width);
}

