/*
 *
 * ©K. D. Hedger. Wed Jan 10 20:31:07 GMT 2018 keithdhedger@gmail.com

 * This file (prefs.cpp) is part of LFSDesktop.

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

#include "globals.h"

const char			*iconTheme;
LFSTK_prefsClass	prefs;

//desktop prefs
int					iconSize=40;
int					gridBorderLeft=2;
int					gridBorderRight=2;
int					gridSize=iconSize+gridBorderLeft;
int					refreshRate=2;
const char			*terminalCommand=NULL;
const char			*fontFace=NULL;
const char			*backCol=NULL;
const char			*foreCol=NULL;
const char			*backAlpha;
const char			*includeList=NULL;
const char			*excludeList=NULL;
bool				doubleClickExecute=false;

bool loadCacheFile(const char *cachefilepath,desktopItemStruct *cfd)
{
	LFSTK_prefsClass cacheprefs;

	if(access(cachefilepath,F_OK)!=0)
		return(false);

	cacheprefs.prefsMap=
		{
			{cacheprefs.LFSTK_hashFromKey("uuid"),{TYPESTRING,"uuid","",false,0}},
			{cacheprefs.LFSTK_hashFromKey("posx"),{TYPEINT,"posx","",false,0}},
			{cacheprefs.LFSTK_hashFromKey("posy"),{TYPEINT,"posy","",false,0}},
			{cacheprefs.LFSTK_hashFromKey("hascustomicon"),{TYPEBOOL,"hascustomicon","",false,0}},
			{cacheprefs.LFSTK_hashFromKey("pathtocustomicon"),{TYPESTRING,"pathtocustomicon","",false,0}}
		};

	cacheprefs.LFSTK_loadVarsFromFile(cachefilepath);

	cfd->uuid=strdup(cacheprefs.LFSTK_getCString("uuid"));
	cfd->posx=cacheprefs.LFSTK_getInt("posx");
	cfd->posy=cacheprefs.LFSTK_getInt("posy");
	cfd->hasCustomIcon=cacheprefs.LFSTK_getBool("hascustomicon");
	cfd->pathToCustomIcon=strdup(cacheprefs.LFSTK_getCString("pathtocustomicon"));
	//cfd->pathToCustomIcon=NULL;//TODO//
	//fprintf(stderr,"--->>>%s<<<---\n",cfd->pathToCustomIcon);
	return(true);
}

void saveCacheFile(const char *cachefilepath,desktopItemStruct *cfd)
{
	LFSTK_prefsClass cacheprefs;
	const char *iconpath;

	if(cfd->pathToCustomIcon==NULL)
		iconpath="\"\"";
	else
		iconpath=cfd->pathToCustomIcon;

	cacheprefs.prefsMap=
		{
			{cacheprefs.LFSTK_hashFromKey("uuid"),{TYPESTRING,"uuid",cfd->uuid,false,0}},
			{cacheprefs.LFSTK_hashFromKey("posx"),{TYPEINT,"posx","",false,cfd->posx}},
			{cacheprefs.LFSTK_hashFromKey("posy"),{TYPEINT,"posy","",false,cfd->posy}},
			{cacheprefs.LFSTK_hashFromKey("hascustomicon"),{TYPEBOOL,"hascustomicon","",cfd->hasCustomIcon,0}},
			{cacheprefs.LFSTK_hashFromKey("pathtocustomicon"),{TYPESTRING,"pathtocustomicon",iconpath,false,0}}
		};

	cacheprefs.LFSTK_saveVarsToFile(cachefilepath);
}

void setDeskNamesProp(void)
{
	const char				*x1;
	std::string				x="";
	std::string				names=prefs.LFSTK_getString("desknames");

	x=LFSTK_UtilityClass::LFSTK_strReplaceAllChar(names,",",std::string("")+='\0');
	x+='\0';
	x1=x.c_str();
	XChangeProperty(apc->display,apc->rootWindow,XInternAtom(apc->display,"_NET_DESKTOP_NAMES",false),
	XInternAtom(apc->display,"UTF8_STRING",false),
	8,
	PropModeReplace,
	(const unsigned char*)*&x1
	,names.length()+1
	);

	XSync(apc->display,false);
	//system("xprop -root _NET_DESKTOP_NAMES");
}

void loadPrefs(void)
{
//{(".*"),(.*),&.*}
//{prefs.LFSTK_hashFromKey(\1),{\2,\1,"",false,0}}
	prefs.prefsMap=
		{
			{prefs.LFSTK_hashFromKey("icontheme"),{TYPESTRING,"icontheme","gnome",false,0}},
			{prefs.LFSTK_hashFromKey("iconsize"),{TYPEINT,"iconsize","",false,32}},
			{prefs.LFSTK_hashFromKey("gridsize"),{TYPEINT,"gridsize","",false,64}},
			{prefs.LFSTK_hashFromKey("gridborderleft"),{TYPEINT,"gridborderleft","",false,2}},
			{prefs.LFSTK_hashFromKey("gridborderrite"),{TYPEINT,"gridborderrite","",false,2}},
			{prefs.LFSTK_hashFromKey("refreshrate"),{TYPEINT,"refreshrate","",false,2}},
			{prefs.LFSTK_hashFromKey("termcommand"),{TYPESTRING,"termcommand","xterm -e ",false,0}},
			{prefs.LFSTK_hashFromKey("showextension"),{TYPEBOOL,"showextension","",false,0}},
			{prefs.LFSTK_hashFromKey("fontface"),{TYPESTRING,"fontface","sans:size=10:bold",false,0}},
			{prefs.LFSTK_hashFromKey("labelbackground"),{TYPESTRING,"labelbackground","#000000",false,0}},
			{prefs.LFSTK_hashFromKey("labelforeground"),{TYPESTRING,"labelforeground","#ffffff",false,0}},
			{prefs.LFSTK_hashFromKey("labelalpha"),{TYPESTRING,"labelalpha","1.0",false,0}},
			{prefs.LFSTK_hashFromKey("includelist"),{TYPESTRING,"includelist","",false,0}},
			{prefs.LFSTK_hashFromKey("excludelist"),{TYPESTRING,"excludelist","",false,0}},
			{prefs.LFSTK_hashFromKey("doubleclickexe"),{TYPEBOOL,"doubleclickexe","",false,0}},
			{prefs.LFSTK_hashFromKey("desknames"),{TYPESTRING,"desknames","Desktop 1,Desktop 2,Desktop 3,Desktop 4,Desktop 5,Desktop 6",false,0}},
		};

	prefs.LFSTK_loadVarsFromFile(prefsPath);

	iconTheme=prefs.LFSTK_getCString("icontheme");
	iconSize=prefs.LFSTK_getInt("iconsize");
	gridSize=prefs.LFSTK_getInt("gridsize");;
	gridBorderLeft=prefs.LFSTK_getInt("gridborderleft");;
	gridBorderRight=prefs.LFSTK_getInt("gridborderrite");;
	terminalCommand=prefs.LFSTK_getCString("termcommand");
	showSuffix=prefs.LFSTK_getBool("showextension");;
	fontFace=prefs.LFSTK_getCString("fontface");
	backCol=prefs.LFSTK_getCString("labelbackground");
	foreCol=prefs.LFSTK_getCString("labelforeground");
	backAlpha=prefs.LFSTK_getCString("labelalpha");
	refreshRate=prefs.LFSTK_getInt("refreshrate");
	includeList=prefs.LFSTK_getCString("includelist");
	excludeList=prefs.LFSTK_getCString("excludelist");
	doubleClickExecute=prefs.LFSTK_getBool("doubleclickexe");
	setDeskNamesProp();
}

void reloadPrefs(void)
{
	apc->globalLib->LFSTK_reloadPrefs();
	loadPrefs();
	for(unsigned j=0;j<desktopItems.size();j++)
		{
			setIconImage(&desktopItems.at(j));
			desktopItems.at(j).item->LFSTK_setImageFromPath(desktopItems.at(j).iconPath,TOOLBAR,true);
			desktopItems.at(j).item->LFSTK_setLabelBGColour(backCol,strtod(backAlpha,NULL));
			desktopItems.at(j).item->newGadgetFGColours[NORMALCOLOUR]=desktopItems.at(j).item->LFSTK_setColour(foreCol);

			desktopItems.at(j).item->LFSTK_setFontString(fontFace,true);
			setItemSize(&desktopItems.at(j));//TODO//
			desktopItems.at(j).item->LFSTK_clearWindow();
		}
	XSync(apc->display,false);
}
