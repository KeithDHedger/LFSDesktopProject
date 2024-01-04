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
#include <alloca.h>
#include <limits.h>

#include "config.h"
#include <lfstk/LFSTKGlobals.h>

#define EDITBOXWIDTH	GADGETWIDTH*2
#define BOXLABEL		"LFS Toolkit Prefs"

LFSTK_applicationClass	*apc=NULL;
LFSTK_windowClass		*wc=NULL;
LFSTK_prefsClass		prefs;
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

//troughs lists and scrollbars
LFSTK_lineEditClass		*scrollTroughColourEdit=NULL;
LFSTK_lineEditClass		*listTroughColourEdit=NULL;
LFSTK_buttonClass		*scrollTrough=NULL;
LFSTK_buttonClass		*listTrough=NULL;

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
LFSTK_buttonClass		*loadSet=NULL;
infoDataStruct				**setNameMenuItems=NULL;
LFSTK_menuClass			*setMenu=NULL;

int						setCnt;
char						*wd;
int						parentWindow=-1;
int						queueID=-1;
char						*themePath=strdup("");
char						*sbTroughColour=strdup("grey50");
char						*listTroughColour=strdup("grey50");

void addSet(void)
{
	LFSTK_findClass	*fc=new LFSTK_findClass;
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

	setNameMenuItems=new infoDataStruct*[fc->data.size()];
	for(int j=0;j<fc->data.size();j++)
		{
			setNameMenuItems[j]=new infoDataStruct;
			setNameMenuItems[j]->label=strdup(fc->data.at(j).path.c_str());
		}
	setCnt=fc->data.size();
	delete fc;
	free(themeDir);
}

