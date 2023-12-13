/*
 *
 * ©K. D. Hedger. Sun 19 Nov 19:09:57 GMT 2023 keithdhedger@gmail.com

 * This file (slider.cpp) is part of LFSDock.

 * LFSDock is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.

 * LFSDock is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

 * You should have received a copy of the GNU General Public License
 * along with LFSDock.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <alsa/asoundlib.h>

#include "globals.h"

LFSTK_scrollBarClass		*vsb=NULL;
LFSTK_windowClass		*scwindow=NULL;
bool						windowVisible=false;
LFSTK_toggleButtonClass	*volumeButton;
char						*iconH=NULL;
char						*iconM=NULL;
char						*iconL=NULL;
char						*iconZ=NULL;
int						oldVolVal=-1;
char						label[32];
bool						sliderIsUp=false;
int iconSize=48;

void setLabel(void)
{
	int	value=(int)(((double)vsb->LFSTK_getValue()/64.0)*100.0);
	sprintf(label,"Vol %i%%",value);
	volumeButton->LFSTK_setLabel((const char*)label);
	setIcon();
}

int getAlsaVolume(bool setvol,int volume)
{
	long					value=-1;
	snd_mixer_t			*handle;
	snd_mixer_selem_id_t	*sid;
	snd_mixer_elem_t		*elem;
	const char			*card="default";
	const char			*selem_name="Master";

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

	if(vol<16)
		volumeButton->LFSTK_setImageFromPath(iconZ,TOOLBAR,true);

	if((vol>=16) && (vol<32))
		volumeButton->LFSTK_setImageFromPath(iconL,TOOLBAR,true);

	if((vol>=32) && (vol<48))
		volumeButton->LFSTK_setImageFromPath(iconM,TOOLBAR,true);

	if(vol>=48)
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
					switch(dockGravity)
						{
							case PANELNORTH:
								scwindow->LFSTK_moveWindow(geom.x+(geom.w/2)-(SLIDERWIDTH/2),geom.y+geom.h,true);
								break;
							case PANELSOUTH:
								scwindow->LFSTK_moveWindow(geom.x+(geom.w/2)-(SLIDERWIDTH/2),geom.y-SCROLLBARWIDTH,true);
								break;
						}
					scwindow->LFSTK_showWindow(true);
					scwindow->LFSTK_redrawAllGadgets();
					apc->windows->at(apc->LFSTK_findWindow(scwindow)).showing=true;
				}
			else
				{
					scwindow->LFSTK_hideWindow();
					apc->windows->at(apc->LFSTK_findWindow(scwindow)).showing=false;
				}
		}
	return(true);
}

bool valChanged(void *p,void* ud)
{
	LFSTK_scrollBarClass	*sb=NULL;
	char					*command;
	char					*vol;
	int					volume=-1;

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
			volumeButton->LFSTK_clearWindow();
			XFlush(apc->display);
		}							
}

bool volExitCB(LFSTK_gadgetClass*p,void* ud)
{
	geometryStruct	geom2;

	if(volumeButton->LFSTK_getValue()==1)
		return(true);
	p->LFSTK_getGeom(&geom2);	
	p->LFSTK_moveGadget(geom2.x,normalY+extraSpace);
	sliderIsUp=false;
	return(true);
}

bool volMoveCB(LFSTK_gadgetClass*p,void* ud)
{
	geometryStruct	geom;

	if(sliderIsUp==true)
		return(true);

	p->LFSTK_getGeom(&geom);	
	p->LFSTK_moveGadget(geom.x,activeY+extraSpace);
	sliderIsUp=true;
	return(true);
}

int addSlider(int x,int y,int grav)
{
	char				*vol=dockWindow->globalLib->LFSTK_oneLiner("amixer get Master|tail -n1|awk '{print $3}'");
	char				*label=dockWindow->globalLib->LFSTK_oneLiner("amixer get Master|tail -n1|awk '{print \"%s \" $4}'|tr -d '[]'",SLIDERLABEL);//TODO//
	windowInitStruct	*win=new windowInitStruct;;
	int				w,h;
	bool				direction=false;

	getAlsaVolume(false,-1);
	
	volumeButton=new LFSTK_toggleButtonClass(dockWindow,label,x,y+extraSpace,iconSize,iconSize);
	volumeButton->LFSTK_setToggleStyle(TOGGLENORMAL);
	volumeButton->LFSTK_setMouseCallBack(NULL,sliderCB,(void*)volumeButton->LFSTK_getLabel());
	volumeButton->LFSTK_setMouseMoveCallBack(volMoveCB,volExitCB,USERDATA(0));

	setGadgetDetails(volumeButton);
	volumeButton->LFSTK_setAlpha(1.0);
	volumeButton->LFSTK_setStyle(BEVELNONE);

	volumeButton->LFSTK_setGadgetColours(GADGETBG,dockBGColour,dockBGColour,dockBGColour,dockBGColour);
	volumeButton->LFSTK_setGadgetColours(GADGETFG,dockTextColour,dockTextColour,dockTextColour,dockTextColour);

	volumeButton->drawLabelBG=true;
	volumeButton->autoLabelBGColour=true;

	iconH=dockWindow->globalLib->LFSTK_findThemedIcon(desktopTheme,"volume-high","");
	iconM=dockWindow->globalLib->LFSTK_findThemedIcon(desktopTheme,"volume-medium","");
	iconL=dockWindow->globalLib->LFSTK_findThemedIcon(desktopTheme,"volume-low","");
	iconZ=dockWindow->globalLib->LFSTK_findThemedIcon(desktopTheme,"volume-zero","");

	win->x=100;
	win->y=100;
	w=100;
	h=16;
	win->w=w;
	win->h=h;
	win->app=apc;
	apc->LFSTK_addToolWindow(win);
	scwindow=apc->windows->back().window;

	vsb=new LFSTK_scrollBarClass(scwindow,direction,0,0,w,h,BUTTONGRAV);
	
	vsb->LFSTK_setMouseCallBack(NULL,valChanged,NULL);
	vsb->LFSTK_setScale(0,64);
	vsb->LFSTK_setValue(atoi(vol));
	vsb->reverse=direction;
	volumeButton->LFSTK_setImageFromPath(iconL,TOOLBAR,true);
	volumeButton->LFSTK_clearWindow();
	freeAndNull(&vol);
	freeAndNull(&label);

	return(iconSize);
}

