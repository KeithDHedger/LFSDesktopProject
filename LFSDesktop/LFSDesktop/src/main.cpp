
/*
 *
 * ©K. D. Hedger. Wed Jan 10 11:27:16 GMT 2018 kdhedger68713@gmail.com

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
#define MAX_MSG_SIZE 256

struct option long_options[] =
{
	{"key",1,0,'k'},
	{"version",0,0,'v'},
	{"help",0,0,'?'},
	{0, 0, 0, 0}
};


struct msgBuffer
{
	long	mType;
	char	mText[MAX_MSG_SIZE];
};

const char			*diskLabelData[]={"Mount","Unmount","Eject","Open","Custom Icon","Remove Icon",NULL};
const char			*diskThemeIconData[]={"drive-harddisk","media-eject","media-eject","document-open","list-add","list-remove"};

LFSTK_labelClass	*label;

bool				needsRefresh=true;
bool				isdragging=false;
pollfd				polldisks;
int					fhfordisks;
pollfd				polldesktop;
int					fhfordesktop;

//msg coms
int					queueID;
msgBuffer			buffer;
bool				reloadPixmap=false;
bool				reloadPrefsFlag=false;
bool				reloadDeskFlag=false;
void readMsg(void)
{
	int retcode;

	retcode=msgrcv(queueID,&buffer,MAX_MSG_SIZE,1,IPC_NOWAIT);

	if(retcode>1)
		{
			if(strcmp(buffer.mText,"reloadbg")==0)
				reloadPixmap=true;
			if(strcmp(buffer.mText,"reloadprefs")==0)
				reloadPrefsFlag=true;
			if(strcmp(buffer.mText,"reloaddesk")==0)
				{
					mainLoop=false;
					reloadDeskFlag=true;
				}
		}
	buffer.mText[0]=0;
}

void printhelp(void)
{
	printf("Usage: lfsdesktop [OPTION]\n"
			"A CLI application\n"
			" -k, --key	Sytem V msg key\n"
			" -v, --version	output version information and exit\n"
			" -h, -?, --help	print this help\n\n"
			"Report bugs to kdhedger@yahoo.co.uk\n"
	      );
}

void doRefresh(void)
{
	long				numRead=0;
	int					ret;
	char				buffer[EVENT_BUF_LEN]={0,};

//refresh disks
	updateDisks();
	ret=poll(&polldisks,POLLIN,20);		
	if(ret!=0)
		{
			numRead=read(fhfordisks,buffer,MAXBUFFER);
			if(numRead>0)
				{
					loadDisks();
					wc->LFSTK_showWindow();
				}
		}

//refresh desktop folder
	ret=poll(&polldesktop,POLLIN,20);		
	if(ret!=0)
		{
			numRead=read(fhfordesktop,buffer,EVENT_BUF_LEN);
			if(numRead>0)
				{
					int cnt=0;
					while(cnt<numRead)
						{
							inotify_event *notifyevent=(inotify_event*)&buffer[cnt];
							if(notifyevent->len>0)
								{
									if(notifyevent->mask & IN_CREATE)
										addDeskItem(notifyevent->name);
									if(notifyevent->mask & IN_DELETE)
										deleteDeskItem(notifyevent->name);
									if(notifyevent->mask & IN_MOVED_FROM)
										deleteDeskItem(notifyevent->name);
										
									if(notifyevent->mask & IN_MOVED_TO)
										addDeskItem(notifyevent->name);
								}
							cnt+=sizeof(inotify_event)+notifyevent->len;
						}
					wc->LFSTK_showWindow();
				}
		}
	needsRefresh=false;
}

void  alarmCallBack(int sig)
{
	XExposeEvent	event;

	if(isdragging==false)
		{
			needsRefresh=true;
			event.type=Expose;
			event.window=wc->window;
			readMsg();
			XSendEvent(wc->display,wc->window,false,ExposureMask,(XEvent*)&event);
			XFlush(wc->display);
			//XSync(wc->display,true);
		}
	alarm(refreshRate);
}

void createDirDesktops(void)
{
	const char	*homedesk="[Desktop Entry]\nName=Home\nExec=xdg-open ~\nIcon=user-home";
	const char	*compdesk="[Desktop Entry]\nName=Computer\nExec=xdg-open /\nIcon=computer";
	char		*filepath;
	FILE		*f;

	filepath=(char*)alloca(strlen(desktopPath) + strlen(".computer.desktop") + 1);

	sprintf(filepath,"%s/.home.desktop",desktopPath);
	f=fopen(filepath,"w");
	if(f != NULL)
		{
			fprintf(f,"%s",homedesk);
			fclose(f);
		}

	sprintf(filepath,"%s/.computer.desktop",desktopPath);
	f=fopen(filepath,"w");
	if(f != NULL)
		{
			fprintf(f,"%s",compdesk);
			fclose(f);
		}
}

int main(int argc, char **argv)
{
	int					c;
	XEvent				event;
	char				*command;
	char				*iconpath=NULL;
	int					sy;
	LFSTK_buttonClass	*bc;

	while(1)
		{
			int option_index=0;
			c = getopt_long (argc, argv, "v?h:l:",long_options, &option_index);
			if (c == -1)
				break;

			switch (c)
				{
				case 'l':
					printf("Arg=%s\n",optarg);
					break;

				case 'v':
					printf("lfsdesktop %s\n",VERSION);
					return 0;
					break;

				case '?':
				case 'h':
					printhelp();
					return 0;
					break;

				default:
					fprintf(stderr,"?? Unknown argument ??\n");
					return UNKNOWNARG;
					break;
				}
		}

	if (optind < argc)
		{
			printf("non-option ARGV-elements: ");
			while (optind < argc)
				printf("%s ",argv[optind++]);
			printf("\n");
		}

	int key=666;

	if((queueID=msgget(key,IPC_CREAT|0660))==-1)
		fprintf(stderr,"Can't create message queue\n");

BACKUP:
	windowInitStruct *wi=new windowInitStruct;
	wi->windowType="_NET_WM_WINDOW_TYPE_DESKTOP";
	wi->overRide=true;
	wi->level=BELOWALL;
	wc=new LFSTK_windowClass(wi);
	display=wc->display;
	wc->LFSTK_moveWindow(0,0,true);

	wc->LFSTK_setWindowPixmap(wc->globalLib->LFSTK_getWindowPixmap(display,wc->rootWindow),DisplayWidth(display,wc->screen),DisplayHeight(display,wc->screen));

	wc->LFSTK_resizeWindow(DisplayWidth(display,wc->screen),DisplayHeight(display,wc->screen),true);
	wc->LFSTK_initDnD();
	wc->acceptOnThis=true;

	asprintf(&diskInfoPath,"%s/.config/LFS/disks2",getenv("HOME"));
	asprintf(&command,"mkdir -p %s 2>&1 >/dev/null",diskInfoPath);
	system(command);
	free(command);

	asprintf(&cacheDisksPath,"%s/.config/LFS/cache/disks",getenv("HOME"));
	asprintf(&command,"mkdir -p %s 2>&1 >/dev/null",cacheDisksPath);
	system(command);
	free(command);

	asprintf(&cacheDeskPath,"%s/.config/LFS/cache/desktop",getenv("HOME"));
	asprintf(&command,"mkdir -p %s 2>&1 >/dev/null",cacheDeskPath);
	system(command);
	free(command);

	asprintf(&desktopPath,"%s/Desktop",getenv("HOME"));
//create home/computer desktop files.
	createDirDesktops();

	asprintf(&prefsPath,"%s/.config/LFS/lfsdesktop.rc",getenv("HOME"));
	asprintf(&iconTheme,"gnome");
	iconSize=32;
	gridSize=64;
	gridBorderLeft=2;
	gridBorderRight=2;
	asprintf(&terminalCommand,"xterm -e ");
	showSuffix=false;
	asprintf(&fontFace,"DejaVu Sans:size=10:bold");
	asprintf(&backCol,"#000000");
	asprintf(&backAlpha,"0x00");
	refreshRate=2;
	includeList=NULL;
	excludeList=NULL;

	loadVarsFromFile(prefsPath,desktopPrefs);
	if(gridSize<1)
		gridSize=1;

	nextXPos=gridBorderLeft;
	nextYPos=gridBorderLeft;

	maxXSlots=DisplayWidth(display,wc->screen)/gridSize;
	maxYSlots=DisplayHeight(display,wc->screen)/gridSize;

//disks
	sy=0;
	diskWindow=new LFSTK_windowClass(100,100,200,200,"xxx",true,true);
	for(int j=MOUNTDISK;j<NOMOREBUTONS;j++)
		{
			diskButtons[j]=new LFSTK_buttonClass(diskWindow,diskLabelData[j],0,sy,GADGETWIDTH,24,NorthWestGravity);
			diskButtons[j]->LFSTK_setCallBack(NULL,doDiskMenuSelect,(void*)(long)(j));
			iconpath=diskWindow->globalLib->LFSTK_findThemedIcon(iconTheme,diskThemeIconData[j],"");
			diskButtons[j]->LFSTK_setImageFromPath(iconpath,LEFT,true);
			sy+=GADGETHITE;
		}
	diskWindow->LFSTK_resizeWindow(GADGETWIDTH,sy,true);
	diskWindow->LFSTK_showWindow(true);
	diskWindow->LFSTK_hideWindow();

//files
	sy=0;
	fileWindow=new LFSTK_windowClass(100,100,200,200,"xxx",true,true);
	for(int j=OPENDISK;j<NOMOREBUTONS;j++)
		{
			fileButtons[j]=new LFSTK_buttonClass(fileWindow,diskLabelData[j],0,sy,GADGETWIDTH,24,NorthWestGravity);
			fileButtons[j]->LFSTK_setCallBack(NULL,doDeskItemMenuSelect,(void*)(long)(j));
			iconpath=diskWindow->globalLib->LFSTK_findThemedIcon(iconTheme,diskThemeIconData[j],"");
			fileButtons[j]->LFSTK_setImageFromPath(iconpath,LEFT,true);
			sy+=GADGETHITE;
		}
	fileWindow->LFSTK_resizeWindow(GADGETWIDTH,sy,true);
	fileWindow->LFSTK_showWindow(true);
	fileWindow->LFSTK_hideWindow();

//icon chooser
	sy=0;
	iconChooser=new LFSTK_windowClass(0,0,DIALOGWIDTH,200,"Icon Chooser",false);

	label=new LFSTK_labelClass(iconChooser,"Enter Path To Icon",BORDER,sy,DIALOGWIDTH-(BORDER*2),GADGETHITE,NorthWestGravity);
	sy+=GADGETHITE;
	iconChooserEdit=new LFSTK_lineEditClass(iconChooser,"",BORDER,sy,DIALOGWIDTH-(BORDER*2),GADGETHITE,NorthWestGravity);
	sy+=GADGETHITE;

	bc=new LFSTK_buttonClass(iconChooser,"--",0,sy,DIALOGWIDTH,GADGETHITE,BUTTONGRAV);
	bc->LFSTK_setStyle(BEVELNONE);
	bc->gadgetDetails.buttonTile=false;
	bc->gadgetDetails.colour=&wc->windowColourNames[NORMALCOLOUR];
	sy+=GADGETHITE;

	bc=new LFSTK_buttonClass(iconChooser,"Apply",BORDER,sy,GADGETWIDTH,GADGETHITE,NorthWestGravity);
	bc->LFSTK_setCallBack(NULL,dialogCB,(void*)DIALOGRETAPPLY);
	bc=new LFSTK_buttonClass(iconChooser,"Cancel",DIALOGWIDTH-BORDER-GADGETWIDTH,sy,GADGETWIDTH,GADGETHITE,NorthWestGravity);
	bc->LFSTK_setCallBack(NULL,dialogCB,(void*)DIALOGRETCANCEL);
	sy+=GADGETHITE;
	sy+=HALFYSPACING;
	iconChooser->LFSTK_resizeWindow(DIALOGWIDTH,sy,true);
	iconChooser->LFSTK_hideWindow();

	loadDisks();
	loadDesktopItems();
	signal(SIGALRM,alarmCallBack);

	wc->LFSTK_showWindow();
	wc->LFSTK_clearWindow();

//disk insert/remove
	fhfordisks=inotify_init();
	polldisks.fd=fhfordisks;
	polldisks.events=POLLIN;
	polldisks.revents=0;
	inotify_add_watch(fhfordisks,"/dev/disk/by-uuid",IN_CREATE|IN_DELETE|IN_MODIFY);

//watch desktop folder
	fhfordesktop=inotify_init();
	polldesktop.fd=fhfordesktop;
	polldesktop.events=POLLIN;
	polldesktop.revents=0;
	inotify_add_watch(fhfordesktop,desktopPath,IN_CREATE|IN_DELETE|IN_MOVED_FROM|IN_MOVED_TO);

	alarm(refreshRate);

	mainLoop=true;
	while(mainLoop==true)
		{
			if(needsRefresh==true)
				doRefresh();

			XNextEvent(wc->display,&event);
			mappedListener *ml=wc->LFSTK_getMappedListener(event.xany.window);
			if(ml!=NULL)
				{
					ml->function(ml->gadget,&event,ml->type);
					if(event.type==ButtonPress)
						{
							if(event.xbutton.button==Button1)
								{
									oldPos.x=static_cast<diskDataStruct*>(ml->gadget->userData)->posx;
									oldPos.y=static_cast<diskDataStruct*>(ml->gadget->userData)->posy;
								}
						}
				}

			switch(event.type)
				{
					case MotionNotify:
						isdragging=true;
						if(ml!=NULL)
							ml->gadget->LFSTK_clearWindow();
						break;
					case ButtonRelease:
						isdragging=false;
					case ButtonPress:
						if(ml==NULL)
							{
								event.xany.window=wc->rootWindow;
								XSendEvent(wc->display,wc->rootWindow,true,ButtonPressMask|ButtonReleaseMask,&event);
							}
						break;
					case Expose:
						if(reloadPrefsFlag==true)
							{
								reloadPrefs();
								reloadPrefsFlag=false;
							}

						if(reloadPixmap==true)
							{
								wc->LFSTK_setWindowPixmap(wc->globalLib->LFSTK_getWindowPixmap(display,wc->rootWindow),DisplayWidth(display,wc->screen),DisplayHeight(display,wc->screen));
								reloadPixmap=false;

								diskLinkedList		*dll=diskLL;
								while(dll!=NULL)
									{
										dll->data->diskImage->LFSTK_setUseWindowPixmap(true);
										XClearWindow(wc->display,diskLL->data->diskImage->window);
										dll->data->diskImage->LFSTK_clearWindow();
										dll=dll->next;
									}
							}
						wc->LFSTK_clearWindow();
						break;

					case ConfigureNotify:
						if(event.xresizerequest.send_event==false)
							{
								wc->LFSTK_resizeWindow(event.xconfigurerequest.width,event.xconfigurerequest.height,false);
								wc->LFSTK_setWindowPixmap(wc->globalLib->LFSTK_getWindowPixmap(display,wc->rootWindow),event.xconfigurerequest.width,event.xconfigurerequest.height);
							}
						break;

					case ClientMessage:
					case SelectionNotify:
							if (event.xclient.message_type == XInternAtom(wc->display, "WM_PROTOCOLS", 1) && (Atom)event.xclient.data.l[0] == XInternAtom(wc->display, "WM_DELETE_WINDOW", 1))
								{
									wc->LFSTK_hideWindow();
									mainLoop=false;
								}
							if(wc->acceptDnd==true)
								{
									wc->LFSTK_handleDnD(&event);
									if((wc->droppedData.type!=-1) && (wc->acceptOnThis==true))
										{
											printf("dropped on window=>>%s<<\n",wc->droppedData.data);
											asprintf(&command,"cp '%s' ~/Desktop",wc->droppedData.data);
											system(command);
											freeAndNull(&command);
											wc->droppedData.type=-1;
										}
								}
						break;
					}
		}

	delete wc;
	XCloseDisplay(display);
	freePrefs();
	wc=NULL;

	diskLinkedList	*list=NULL;
	diskLinkedList	*holdlist=NULL;
	while(list!=NULL)
		{
			if(list->data!=NULL)
				{
					freeAndNull(&list->data->label);
					freeAndNull(&list->data->devName);
					freeAndNull(&list->data->uuid);
					freeAndNull(&list->data->pathToIcon);
					free(list->data);
				}
			holdlist=list;
			list=list->next;
			free(holdlist);
		}
	diskLL=NULL;

	if(reloadDeskFlag==true)
		{
			mainLoop=false;
			reloadPixmap=false;
			reloadPrefsFlag=false;
			reloadDeskFlag=false;
			needsRefresh=true;
			isdragging=false;
			goto BACKUP;
		}
	return 0;
}
