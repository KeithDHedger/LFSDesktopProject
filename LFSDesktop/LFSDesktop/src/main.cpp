
/*
 *
 * ©K. D. Hedger. Wed Jan 10 11:27:16 GMT 2018 keithdhedger@gmail.com

 * This file (main.cpp) is part of LFSDesktop.

 * LFSDesktop is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * at your option) any later version.

 * LFSDesktop is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

 * You should have received a copy of the GNU General Public License
 * along with LFSDesktop.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <sys/msg.h>
#include "config.h"
#include "globals.h"

#define UNKNOWNARG -100
//TODO//
struct option long_options[] =
{
	{"key",1,0,'k'},
	{"clean",0,0,'c'},
	{"tidy",0,0,'t'},
	{"theme",1,0,'a'},
	{"term-command",1,0,'x'},
	{"fore-colour",1,0,'4'},
	{"back-colour",1,0,'b'},
	{"back-alpha",1,0,'A'},
	{"ignore",1,0,'i'},
	{"debug",0,0,'d'},
	{"show-extension",0,0,'s'},
	{"font",1,0,'f'},
	{"version",0,0,'v'},
	{"help",0,0,'?'},
	{0, 0, 0, 0}
};

const char			*deskItemLabelData[]={"Open","Execute","Custom Icon","Remove Icon",NULL};
const char			*diskLabelData[]={"Mount","Unmount","Eject","Open","Custom Icon","Remove Icon",NULL};
const char			*diskThemeIconData[]={"drive-harddisk","media-eject","media-eject","document-open","list-add","list-remove"};

//msg coms
int					queueID;
msgBuffer			buffer;

void readMsg(void)
{
	int		retcode;
	char	*command;
	FILE	*fd=NULL;
	char	buff[2048];

	buff[0]=0;
	retcode=msgrcv(queueID,&buffer,MAX_MSG_SIZE,DESKTOP_MSG,IPC_NOWAIT);
	if(retcode>0)
		{
			if(strcmp(buffer.mText,"reloaddesk")==0)
				{
					wc->LFSTK_setWindowPixmap(apc->globalLib->LFSTK_getWindowPixmap(apc->display,apc->rootWindow),apc->displayWidth,apc->displayHeight,true);
					wc->LFSTK_clearWindow(true);
				}

			if(strcmp(buffer.mText,"reloadprefs")==0)
				reloadPrefs();

			if(strcmp(buffer.mText,"cleandesktopcache")==0)
				{
					asprintf(&command,"find %s -maxdepth 1 -mindepth 1 |sort",cachePath);
					fd=popen(command,"r");
					free(command);
					if(fd!=NULL)
						{
							while(feof(fd)==0)
								{
									bool flag=false;
									buff[0]=0;
									fgets(buff,2048,fd);
									if(strlen(buff)>0)
										{
											buff[strlen(buff)-1]=0;
											for(unsigned j=0;j<desktopItems.size();j++)
												{
													if(strcmp(desktopItems.at(j).uuid,basename(buff))==0)
														{
															flag=true;
															break;
														}
												}
											if(flag==false)
												unlink(buff);
											else
												continue;
										}
								}
							pclose(fd);
						}
				}
		}
	buffer.mText[0]=0;
}

void printhelp(void)//TODO//
{
	printf("Usage: lfsdesktop [OPTION]\n"
			"A CLI application\n"
			" -k, --key	Sytem V msg key\n"
			" -v, --version	output version information and exit\n"
			" -h, -?, --help	print this help\n\n"
			"Report bugs to kdhedger@yahoo.co.uk\n"
	      );
}

bool doQuit(void *p,void* ud)
{
	apc->exitValue=0;
	apc->mainLoop=false;
	return(false);
}

int main(int argc, char **argv)
{
	char				*command;
	int					sy=0;
	char				*iconpath=NULL;
	LFSTK_buttonClass	*button;
	int					key=666;

	apc=new LFSTK_applicationClass();

	command=apc->globalLib->LFSTK_oneLiner("sed -n '2p' %s/lfsappearance.rc",apc->configDir);
	key=atoi(command);
	freeAndNull(&command);

	if((queueID=msgget(key,IPC_CREAT|0660))==-1)
		fprintf(stderr,"Can't create message queue\n");

	udev=udev_new();
	windowInitStruct *wi=new windowInitStruct;
	wi->windowType="_NET_WM_DESKTOP";
	wi->overRide=true;
	wi->level=BELOWALL;
	wi->decorated=false;

	apc->LFSTK_addWindow(wi,"Desktop");
	wc=apc->mainWindow;
	wc->passEventToRoot=true;

//TODO//debug to go
#ifdef _ENABLEDEBUG_
	button=new LFSTK_buttonClass(wc,"quit",0,0,GADGETWIDTH,GADGETHITE);//TODO//
	button->LFSTK_setMouseCallBack(NULL,doQuit,NULL);
#endif
apc->globalLib->LFSTK_setUseTheme(false);
	wc->LFSTK_setWindowPixmap(apc->globalLib->LFSTK_getWindowPixmap(apc->display,apc->rootWindow),apc->displayWidth,apc->displayHeight);
	asprintf(&cachePath,"%s/lfsdesktop/cache",apc->configDir);
	asprintf(&command,"mkdir -p %s 2>&1 >/dev/null",cachePath);
	system(command);
	free(command);

	asprintf(&desktopPath,"%s/Desktop",getenv("HOME"));
	mkdir(desktopPath,0755);
	asprintf(&documentsPath,"%s/Documents",getenv("HOME"));
	mkdir(documentsPath,0755);

	asprintf(&prefsPath,"%s/lfsdesktop.rc",apc->configDir);

	loadPrefs();

	apc->LFSTK_setTimer(refreshRate);
	apc->LFSTK_setTimerCallBack(timerCB,NULL);

	if(gridSize<1)
		gridSize=1;

	windowInitStruct	*win;

	win=new windowInitStruct;
	win->app=apc;
	win->name="";
	win->loadVars=true;
	win->wc=wc;
	win->windowType="_NET_WM_WINDOW_TYPE_MENU";
	win->decorated=false;
	win->overRide=true;
	win->level=ABOVEALL;

	sy=0;
	apc->LFSTK_addWindow(win,NULL);
	fileWindow=apc->windows->back().window;
	for(int j=DESKITEMOPEN;j<NOMOREDESKITEMBUTONS;j++)
		{
			button=new LFSTK_buttonClass(fileWindow,deskItemLabelData[j],0,sy,GADGETWIDTH*2,24);//TODO//
			button->LFSTK_setMouseCallBack(NULL,doDeskItemMenuSelect,(void*)(long)(j));
			iconpath=apc->globalLib->LFSTK_findThemedIcon(iconTheme,diskThemeIconData[j],"");
			button->LFSTK_setImageFromPath(iconpath,LEFT,true);
			freeAndNull(&iconpath);
			sy+=GADGETHITE;
		}
	fileWindow->LFSTK_resizeWindow(GADGETWIDTH+16,sy,true);

	win=new windowInitStruct;
	win->app=apc;
	win->name="";
	win->loadVars=true;
	win->wc=wc;
	win->windowType="_NET_WM_WINDOW_TYPE_MENU";
	win->decorated=false;
	win->overRide=true;
	win->level=ABOVEALL;

	sy=0;
	apc->LFSTK_addWindow(win,NULL);
	diskWindow=apc->windows->back().window;
	for(int j=MOUNTDISK;j<NOMOREBUTONS;j++)
		{
			button=new LFSTK_buttonClass(diskWindow,diskLabelData[j],0,sy,GADGETWIDTH+24,24);//TODO//
			button->LFSTK_setMouseCallBack(NULL,doDiskMenuSelect,(void*)(long)(j));
			iconpath=apc->globalLib->LFSTK_findThemedIcon(iconTheme,diskThemeIconData[j],"");
			button->LFSTK_setImageFromPath(iconpath,LEFT,true);
			sy+=GADGETHITE;
			freeAndNull(&iconpath);
		}
	diskWindow->LFSTK_resizeWindow(GADGETWIDTH+16,sy,true);

	makeDiskButtons();
	makeFileButtons(false);
	updateMounted();
	diskWatch=apc->globalLib->LFSTK_oneLiner("%s","ls -1 /dev/disk/by-partuuid|md5sum|awk '{print $1}'");
	desktopWatch=apc->globalLib->LFSTK_oneLiner("ls -1 '%s'|md5sum|awk '{print $1}'",desktopPath);
	wc->LFSTK_initDnD(true);
	wc->LFSTK_setWindowDropCallBack(windowDrop,(void*)0xdeadbeef);
	wc->LFSTK_showWindow(true);
	wc->LFSTK_resizeWindow(apc->displayWidth,apc->displayHeight,true);

	int retval=apc->LFSTK_runApp();

	free(documentsPath);
	free(homePath);
	free(diskWatch);
	free(desktopWatch);
	free(cachePath);
	free(prefsPath);
	free(desktopPath);
	udev_unref(udev);
	for(unsigned j=0;j<desktopItems.size();j++)
		{
			if(desktopItems.at(j).uuid!=NULL)
				free(desktopItems.at(j).uuid);
			if(desktopItems.at(j).itemPath!=NULL)
				free(desktopItems.at(j).itemPath);
			if(desktopItems.at(j).label!=NULL)
				free(desktopItems.at(j).label);
			if(desktopItems.at(j).iconPath!=NULL)
				free(desktopItems.at(j).iconPath);
		}
	desktopItems.clear();

	delete apc;
	return(retval);
}
