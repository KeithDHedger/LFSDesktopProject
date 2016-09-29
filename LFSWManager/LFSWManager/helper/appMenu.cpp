/*
 *
 * ©K. D. Hedger. Fri 31 Jul 17:35:44 BST 2015 kdhedger68713@gmail.com

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
LFSTK_windowClass		*twc;
int						iconSize=16;

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
	delete wc;
	delete twc;
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
											const char *imagefile=twc->globalLib->LFSTK_findThemedIcon(desktopTheme,foundiconbuffer,"");
											if(imagefile!=NULL)
												{
													twc->globalLib->LFSTK_setPixmapsFromPath(twc->display,twc->visual,twc->cm,twc->window,imagefile,&mainMenus[foundcatmatch].entry[mainMenus[foundcatmatch].maxentrys].pm[0],&mainMenus[foundcatmatch].entry[mainMenus[foundcatmatch].maxentrys].pm[1],16);
													mainMenus[foundcatmatch].entry[mainMenus[foundcatmatch].maxentrys].gotIcon=true;
												}
										}

									mainMenus[foundcatmatch].maxentrys++;
								}
							fclose(filedata);
						}
				}
			pclose(fp);
		}
}

bool inWindow(void)
{
	Window			root_return;
	Window			child_return;
	int				root_x_return;
	int				root_y_return;
	int				win_x_return;
	int				win_y_return;
	unsigned int	mask_return;

	if(XQueryPointer(wc->display,wc->rootWindow,&root_return,&child_return,&root_x_return,&root_y_return,&win_x_return,&win_y_return, &mask_return)==true)
		{
			const geometryStruct *g=wc->LFSTK_getWindowGeom();
			if((root_x_return>g->x) && (root_x_return<(int)(g->x+g->w)) && (root_y_return>g->y) && (root_y_return<(int)(g->y+g->h)))
				return(true);
		}
	return(false);
}

int main(int argc, char **argv)
{
	XEvent			event;
	int				menucount=0;
	menuItemStruct *ms,*pms;
	int				sx=0;
	int				sy=0;
	Window			dumpwind;
	int				dumpint;
	int				win_x_return;
	int				win_y_return;
	unsigned int	mask_return;
	Display			*disp;
	fontStruct		*tfont;
	const char		*itemfont;

	twc=new LFSTK_windowClass(0,0,800,400,"appmenu",true);
	desktopTheme=twc->globalLib->LFSTK_oneLiner("cat %s/.config/LFS/lfsdesktop.rc|grep icontheme|awk '{print $2}'",getenv("HOME"));

	itemfont=twc->globalLib->LFSTK_getGlobalString(-1,TYPEMENUITEMFONT);
	tfont=twc->globalLib->LFSTK_loadFont(twc->display,twc->screen,itemfont);
	iconSize=tfont->size;
	free(tfont);

	setCatagories();
	makestatic=true;

	disp=XOpenDisplay(NULL);
	if(disp==NULL)
		exit(1);

	if(argc==2)
		{
			if(XQueryPointer(disp,DefaultRootWindow(disp),&dumpwind,&dumpwind,&dumpint,&dumpint,&win_x_return,&win_y_return,&mask_return)==true)
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
	delete wc;
	
	wc=new LFSTK_windowClass(sx,sy,800,400,"appmenu",true);
	wc->LFSTK_setDecorated(true);

	sx=0;
	sy=0;

	int addto=wc->font->ascent+wc->font->descent+8;
	int maxwid=0;

	while(myCats[sx]!=NULL)
		{
			XftFont *font=(XftFont*)wc->font->data;
			XGlyphInfo info;
			
			XftTextExtentsUtf8(wc->display,font,(XftChar8 *)myCats[sx],strlen(myCats[sx]),&info);
			sx++;
			if((info.width-info.x)>maxwid)
				maxwid=info.width;
		}

	maxwid+=8+addto;
	sx=0;
	sy=0;

	const char	*themeicon=NULL;
	for(int j=0; j<MAXCATS; j++)
		{
			bc[j]=NULL;
			if(mainMenus[j].name!=NULL)
				{
					bc[menucount]=new LFSTK_menuButtonClass(wc,(char*)mainMenus[j].name,sx,sy,maxwid,addto,0);
					themeicon=wc->globalLib->LFSTK_findThemedIcon(desktopTheme,catImageNames[j],"categories");
					bc[menucount]->LFSTK_setLabelOriention(CENTRE);
					if(themeicon!=NULL)
						{
							//bc[menucount]->LFSTK_setIconFromPath(themeicon,addto-2);
							bc[menucount]->LFSTK_setIconFromPath(themeicon,iconSize);
							bc[menucount]->LFSTK_setLabelOriention(LEFT);
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
							pms->useIcon=mainMenus[j].entry[k].gotIcon;
							pms->icon[0]=mainMenus[j].entry[k].pm[0];
							pms->icon[1]=mainMenus[j].entry[k].pm[1];
							pms->iconSize=iconSize;
							pms++;
						}
					bc[menucount]->LFSTK_addMenus(ms,mainMenus[j].maxentrys);
					menucount++;
				}
		}
	XResizeWindow(wc->display,wc->window,maxwid,sy);
	wc->LFSTK_resizeWindow(maxwid,sy);
	wc->LFSTK_clearWindow();
	XMapWindow(wc->display,wc->window);

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
						mainloop=inWindow();
						break;
					case Expose:
						wc->LFSTK_clearWindow();
						break;
					case ConfigureNotify:
						wc->LFSTK_resizeWindow(event.xconfigurerequest.width,event.xconfigurerequest.height);
						wc->LFSTK_clearWindow();
						break;
				}
		}

	if(desktopTheme!=NULL)
		free(desktopTheme);

	freeData();
	return 0;
}