void setPreviewData(void);
bool menuCB(void *p,void* ud)
{
	char		*buffer=(char*)malloc(PATH_MAX);

	static_cast<LFSTK_gadgetClass*>(p)->wc->LFSTK_hideWindow();
	const char *label=static_cast<LFSTK_gadgetClass*>(p)->LFSTK_getLabel().c_str();
	freeAndNull(&themePath);
	themePath=strdup(label);

	prefs.prefsMap=
		{
			{prefs.LFSTK_hashFromKey("window_normal"),{TYPESTRING,"window_normal","",false,0}},
			{prefs.LFSTK_hashFromKey("window_prelight"),{TYPESTRING,"window_prelight","",false,0}},
			{prefs.LFSTK_hashFromKey("window_active"),{TYPESTRING,"window_active","",false,0}},
			{prefs.LFSTK_hashFromKey("window_inactive"),{TYPESTRING,"window_inactive","",false,0}},
			{prefs.LFSTK_hashFromKey("windowtile"),{TYPESTRING,"windowtile","",false,0}},

			{prefs.LFSTK_hashFromKey("button_normal"),{TYPESTRING,"button_normal","",false,0}},
			{prefs.LFSTK_hashFromKey("button_prelight"),{TYPESTRING,"button_prelight","",false,0}},
			{prefs.LFSTK_hashFromKey("button_active"),{TYPESTRING,"button_active","",false,0}},
			{prefs.LFSTK_hashFromKey("button_inactive"),{TYPESTRING,"button_inactive","",false,0}},
			{prefs.LFSTK_hashFromKey("buttontile"),{TYPESTRING,"buttontile","",false,0}},

			{prefs.LFSTK_hashFromKey("menuitem_normal"),{TYPESTRING,"menuitem_normal","",false,0}},
			{prefs.LFSTK_hashFromKey("menuitem_prelight"),{TYPESTRING,"menuitem_prelight","",false,0}},
			{prefs.LFSTK_hashFromKey("menuitem_active"),{TYPESTRING,"menuitem_active","",false,0}},
			{prefs.LFSTK_hashFromKey("menuitem_inactive"),{TYPESTRING,"menuitem_inactive","",false,0}},

			{prefs.LFSTK_hashFromKey("menuitem_font"),{TYPESTRING,"menuitem_font","",false,0}},
			{prefs.LFSTK_hashFromKey("menuitem_font_normal"),{TYPESTRING,"menuitem_font_normal","",false,0}},
			{prefs.LFSTK_hashFromKey("menuitem_font_prelight"),{TYPESTRING,"menuitem_font_prelight","",false,0}},
			{prefs.LFSTK_hashFromKey("menuitem_font_active"),{TYPESTRING,"menuitem_font_active","",false,0}},
			{prefs.LFSTK_hashFromKey("menuitem_font_inactive"),{TYPESTRING,"menuitem_font_inactive","",false,0}},
			{prefs.LFSTK_hashFromKey("menuitemtile"),{TYPESTRING,"menuitemtile","",false,0}},

			{prefs.LFSTK_hashFromKey("font"),{TYPESTRING,"font","",false,0}},
			{prefs.LFSTK_hashFromKey("font_normal"),{TYPESTRING,"font_normal","",false,0}},
			{prefs.LFSTK_hashFromKey("font_prelight"),{TYPESTRING,"font_prelight","",false,0}},
			{prefs.LFSTK_hashFromKey("font_active"),{TYPESTRING,"font_active","",false,0}},
			{prefs.LFSTK_hashFromKey("font_inactive"),{TYPESTRING,"font_inactive","",false,0}},

			{prefs.LFSTK_hashFromKey("autotextcolour"),{TYPEBOOL,"autotextcolour","",false,0}},
			{prefs.LFSTK_hashFromKey("usetheme"),{TYPEBOOL,"usetheme","",false,0}},
			{prefs.LFSTK_hashFromKey("themepath"),{TYPESTRING,"themepath","",false,0}},
			{prefs.LFSTK_hashFromKey("monofont"),{TYPESTRING,"monofont","",false,0}},
			{prefs.LFSTK_hashFromKey("cursorcolour"),{TYPESTRING,"cursorcolour","",false,0}},
			{prefs.LFSTK_hashFromKey("sbtroughcolour"),{TYPESTRING,"sbtroughcolour","",false,0}},
			{prefs.LFSTK_hashFromKey("listtroughcolour"),{TYPESTRING,"listtroughcolour","",false,0}}
		};

	sprintf(buffer,"%s/window.png",label);
	windowTileEdit->LFSTK_setBuffer(buffer);
	sprintf(buffer,"%s/button.png",label);
	buttonTileEdit->LFSTK_setBuffer(buffer);
	sprintf(buffer,"%s/menuitem.png",label);
	menuTileEdit->LFSTK_setBuffer(buffer);

	sprintf(buffer,"%s/lfstoolkit.rc",label);
	prefs.LFSTK_loadVarsFromFile(buffer);
	prefs.LFSTK_saveVarsToFile("-");
//button back
	previeBackColourEdit[0]->LFSTK_setBuffer(prefs.LFSTK_getCString("button_normal"));
	previeBackColourEdit[1]->LFSTK_setBuffer(prefs.LFSTK_getCString("button_prelight"));
	previeBackColourEdit[2]->LFSTK_setBuffer(prefs.LFSTK_getCString("button_active"));
	previeBackColourEdit[3]->LFSTK_setBuffer(prefs.LFSTK_getCString("button_inactive"));
//button font
	previeFontColourEdit[0]->LFSTK_setBuffer(prefs.LFSTK_getCString("font_normal"));
	previeFontColourEdit[1]->LFSTK_setBuffer(prefs.LFSTK_getCString("font_prelight"));
	previeFontColourEdit[2]->LFSTK_setBuffer(prefs.LFSTK_getCString("font_active"));
	previeFontColourEdit[3]->LFSTK_setBuffer(prefs.LFSTK_getCString("font_inactive"));
//menu item
	previeMenuBackColourEdit[0]->LFSTK_setBuffer(prefs.LFSTK_getCString("menuitem_normal"));
	previeMenuBackColourEdit[1]->LFSTK_setBuffer(prefs.LFSTK_getCString("menuitem_prelight"));
	previeMenuBackColourEdit[2]->LFSTK_setBuffer(prefs.LFSTK_getCString("menuitem_active"));
	previeMenuBackColourEdit[3]->LFSTK_setBuffer(prefs.LFSTK_getCString("menuitem_inactive"));
//menu item font
	previeMenuFontColourEdit[0]->LFSTK_setBuffer(prefs.LFSTK_getCString("menuitem_font_normal"));
	previeMenuFontColourEdit[1]->LFSTK_setBuffer(prefs.LFSTK_getCString("menuitem_font_prelight"));
	previeMenuFontColourEdit[2]->LFSTK_setBuffer(prefs.LFSTK_getCString("menuitem_font_active"));
	previeMenuFontColourEdit[3]->LFSTK_setBuffer(prefs.LFSTK_getCString("menuitem_font_inactive"));

//troughs
	scrollTroughColourEdit->LFSTK_setBuffer(prefs.LFSTK_getCString("sbtroughcolour"));
	listTroughColourEdit->LFSTK_setBuffer(prefs.LFSTK_getCString("listtroughcolour"));

//window
	windowColourEdit->LFSTK_setBuffer(prefs.LFSTK_getCString("window_normal"));
//cursor
	cursorColourEdit->LFSTK_setBuffer(prefs.LFSTK_getCString("cursorcolour"));
//fonts
	buttonFontEdit->LFSTK_setBuffer(prefs.LFSTK_getCString("font"));
	menuFontEdit->LFSTK_setBuffer(prefs.LFSTK_getCString("menuitem_font"));
	monoFontEdit->LFSTK_setBuffer(prefs.LFSTK_getCString("monofont"));
//odds
	autoColourCheck->LFSTK_setValue(prefs.LFSTK_getBool("autotextcolour"));
	useTheme->LFSTK_setValue(prefs.LFSTK_getBool("usetheme"));

	freeAndNull(&themePath);
	themePath=strdup(prefs.LFSTK_getCString("themepath"));

	setPreviewData();
	free(buffer);
	return(true);
}

bool doQuit(void *p,void* ud)
{
	apc->exitValue=0;
	apc->mainLoop=false;
	return(false);
}

