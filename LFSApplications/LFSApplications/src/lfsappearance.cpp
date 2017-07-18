/*
 *
 * ©K. D. Hedger. Wed 16 Sep 11:55:33 BST 2015 kdhedger68713@gmail.com

 * This file (lfsappearance) is part of LFSApplications.

 * LFSApplications is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * at your option) any later version.

 * LFSApplications is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

 * You should have received a copy of the GNU General Public License
 * along with LFSApplications.  If not, see <http://www.gnu.org/licenses/>.
 */


#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>

#include <lfstk/LFSTKGlobals.h>

enum {EXIT=0,APPLY,NEWGROUP,UPDATEGROUP,NOMOREBUTTONS};
enum {WALLPAPER=0,TOOLKIT,DESKTOP,WMANAGER,PANELPREFS,NOMORELAUNCHERS};
enum {LAUNCHWALLPAPER=100,LAUNCHTOOLKIT,LAUNCHDESKTOP,LAUNCHWMANAGER,LAUNCHPANEL};
LFSTK_windowClass	*wc;
LFSTK_lineEditClass	*current=NULL;
LFSTK_lineEditClass	*newgroup=NULL;
LFSTK_menuButtonClass *mb=NULL;
LFSTK_buttonClass	*guibc[NOMOREBUTTONS]={NULL,};
LFSTK_buttonClass	*launch[NOMORELAUNCHERS]={NULL,};
LFSTK_labelClass	*label[NOMORELAUNCHERS]={NULL,};
LFSTK_labelClass	*spacer=NULL;

int					bwidth=96;
int					bigbwidth=128;
int					spacing=bwidth+10;
int					col1=10,col2=col1+bwidth+spacing+20,col3=col2+bwidth+spacing+20,col4;
bool				mainloop=false;
int					maxGroups=20;
int					numGroups=0;
menuItemStruct		*groups;
char				currentBuffer[256];

#define BIG col2-col1+bwidth

int fileExists(const char *name)
{
	struct stat buffer;
	return (stat(name,&buffer));
}

void makeGroup(const char *grpname)
{
	char	*grp;
	char	*command;

	asprintf(&grp,"%s/.config/LFS/lfsgroupsprefs/%s",getenv("HOME"),grpname);
	if(fileExists(grp)!=0)
		mkdir(grp,0755);

	asprintf(&command,"cp %s/.config/LFS/lfs*.rc \"%s\"",getenv("HOME"),grp);
	system(command);

	free(command);
	free(grp);
}

void setGroup(void)
{
	char	*command;
	char	*grp;

	asprintf(&grp,"%s/.config/LFS/lfsgroupsprefs/%s",getenv("HOME"),current->LFSTK_getBuffer()->c_str());
	if(fileExists(grp)==0)
		{
			asprintf(&command,"cp \"%s/.config/LFS/lfsgroupsprefs/%s/\"lfs*.rc %s/.config/LFS",getenv("HOME"),current->LFSTK_getBuffer()->c_str(),getenv("HOME"));
			system(command);
			free(command);
		}
	free(grp);
	snprintf(currentBuffer,255,"%s",current->LFSTK_getBuffer()->c_str());

	asprintf(&command,"echo \"%s\" > %s/.config/LFS/lfsappearance.rc",currentBuffer,getenv("HOME"));
	system(command);
	free(command);

}

bool callback(void *p,void* ud)
{
	char	*command;

	if((long)ud==EXIT)
		{
			mainloop=false;
			return(false);
		}

	switch((long)ud)
		{
			case LAUNCHWALLPAPER:
				asprintf(&command,"lfsbackdropprefs --window=%i",wc->window);
				system(command);
				free(command);
				break;
			case LAUNCHTOOLKIT:
				asprintf(&command,"lfstkprefs --window=%i",wc->window);
				system(command);
				free(command);
				break;
			case LAUNCHDESKTOP:
				asprintf(&command,"lfsdesktopprefs --window=%i",wc->window);
				system(command);
				free(command);
				break;
			case LAUNCHWMANAGER:
				asprintf(&command,"lfswmprefs --window=%i",wc->window);
				system(command);
				free(command);
				break;
			case LAUNCHPANEL:
				asprintf(&command,"lfspanelprefs --window=%i",wc->window);
				system(command);
				free(command);
				break;

			case UPDATEGROUP:
				if(strlen(current->LFSTK_getBuffer()->c_str())>0)
					makeGroup(current->LFSTK_getBuffer()->c_str());
				break;

			case NEWGROUP:
				if(strlen(newgroup->LFSTK_getBuffer()->c_str())<1)
					return(true);
				groups[numGroups].label=(char*)strdup(newgroup->LFSTK_getBuffer()->c_str());
				groups[numGroups].userData=(void*)(long)(numGroups+1);
				groups[numGroups].bc=NULL;
				numGroups++;
				mb->LFSTK_addMenus(groups,numGroups);
				makeGroup(newgroup->LFSTK_getBuffer()->c_str());
				break;

			case APPLY:
				if(strlen(current->LFSTK_getBuffer()->c_str())>0)
					setGroup();
				system("lfsdesktop &>/dev/null &");
				system("lfssetwallpaper &>/dev/null");
				system("killall lfswmanager;sleep 1;nohup lfswmanager &>/dev/null &");
				system("killall lfspanel;sleep 1;nohup lfslaunchpanels &>/dev/null &");
				break;
		}

	wc->LFSTK_reloadGlobals();
	for(int j=WALLPAPER;j<NOMORELAUNCHERS;j++)
		{
			launch[j]->LFSTK_reloadColours();
			label[j]->LFSTK_reloadColours();
			label[j]->LFSTK_clearWindow();
		}
	for(int j=EXIT;j<NOMOREBUTTONS;j++)
		guibc[j]->LFSTK_reloadColours();
	mb->LFSTK_reloadColours();

	usleep(350000);
	wc->LFSTK_showWindow(false);
	return(true);
}

