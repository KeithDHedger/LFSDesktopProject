/*
 *
 * ©K. D. Hedger. Sat 25 Nov 17:33:52 GMT 2023 keithdhedger@gmail.com

 * This file (lfsdockprefs.cpp) is part of LFSApplications.

 * LFSApplications is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.

 * LFSApplications is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

 * You should have received a copy of the GNU General Public License
 * along with LFSApplications.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <string>
#include <getopt.h>

#include "lfstk/LFSTKGlobals.h"

#undef GADGETWIDTH
#define GADGETWIDTH	128
#undef DIALOGWIDTH
#define DIALOGWIDTH	(GADGETWIDTH*3)+(BORDER*3)
#define BOXLABEL	"LFS Dock Prefs"

LFSTK_applicationClass		*apc=NULL;
LFSTK_prefsClass				prefs;
LFSTK_windowClass			*wc=NULL;
LFSTK_labelClass				*label=NULL;
LFSTK_labelClass				*personal=NULL;
LFSTK_labelClass				*copyrite=NULL;
LFSTK_buttonClass			*seperator=NULL;
LFSTK_buttonClass			*quit=NULL;
LFSTK_buttonClass			*apply=NULL;

//dock data
LFSTK_buttonClass			*dockSize=NULL;
LFSTK_lineEditClass			*dockSizeEdit=NULL;
infoDataStruct				**dockSizeMenuGrav=NULL;
LFSTK_menuClass				*sizeMenu=NULL;

LFSTK_buttonClass			*dockGrav=NULL;
LFSTK_lineEditClass			*dockGravEdit=NULL;
infoDataStruct				**dockGravMenu=NULL;
LFSTK_menuClass				*gravMenu=NULL;

LFSTK_lineEditClass			*dockOnMonitor=NULL;
LFSTK_lineEditClass			*dockGadgets=NULL;

LFSTK_lineEditClass			*dockBGColourEdit=NULL;
LFSTK_lineEditClass			*dockTextColourEdit=NULL;

LFSTK_lineEditClass			*dockRefreshEdit=NULL;
LFSTK_toggleButtonClass		*dockUseMicro=NULL;


LFSTK_toggleButtonClass		*dockUseBG=NULL;
LFSTK_lineEditClass			*dockBGPathEdit=NULL;

std::map<int,const char*>	dockGravConvertToStr={{1,"North"},{2,"South"}};
std::map<int,const char*>	dockSizeConvertToStr={{1,"Small"},{2,"Medium"},{3,"Large"},{4,"Huge"}};

//action data
LFSTK_lineEditClass			*termCommand=NULL;

//font
LFSTK_fontDialogClass		*fontDialogButton=NULL;
LFSTK_lineEditClass			*fontEdit=NULL;

int							dockGravityPref=2;
int							dockSizePref=3;

//dock select
LFSTK_buttonClass			*selectDock=NULL;
LFSTK_lineEditClass			*dockNameEdit=NULL;
infoDataStruct				**dockNames=NULL;
LFSTK_menuClass				*dockMenu=NULL;
int							dockCnt=0;
std::string					dockName="";

bool doQuit(void *p,void* ud)
{
	apc->exitValue=0;
	apc->mainLoop=false;
	return(false);
}

bool buttonCB(void *p,void* ud)
{
	const fontDataStruct		*fd;

	if(ud!=NULL)
		{
			if(strcmp((char*)ud,"SHOWPANELMENU")==0)
				dockMenu->LFSTK_showMenu();
			if(strcmp((char*)ud,"SHOWGRAVMENU")==0)
				gravMenu->LFSTK_showMenu();
			if(strcmp((char*)ud,"SHOWSIZEMENU")==0)
				sizeMenu->LFSTK_showMenu();
			if(strcmp((char*)ud,"SELECTFONT")==0)
				{
					fontDialogButton->LFSTK_showDialog(fontEdit->LFSTK_getCStr());
					fd=fontDialogButton->LFSTK_getFontData(false);
					if(fd->isValid==true)
						fontEdit->LFSTK_setBuffer(fd->fontString);
					return(true);
				}
		}
	return(true);
}

bool coleditCB(void *p,void* ud)
{
	LFSTK_lineEditClass	*ed=static_cast<LFSTK_lineEditClass*>(p);

	if(ed==NULL)
		return(true);

	if((ed->mouseEvent->state & Button2Mask)!=0)
		{
			std::string col=NULL;
			col=apc->globalLib->LFSTK_oneLiner("lfscolourchooser -w %i \"%s\"",wc->window,ed->LFSTK_getCStr());
			if(col.length()>0)
				ed->LFSTK_setBuffer(col.c_str());//TODO//
		}
	return(true);
}

void getEdits(void)
{
	prefs.prefsMap=
		{
			{LFSTK_UtilityClass::LFSTK_hashFromKey("onmonitor"),{TYPEINT,"onmonitor","",false,atoi(dockOnMonitor->LFSTK_getCStr())}},
			{LFSTK_UtilityClass::LFSTK_hashFromKey("dockgrav"),{TYPEINT,"dockgrav","",false,dockGravityPref}},
			{LFSTK_UtilityClass::LFSTK_hashFromKey("docksize"),{TYPEINT,"docksize","",false,dockSizePref}},
			{LFSTK_UtilityClass::LFSTK_hashFromKey("dockbgcolour"),{TYPESTRING,"dockbgcolour",dockBGColourEdit->LFSTK_getCStr(),false,0}},
			{LFSTK_UtilityClass::LFSTK_hashFromKey("textcolour"),{TYPESTRING,"textcolour",dockTextColourEdit->LFSTK_getCStr(),false,0}},
			{LFSTK_UtilityClass::LFSTK_hashFromKey("gadgetsleft"),{TYPESTRING,"gadgetsleft",dockGadgets->LFSTK_getCStr(),false,0}},
			{LFSTK_UtilityClass::LFSTK_hashFromKey("termcommand"),{TYPESTRING,"termcommand",termCommand->LFSTK_getCStr(),false,0}},
			{LFSTK_UtilityClass::LFSTK_hashFromKey("font"),{TYPESTRING,"font",fontEdit->LFSTK_getCStr(),false,0}},
			{LFSTK_UtilityClass::LFSTK_hashFromKey("refreshrate"),{TYPESTRING,"refreshrate",dockRefreshEdit->LFSTK_getCStr(),false,0}},
			{LFSTK_UtilityClass::LFSTK_hashFromKey("usemicroseconds"),{TYPEBOOL,"usemicroseconds","",dockUseMicro->LFSTK_getValue(),0}},
			{LFSTK_UtilityClass::LFSTK_hashFromKey("usebg"),{TYPEBOOL,"usebg","",dockUseBG->LFSTK_getValue(),0}},
			{LFSTK_UtilityClass::LFSTK_hashFromKey("usebgpath"),{TYPESTRING,"usebgpath",dockBGPathEdit->LFSTK_getCStr(),false,0}},
		};
}

bool applyCB(void *p,void* ud)
{
	std::map<int,const char*>::const_iterator	it;
	std::string									prefsfile;

	if(ud!=NULL)
		{
//dock grav
			if(isdigit(dockGravEdit->LFSTK_getCStr()[0])==true)
				dockGravityPref=atoi(dockGravEdit->LFSTK_getCStr());
			else
				{
					for(it=dockGravConvertToStr.begin();it != dockGravConvertToStr.end();++it)
						{
							if((it->second!=NULL) && (strcmp(it->second,dockGravEdit->LFSTK_getCStr())==0))
								{
									dockGravityPref=it->first;
									break;
								}
						}
				}
//dock size
			if(isdigit(dockSizeEdit->LFSTK_getCStr()[0])==true)
				dockSizePref=atoi(dockSizeEdit->LFSTK_getCStr());
			else
				{
					for(it=dockSizeConvertToStr.begin();it != dockSizeConvertToStr.end();++it)
						{
							if((it->second!=NULL) && (strcmp(it->second,dockSizeEdit->LFSTK_getCStr())==0))
								{
									dockSizePref=it->first;
									break;
								}
						}
				}
			getEdits();
			prefsfile=apc->configDir+"/"+dockName;
			prefs.LFSTK_saveVarsToFile(prefsfile.c_str());
			//prefs.LFSTK_saveVarsToFile("-");
			system("killall lfsdock");
		}
	return(true);
}

void setEdits(void)
{
	dockSizeEdit->LFSTK_setBuffer(dockSizeConvertToStr[prefs.LFSTK_getInt("docksize")]);
	dockGravEdit->LFSTK_setBuffer(dockGravConvertToStr[prefs.LFSTK_getInt("dockgrav")]);
	dockOnMonitor->LFSTK_setBuffer(std::to_string(prefs.LFSTK_getInt("onmonitor")).c_str());
	termCommand->LFSTK_setBuffer(prefs.LFSTK_getCString("termcommand"));
	dockGadgets->LFSTK_setBuffer(prefs.LFSTK_getCString("gadgetsleft"));
	dockBGColourEdit->LFSTK_setBuffer(prefs.LFSTK_getCString("dockbgcolour"));
	dockTextColourEdit->LFSTK_setBuffer(prefs.LFSTK_getCString("textcolour"));
	fontEdit->LFSTK_setBuffer(prefs.LFSTK_getCString("font"));
	dockRefreshEdit->LFSTK_setBuffer(prefs.LFSTK_getCString("refreshrate"));
	dockUseMicro->LFSTK_setValue(prefs.LFSTK_getBool("usemicroseconds"));

	dockUseBG->LFSTK_setValue(prefs.LFSTK_getBool("usebg"));
	dockBGPathEdit->LFSTK_setBuffer(prefs.LFSTK_getCString("usebgpath"));
}

void getPrefs(void)
{
	char	*env=NULL;

	prefs.prefsMap=
		{
			{LFSTK_UtilityClass::LFSTK_hashFromKey("docksize"),{TYPEINT,"docksize","",false,3}},
			{LFSTK_UtilityClass::LFSTK_hashFromKey("onmonitor"),{TYPEINT,"onmonitor","",false,0}},
			{LFSTK_UtilityClass::LFSTK_hashFromKey("dockgrav"),{TYPEINT,"dockgrav","",false,2}},
			{LFSTK_UtilityClass::LFSTK_hashFromKey("termcommand"),{TYPESTRING,"termcommand","kkterminal -m -l -e ",false,0}},
			{LFSTK_UtilityClass::LFSTK_hashFromKey("gadgetsleft"),{TYPESTRING,"gadgetsleft","LCSD",false,0}},
			{LFSTK_UtilityClass::LFSTK_hashFromKey("dockbgcolour"),{TYPESTRING,"dockbgcolour","#00000000",false,0}},
			{LFSTK_UtilityClass::LFSTK_hashFromKey("textcolour"),{TYPESTRING,"textcolour","black",false,0}},
			{LFSTK_UtilityClass::LFSTK_hashFromKey("font"),{TYPESTRING,"font","Liberation Mono:size=12",false,0}},
			{LFSTK_UtilityClass::LFSTK_hashFromKey("refreshrate"),{TYPESTRING,"refreshrate","1",false,0}},
			{LFSTK_UtilityClass::LFSTK_hashFromKey("usemicroseconds"),{TYPEBOOL,"usemicroseconds","",false,0}},
			{LFSTK_UtilityClass::LFSTK_hashFromKey("usebg"),{TYPEBOOL,"usebg","",false,0}},
			{LFSTK_UtilityClass::LFSTK_hashFromKey("usebgpath"),{TYPESTRING,"usebgpath","",false,0}},
		};

	asprintf(&env,"%s/%s",apc->configDir.c_str(),dockName.c_str());
	prefs.LFSTK_loadVarsFromFile(env);
	free(env);
}

bool dockGravCB(void *p,void* ud)
{
	static_cast<LFSTK_gadgetClass*>(p)->wc->LFSTK_hideWindow();
	dockGravEdit->LFSTK_setBuffer(dockGravConvertToStr[(int)(long)ud]);
	return(true);
}

bool dockSizeCB(void *p,void* ud)
{
	static_cast<LFSTK_gadgetClass*>(p)->wc->LFSTK_hideWindow();
	dockSizeEdit->LFSTK_setBuffer(dockSizeConvertToStr[(int)(long)ud]);
	return(true);
}

bool dockSelectCB(void *p,void* ud)
{
	static_cast<LFSTK_gadgetClass*>(p)->wc->LFSTK_hideWindow();
	dockNameEdit->LFSTK_setBuffer(static_cast<LFSTK_gadgetClass*>(p)->LFSTK_getLabel());
	dockName=dockNameEdit->LFSTK_getBuffer();
	getPrefs();
	setEdits();
	return(true);
}

int main(int argc, char **argv)
{
	int				sy=0;
	int				parentWindow=-1;
	int				c=0;
	int				option_index=0;
	const char		*shortOpts="h?w:";		
	option			longOptions[]=
		{
			{"window",1,0,'w'},
			{"dock",1,0,'d'},
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
					case 'd':
						dockName=optarg;
						break;
				}
		}

	apc=new LFSTK_applicationClass();
	apc->LFSTK_addWindow(NULL,BOXLABEL,"LFSTKPrefs");
	wc=apc->mainWindow;

	copyrite=new LFSTK_labelClass(wc,COPYRITE,BORDER,sy,DIALOGWIDTH-BORDER-BORDER,GADGETHITE);
	sy+=HALFYSPACING;
	personal=new LFSTK_labelClass(wc,PERSONAL,BORDER,sy,DIALOGWIDTH-BORDER-BORDER,GADGETHITE);
	personal->LFSTK_setCairoFontDataParts("B");
	sy+=YSPACING;

//docks
	LFSTK_findClass	*find;

	find=new LFSTK_findClass;
	find->LFSTK_setFindType(FILETYPE);
	find->LFSTK_setIgnoreBroken(true);
	find->LFSTK_setSortDecending(true);
	find->LFSTK_setIgnoreFolders(true);
	find->LFSTK_setFileTypes(".rc");
	find->LFSTK_setNameFilter("lfsdock");
	find->LFSTK_findFiles(apc->configDir.c_str(),false);
	find->LFSTK_sortByName();

	dockCnt=find->LFSTK_getDataCount();

	dockNames=new infoDataStruct*[dockCnt];
	for(int j=0;j<dockCnt;j++)
		{
			dockNames[j]=new infoDataStruct;
			dockNames[j]->label=strdup(find->data.at(j).name.c_str());
			dockNames[j]->userData=(void*)(long)j;
		}

//dock config
//select
	if(dockName.length()==0)
		dockName="lfsdock-MAINDOCK.rc";
	else
		dockName="lfsdock-"+dockName+".rc";
	selectDock=new LFSTK_buttonClass(wc,"Dock Config",BORDER,sy,GADGETWIDTH,GADGETHITE,BUTTONGRAV);
	selectDock->LFSTK_setIndicator(DISCLOSURE);

	selectDock->LFSTK_setMouseCallBack(NULL,buttonCB,(void*)"SHOWPANELMENU");
	dockMenu=new LFSTK_menuClass(wc,BORDER+GADGETWIDTH,sy,1,1);
	dockMenu->LFSTK_setMouseCallBack(NULL,dockSelectCB,NULL);
	dockMenu->LFSTK_addMainMenus(dockNames,dockCnt);

	dockNameEdit=new LFSTK_lineEditClass(wc,dockName.c_str(),BORDER+GADGETWIDTH+BORDER,sy,GADGETWIDTH*2,GADGETHITE,BUTTONGRAV);	
	sy+=YSPACING;

//do prefs
	getPrefs();
//size
	dockSize=new LFSTK_buttonClass(wc,"Dock Size",BORDER,sy,GADGETWIDTH,GADGETHITE,BUTTONGRAV);
	dockSize->LFSTK_setIndicator(DISCLOSURE);
	dockSize->LFSTK_setMouseCallBack(NULL,buttonCB,(void*)"SHOWSIZEMENU");
	dockSizeMenuGrav=new infoDataStruct*[4];
	for(long j=0;j<4;j++)
		{
			dockSizeMenuGrav[j]=new infoDataStruct;
			dockSizeMenuGrav[j]->label=strdup(dockSizeConvertToStr[j+1]);
			dockSizeMenuGrav[j]->userData=(void*)(j+1);
		}
	sizeMenu=new LFSTK_menuClass(wc,BORDER+GADGETWIDTH,sy,1,1);
	sizeMenu->LFSTK_setMouseCallBack(NULL,dockSizeCB,NULL);
	sizeMenu->LFSTK_addMainMenus(dockSizeMenuGrav,4);
	dockSizeEdit=new LFSTK_lineEditClass(wc,"",BORDER+GADGETWIDTH+BORDER,sy,GADGETWIDTH*2,GADGETHITE,BUTTONGRAV);
	sy+=YSPACING;

//dock grav
	dockGrav=new LFSTK_buttonClass(wc,"Dock Gravity",BORDER,sy,GADGETWIDTH,GADGETHITE,BUTTONGRAV);
	dockGrav->LFSTK_setIndicator(DISCLOSURE);
	dockGrav->LFSTK_setMouseCallBack(NULL,buttonCB,(void*)"SHOWGRAVMENU");
	dockGravMenu=new infoDataStruct*[4];
	for(long j=0;j<2;j++)
		{
			dockGravMenu[j]=new infoDataStruct;
			dockGravMenu[j]->label=strdup(dockGravConvertToStr[j+1]);
			dockGravMenu[j]->userData=(void*)(j+1);
		}
	gravMenu=new LFSTK_menuClass(wc,BORDER+GADGETWIDTH,sy,1,1);
	gravMenu->LFSTK_setMouseCallBack(NULL,dockGravCB,NULL);
	gravMenu->LFSTK_addMainMenus(dockGravMenu,2);
	dockGravEdit=new LFSTK_lineEditClass(wc,"",BORDER+GADGETWIDTH+BORDER,sy,GADGETWIDTH*2,GADGETHITE,BUTTONGRAV);
	sy+=YSPACING;

//font
	fontDialogButton=new LFSTK_fontDialogClass(wc,"Dock Font",BORDER,sy,GADGETWIDTH,GADGETHITE,BUTTONGRAV);
	fontDialogButton->LFSTK_setMouseCallBack(NULL,buttonCB,(void*)"SELECTFONT");
	fontEdit=new LFSTK_lineEditClass(wc,apc->globalLib->LFSTK_getGlobalString(0,TYPEFONT),BORDER+GADGETWIDTH+BORDER,sy,GADGETWIDTH*2,GADGETHITE,BUTTONGRAV);
	sy+=YSPACING;

//dock back
	dockUseBG=new LFSTK_toggleButtonClass(wc,"Use BG",BORDER,sy,GADGETWIDTH,GADGETHITE,BUTTONGRAV);
	//dockUseBG->LFSTK_setMouseCallBack(NULL,buttonCB,(void*)toggle->LFSTK_getLabel().c_str());
	//dockUseBG->LFSTK_setMouseMoveCallBack(enterCB,exitCB,USERDATA(0));
	dockUseBG->LFSTK_setToggleStyle(TOGGLENORMAL);
	dockUseBG->userData=USERDATA(1);
	dockBGPathEdit=new LFSTK_lineEditClass(wc,"",BORDER+GADGETWIDTH+BORDER,sy,GADGETWIDTH*2,GADGETHITE,BUTTONGRAV);
	//dockBGPathEdit->LFSTK_setMouseCallBack(NULL,coleditCB,NULL);
	sy+=YSPACING;

//dock colour
	label=new LFSTK_labelClass(wc,"Dock Colour",BORDER,sy,GADGETWIDTH,GADGETHITE,LEFT);
	dockBGColourEdit=new LFSTK_lineEditClass(wc,"",BORDER+GADGETWIDTH+BORDER,sy,GADGETWIDTH*2,GADGETHITE,BUTTONGRAV);
	dockBGColourEdit->LFSTK_setMouseCallBack(NULL,coleditCB,NULL);
	sy+=YSPACING;
	
//dock text colour
	label=new LFSTK_labelClass(wc,"Text Colour",BORDER,sy,GADGETWIDTH,GADGETHITE,LEFT);
	dockTextColourEdit=new LFSTK_lineEditClass(wc,"black",BORDER+GADGETWIDTH+BORDER,sy,GADGETWIDTH*2,GADGETHITE,BUTTONGRAV);
	dockTextColourEdit->LFSTK_setMouseCallBack(NULL,coleditCB,NULL);
	sy+=YSPACING;

//on monitor
	label=new LFSTK_labelClass(wc,"On Monitor",BORDER,sy,GADGETWIDTH,GADGETHITE,LEFT);
	dockOnMonitor=new LFSTK_lineEditClass(wc,"",BORDER+GADGETWIDTH+BORDER,sy,GADGETWIDTH*2,GADGETHITE,BUTTONGRAV);
	sy+=YSPACING;

//term comm
	label=new LFSTK_labelClass(wc,"Terminal CMD",BORDER,sy,GADGETWIDTH,GADGETHITE,LEFT);
	termCommand=new LFSTK_lineEditClass(wc,"",BORDER+GADGETWIDTH+BORDER,sy,GADGETWIDTH*2,GADGETHITE,BUTTONGRAV);
	sy+=YSPACING;

//left gadgets
	label=new LFSTK_labelClass(wc,"Gadgets",BORDER,sy,GADGETWIDTH,GADGETHITE,LEFT);
	dockGadgets=new LFSTK_lineEditClass(wc,"",BORDER+GADGETWIDTH+BORDER,sy,GADGETWIDTH*2,GADGETHITE,BUTTONGRAV);
	sy+=YSPACING;

//set refresh rate
	label=new LFSTK_labelClass(wc,"Refresh",BORDER,sy,GADGETWIDTH,GADGETHITE,LEFT);
	dockRefreshEdit=new LFSTK_lineEditClass(wc,prefs.LFSTK_getCString("refreshrate"),BORDER+GADGETWIDTH+BORDER,sy,GADGETWIDTH-(GADGETWIDTH/3),GADGETHITE,BUTTONGRAV);
	dockUseMicro=new LFSTK_toggleButtonClass(wc,"Use uSeconds",BORDER+GADGETWIDTH+BORDER+(GADGETWIDTH-(GADGETWIDTH/3))+10,sy+(GADGETHITE/3),GADGETWIDTH,CHECKBOXSIZE,NorthWestGravity);
	dockUseMicro->LFSTK_setValue(prefs.LFSTK_getBool("usemicroseconds"));
	sy+=YSPACING;

	setEdits();

//line
	seperator=new LFSTK_buttonClass(wc,"--",0,sy,DIALOGWIDTH,GADGETHITE,BUTTONGRAV);
	seperator->LFSTK_setStyle(BEVELNONE);
	seperator->gadgetDetails.buttonTile=false;
	seperator->gadgetDetails.colour=&wc->windowColourNames[NORMALCOLOUR];
	sy+=YSPACING;

//quit
	quit=new LFSTK_buttonClass(wc,"Quit",BORDER,sy,GADGETWIDTH,GADGETHITE,BUTTONGRAV);
	quit->LFSTK_setMouseCallBack(NULL,doQuit,NULL);

//apply
	apply=new LFSTK_buttonClass(wc,"Apply",DIALOGWIDTH-BORDER-GADGETWIDTH,sy,GADGETWIDTH,GADGETHITE,BUTTONGRAV);
	apply->LFSTK_setMouseCallBack(NULL,applyCB,(void*)"APPLY");
	sy+=YSPACING;

	wc->LFSTK_resizeWindow(DIALOGWIDTH,sy,true);
	wc->LFSTK_showWindow();
	wc->LFSTK_setKeepAbove(true);
	if(parentWindow!=-1)
		wc->LFSTK_setTransientFor(parentWindow);

	int retval=apc->LFSTK_runApp();

	delete gravMenu;
	delete sizeMenu;
	delete apc;

	return(retval);
}