void setVars(void)
{
	prefs.prefsMap=
		{
			{prefs.LFSTK_hashFromKey("window_normal"),{TYPESTRING,"window_normal",windowColourEdit->LFSTK_getCStr(),false,0}},
			{prefs.LFSTK_hashFromKey("window_prelight"),{TYPESTRING,"window_prelight",windowColourEdit->LFSTK_getCStr(),false,0}},
			{prefs.LFSTK_hashFromKey("window_active"),{TYPESTRING,"window_active",windowColourEdit->LFSTK_getCStr(),false,0}},
			{prefs.LFSTK_hashFromKey("window_inactive"),{TYPESTRING,"window_inactive",windowColourEdit->LFSTK_getCStr(),false,0}},
			{prefs.LFSTK_hashFromKey("windowtile"),{TYPESTRING,"windowtile",windowTileEdit->LFSTK_getCStr(),false,0}},

			{prefs.LFSTK_hashFromKey("button_normal"),{TYPESTRING,"button_normal",previeBackColourEdit[0]->LFSTK_getCStr(),false,0}},
			{prefs.LFSTK_hashFromKey("button_prelight"),{TYPESTRING,"button_prelight",previeBackColourEdit[1]->LFSTK_getCStr(),false,0}},
			{prefs.LFSTK_hashFromKey("button_active"),{TYPESTRING,"button_active",previeBackColourEdit[2]->LFSTK_getCStr(),false,0}},
			{prefs.LFSTK_hashFromKey("button_inactive"),{TYPESTRING,"button_inactive",previeBackColourEdit[3]->LFSTK_getCStr(),false,0}},
			{prefs.LFSTK_hashFromKey("buttontile"),{TYPESTRING,"buttontile",buttonTileEdit->LFSTK_getCStr(),false,0}},

			{prefs.LFSTK_hashFromKey("menuitem_normal"),{TYPESTRING,"menuitem_normal",previeMenuBackColourEdit[0]->LFSTK_getCStr(),false,0}},
			{prefs.LFSTK_hashFromKey("menuitem_prelight"),{TYPESTRING,"menuitem_prelight",previeMenuBackColourEdit[0]->LFSTK_getCStr(),false,0}},
			{prefs.LFSTK_hashFromKey("menuitem_active"),{TYPESTRING,"menuitem_active",previeMenuBackColourEdit[2]->LFSTK_getCStr(),false,0}},
			{prefs.LFSTK_hashFromKey("menuitem_inactive"),{TYPESTRING,"menuitem_inactive",previeMenuBackColourEdit[3]->LFSTK_getCStr(),false,0}},

			{prefs.LFSTK_hashFromKey("menuitem_font"),{TYPESTRING,"menuitem_font",menuFontEdit->LFSTK_getCStr(),false,0}},
			{prefs.LFSTK_hashFromKey("menuitem_font_normal"),{TYPESTRING,"menuitem_font_normal",previeMenuFontColourEdit[0]->LFSTK_getCStr(),false,0}},
			{prefs.LFSTK_hashFromKey("menuitem_font_prelight"),{TYPESTRING,"menuitem_font_prelight",previeMenuFontColourEdit[1]->LFSTK_getCStr(),false,0}},
			{prefs.LFSTK_hashFromKey("menuitem_font_active"),{TYPESTRING,"menuitem_font_active",previeMenuFontColourEdit[2]->LFSTK_getCStr(),false,0}},
			{prefs.LFSTK_hashFromKey("menuitem_font_inactive"),{TYPESTRING,"menuitem_font_inactive",previeMenuFontColourEdit[3]->LFSTK_getCStr(),false,0}},
			{prefs.LFSTK_hashFromKey("menuitemtile"),{TYPESTRING,"menuitemtile",menuTileEdit->LFSTK_getCStr(),false,0}},

			{prefs.LFSTK_hashFromKey("font"),{TYPESTRING,"font",buttonFontEdit->LFSTK_getCStr(),false,0}},
			{prefs.LFSTK_hashFromKey("font_normal"),{TYPESTRING,"font_normal",previeFontColourEdit[0]->LFSTK_getCStr(),false,0}},
			{prefs.LFSTK_hashFromKey("font_prelight"),{TYPESTRING,"font_prelight",previeFontColourEdit[1]->LFSTK_getCStr(),false,0}},
			{prefs.LFSTK_hashFromKey("font_active"),{TYPESTRING,"font_active",previeFontColourEdit[2]->LFSTK_getCStr(),false,0}},
			{prefs.LFSTK_hashFromKey("font_inactive"),{TYPESTRING,"font_inactive",previeFontColourEdit[3]->LFSTK_getCStr(),false,0}},

			{prefs.LFSTK_hashFromKey("autotextcolour"),{TYPEBOOL,"autotextcolour","",autoColourCheck->LFSTK_getValue(),0}},
			{prefs.LFSTK_hashFromKey("usetheme"),{TYPEBOOL,"usetheme","",useTheme->LFSTK_getValue(),0}},
			{prefs.LFSTK_hashFromKey("themepath"),{TYPESTRING,"themepath",themePath,false,0}},
			{prefs.LFSTK_hashFromKey("monofont"),{TYPESTRING,"monofont",monoFontEdit->LFSTK_getCStr(),false,0}},
			{prefs.LFSTK_hashFromKey("cursorcolour"),{TYPESTRING,"cursorcolour",cursorColourEdit->LFSTK_getCStr(),false,0}},
			{prefs.LFSTK_hashFromKey("sbtroughcolour"),{TYPESTRING,"sbtroughcolour",scrollTroughColourEdit->LFSTK_getCStr(),false,0}},
			{prefs.LFSTK_hashFromKey("listtroughcolour"),{TYPESTRING,"listtroughcolour",listTroughColourEdit->LFSTK_getCStr(),false,0}}
		};
}

