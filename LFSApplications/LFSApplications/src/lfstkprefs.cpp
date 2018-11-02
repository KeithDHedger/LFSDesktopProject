/*
 *
 * ©K. D. Hedger. Sun 13 Aug 18:33:35 BST 2017 keithdhedger@gmail.com

 * This file (lfstkprefs.cpp) is part of LFSApplications.

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

#include <getopt.h>
#include <lfstk/LFSTKGlobals.h>
#include <alloca.h>
#include <limits.h>

#define EDITBOXWIDTH	GADGETWIDTH*2

LFSTK_windowClass		*wc=NULL;
LFSTK_labelClass		*label=NULL;
LFSTK_labelClass		*cursorlabel=NULL;
LFSTK_labelClass		*personal=NULL;
LFSTK_labelClass		*copyrite=NULL;
LFSTK_buttonClass		*seperator=NULL;
LFSTK_buttonClass		*quit=NULL;
LFSTK_buttonClass		*apply=NULL;
LFSTK_buttonClass		*test=NULL;

//preview buttons
LFSTK_buttonClass		*previewButtons[4]={NULL,};
LFSTK_lineEditClass		*previeBackColourEdit[4]={NULL,};
LFSTK_lineEditClass		*previeFontColourEdit[4]={NULL,};
const char				*previewButtonLabels[4]={"Button Normal","Button Prelight","Button Active","Button Inactive"};

//menu buttons
LFSTK_buttonClass		*previewMenus[4]={NULL,};
LFSTK_lineEditClass		*previeMenuBackColourEdit[4]={NULL,};
LFSTK_lineEditClass		*previeMenuFontColourEdit[4]={NULL,};
const char				*previewMenuLabels[4]={"Menu Normal","Menu Prelight","Menu Active","Menu Inactive"};

//buton font
LFSTK_fontDialogClass	*buttonFontDialogButton=NULL;
LFSTK_lineEditClass		*buttonFontEdit=NULL;

//menu font
LFSTK_fontDialogClass	*menuFontDialogButton=NULL;
LFSTK_lineEditClass		*menuFontEdit=NULL;

//mono font
LFSTK_fontDialogClass	*monoFontDialogButton=NULL;
LFSTK_lineEditClass		*monoFontEdit=NULL;

//cursor prefs
LFSTK_lineEditClass		*cursorColourEdit=NULL;

//window stuff
LFSTK_lineEditClass		*windowColourEdit=NULL;
LFSTK_toggleButtonClass	*autoColourCheck=NULL;

//themes
LFSTK_toggleButtonClass	*useTheme=NULL;
LFSTK_buttonClass		*windowTile=NULL;
LFSTK_buttonClass		*buttonTile=NULL;
LFSTK_buttonClass		*menuTile=NULL;
LFSTK_lineEditClass		*windowTileEdit=NULL;
LFSTK_lineEditClass		*buttonTileEdit=NULL;
LFSTK_lineEditClass		*menuTileEdit=NULL;
LFSTK_fileDialogClass	*tileDialog=NULL;
LFSTK_menuButtonClass	*loadSet=NULL;
menuItemStruct			*setNameMenuItems=NULL;
int						setCnt;

bool					mainLoop=true;
Display					*display;
char					*wd;
int						parentWindow=-1;
int						queueID=-1;

void addSet(void)
{
	LFSTK_FindClass	*fc=new LFSTK_FindClass;
	char			*themeDir=NULL;

	asprintf(&themeDir,"%s/.themes",getenv("HOME"));
	fc->LFSTK_setFullPath(true);
	fc->LFSTK_setDepth(1,1);
	fc->LFSTK_setFindType(FOLDERTYPE);
	fc->LFSTK_setIncludeHidden(true);
	fc->LFSTK_setFileTypes(".lfstk");
	fc->LFSTK_findFiles(themeDir);
	fc->LFSTK_findFiles("/usr/share/themes",true);
	fc->LFSTK_setSort(true);
	fc->LFSTK_sortByPath();
	for(int j=0;j<fc->data.size();j++)
		{
			fprintf(stderr,"j=%i name=%s path=%s type=%i\n",j,fc->data.at(j).name.c_str(),fc->data.at(j).path.c_str(),fc->data.at(j).fileType);
		//	fprintf(stderr,"j=%i name=%s path=%s type=%i\n",j,fc->data.at(j)->name.c_str(),fc->data.at(j)->path.c_str(),fc->data.at(j)->fileType);
		}

	setNameMenuItems=new menuItemStruct[fc->data.size()];
	for(int j=0;j<fc->data.size();j++)
		setNameMenuItems[j].label=strdup(fc->data.at(j).path.c_str());
	setCnt=fc->data.size();
	delete fc;
	free(themeDir);
}

bool menuCB(void *p,void* ud)
{
	char	*buffer=(char*)alloca(PATH_MAX);
	menuItemStruct	*menuitem=(menuItemStruct*)ud;
	char *env=NULL;
//temp theme stuff
	char	*bnormal=NULL;
	char	*bpre=NULL;
	char	*bactive=NULL;
	char	*binactive=NULL;
	char	*bfnormal=NULL;
	char	*bfpre=NULL;
	char	*bfactive=NULL;
	char	*bfinactive=NULL;

	char	*mnormal=NULL;
	char	*mpre=NULL;
	char	*mactive=NULL;
	char	*minactive=NULL;
	char	*mfnormal=NULL;
	char	*mfpre=NULL;
	char	*mfactive=NULL;
	char	*mfinactive=NULL;

	char	*wnormal=NULL;

	char	*ccolour=NULL;

	char	*bfont=NULL;
	char	*mfont=NULL;
	char	*monofont=NULL;

	args myargs[]=
	{
//window
		{"window_normal",TYPESTRING,&wnormal},
//		{"window_prelight",TYPESTRING,&bpre},
//		{"window_active",TYPESTRING,&bactive},
//		{"window_inactive",TYPESTRING,&binactive},
//		{"windowtile",TYPESTRING,&wtile},
//button
		{"button_normal",TYPESTRING,&bnormal},
		{"button_prelight",TYPESTRING,&bpre},
		{"button_active",TYPESTRING,&bactive},
		{"button_inactive",TYPESTRING,&binactive},
//		{"buttontile",TYPESTRING,&(this->globalButtonTile)},
////menu button
		{"menuitem_normal",TYPESTRING,&mnormal},
		{"menuitem_prelight",TYPESTRING,&mpre},
		{"menuitem_active",TYPESTRING,&mactive},
		{"menuitem_inactive",TYPESTRING,&minactive},

		{"menuitem_font",TYPESTRING,&mfont},
		{"menuitem_font_normal",TYPESTRING,&mfnormal},
		{"menuitem_font_prelight",TYPESTRING,&mfpre},
		{"menuitem_font_active",TYPESTRING,&mfactive},
		{"menuitem_font_inactive",TYPESTRING,&mfinactive},
//		{"menuitemtile",TYPESTRING,&(this->globalMenuItemTile)},
//
////font
		{"font",TYPESTRING,&bfont},
		{"font_normal",TYPESTRING,&bfnormal},
		{"font_prelight",TYPESTRING,&bfpre},
		{"font_active",TYPESTRING,&bfactive},
		{"font_inactive",TYPESTRING,&bfinactive},
////other
//		{"autotextcolour",TYPEBOOL,&(this->autoLabelColour)},
//		{"usetheme",TYPEBOOL,&(this->useTheme)},
//monofont
		{"monofont",TYPESTRING,&monofont},
//cursor colour
		{"cursorcolour",TYPESTRING,&ccolour},
		{NULL,0,NULL},
	};


	if(ud==NULL)
		return(true);

//	sprintf(buffer,"%s/.themes/%s/window.png",getenv("HOME"),menuitem->label);
	sprintf(buffer,"%s/window.png",menuitem->label);
	windowTileEdit->LFSTK_setBuffer(buffer);
//	sprintf(buffer,"%s/.themes/%s/button.png",getenv("HOME"),menuitem->label);
	sprintf(buffer,"%s/button.png",menuitem->label);
	buttonTileEdit->LFSTK_setBuffer(buffer);
	//sprintf(buffer,"%s/.themes/%s/menuitem.png",getenv("HOME"),menuitem->label);
	sprintf(buffer,"%s/menuitem.png",menuitem->label);
	menuTileEdit->LFSTK_setBuffer(buffer);

	//sprintf(buffer,"%s/.themes/%s/lfstoolkit.rc",getenv("HOME"),menuitem->label);
	sprintf(buffer,"%s/lfstoolkit.rc",menuitem->label);
	wc->globalLib->LFSTK_loadVarsFromFile(buffer,myargs);

//button back
	previeBackColourEdit[0]->LFSTK_setBuffer(bnormal);
	previeBackColourEdit[1]->LFSTK_setBuffer(bpre);
	previeBackColourEdit[2]->LFSTK_setBuffer(bactive);
	previeBackColourEdit[3]->LFSTK_setBuffer(binactive);
//button font
	previeFontColourEdit[0]->LFSTK_setBuffer(bfnormal);
	previeFontColourEdit[1]->LFSTK_setBuffer(bfpre);
	previeFontColourEdit[2]->LFSTK_setBuffer(bfactive);
	previeFontColourEdit[3]->LFSTK_setBuffer(bfinactive);
//menu item
	previeMenuBackColourEdit[0]->LFSTK_setBuffer(mnormal);
	previeMenuBackColourEdit[1]->LFSTK_setBuffer(mpre);
	previeMenuBackColourEdit[2]->LFSTK_setBuffer(mactive);
	previeMenuBackColourEdit[3]->LFSTK_setBuffer(minactive);
//menu item font
	previeMenuFontColourEdit[0]->LFSTK_setBuffer(mfnormal);
	previeMenuFontColourEdit[1]->LFSTK_setBuffer(mfpre);
	previeMenuFontColourEdit[2]->LFSTK_setBuffer(mfactive);
	previeMenuFontColourEdit[3]->LFSTK_setBuffer(mfinactive);
//window
	windowColourEdit->LFSTK_setBuffer(wnormal);
//cursor
	cursorColourEdit->LFSTK_setBuffer(ccolour);
//fonts
	buttonFontEdit->LFSTK_setBuffer(bfont);
	menuFontEdit->LFSTK_setBuffer(mfont);
	monoFontEdit->LFSTK_setBuffer(monofont);

	free(bnormal);
	free(bpre);
	free(bactive);
	free(binactive);
	free(bfnormal);
	free(bfpre);
	free(bfactive);
	free(bfinactive);
	free(mnormal);
	free(mpre);
	free(mactive);
	free(minactive);
	free(mfnormal);
	free(mfpre);
	free(mfactive);
	free(mfinactive);
	free(wnormal);
	free(ccolour);
	free(bfont);
	free(mfont);
	free(monofont);
	return(true);
}

bool doQuit(void *p,void* ud)
{
	mainLoop=false;
	XFlush(wc->display);
	XSync(wc->display,true);
	return(false);
}

void setVars(void)
{
	for(int j=0;j<4;j++)
		{
//buttons
			wc->globalLib->LFSTK_setGlobalString(j,TYPEBUTTON,previeBackColourEdit[j]->LFSTK_getCStr());
			wc->globalLib->LFSTK_setGlobalString(j,TYPEFONTCOLOUR,previeFontColourEdit[j]->LFSTK_getCStr());
//menus
			wc->globalLib->LFSTK_setGlobalString(j,TYPEMENUITEM,previeMenuBackColourEdit[j]->LFSTK_getCStr());
			wc->globalLib->LFSTK_setGlobalString(j,TYPEMENUITEMFONTCOLOUR,previeMenuFontColourEdit[j]->LFSTK_getCStr());
		}

	wc->globalLib->LFSTK_setGlobalString(0,TYPEWINDOW,windowColourEdit->LFSTK_getCStr());
	wc->globalLib->LFSTK_setAutoLabelColour(autoColourCheck->LFSTK_getValue());

	if(((strcmp(windowTileEdit->LFSTK_getCStr(),"")==0) || (strcmp(buttonTileEdit->LFSTK_getCStr(),"")==0) || (strcmp(menuTileEdit->LFSTK_getCStr(),"")==0))==true)
		useTheme->LFSTK_setValue(false);
	wc->globalLib->LFSTK_setUseTheme(useTheme->LFSTK_getValue());
//tiles
	wc->globalLib->LFSTK_setGlobalString(0,TYPEWINDOWTILE,windowTileEdit->LFSTK_getCStr());
	wc->globalLib->LFSTK_setGlobalString(0,TYPEBUTTONTILE,buttonTileEdit->LFSTK_getCStr());
	wc->globalLib->LFSTK_setGlobalString(0,TYPEMENUITEMTILE,menuTileEdit->LFSTK_getCStr());
//fonts
	wc->globalLib->LFSTK_setGlobalString(0,TYPEFONT,buttonFontEdit->LFSTK_getCStr());
	wc->globalLib->LFSTK_setGlobalString(0,TYPEMENUITEMFONT,menuFontEdit->LFSTK_getCStr());
	wc->globalLib->LFSTK_setGlobalString(0,TYPEMONOFONT,monoFontEdit->LFSTK_getCStr());
	wc->globalLib->LFSTK_setGlobalString(0,TYPECURSORCOLOUR,cursorColourEdit->LFSTK_getCStr());
}

void setPreviewData(void)
{
	std::map<int,mappedListener*>	*ml=wc->LFSTK_getGadgets();
	int cnt=0;
	if((!ml->empty()) )
		{
			if(useTheme->LFSTK_getValue()==true)
				wc->LFSTK_setTile(windowTileEdit->LFSTK_getCStr(),-1);
			else
				{
					wc->LFSTK_setTile(NULL,0);
					wc->LFSTK_setWindowColourName(0,windowColourEdit->LFSTK_getCStr());
				}

			for (std::map<int,mappedListener*>::iterator it=ml->begin();it!=ml->end();++it)
				{
					mappedListener	*mls=it->second;
					if (mls!=NULL)
						{
							if(mls->gadget!=NULL)
								{
									if(mls->type!=LINEEDITGADGET)
										{
//button
											mls->gadget->LFSTK_setFontString(buttonFontEdit->LFSTK_getCStr(),true);	
											mls->gadget-> LFSTK_setLabelAutoColour(autoColourCheck->LFSTK_getValue());
											mls->gadget->LFSTK_setColourName(NORMALCOLOUR,previeBackColourEdit[NORMALCOLOUR]->LFSTK_getCStr());
											mls->gadget->LFSTK_setColourName(PRELIGHTCOLOUR,previeBackColourEdit[PRELIGHTCOLOUR]->LFSTK_getCStr());
											mls->gadget->LFSTK_setColourName(ACTIVECOLOUR ,previeBackColourEdit[ACTIVECOLOUR]->LFSTK_getCStr());
											mls->gadget->LFSTK_setColourName(INACTIVECOLOUR,previeBackColourEdit[INACTIVECOLOUR]->LFSTK_getCStr());
											if(useTheme->LFSTK_getValue()==true)
												{
													if(mls->type==MENUBUTTONGADGET)
														mls->gadget->LFSTK_setTile(menuTileEdit->LFSTK_getCStr(),-1);
													else
														mls->gadget->LFSTK_setTile(buttonTileEdit->LFSTK_getCStr(),-1);
												}
											else
												mls->gadget->LFSTK_setTile(NULL,0);
											if(mls->type==MENUBUTTONGADGET)
												mls->gadget->LFSTK_setFontString(menuFontEdit->LFSTK_getCStr(),true);
//font
											mls->gadget->LFSTK_setFontColourName(NORMALCOLOUR,previeFontColourEdit[NORMALCOLOUR]->LFSTK_getCStr(),false);
											mls->gadget->LFSTK_setFontColourName(PRELIGHTCOLOUR,previeFontColourEdit[PRELIGHTCOLOUR]->LFSTK_getCStr(),false);
											mls->gadget->LFSTK_setFontColourName(ACTIVECOLOUR,previeFontColourEdit[ACTIVECOLOUR]->LFSTK_getCStr(),false);
											mls->gadget->LFSTK_setFontColourName(INACTIVECOLOUR,previeFontColourEdit[INACTIVECOLOUR]->LFSTK_getCStr(),false);
										}
									else
										{
											mls->gadget->LFSTK_setFontString(monoFontEdit->LFSTK_getCStr(),true);
											static_cast<LFSTK_lineEditClass*>(mls->gadget)->LFSTK_setCursorColourName(cursorColourEdit->LFSTK_getCStr());
										}
								}
						}
				}
//set examples
//buttons
			previewButtons[0]->LFSTK_setColourName(NORMALCOLOUR,previeBackColourEdit[NORMALCOLOUR]->LFSTK_getCStr());
			previewButtons[1]->LFSTK_setColourName(NORMALCOLOUR,previeBackColourEdit[PRELIGHTCOLOUR]->LFSTK_getCStr());
			previewButtons[2]->LFSTK_setColourName(NORMALCOLOUR,previeBackColourEdit[ACTIVECOLOUR]->LFSTK_getCStr());
			previewButtons[3]->LFSTK_setColourName(NORMALCOLOUR,previeBackColourEdit[INACTIVECOLOUR]->LFSTK_getCStr());
//menuitems
			previewMenus[0]->LFSTK_setColourName(NORMALCOLOUR,previeMenuBackColourEdit[NORMALCOLOUR]->LFSTK_getCStr());
			previewMenus[1]->LFSTK_setColourName(NORMALCOLOUR,previeMenuBackColourEdit[PRELIGHTCOLOUR]->LFSTK_getCStr());
			previewMenus[2]->LFSTK_setColourName(NORMALCOLOUR,previeMenuBackColourEdit[ACTIVECOLOUR]->LFSTK_getCStr());
			previewMenus[3]->LFSTK_setColourName(NORMALCOLOUR,previeMenuBackColourEdit[INACTIVECOLOUR]->LFSTK_getCStr());
			for(int j=0;j<4;j++)
				previewMenus[j]->LFSTK_setFontString(menuFontEdit->LFSTK_getCStr(),true);
			if(useTheme->LFSTK_getValue()==true)
				{
					previewMenus[0]->LFSTK_setTile(menuTileEdit->LFSTK_getCStr(),-1);
					previewMenus[1]->LFSTK_setTile(menuTileEdit->LFSTK_getCStr(),-1);
					previewMenus[2]->LFSTK_setTile(menuTileEdit->LFSTK_getCStr(),-1);
					previewMenus[3]->LFSTK_setTile(menuTileEdit->LFSTK_getCStr(),-1);
				}
			
//buttons font
			previewButtons[0]->LFSTK_setFontColourName(NORMALCOLOUR,previeFontColourEdit[NORMALCOLOUR]->LFSTK_getCStr(),false);
			previewButtons[1]->LFSTK_setFontColourName(NORMALCOLOUR,previeFontColourEdit[PRELIGHTCOLOUR]->LFSTK_getCStr(),false);
			previewButtons[2]->LFSTK_setFontColourName(NORMALCOLOUR,previeFontColourEdit[ACTIVECOLOUR]->LFSTK_getCStr(),false);
			previewButtons[3]->LFSTK_setFontColourName(NORMALCOLOUR,previeFontColourEdit[INACTIVECOLOUR]->LFSTK_getCStr(),false);

//menuitems font
			previewMenus[0]->LFSTK_setFontColourName(NORMALCOLOUR,previeMenuFontColourEdit[NORMALCOLOUR]->LFSTK_getCStr(),false);
			previewMenus[1]->LFSTK_setFontColourName(NORMALCOLOUR,previeMenuFontColourEdit[PRELIGHTCOLOUR]->LFSTK_getCStr(),false);
			previewMenus[2]->LFSTK_setFontColourName(NORMALCOLOUR,previeMenuFontColourEdit[ACTIVECOLOUR]->LFSTK_getCStr(),false);
			previewMenus[3]->LFSTK_setFontColourName(NORMALCOLOUR,previeMenuFontColourEdit[INACTIVECOLOUR]->LFSTK_getCStr(),false);
		}

	cursorColourEdit->LFSTK_setColourName(NORMALCOLOUR,cursorColourEdit->LFSTK_getCStr());
	cursorColourEdit->LFSTK_setCursorColourName("#808080");
	wc->LFSTK_clearWindow(true);
	XFlush(wc->display);
}

bool selectfile(void *object,void* ud)
{
	char	*buffer;

	LFSTK_lineEditClass	*fd=static_cast<LFSTK_lineEditClass*>(ud);

	tileDialog->LFSTK_showFileDialog(wd,"Select A File");
	if(tileDialog->LFSTK_isValid()==true)
		{
			asprintf(&buffer,"%s/%s",tileDialog->LFSTK_getCurrentDir(),tileDialog->LFSTK_getCurrentFile());
			fd->LFSTK_setBuffer(buffer);
			free(buffer);
			free(wd);
			wd=strdup(tileDialog->LFSTK_getCurrentDir());			
		}
	return(true);
}

bool buttonCB(void *p,void* ud)
{
	char					*prefsfile;
	const fontDataStruct	*fd;
	int						receiveType=IPC_NOWAIT;
	msgBuffer				buffer;
	bool					flag=false;
	int						retcode;

	if(ud!=NULL)
		{
			if(strcmp((char*)ud,"SELECTBUTTONFONT")==0)
				{
					buttonFontDialogButton->LFSTK_showDialog("");
					fd=buttonFontDialogButton->LFSTK_getFontData(false);
					if(fd->isValid==true)
						buttonFontEdit->LFSTK_setBuffer(fd->fontString);
					return(true);
				}

			if(strcmp((char*)ud,"SELECTMENUFONT")==0)
				{
					menuFontDialogButton->LFSTK_showDialog("");
					fd=menuFontDialogButton->LFSTK_getFontData(false);
					if(fd->isValid==true)
						menuFontEdit->LFSTK_setBuffer(fd->fontString);
					return(true);
				}

			if(strcmp((char*)ud,"SELECTMONOFONT")==0)
				{
					monoFontDialogButton->LFSTK_showDialog("");
					fd=monoFontDialogButton->LFSTK_getFontData(false);
					if(fd->isValid==true)
						monoFontEdit->LFSTK_setBuffer(fd->fontString);
					return(true);
				}

			if(strcmp((char*)ud,"TEST")==0)
				setPreviewData();

			if(strcmp((char*)ud,"APPLY")==0)
				{
					setPreviewData();
					setVars();
					asprintf(&prefsfile,"%s/.config/LFS/lfstoolkit.rc",getenv("HOME"));
					wc->globalLib->LFSTK_saveVarsToFile(prefsfile,wc->globalLib->LFSTK_getTKArgs());
					free(prefsfile);
//flush message queue
					while(flag==false)
						{
							retcode=msgrcv(queueID,&buffer,MAX_MSG_SIZE,MSGANY,receiveType);
							if(retcode<=1)
								flag=true;
						}

					buffer.mType=DESKTOP_MSG;
					sprintf(buffer.mText,"reloaddesk");
					if((msgsnd(queueID,&buffer,strlen(buffer.mText)+1,0))==-1)
						fprintf(stderr,"Can't send message :(\n");
					system("killall lfspanel &");	
				}
		}
	return(true);
}

int main(int argc, char **argv)
{
	XEvent		event;
	int			sy=0;
	int			sx=BORDER;
	int			c=0;
	int			option_index=0;
	int			key=666;
	char		*command;
	const char	*shortOpts="h?w:";
	option		longOptions[]=
		{
			{"window",1,0,'w'},
			{"help",0,0,'h'},
			{0, 0, 0, 0}
		};

	while(1)
		{
			option_index=0;
			c=getopt_long_only(argc,argv,shortOpts,longOptions,&option_index);
			if (c==-1)
				break;
			switch (c)
				{
					case 'h':
					case '?':
						printf("-?,-h,--help\t\tPrint this help\n");
						printf("-w,--window\t\tSet transient for window\n");
						exit(0);
					case 'w':
						parentWindow=atoi(optarg);
						break;
				}
		}
	
	wc=new LFSTK_windowClass(0,0,DIALOGWIDTH-BORDER-BORDER-BORDER,DIALOGHITE,"LFS Toolkit Prefs",false);
	display=wc->display;

	command=wc->globalLib->LFSTK_oneLiner("sed -n '2p' %s/lfsappearance.rc",wc->configDir);
	key=atoi(command);
	freeAndNull(&command);

	if((queueID=msgget(key,IPC_CREAT|0660))==-1)
		fprintf(stderr,"Can't create message queue\n");

	wd=strdup(wc->configDir);
	tileDialog=new LFSTK_fileDialogClass(wc,"Select File",wd,false);

	copyrite=new LFSTK_labelClass(wc,COPYRITE,BORDER,sy,DIALOGWIDTH-BORDER-BORDER,GADGETHITE);
	sy+=HALFYSPACING;
	personal=new LFSTK_labelClass(wc,PERSONAL,BORDER,sy,DIALOGWIDTH-BORDER-BORDER,GADGETHITE);
	personal->LFSTK_setCairoFontDataParts("B");
	sy+=YSPACING;

//normal buttons
	for(int j=0;j<4;j++)
		{
			previewButtons[j]=new LFSTK_buttonClass(wc,previewButtonLabels[j],sx,sy,GADGETWIDTH,GADGETHITE,BUTTONGRAV);
			previewButtons[j]->LFSTK_setIgnoreCB(true);
			sx+=GADGETWIDTH+BORDER;
			previeBackColourEdit[j]=new LFSTK_lineEditClass(wc,wc->globalLib->LFSTK_getGlobalString(j,TYPEBUTTON),sx,sy,EDITBOXWIDTH,GADGETHITE,BUTTONGRAV);
			sx+=EDITBOXWIDTH+BORDER;
			previeFontColourEdit[j]=new LFSTK_lineEditClass(wc,wc->globalLib->LFSTK_getGlobalString(j,TYPEFONTCOLOUR),sx,sy,EDITBOXWIDTH,GADGETHITE,BUTTONGRAV);
			sy+=YSPACING;
			sx=BORDER;
		}
	sx=BORDER;

//button font
	buttonFontDialogButton=new LFSTK_fontDialogClass(wc,"Button Font",sx,sy,GADGETWIDTH,GADGETHITE,BUTTONGRAV);
	buttonFontDialogButton->LFSTK_setCallBack(NULL,buttonCB,(void*)"SELECTBUTTONFONT");
	sx+=GADGETWIDTH+BORDER;
	buttonFontEdit=new LFSTK_lineEditClass(wc,wc->globalLib->LFSTK_getGlobalString(0,TYPEFONT),sx,sy,EDITBOXWIDTH*2+BORDER,GADGETHITE,BUTTONGRAV);
	sy+=YSPACING;

	sx=BORDER;
//menu buttons
	for(int j=0;j<4;j++)
		{
			previewMenus[j]=new LFSTK_buttonClass(wc,previewMenuLabels[j],sx,sy,GADGETWIDTH,GADGETHITE,BUTTONGRAV);
			previewMenus[j]->LFSTK_setIgnoreCB(true);
			sx+=GADGETWIDTH+BORDER;
			previeMenuBackColourEdit[j]=new LFSTK_lineEditClass(wc,wc->globalLib->LFSTK_getGlobalString(j,TYPEMENUITEM),sx,sy,EDITBOXWIDTH,GADGETHITE,BUTTONGRAV);
			sx+=EDITBOXWIDTH+BORDER;
			previeMenuFontColourEdit[j]=new LFSTK_lineEditClass(wc,wc->globalLib->LFSTK_getGlobalString(j,TYPEMENUITEMFONTCOLOUR),sx,sy,EDITBOXWIDTH,GADGETHITE,BUTTONGRAV);
			sy+=YSPACING;
			sx=BORDER;
		}
	sx=BORDER;
//menu font
	menuFontDialogButton=new LFSTK_fontDialogClass(wc,"Menu Font",sx,sy,GADGETWIDTH,GADGETHITE,BUTTONGRAV);
	menuFontDialogButton->LFSTK_setCallBack(NULL,buttonCB,(void*)"SELECTMENUFONT");
	sx+=GADGETWIDTH+BORDER;
	menuFontEdit=new LFSTK_lineEditClass(wc,wc->globalLib->LFSTK_getGlobalString(0,TYPEMENUITEMFONT),sx,sy,EDITBOXWIDTH*2+BORDER,GADGETHITE,BUTTONGRAV);
	sy+=YSPACING;
	sx=BORDER;
//mono font
	monoFontDialogButton=new LFSTK_fontDialogClass(wc,"Mono Font",sx,sy,GADGETWIDTH,GADGETHITE,BUTTONGRAV);
	monoFontDialogButton->LFSTK_setCallBack(NULL,buttonCB,(void*)"SELECTMONOFONT");
	sx+=GADGETWIDTH+BORDER;
	monoFontEdit=new LFSTK_lineEditClass(wc,wc->globalLib->LFSTK_getGlobalString(0,TYPEMONOFONT),sx,sy,EDITBOXWIDTH*2+BORDER,GADGETHITE,BUTTONGRAV);
	sy+=YSPACING;
	sx=BORDER;
//cursor prefs
	cursorlabel=new LFSTK_labelClass(wc,"Cursor Col",BORDER,sy,GADGETWIDTH,GADGETHITE,BUTTONGRAV);
	sx+=BORDER+GADGETWIDTH;
	cursorColourEdit=new LFSTK_lineEditClass(wc,wc->globalLib->LFSTK_getGlobalString(-1,TYPECURSORCOLOUR),sx,sy,EDITBOXWIDTH,GADGETHITE,BUTTONGRAV);
	cursorColourEdit->LFSTK_setCursorColourName("black");
	sy+=YSPACING;
	sx=BORDER;

//window
	label=new LFSTK_labelClass(wc,"Window Col",BORDER,sy,GADGETWIDTH,GADGETHITE,BUTTONGRAV);
	sx+=BORDER+GADGETWIDTH;
	windowColourEdit=new LFSTK_lineEditClass(wc,wc->globalLib->LFSTK_getGlobalString(0,TYPEWINDOW),sx,sy,EDITBOXWIDTH,GADGETHITE,BUTTONGRAV);
	sx+=BORDER+EDITBOXWIDTH;
	autoColourCheck=new LFSTK_toggleButtonClass(wc,"Auto Colour",sx,sy,GADGETWIDTH,GADGETHITE,NorthWestGravity);
	sy+=YSPACING;

//themes
	useTheme=new LFSTK_toggleButtonClass(wc,"Use Theme",BORDER,sy,GADGETWIDTH,GADGETHITE,NorthWestGravity);
//load set
	addSet();

	loadSet=new LFSTK_menuButtonClass(wc,"Load Theme",BORDER+GADGETWIDTH+BORDER,sy,GADGETWIDTH,GADGETHITE,BUTTONGRAV);
	loadSet->LFSTK_setCallBack(NULL,menuCB,NULL);
	loadSet->LFSTK_addMenus(setNameMenuItems,setCnt);
	loadSet->LFSTK_setLabelGravity(CENTRE);
	sy+=YSPACING;
	sx=BORDER;

	if(setCnt==0)
		loadSet->LFSTK_setActive(false);	

//window tile
	windowTile=new LFSTK_buttonClass(wc,"Window Tile",sx,sy,GADGETWIDTH,GADGETHITE,BUTTONGRAV);
	sx+=GADGETWIDTH+BORDER;
	windowTileEdit=new LFSTK_lineEditClass(wc,wc->globalLib->LFSTK_getGlobalString(0,TYPEWINDOWTILE),sx,sy,EDITBOXWIDTH*2+BORDER,GADGETHITE,BUTTONGRAV);
	windowTile->LFSTK_setCallBack(NULL,selectfile,(void*)windowTileEdit);
	sy+=YSPACING;
	sx=BORDER;
//button tile
	buttonTile=new LFSTK_buttonClass(wc,"Button Tile",sx,sy,GADGETWIDTH,GADGETHITE,BUTTONGRAV);
	sx+=GADGETWIDTH+BORDER;
	buttonTileEdit=new LFSTK_lineEditClass(wc,wc->globalLib->LFSTK_getGlobalString(0,TYPEBUTTONTILE),sx,sy,EDITBOXWIDTH*2+BORDER,GADGETHITE,BUTTONGRAV);
	buttonTile->LFSTK_setCallBack(NULL,selectfile,(void*)buttonTileEdit);
	sy+=YSPACING;
	sx=BORDER;
//menu tile
	menuTile=new LFSTK_buttonClass(wc,"Menu Tile",sx,sy,GADGETWIDTH,GADGETHITE,BUTTONGRAV);
	sx+=GADGETWIDTH+BORDER;
	menuTileEdit=new LFSTK_lineEditClass(wc,wc->globalLib->LFSTK_getGlobalString(0,TYPEMENUITEMTILE),sx,sy,EDITBOXWIDTH*2+BORDER,GADGETHITE,BUTTONGRAV);
	menuTile->LFSTK_setCallBack(NULL,selectfile,(void*)menuTileEdit);
	sy+=YSPACING;

	autoColourCheck->LFSTK_setValue(wc->autoLabelColour);
	useTheme->LFSTK_setValue(wc->globalLib->LFSTK_getUseTheme());

	sx=BORDER;

	sx=EDITBOXWIDTH*2+(BORDER*4)+GADGETWIDTH;
//line
	seperator=new LFSTK_buttonClass(wc,"--",0,sy,DIALOGWIDTH,GADGETHITE,BUTTONGRAV);
	seperator->LFSTK_setStyle(BEVELNONE);
	seperator->gadgetDetails.buttonTile=false;
	seperator->gadgetDetails.colour=&wc->windowColourNames[NORMALCOLOUR];
	sy+=YSPACING;

//quit
	quit=new LFSTK_buttonClass(wc,"Quit",BORDER,sy,GADGETWIDTH,GADGETHITE,BUTTONGRAV);
	quit->LFSTK_setCallBack(NULL,doQuit,NULL);

//test
	test=new LFSTK_buttonClass(wc,"Test",DIALOGMIDDLE-HALFGADGETWIDTH,sy,GADGETWIDTH,GADGETHITE,BUTTONGRAV);
	test->LFSTK_setCallBack(NULL,buttonCB,(void*)"TEST");

//apply
	apply=new LFSTK_buttonClass(wc,"Apply",sx-GADGETWIDTH-BORDER,sy,GADGETWIDTH,GADGETHITE,BUTTONGRAV);
	apply->LFSTK_setCallBack(NULL,buttonCB,(void*)"APPLY");
	sy+=YSPACING;

	setPreviewData();
	wc->LFSTK_resizeWindow(sx,sy,true);
	wc->LFSTK_showWindow();
	wc->LFSTK_setKeepAbove(true);
	if(parentWindow!=-1)
		wc->LFSTK_setTransientFor(parentWindow);

	mainLoop=true;
	while(mainLoop==true)
		{
			XNextEvent(wc->display,&event);
			mappedListener *ml=wc->LFSTK_getMappedListener(event.xany.window);

			if(ml!=NULL)
				ml->function(ml->gadget,&event,ml->type);

			if(wc->LFSTK_handleWindowEvents(&event)<0)
				mainLoop=false;
		}

	delete wc;
	XCloseDisplay(display);
	for(int j=0;j<setCnt;j++)
		{
			free(setNameMenuItems[j].label);
		//	delete setNameMenuItems[j];
		}

	delete[] setNameMenuItems;
	cairo_debug_reset_static_data();
	return 0;
}