void loadGroups()
{
	char	*groupfolder;
	char	*command;
	FILE*	fp=NULL;
	char	buffer[2048];
	int		cnt=0;
	char	*ptr;

//init group array
	for(int j=0;j<maxGroups;j++)
		{
			groups[j].label=NULL;
			groups[j].userData=NULL;
			groups[j].bc=NULL;
			groups[j].subMenus=NULL;
			groups[j].subMenuCnt=0;
			groups[j].imagePath=NULL;
		}

	asprintf(&groupfolder,"%s/.config/LFS/lfsgroupsprefs",getenv("HOME"));
	if(fileExists(groupfolder)!=0)
		{
			mkdir(groupfolder,0755);
			makeGroup("Current Set");
		}

	asprintf(&command,"%s/Current Set",groupfolder);
	if(fileExists(command)!=0)
		makeGroup("Current Set");
	free(command);

	asprintf(&command,"find %s -mindepth 1 -maxdepth 1 -type d -printf \"%%f\\n\"",groupfolder);
	fp=popen(command,"r");
	if(fp!=NULL)
		{
			buffer[0]=0;
			while(fgets(buffer,2048,fp))
				{
					if(strlen(buffer)>0)
						{
							buffer[strlen(buffer)-1]=0;
							groups[cnt].label=strdup(buffer);
							groups[cnt].userData=(void*)(long)(cnt+1);
							if(strcmp(currentBuffer,buffer)==0)
								current->LFSTK_setBuffer(buffer);
							cnt++;
						}
				}
			pclose(fp);
		}
	numGroups=cnt;
	free(command);
	free(groupfolder);
}

bool bcb(void *p,void* ud)
{
	menuItemStruct	*menuitem=(menuItemStruct*)ud;

	if(menuitem==NULL)
		return(true);

	current->LFSTK_setBuffer(menuitem->label);
	current->LFSTK_clearWindow();
	return(true);
}