void setPreviewData(void)
{
	std::map<int,mappedListener*>	*ml=wc->LFSTK_getGadgets();
	if((!ml->empty()) )
		{
			if(useTheme->LFSTK_getValue()==true)
				wc->LFSTK_setTile(windowTileEdit->LFSTK_getCStr(),-1);
			else
				{
					wc->LFSTK_setTile(NULL,0);
					wc->LFSTK_setWindowColourName(0,windowColourEdit->LFSTK_getCStr());
				}
//
//			for (std::map<int,mappedListener*>::iterator it=ml->begin();it!=ml->end();++it)
//				{
//					mappedListener	*mls=it->second;
//					if (mls!=NULL)
//						{
//							if(mls->gadget!=NULL)
//								{
//									if(mls->type!=LINEEDITGADGET)
//										{
////button
//											mls->gadget->LFSTK_setFontString(buttonFontEdit->LFSTK_getCStr(),true);	
//											mls->gadget-> LFSTK_setLabelAutoColour(autoColourCheck->LFSTK_getValue());
//mls->gadget->LFSTK_setGadgetColours(GADGETBG,
//											previeBackColourEdit[NORMALCOLOUR]->LFSTK_getBuffer(),
//											previeBackColourEdit[PRELIGHTCOLOUR]->LFSTK_getBuffer(),
//											previeBackColourEdit[ACTIVECOLOUR]->LFSTK_getBuffer(),
//											previeBackColourEdit[INACTIVECOLOUR]->LFSTK_getBuffer()
//									);
//
//											////for(int j=0;j<4;j++)
//												//mls->gadget->LFSTK_setColourName(j,previeBackColourEdit[j]->LFSTK_getCStr());
//											////	mls->gadget->newGadgetBGColours[j]=mls->gadget->LFSTK_setColour(previeBackColourEdit[j]->LFSTK_getBuffer());
//											if(useTheme->LFSTK_getValue()==true)
//												{
//													if(mls->type==MENUBUTTONGADGET)
//														mls->gadget->LFSTK_setTile(menuTileEdit->LFSTK_getCStr(),-1);
//													else
//														mls->gadget->LFSTK_setTile(buttonTileEdit->LFSTK_getCStr(),-1);
//												}
//											else
//												mls->gadget->LFSTK_setTile(NULL,0);
//											if(mls->type==MENUBUTTONGADGET)
//												mls->gadget->LFSTK_setFontString(menuFontEdit->LFSTK_getCStr(),true);
////font
////mls->gadget-> LFSTK_setLabelAutoColour(true);
//mls->gadget->LFSTK_setGadgetColours(GADGETBG,
//											std::string("white"),
//											std::string("green"),
//											std::string("green"),
//											std::string("green")
//											//previeFontColourEdit[PRELIGHTCOLOUR]->LFSTK_getBuffer(),
//											//previeFontColourEdit[ACTIVECOLOUR]->LFSTK_getBuffer(),
//											//previeFontColourEdit[INACTIVECOLOUR]->LFSTK_getBuffer()
//									);
//											//std::string normal,std::string prelight,std::string active,std::string inactive)
//											//for(int j=0;j<4;j++)
//											//	mls->gadget->LFSTK_clearWindow();
//												//mls->gadget->LFSTK_setFontColourName(j,previeFontColourEdit[j]->LFSTK_getCStr(),false);
//											////	mls->gadget->newGadgetFGColours[j]=mls->gadget->LFSTK_setColour(previeFontColourEdit[j]->LFSTK_getBuffer());
//												//mls->gadget->LFSTK_setFontColourName(j,previeFontColourEdit[j]->LFSTK_getCStr(),false);
//										}
//									else
//										{
//											mls->gadget->LFSTK_setFontString(monoFontEdit->LFSTK_getCStr(),true);
//											static_cast<LFSTK_lineEditClass*>(mls->gadget)->LFSTK_setCursorColourName(cursorColourEdit->LFSTK_getCStr());
//										}
//								}
//						}
//				}
//set examples

	for (std::map<int,mappedListener*>::iterator it=ml->begin();it!=ml->end();++it)
		{
			mappedListener	*mls=it->second;
			if (mls!=NULL)
				{
					if(mls->gadget!=NULL)
						{
							if(mls->type!=LINEEDITGADGET)
								{
									if(useTheme->LFSTK_getValue()==true)
										mls->gadget->LFSTK_setTile(buttonTileEdit->LFSTK_getCStr(),-1);
									else
										mls->gadget->LFSTK_setTile(NULL,-1);
								}
						}
				}
		}



//set use theme
	if(useTheme->LFSTK_getValue()==true)
		{
			for(int j=0;j<4;j++)
				{
					previewButtons[j]->LFSTK_setTile(buttonTileEdit->LFSTK_getCStr(),-1);
					previewMenus[j]->LFSTK_setTile(menuTileEdit->LFSTK_getCStr(),-1);
				}
		}
	else
		{
			for(int j=0;j<4;j++)
				{
					previewButtons[j]->LFSTK_setTile(NULL,-1);
					previewMenus[j]->LFSTK_setTile(NULL,-1);
				}
		}

//buttons
	for(int j=0;j<4;j++)
		{
			previewButtons[j]->LFSTK_setLabelAutoColour(autoColourCheck->LFSTK_getValue());
			previewButtons[j]->LFSTK_setGadgetColourPair(NORMALCOLOUR,previeBackColourEdit[j]->LFSTK_getBuffer(),previeFontColourEdit[j]->LFSTK_getBuffer());
		}
//menus
	for(int j=0;j<4;j++)
		{
			previewMenus[j]->LFSTK_setLabelAutoColour(autoColourCheck->LFSTK_getValue());
			previewMenus[j]->LFSTK_setGadgetColourPair(NORMALCOLOUR,previeMenuBackColourEdit[j]->LFSTK_getBuffer(),previeMenuFontColourEdit[j]->LFSTK_getBuffer());
		}
//buttons font
	for(int j=0;j<4;j++)
		previewButtons[j]->LFSTK_setFontString(buttonFontEdit->LFSTK_getCStr(),true);

//menu font
	for(int j=0;j<4;j++)
		previewMenus[j]->LFSTK_setFontString(menuFontEdit->LFSTK_getCStr(),true);

//mono font
	monoFontDialogButton->LFSTK_setFontString(monoFontEdit->LFSTK_getCStr(),true);

//others
//
////menuitems
//			for(int j=0;j<4;j++)
//				//previewMenus[j]->LFSTK_setColourName(NORMALCOLOUR,previeMenuBackColourEdit[j]->LFSTK_getCStr());
//				previewMenus[j]->newGadgetBGColours[NORMALCOLOUR]=previewMenus[j]->LFSTK_setColour(previeMenuBackColourEdit[j]->LFSTK_getBuffer());
//
//			for(int j=0;j<4;j++)
//				previewMenus[j]->LFSTK_setFontString(menuFontEdit->LFSTK_getCStr(),true);
//			if(useTheme->LFSTK_getValue()==true)
//				{
//					for(int j=0;j<4;j++)
//						previewMenus[j]->LFSTK_setTile(menuTileEdit->LFSTK_getCStr(),-1);
//				}
//			
////buttons font
//			for(int j=0;j<4;j++)
//				//previewButtons[j]->LFSTK_setFontColourName(NORMALCOLOUR,previeFontColourEdit[j]->LFSTK_getCStr(),false);
//				previewButtons[j]->newGadgetFGColours[NORMALCOLOUR]=previewButtons[j]->LFSTK_setColour(previeFontColourEdit[j]->LFSTK_getBuffer());
//
////menuitems font
//			for(int j=0;j<4;j++)
//				previewMenus[j]->newGadgetFGColours[NORMALCOLOUR]=previewMenus[j]->LFSTK_setColour(previeMenuFontColourEdit[j]->LFSTK_getBuffer());
//				//previewMenus[j]->LFSTK_setFontColourName(NORMALCOLOUR,previeMenuFontColourEdit[j]->LFSTK_getCStr(),false);
		}
//
//	//cursorColourEdit->LFSTK_setColourName(NORMALCOLOUR,cursorColourEdit->LFSTK_getCStr());


	cursorColourEdit->LFSTK_setLabelAutoColour(true);
	cursorColourEdit->LFSTK_setGadgetColourPair(NORMALCOLOUR,cursorColourEdit->LFSTK_getBuffer(),"#808080");
	cursorColourEdit->LFSTK_setCursorColourName("#808080");
	//cursorColourEdit->LFSTK_setCursorColourName(cursorColourEdit->newGadgetFGColours.at(NORMALCOLOUR).name.c_str());

	scrollTroughColourEdit->LFSTK_setLabelAutoColour(true);
	scrollTroughColourEdit->LFSTK_setGadgetColourPair(NORMALCOLOUR,scrollTroughColourEdit->LFSTK_getBuffer(),"#808080");
	scrollTroughColourEdit->LFSTK_setCursorColourName("#808080");

	listTroughColourEdit->LFSTK_setLabelAutoColour(true);
	listTroughColourEdit->LFSTK_setGadgetColourPair(NORMALCOLOUR,listTroughColourEdit->LFSTK_getBuffer(),"#808080");
	listTroughColourEdit->LFSTK_setCursorColourName("#808080");

	//cursorColourEdit->newGadgetBGColours[NORMALCOLOUR]=cursorColourEdit->LFSTK_setColour(cursorColourEdit->LFSTK_getBuffer());
//	//scrollTroughColourEdit->LFSTK_setColourName(NORMALCOLOUR,scrollTroughColourEdit->LFSTK_getCStr());
//	scrollTroughColourEdit->newGadgetBGColours[NORMALCOLOUR]=scrollTroughColourEdit->LFSTK_setColour(scrollTroughColourEdit->LFSTK_getBuffer());
//	scrollTroughColourEdit->LFSTK_setCursorColourName("#808080");
//	//listTroughColourEdit->LFSTK_setColourName(NORMALCOLOUR,listTroughColourEdit->LFSTK_getCStr());
//	listTroughColourEdit->newGadgetBGColours[NORMALCOLOUR]=listTroughColourEdit->LFSTK_setColour(listTroughColourEdit->LFSTK_getBuffer());
//	listTroughColourEdit->LFSTK_setCursorColourName("#808080");
//	//windowColourEdit->LFSTK_setColourName(NORMALCOLOUR,windowColourEdit->LFSTK_getCStr());
//	windowColourEdit->newGadgetBGColours[NORMALCOLOUR]=windowColourEdit->LFSTK_setColour(windowColourEdit->LFSTK_getBuffer());
//	windowColourEdit->LFSTK_setCursorColourName("#808080");

	wc->LFSTK_clearWindow(true);
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
			wd=strdup(tileDialog->LFSTK_getCurrentDir().c_str());			
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
			if(strcmp((char*)ud,"SHOWSETMENU")==0)
				{
					setMenu->LFSTK_showMenu();
					return(true);
				}

			if(strcmp((char*)ud,"SELECTBUTTONFONT")==0)
				{
					buttonFontDialogButton->LFSTK_showDialog(buttonFontEdit->LFSTK_getCStr());
					fd=buttonFontDialogButton->LFSTK_getFontData(false);
					if(fd->isValid==true)
						buttonFontEdit->LFSTK_setBuffer(fd->fontString);
					return(true);
				}

			if(strcmp((char*)ud,"SELECTMENUFONT")==0)
				{
					menuFontDialogButton->LFSTK_showDialog(menuFontEdit->LFSTK_getCStr());
					fd=menuFontDialogButton->LFSTK_getFontData(false);
					if(fd->isValid==true)
						menuFontEdit->LFSTK_setBuffer(fd->fontString);
					return(true);
				}

			if(strcmp((char*)ud,"SELECTMONOFONT")==0)
				{
					monoFontDialogButton->LFSTK_showDialog(monoFontEdit->LFSTK_getCStr());
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
					prefs.LFSTK_saveVarsToFile(prefsfile);
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

bool coleditCB(void *p,void* ud)
{
	int					pw=parentWindow;
	LFSTK_lineEditClass	*ed=static_cast<LFSTK_lineEditClass*>(p);

	if(ed==NULL)
		return(true);

	if(pw==-1)
		pw=wc->window;
	if((ed->mouseEvent->state & Button3Mask)!=0)
		{
			char *col=NULL;
#ifdef _ENABLEDEBUG_//TODO//
			col=strdup(apc->globalLib->LFSTK_oneLiner("LD_LIBRARY_PATH=../LFSToolKit/LFSToolKit/app/.libs LFSApplications/app/lfscolourchooser -w %i \"%s\"",pw,ed->LFSTK_getCStr()).c_str());
#else
			col=strdup(apc->globalLib->LFSTK_oneLiner("lfscolourchooser -w %i \"%s\"",pw,ed->LFSTK_getCStr()).c_str());
#endif
			if(strlen(col)>0)
				ed->LFSTK_setBuffer(col);
			free(col);
			setPreviewData();
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
	std::string	command;
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
						printf("Right click in a colour edit box for a colour chooser.\n");
						exit(0);
					case 'w':
						parentWindow=atoi(optarg);
						break;
				}
		}

	apc=new LFSTK_applicationClass();
	apc->LFSTK_addWindow(NULL,BOXLABEL,"LFSTKPrefs");
	wc=apc->mainWindow;

	command=apc->globalLib->LFSTK_oneLiner("sed -n '2p' %s/lfsappearance.rc",apc->configDir.c_str());
	key=std::stoi(command,nullptr,10);
//	freeAndNull(&command);

	if((queueID=msgget(key,IPC_CREAT|0660))==-1)
		fprintf(stderr,"Can't create message queue\n");

	wd=strdup(apc->configDir.c_str());
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
			previewButtons[j]->LFSTK_setIgnores(false,true);
			sx+=GADGETWIDTH+BORDER;
			previeBackColourEdit[j]=new LFSTK_lineEditClass(wc,apc->globalLib->LFSTK_getGlobalString(j,TYPEBUTTON),sx,sy,EDITBOXWIDTH,GADGETHITE,BUTTONGRAV);
			previeBackColourEdit[j]->LFSTK_setMouseCallBack(NULL,coleditCB,NULL);
			previeBackColourEdit[j]->LFSTK_setContextWindow(NULL);
			sx+=EDITBOXWIDTH+BORDER;
			previeFontColourEdit[j]=new LFSTK_lineEditClass(wc,apc->globalLib->LFSTK_getGlobalString(j,TYPEFONTCOLOUR),sx,sy,EDITBOXWIDTH,GADGETHITE,BUTTONGRAV);
			previeFontColourEdit[j]->LFSTK_setMouseCallBack(NULL,coleditCB,NULL);
			previeFontColourEdit[j]->LFSTK_setContextWindow(NULL);
			sy+=YSPACING;
			sx=BORDER;
		}
	sx=BORDER;

//button font
	buttonFontDialogButton=new LFSTK_fontDialogClass(wc,"Button Font",sx,sy,GADGETWIDTH,GADGETHITE,BUTTONGRAV);
	buttonFontDialogButton->LFSTK_setMouseCallBack(NULL,buttonCB,(void*)"SELECTBUTTONFONT");
	sx+=GADGETWIDTH+BORDER;
	buttonFontEdit=new LFSTK_lineEditClass(wc,apc->globalLib->LFSTK_getGlobalString(0,TYPEFONT),sx,sy,EDITBOXWIDTH*2+BORDER,GADGETHITE,BUTTONGRAV);
	sy+=YSPACING;

	sx=BORDER;
//menu buttons
	for(int j=0;j<4;j++)
		{
			previewMenus[j]=new LFSTK_buttonClass(wc,previewMenuLabels[j],sx,sy,GADGETWIDTH,GADGETHITE,BUTTONGRAV);
			previewMenus[j]->LFSTK_setIgnores(false,true);
			sx+=GADGETWIDTH+BORDER;
			previeMenuBackColourEdit[j]=new LFSTK_lineEditClass(wc,apc->globalLib->LFSTK_getGlobalString(j,TYPEMENUITEM),sx,sy,EDITBOXWIDTH,GADGETHITE,BUTTONGRAV);
			sx+=EDITBOXWIDTH+BORDER;
			previeMenuFontColourEdit[j]=new LFSTK_lineEditClass(wc,apc->globalLib->LFSTK_getGlobalString(j,TYPEMENUITEMFONTCOLOUR),sx,sy,EDITBOXWIDTH,GADGETHITE,BUTTONGRAV);
			previeMenuBackColourEdit[j]->LFSTK_setMouseCallBack(NULL,coleditCB,NULL);
			previeMenuFontColourEdit[j]->LFSTK_setMouseCallBack(NULL,coleditCB,NULL);
			previeMenuBackColourEdit[j]->LFSTK_setContextWindow(NULL);
			previeMenuFontColourEdit[j]->LFSTK_setContextWindow(NULL);
			sy+=YSPACING;
			sx=BORDER;
		}
	sx=BORDER;
//menu font
	menuFontDialogButton=new LFSTK_fontDialogClass(wc,"Menu Font",sx,sy,GADGETWIDTH,GADGETHITE,BUTTONGRAV);
	menuFontDialogButton->LFSTK_setMouseCallBack(NULL,buttonCB,(void*)"SELECTMENUFONT");
	sx+=GADGETWIDTH+BORDER;
	menuFontEdit=new LFSTK_lineEditClass(wc,apc->globalLib->LFSTK_getGlobalString(0,TYPEMENUITEMFONT),sx,sy,EDITBOXWIDTH*2+BORDER,GADGETHITE,BUTTONGRAV);
	sy+=YSPACING;
	sx=BORDER;
//mono font
	monoFontDialogButton=new LFSTK_fontDialogClass(wc,"Mono Font",sx,sy,GADGETWIDTH,GADGETHITE,BUTTONGRAV);
	monoFontDialogButton->LFSTK_setMouseCallBack(NULL,buttonCB,(void*)"SELECTMONOFONT");
	sx+=GADGETWIDTH+BORDER;
	monoFontEdit=new LFSTK_lineEditClass(wc,apc->globalLib->LFSTK_getGlobalString(0,TYPEMONOFONT),sx,sy,EDITBOXWIDTH*2+BORDER,GADGETHITE,BUTTONGRAV);
	sy+=YSPACING;
	sx=BORDER;

//scroll trough prefs
	LFSTK_labelClass		*label=NULL;
	label=new LFSTK_labelClass(wc,"Scroll Trough Col",BORDER,sy,GADGETWIDTH,GADGETHITE,BUTTONGRAV);
	sx+=BORDER+GADGETWIDTH;
	scrollTroughColourEdit=new LFSTK_lineEditClass(wc,apc->globalLib->LFSTK_getGlobalString(-1,TYPESBTROUGHCOLOUR),sx,sy,EDITBOXWIDTH,GADGETHITE,BUTTONGRAV);
	scrollTroughColourEdit->LFSTK_setCursorColourName("grey80");
	scrollTroughColourEdit->LFSTK_setMouseCallBack(NULL,coleditCB,NULL);
	scrollTroughColourEdit->LFSTK_setContextWindow(NULL);
	sy+=YSPACING;
	sx=BORDER;
	
//list trough prefs
	label=new LFSTK_labelClass(wc,"List Trough Col",BORDER,sy,GADGETWIDTH,GADGETHITE,BUTTONGRAV);
	sx+=BORDER+GADGETWIDTH;
	listTroughColourEdit=new LFSTK_lineEditClass(wc,apc->globalLib->LFSTK_getGlobalString(-1,TYPELISTTROUGHCOLOUR),sx,sy,EDITBOXWIDTH,GADGETHITE,BUTTONGRAV);
	listTroughColourEdit->LFSTK_setCursorColourName("grey80");
	listTroughColourEdit->LFSTK_setMouseCallBack(NULL,coleditCB,NULL);
	listTroughColourEdit->LFSTK_setContextWindow(NULL);
	sy+=YSPACING;
	sx=BORDER;

//cursor prefs
	cursorlabel=new LFSTK_labelClass(wc,"Cursor Col",BORDER,sy,GADGETWIDTH,GADGETHITE,BUTTONGRAV);
	sx+=BORDER+GADGETWIDTH;
	cursorColourEdit=new LFSTK_lineEditClass(wc,apc->globalLib->LFSTK_getGlobalString(-1,TYPECURSORCOLOUR),sx,sy,EDITBOXWIDTH,GADGETHITE,BUTTONGRAV);
	cursorColourEdit->LFSTK_setCursorColourName("black");
	cursorColourEdit->LFSTK_setMouseCallBack(NULL,coleditCB,NULL);
	cursorColourEdit->LFSTK_setContextWindow(NULL);
	sy+=YSPACING;
	sx=BORDER;

//window
	label=new LFSTK_labelClass(wc,"Window Col",BORDER,sy,GADGETWIDTH,GADGETHITE,BUTTONGRAV);
	sx+=BORDER+GADGETWIDTH;
	windowColourEdit=new LFSTK_lineEditClass(wc,apc->globalLib->LFSTK_getGlobalString(0,TYPEWINDOW),sx,sy,EDITBOXWIDTH,GADGETHITE,BUTTONGRAV);
	windowColourEdit->LFSTK_setMouseCallBack(NULL,coleditCB,NULL);
	windowColourEdit->LFSTK_setContextWindow(NULL);
	sx+=BORDER+EDITBOXWIDTH;
	autoColourCheck=new LFSTK_toggleButtonClass(wc,"Auto Colour",sx,sy,GADGETWIDTH,CHECKBOXSIZE,NorthWestGravity);
	sy+=YSPACING;

//themes
	useTheme=new LFSTK_toggleButtonClass(wc,"Use Theme",BORDER,sy,GADGETWIDTH,CHECKBOXSIZE,NorthWestGravity);
	if(apc->globalLib->LFSTK_getThemePath()!=NULL)
		{
			freeAndNull(&themePath);
			themePath=strdup(apc->globalLib->LFSTK_getThemePath());
		}
//load set
	addSet();

	loadSet=new LFSTK_buttonClass(wc,"Load Theme",BORDER+GADGETWIDTH+BORDER,sy,GADGETWIDTH+HALFGADGETWIDTH,GADGETHITE,BUTTONGRAV);
	loadSet->LFSTK_setMouseCallBack(NULL,buttonCB,(void*)"SHOWSETMENU");
	loadSet->LFSTK_setIndicator(DISCLOSURE);
	setMenu=new LFSTK_menuClass(wc,BORDER+GADGETWIDTH+BORDER+GADGETWIDTH+HALFGADGETWIDTH,sy,1,1);
	setMenu->LFSTK_setMouseCallBack(NULL,menuCB,NULL);
	setMenu->LFSTK_addMainMenus(setNameMenuItems,setCnt);
	sy+=YSPACING;
	sx=BORDER;

	if(setCnt==0)
		loadSet->LFSTK_setActive(false);	

//window tile
	windowTile=new LFSTK_buttonClass(wc,"Window Tile",sx,sy,GADGETWIDTH,GADGETHITE,BUTTONGRAV);
	sx+=GADGETWIDTH+BORDER;
	windowTileEdit=new LFSTK_lineEditClass(wc,apc->globalLib->LFSTK_getGlobalString(0,TYPEWINDOWTILE),sx,sy,EDITBOXWIDTH*2+BORDER,GADGETHITE,BUTTONGRAV);
	windowTile->LFSTK_setMouseCallBack(NULL,selectfile,(void*)windowTileEdit);
	sy+=YSPACING;
	sx=BORDER;
//button tile
	buttonTile=new LFSTK_buttonClass(wc,"Button Tile",sx,sy,GADGETWIDTH,GADGETHITE,BUTTONGRAV);
	sx+=GADGETWIDTH+BORDER;
	buttonTileEdit=new LFSTK_lineEditClass(wc,apc->globalLib->LFSTK_getGlobalString(0,TYPEBUTTONTILE),sx,sy,EDITBOXWIDTH*2+BORDER,GADGETHITE,BUTTONGRAV);
	buttonTile->LFSTK_setMouseCallBack(NULL,selectfile,(void*)buttonTileEdit);
	sy+=YSPACING;
	sx=BORDER;
//menu tile
	menuTile=new LFSTK_buttonClass(wc,"Menu Tile",sx,sy,GADGETWIDTH,GADGETHITE,BUTTONGRAV);
	sx+=GADGETWIDTH+BORDER;
	menuTileEdit=new LFSTK_lineEditClass(wc,apc->globalLib->LFSTK_getGlobalString(0,TYPEMENUITEMTILE),sx,sy,EDITBOXWIDTH*2+BORDER,GADGETHITE,BUTTONGRAV);
	menuTile->LFSTK_setMouseCallBack(NULL,selectfile,(void*)menuTileEdit);
	sy+=YSPACING;

	autoColourCheck->LFSTK_setValue(wc->autoLabelColour);
	useTheme->LFSTK_setValue(apc->globalLib->LFSTK_getUseTheme());

	sx=BORDER;

	sx=EDITBOXWIDTH*2+(BORDER*4)+GADGETWIDTH;
//line
	seperator=new LFSTK_buttonClass(wc,"--",0,sy,DIALOGWIDTH+BORDER,GADGETHITE,BUTTONGRAV);
	seperator->LFSTK_setStyle(BEVELNONE);
	seperator->gadgetDetails.buttonTile=false;
	seperator->gadgetDetails.colour=&wc->windowColourNames[NORMALCOLOUR];
	sy+=YSPACING;

//quit
	quit=new LFSTK_buttonClass(wc,"Quit",BORDER,sy,GADGETWIDTH,GADGETHITE,BUTTONGRAV);
	quit->LFSTK_setMouseCallBack(NULL,doQuit,NULL);

//test
	test=new LFSTK_buttonClass(wc,"Test",DIALOGMIDDLE-HALFGADGETWIDTH,sy,GADGETWIDTH,GADGETHITE,BUTTONGRAV);
	test->LFSTK_setMouseCallBack(NULL,buttonCB,(void*)"TEST");

//apply
	apply=new LFSTK_buttonClass(wc,"Apply",sx-GADGETWIDTH-BORDER,sy,GADGETWIDTH,GADGETHITE,BUTTONGRAV);
	apply->LFSTK_setMouseCallBack(NULL,buttonCB,(void*)"APPLY");
	sy+=YSPACING;

	setPreviewData();
	wc->LFSTK_resizeWindow(sx,sy,true);
	wc->LFSTK_showWindow();
	wc->LFSTK_setKeepAbove(true);
	if(parentWindow!=-1)
		wc->LFSTK_setTransientFor(parentWindow);

	int retval=apc->LFSTK_runApp();

	delete tileDialog;
	delete setMenu;
	delete apc;
	freeAndNull(&sbTroughColour);
	return(retval);
}
