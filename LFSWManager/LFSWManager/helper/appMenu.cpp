/*
 *
 * ©K. D. Hedger. Fri 31 Jul 17:35:44 BST 2015 keithdhedger@gmail.com

 * This file (appMenu.cpp) is part of LFSWManager.

 * LFSWManager is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * at your option) any later version.

 * LFSWManager is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

 * You should have received a copy of the GNU General Public License
 * along with LFSWManager.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>

#include <lfstk/LFSTKGlobals.h>

#define BUFFERSIZE 2048
#define MAXCATS 14
#define MAXENTRYS 500

struct menuEntryStruct
{
	char				*name;
	char				*exec;
	bool				inTerm;
	Pixmap				pm[2];
	bool				gotIcon;
	char				*imagePath;
};

struct appMenuStruct
{
	const char			*name;
	menuEntryStruct		entry[MAXENTRYS];
	int					maxentrys;
};

const char				*myCats[]= {"Settings","Utility","Development","Education","Graphics","Network","AudioVideo","Audio","Video","Office","Shell","Game","System",NULL};
const char				*catImageNames[]={"preferences-desktop","applications-utilities","applications-development","applications-science","applications-graphics","applications-internet","applications-multimedia","applications-multimedia","applications-multimedia","applications-office","applications-engineering","applications-games","applications-system",NULL};

const char				*terminalCommand="xterm -e ";
appMenuStruct			mainMenus[MAXCATS];
bool					mainloop=false;
bool					makestatic=false;
LFSTK_windowClass		*wc;
LFSTK_menuButtonClass	*bc[MAXCATS]={NULL,};
char					*desktopTheme=NULL;

void freeData(void)
{
	for(int j=0;j<MAXCATS;j++)
		{
			for(int k=0;k<mainMenus[j].maxentrys;k++)
				{
					if(mainMenus[j].entry[k].exec!=NULL)
						free(mainMenus[j].entry[k].exec);
					if(mainMenus[j].entry[k].name!=NULL)
						free(mainMenus[j].entry[k].name);
				}
		}
}

bool bcb(void *p,void* ud)
{
	char			buffer[BUFFERSIZE];

	menuItemStruct	*menuitem;
	menuEntryStruct	*menuentry;
	menuitem=(menuItemStruct*)ud;

	if(menuitem==NULL)
		return(true);

	if(menuitem->userData==NULL)
		return(true);
	menuentry=(menuEntryStruct*)(menuitem->userData);
	if(menuentry!=NULL)
		{
			if(menuentry->inTerm==false)
				sprintf(buffer,"%s &",menuentry->exec);
			else
				sprintf(buffer,"%s %s &",terminalCommand,menuentry->exec);

			system(buffer);

			freeData();
			exit(0);
		}
	return(true);
}

void setCatagories(void)
{
	FILE	*fp;
	char	buffer[BUFFERSIZE];
	char	*splitstr=NULL;
	int		mycatcnt;
	bool	foundmatch;
	FILE	*filedata;
	int		foundcatmatch=-1;
	char	foundnamebuffer[BUFFERSIZE];
	char	foundexecbuffer[BUFFERSIZE];
	char	foundiconbuffer[BUFFERSIZE];
	bool	overridefound=false;
	bool	interm=false;
	bool	foundicon=false;

	for(int j=0; j<MAXCATS; j++)
		{
			mainMenus[j].name=NULL;
			mainMenus[j].maxentrys=0;
			bc[j]=NULL;
			for(int k=0; k<MAXENTRYS; k++)
				{
					mainMenus[j].entry[k].name=NULL;
					mainMenus[j].entry[k].exec=NULL;
					mainMenus[j].entry[k].inTerm=false;
				}
		}

	fp=popen("find /usr/share/applications -name \"*.desktop\"|sort","r");
	if(fp!=NULL)
		{
			buffer[0]=0;
			while(fgets(buffer,BUFFERSIZE,fp))
				{
					buffer[strlen(buffer)-1]=0;
					filedata=fopen(buffer,"r");
					if(filedata!=NULL)
						{
							foundmatch=false;
							overridefound=false;
							interm=false;
							foundicon=false;
							while(fgets(buffer,BUFFERSIZE,filedata))
								{
									if(buffer[strlen(buffer)-1]=='\n')
										buffer[strlen(buffer)-1]=0;
									splitstr=NULL;
									splitstr=strtok(buffer,"=");

									if(splitstr!=NULL)
										{
											if(strncmp(splitstr,"Categories",10)==0)
												{
													mycatcnt=0;
													foundcatmatch=-1;
													splitstr=strtok(NULL,"=");
													while(myCats[mycatcnt]!=NULL)
														{
															if(strstr(splitstr,myCats[mycatcnt])!=NULL)
																{
																	foundmatch=true;
																	foundcatmatch=mycatcnt;
																	break;
																}
															mycatcnt++;
														}
												}
											if(strcmp(splitstr,"Name")==0)
												{
													splitstr=strtok(NULL,"=");
													sprintf(foundnamebuffer,"%s",splitstr);
												}
											if(strcmp(splitstr,"Icon")==0)
												{
													foundicon=true;
													splitstr=strtok(NULL,"=");
													sprintf(foundiconbuffer,"%s",splitstr);
												}
											if(strcmp(splitstr,"Exec")==0)
												{
													splitstr=strtok(NULL,"=");
													sprintf(foundexecbuffer,"%s",splitstr);
												}
											if(strcmp(splitstr,"NoDisplay")==0)
												{
													splitstr=strtok(NULL,"=");
													if(strcasecmp(splitstr,"true")==0)
														overridefound=true;
												}
											if(strcmp(splitstr,"Terminal")==0)
												{
													splitstr=strtok(NULL,"=");
													if(strcasecmp(splitstr,"true")==0)
														interm=true;
													else
														interm=false;
												}
										}
								}

							if((foundmatch==true) && (overridefound==false))
								{
									mainMenus[foundcatmatch].name=myCats[foundcatmatch];
									char *ptr=strstr(foundexecbuffer,"%");
									while(ptr!=NULL)
										{
											sprintf(ptr,"  ");
											ptr=strstr(foundexecbuffer,"%");
										}
									mainMenus[foundcatmatch].entry[mainMenus[foundcatmatch].maxentrys].name=strdup(foundnamebuffer);
									mainMenus[foundcatmatch].entry[mainMenus[foundcatmatch].maxentrys].exec=strdup(foundexecbuffer);
									mainMenus[foundcatmatch].entry[mainMenus[foundcatmatch].maxentrys].inTerm=interm;
									mainMenus[foundcatmatch].entry[mainMenus[foundcatmatch].maxentrys].gotIcon=false;

									if(foundicon==true)
										{
											const char *imagefile=wc->globalLib->LFSTK_findThemedIcon(desktopTheme,foundiconbuffer,"");
											if(imagefile!=NULL)
												{
													mainMenus[foundcatmatch].entry[mainMenus[foundcatmatch].maxentrys].imagePath=strdup(imagefile);
												}
										}
									else
										mainMenus[foundcatmatch].entry[mainMenus[foundcatmatch].maxentrys].imagePath=NULL;

									mainMenus[foundcatmatch].maxentrys++;
								}
							fclose(filedata);
						}
				}
			pclose(fp);
		}
}

int main(int argc, char **argv)
{
	XEvent					event;
	int						menucount=0;
	menuItemStruct			*ms,*pms;
	int						sx=0;
	int						sy=0;
	Window					dumpwind;
	int						dumpint;
	int						win_x_return;
	int						win_y_return;
	unsigned int			mask_return;
	Display					*disp;
	const char				*itemfont;
	LFSTK_menuButtonClass	*test=NULL;
	int						maxbuttons=0;

	wc=new LFSTK_windowClass(0,0,1,1,"appmenu",true,true);
	desktopTheme=wc->globalLib->LFSTK_oneLiner("cat %s/.config/LFS/lfsdesktop.rc|grep icontheme|awk '{print $2}'",getenv("HOME"));

	setCatagories();
	printf("mycatcnt=%i\n",mainMenus[0].maxentrys);
	makestatic=true;

	disp=wc->display;

	if(argc==2)
		{
			if(XQueryPointer(wc->display,wc->rootWindow,&dumpwind,&dumpwind,&dumpint,&dumpint,&win_x_return,&win_y_return,&mask_return)==true)
				{
					sx=win_x_return-10;
					sy=win_y_return-10;
				}
		}
	else
		{
			sx=atoi(argv[2]);
			sy=atoi(argv[3]);
		}
	terminalCommand=argv[1];

	sx=0;
	sy=0;

	test=new LFSTK_menuButtonClass(wc,"|",-10,-10,1,1,NorthGravity);
	int addto=test->LFSTK_getTextHeight("|")+10;
	int maxwid=0;

	while(myCats[sx]!=NULL)
		{
			if(test->LFSTK_getTextWidth(myCats[sx])>maxwid)
				maxwid=test->LFSTK_getTextWidth(myCats[sx]);
			sx++;
		}

	maxwid+=10+(addto*2);
	sx=0;
	sy=0;

	const char	*themeicon=NULL;
	for(int j=0; j<MAXCATS; j++)
		{
			bc[j]=NULL;
			if(mainMenus[j].name!=NULL)
				{
				maxbuttons++;
					bc[menucount]=new LFSTK_menuButtonClass(wc,(char*)mainMenus[j].name,sx,sy,maxwid,addto,0);
					themeicon=wc->globalLib->LFSTK_findThemedIcon(desktopTheme,catImageNames[j],"categories");
					bc[menucount]->LFSTK_setLabelGravity(MENU);
					if(themeicon!=NULL)
						{
							bc[menucount]->LFSTK_setImageFromPath(themeicon,LEFT,true);
							bc[menucount]->LFSTK_setLabelGravity(MENU);
						}
					bc[menucount]->LFSTK_setCallBack(NULL,bcb,NULL);
					bc[menucount]->LFSTK_setStyle(BEVELOUT);

					XMapWindow(wc->display,bc[menucount]->LFSTK_getWindow());
					sy+=addto;
					ms=new menuItemStruct[mainMenus[j].maxentrys];
					pms=ms;
					for(int k=0; k<mainMenus[j].maxentrys; k++)
						{
							pms->label=mainMenus[j].entry[k].name;
							pms->userData=(void*)&mainMenus[j].entry[k];
							pms->bc=NULL;
							pms->subMenus=NULL;
							pms->subMenuCnt=0;
							pms->imagePath=mainMenus[j].entry[k].imagePath;
							pms++;
						}
					bc[menucount]->LFSTK_addMenus(ms,mainMenus[j].maxentrys);
					menucount++;
				}
		}

	XQueryPointer(wc->display, wc->rootWindow,&dumpwind,&dumpwind,&dumpint,&dumpint,&win_x_return,&win_y_return,&mask_return);
		{
			sx=win_x_return-10;
			sy=win_y_return-10;
		}

	wc->LFSTK_resizeWindow(maxwid,maxbuttons*addto,true);
	wc->LFSTK_moveWindow(sx,sy,true);
	wc->LFSTK_showWindow(true);

	Window					childwindow;
	int						sinkx;
	int						sinky;
	bool					runsub=true;
	unsigned int			buttonmask;

	mainloop=true;
	while(mainloop==true)
		{
			mappedListener *ml=wc->LFSTK_getMappedListener(event.xany.window);
			if(ml!=NULL)
				ml->function(ml->gadget,&event,ml->type);

			XNextEvent(wc->display,&event);
			switch(event.type)
				{
					case LeaveNotify:
						if(event.xany.window==wc->window)
							{
								runsub=true;
								while(runsub==true)
									{
										XQueryPointer(wc->display,wc->window,&dumpwind,&childwindow,&sinkx,&sinky,&sinkx,&sinky,&buttonmask);
										if(childwindow!=0)
											runsub=false;

										if((childwindow==0) && (buttonmask != 0))
											{
												runsub=false;
												mainloop=false;
											}
									}
								wc->LFSTK_clearWindow();
							}
						break;
						break;
					case Expose:
						wc->LFSTK_clearWindow();
						break;
					case ConfigureNotify:
						wc->LFSTK_clearWindow();
						break;
				}
		}

	if(desktopTheme!=NULL)
		free(desktopTheme);
	wc->LFSTK_hideWindow();
	XSync(wc->display,true);
	freeData();
	delete wc;
	XCloseDisplay(disp);
	return 0;
}
