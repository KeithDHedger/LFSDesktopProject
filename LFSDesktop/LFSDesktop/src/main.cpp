
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

#include "config.h"
#include "globals.h"

#define UNKNOWNARG -100

struct option long_options[] =
{
	{"long1",1,0,'l'},
	{"version",0,0,'v'},
	{"help",0,0,'?'},
	{0, 0, 0, 0}
};

const char			*diskLabelData[]={"Mount","Unmount","Eject","Open","Custom Icon","Remove Icon",NULL};
const char			*diskThemeIconData[]={"drive-harddisk","media-eject","media-eject","document-open","list-add","list-remove"};

LFSTK_labelClass	*label;

void printhelp(void)
{
	printf("Usage: lfsdesktop [OPTION]\n"
			"A CLI application\n"
			" -l, --long1	Do somthing good\n"
			" -v, --version	output version information and exit\n"
			" -h, -?, --help	print this help\n\n"
			"Report bugs to kdhedger@yahoo.co.uk\n"
	      );
}

Pixmap GetRootPixmap(Display* display, Window *root)
{
    Pixmap			currentRootPixmap=0;
    Atom			act_type;
    int				act_format;
    unsigned long	nitems;
    unsigned long	bytes_after;
    unsigned char	*data=NULL;
    Atom			_XROOTPMAP_ID;

    _XROOTPMAP_ID=XInternAtom(display,"_XROOTPMAP_ID",False);

    if (XGetWindowProperty(display,*root,_XROOTPMAP_ID,0,1,False,XA_PIXMAP,&act_type,&act_format,&nitems,&bytes_after,&data)==Success)
    	{
	        if (data)
	        	{
					currentRootPixmap=*((Pixmap *)data);
					XFree(data);
        		}
    	}
	return currentRootPixmap;
}

bool needsRefresh=true;

void  alarmCallBack(int sig)
{
	signal(SIGALRM,SIG_IGN);
	needsRefresh=true;
	signal(SIGALRM,alarmCallBack);
	alarm(refreshRate);
}

int main(int argc, char **argv)
{
	int					c;
	XEvent				event;
	char				*command;
	pollfd				polldisks;
	int					fhfordisks;
	pollfd				polldesktop;
	int					fhfordesktop;
	long				numRead=0;
	int					ret;
	char				buffer[EVENT_BUF_LEN]={0,};
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

	windowInitStruct *wi=new windowInitStruct;
	wi->windowType="_NET_WM_WINDOW_TYPE_DESKTOP";
	wi->overRide=true;
	wi->level=BELOWALL;
	wi->w=1920;
	wi->h=1980;
//	wi->decorated=false;
//	wc=new LFSTK_windowClass(0,0,1,1,"LFS Desktop Project",true,true);
//	wc=new LFSTK_windowClass(0,0,1,1,"LFS Desktop Project",false);
//	wc=new LFSTK_windowClass(0,0,1,1,"LFS Desktop Project",true);
	wc=new LFSTK_windowClass(wi);
	display=wc->display;
//	wc->LFSTK_moveWindow(0,0,true);


//	wc->LFSTK_resizeWindow(1000,800,true);
	wc->LFSTK_resizeWindow(DisplayWidth(display,wc->screen),DisplayHeight(display,wc->screen),true);
	wc->LFSTK_setWindowPixmap(wc->globalLib->LFSTK_getWindowPixmap(display,wc->rootWindow),DisplayWidth(display,wc->screen),DisplayHeight(display,wc->screen));
	wc->LFSTK_initDnD();
	wc->acceptOnThis=true;
	//wc->LFSTK_setWindowType("_NET_WM_WINDOW_TYPE_DESKTOP");

//wc->LFSTK_moveWindow(0,0,true);
//
//wc->LFSTK_setDecorated(false);
//wc->LFSTK_setSticky(true);
//wc->LFSTK_setKeepAbove(false);
DEBUGFUNC("sx=%i sy=%i",DisplayWidth(display,wc->screen),DisplayHeight(display,wc->screen));
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

	asprintf(&prefsPath,"%s/.config/LFS/lfsdesktop2.rc",getenv("HOME"));
	asprintf(&iconTheme,"gnome");
	iconSize=32;
	gridSize=64;
	gridBorder=32;
	asprintf(&terminalCommand,"xterm -e ");
	showSuffix=false;
	asprintf(&fontFace,"Sans;0;0;10");
	asprintf(&foreCol,"#ffffff");
	asprintf(&backCol,"#000000");
	asprintf(&backAlpha,"0x00");
	refreshRate=2;
	includeList=NULL;
	excludeList=NULL;

	loadVarsFromFile(prefsPath,desktopPrefs);
	if(gridSize<1)
		gridSize=1;

	maxXSlots=1920/gridSize;
	maxYSlots=1980/gridSize;

	xySlot=(int**)malloc(maxXSlots*sizeof(int*));
	for(int j=0; j<maxXSlots; j++)
		xySlot[j]=(int*)malloc(maxYSlots*sizeof(int));

	for(int yy=0;yy<maxYSlots;yy++)
		for(int xx=0;xx<maxXSlots;xx++)
			xySlot[xx][yy]=0;

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

	//wc->LFSTK_setDecorated(false);
	//wc->LFSTK_setKeepBelow(true);
//wc->LFSTK_setWindowPixmap(wc->globalLib->LFSTK_getWindowPixmap(display,wc->rootWindow),DisplayWidth(display,wc->screen),DisplayHeight(display,wc->screen));
	wc->LFSTK_showWindow();
	//XLowerWindow(wc->display,wc->window);
//	wc->LFSTK_moveWindow(0,0,true);
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
				{
				//TODO//
					//wc->LFSTK_setWindowPixmap(wc->globalLib->LFSTK_getWindowPixmap(display,wc->rootWindow),DisplayWidth(display,wc->screen),DisplayHeight(display,wc->screen));
					//wc->LFSTK_clearWindow();

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
											inotify_event *event=(inotify_event*)&buffer[cnt];
											if(event->len>0)
												{
													if(event->mask & IN_CREATE)
														addDeskItem(event->name);
													if(event->mask & IN_DELETE)
														deleteDeskItem(event->name);
													if(event->mask & IN_MOVED_FROM)
														addDeskItem(event->name);
													if(event->mask & IN_MOVED_TO)
														deleteDeskItem(event->name);
												}
											cnt+=sizeof(inotify_event)+event->len;
										}
									wc->LFSTK_showWindow();
								}
							}
					needsRefresh=false;
				}

			while (XPending(display))
				{
					XNextEvent(wc->display,&event);

					mappedListener *ml=wc->LFSTK_getMappedListener(event.xany.window);
					if(ml!=NULL)
						{
							ml->function(ml->gadget,&event,ml->type);
							if(event.type==ButtonPress)
								{
									if(event.xbutton.button==Button1)
										ml->gadget->LFSTK_getGeom(&oldPos);
								}
						}

					switch(event.type)
						{
							case MotionNotify:
								if(ml!=NULL)
									ml->gadget->LFSTK_clearWindow();
								break;
								
							case ButtonRelease:
							case ButtonPress:
								if(ml==NULL)
									{
										event.xany.window=wc->rootWindow;
										XSendEvent(wc->display,wc->rootWindow, true, ButtonPressMask|ButtonReleaseMask,&event);
									}
								break;
							case Expose:
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
		}

	delete wc;
	XCloseDisplay(display);
	freePrefs();
	return 0;
}