int main(int argc, char **argv)
{
	XEvent					event;
	int						sx=0;
	int						sy=0;
	const geometryStruct	*geom;
	int						bhite=24;
	int						vspacing=bhite+10;
	FILE*					fp=NULL;
	char					*command;
	Display					*display;

	asprintf(&command,"cat %s/.config/LFS/lfsappearance.rc",getenv("HOME"));
	fp=popen(command,"r");
	if(fp!=NULL)
		{
			currentBuffer[0]=0;
			fgets(currentBuffer,255,fp);
			if(strlen(currentBuffer)>0)
				currentBuffer[strlen(currentBuffer)-1]=0;
			pclose(fp);
		}
	free(command);

	wc=new LFSTK_windowClass(sx,sy,800,600,"LFS Appearance",false);
	display=wc->display;

	wc->LFSTK_setDecorated(true);
	wc->autoLabelColour=true;
	geom=wc->LFSTK_getWindowGeom();
	guibc[EXIT]=new LFSTK_buttonClass(wc,"Exit",10,geom->h-32,64,24,SouthWestGravity);
	guibc[EXIT]->LFSTK_setCallBack(NULL,callback,(void*)EXIT);

	guibc[APPLY]=new LFSTK_buttonClass(wc,"Apply",geom->w-74,geom->h-32,64,24,SouthEastGravity);
	guibc[APPLY]->LFSTK_setCallBack(NULL,callback,(void*)APPLY);

	guibc[UPDATEGROUP]=new LFSTK_buttonClass(wc,"Update",(geom->w/2)-(bwidth/2),geom->h-32,64,24,SouthGravity);
	guibc[UPDATEGROUP]->LFSTK_setCallBack(NULL,callback,(void*)UPDATEGROUP);

	sx=col1;
	sy=10;
//wallpaper
	launch[WALLPAPER]=new LFSTK_buttonClass(wc,"Wallpaper Prefs",sx,sy,bigbwidth,24,NorthWestGravity);
	launch[WALLPAPER]->LFSTK_setCallBack(NULL,callback,(void*)LAUNCHWALLPAPER);
	sx+=spacing+32;
	label[WALLPAPER]=new LFSTK_labelClass(wc,"Launch Wallpaper Prefs Dialog",sx,sy,BIG,24,NorthWestGravity);
//toolkit
	sx=col1;
	sy+=vspacing;
	launch[TOOLKIT]=new LFSTK_buttonClass(wc,"Toolkit Prefs",sx,sy,bigbwidth,24,NorthWestGravity);
	launch[TOOLKIT]->LFSTK_setCallBack(NULL,callback,(void*)LAUNCHTOOLKIT);
	sx+=spacing+32;
	label[TOOLKIT]=new LFSTK_labelClass(wc,"Launch Toolkit Prefs Dialog",sx,sy,BIG,24,NorthWestGravity);
//desktop
	sx=col1;
	sy+=vspacing;
	launch[DESKTOP]=new LFSTK_buttonClass(wc,"Desktop Prefs",sx,sy,bigbwidth,24,NorthWestGravity);
	launch[DESKTOP]->LFSTK_setCallBack(NULL,callback,(void*)LAUNCHDESKTOP);
	sx+=spacing+32;
	label[DESKTOP]=new LFSTK_labelClass(wc,"Launch Desktop Prefs Dialog",sx,sy,BIG,24,NorthWestGravity);
//wmanager
	sx=col1;
	sy+=vspacing;
	launch[WMANAGER]=new LFSTK_buttonClass(wc,"WM Prefs",sx,sy,bigbwidth,24,NorthWestGravity);
	launch[WMANAGER]->LFSTK_setCallBack(NULL,callback,(void*)LAUNCHWMANAGER);
	sx+=spacing+32;
	label[WMANAGER]=new LFSTK_labelClass(wc,"Launch Window Prefs Dialog",sx,sy,BIG,24,NorthWestGravity);
//panel prefs
	sx=col1;
	sy+=vspacing;
	launch[PANELPREFS]=new LFSTK_buttonClass(wc,"Panel Prefs",sx,sy,bigbwidth,24,NorthWestGravity);
	launch[PANELPREFS]->LFSTK_setCallBack(NULL,callback,(void*)LAUNCHPANEL);
	sx+=spacing+32;
	label[PANELPREFS]=new LFSTK_labelClass(wc,"Launch Panel Prefs Dialog",sx,sy,BIG,24,NorthWestGravity);

	sx=col1;
	sy+=vspacing;
	mb=new LFSTK_menuButtonClass(wc,"Load Set",sx,sy,bwidth,24,NorthWestGravity);
	mb->LFSTK_setStyle(BEVELOUT);
	mb->LFSTK_setLabelGravity(CENTRE);
	groups=new menuItemStruct[maxGroups];
	mb->LFSTK_setCallBack(NULL,bcb,NULL);
	
	sx+=spacing;;
	current=new LFSTK_lineEditClass(wc,"",sx,sy-1,BIG,24,NorthWestGravity);

	loadGroups();
	mb->LFSTK_addMenus(groups,numGroups);
	sy+=vspacing;
	
	sx=col1;
	guibc[NEWGROUP]=new LFSTK_buttonClass(wc,"New Set",sx,sy,bwidth,24,NorthWestGravity);
	guibc[NEWGROUP]->LFSTK_setCallBack(NULL,callback,(void*)NEWGROUP);
	sx+=spacing;;
	newgroup=new LFSTK_lineEditClass(wc,"",sx,sy-1,BIG,24,NorthWestGravity);

	sy+=vspacing;
	spacer=new LFSTK_labelClass(wc,"--",0,sy,col3,8,NorthWestGravity);
	sy+=16;
	sy+=vspacing;
	wc->LFSTK_resizeWindow(col3-10,sy);
	wc->LFSTK_showWindow();
	wc->LFSTK_setKeepAbove(true);

	XFlush(wc->display);
	XSync(wc->display,false);

	mainloop=true;
	while(mainloop==true)
		{
			XNextEvent(wc->display,&event);
			mappedListener *ml=wc->LFSTK_getMappedListener(event.xany.window);
			if(ml!=NULL)
				ml->function(ml->gadget,&event,ml->type);
			switch(event.type)
				{
						break;
					case Expose:
						wc->LFSTK_clearWindow();
						XSetInputFocus(wc->display,wc->window,RevertToPointerRoot,CurrentTime);
						break;
					case ConfigureNotify:
						wc->LFSTK_resizeWindow(event.xconfigurerequest.width,event.xconfigurerequest.height,false);
						break;
					case ClientMessage:
					case SelectionNotify:
						if (event.xclient.message_type == XInternAtom(wc->display, "WM_PROTOCOLS", 1) && (Atom)event.xclient.data.l[0] == XInternAtom(wc->display, "WM_DELETE_WINDOW", 1))
							mainloop=false;
						if(wc->acceptDnd==true)
							{
								wc->LFSTK_handleDnD(&event);
							}
					default:
						break;
				}
		}

	wc->LFSTK_hideWindow();
	for(int j=0;j<maxGroups;j++)
		if(groups[j].label!=NULL)
			free((char*)groups[j].label);

	delete wc;
	XCloseDisplay(display);

	return(0);
}
