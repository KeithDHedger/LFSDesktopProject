/*
 *
 * ©K. D. Hedger. Tue 21 Jan 15:56:50 GMT 2025 keithdhedger@gmail.com

 * This file (main.cpp) is part of LFSTray.

 * LFSTray is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.

 * LFSTray is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

 * You should have received a copy of the GNU General Public License
 * along with LFSTray.  If not, see <http://www.gnu.org/licenses/>.
*/

#include <lfstk/LFSTKGlobals.h>

#include "globals.h"

LFSTray_trayClass	*trayClass=NULL;
LFSTK_windowClass	*transwc=NULL;
LFSTK_prefsClass		prefs(PACKAGE,VERSION);

option long_options[]=
	{
		{"monitor",required_argument,NULL,'m'},
		{"iconsize",required_argument,NULL,'i'},
		{"gravity",required_argument,NULL,'g'},
		{"vertical",no_argument,NULL,'V'},
		{"below",no_argument,NULL,'b'},
		{"no-duplicates",no_argument,NULL,'d'},
		{"filepath",required_argument,NULL,'f'},
		{0,0,0,0}
	};

void setPrefs(int argc,char **argv)
{
	std::string	configfile=getenv("HOME");
	configfile+="/.config/LFS/tray.rc";

	prefs.prefsMap=
		{
			{LFSTK_UtilityClass::LFSTK_hashFromKey("monitor"),{TYPEINT,"monitor","Place on monitor ARG","",false,0}},
			{LFSTK_UtilityClass::LFSTK_hashFromKey("iconsize"),{TYPEINT,"iconsize","Iconsize ARG","",false,32}},
			{LFSTK_UtilityClass::LFSTK_hashFromKey("gravity"),{TYPEINT,"gravity","Gravity NW ARG=1,NE ARG=2,SE ARG=3,SW ARG=4,N ARG=5,E ARG=6,S ARG=7,W ARG=8","",false,NW}},
			{LFSTK_UtilityClass::LFSTK_hashFromKey("vertical"),{TYPEBOOL,"vertical","Vertical systray ( default horizontal )","",false,0}},
			{LFSTK_UtilityClass::LFSTK_hashFromKey("below"),{TYPEBOOL,"below","Below all windows ( default above )","",false,0}},
			{LFSTK_UtilityClass::LFSTK_hashFromKey("filepath"),{TYPESTRING,"filepath","Use external file","",false,0}},
			{LFSTK_UtilityClass::LFSTK_hashFromKey("no-duplicates"),{TYPEBOOL,"no-duplicates","Don't allow duplicate items ( by _NET_WM_NAME property )","",false,0}},
		};
	prefs.LFSTK_loadVarsFromFile(configfile);
}

int main(int argc,char **argv)
{
	LFSTK_applicationClass	*apc=NULL;
	windowInitStruct			*wi=NULL;
	apc=new LFSTK_applicationClass;
	trayClass=new LFSTray_trayClass(apc);

	setPrefs(argc,argv);

	if(prefs.LFSTK_argsToPrefs(argc,argv,long_options,true)==false)
		{
			delete trayClass;
			delete apc;
			cairo_debug_reset_static_data();
			return(1);
		}
	else
		{
			trayClass->vertical=prefs.LFSTK_getBool("vertical")	;
			trayClass->isBelow=prefs.LFSTK_getBool("below");
			trayClass->allowDuplicates=!prefs.LFSTK_getBool("no-duplicates");
			trayClass->onMonitor=prefs.LFSTK_getInt("monitor");
			trayClass->gravity=(TrayPos)prefs.LFSTK_getInt("gravity");
			trayClass->iconSize=prefs.LFSTK_getInt("iconsize");
			trayClass->imagePath=prefs.LFSTK_getString("filepath");
		}

	wi=apc->LFSTK_getDefaultWInit();
	wi->overRide=false;

	if(trayClass->isBelow==true)
		wi->level=BELOWALL;
	else
		wi->level=ABOVEALL;

	wi->decorated=false;
	wi->className="LFSDOCK";
	wi->windowType=apc->appAtomsHashed.at(LFSTK_UtilityClass::LFSTK_hashFromKey("_NET_WM_WINDOW_TYPE_DOCK"));

	apc->LFSTK_addWindow(wi,PACKAGE);
	transwc=apc->mainWindow;

	if(access(trayClass->imagePath.c_str(),F_OK)==F_OK)
		{
			Imlib_Image buffer;

			imlib_context_set_display(apc->display);
			imlib_context_set_visual(DefaultVisual(apc->display, 0));
			buffer=imlib_load_image(trayClass->imagePath.c_str());
			imlib_context_set_image(buffer);
			imlib_context_set_drawable(apc->rootWindow);
			imlib_context_set_dither(0);
			imlib_image_set_has_alpha(1);
			imlib_render_pixmaps_for_whole_image(&trayClass->externalPixmap,&trayClass->externalMaskPixmap);
			imlib_free_image();
			imlib_image_decache_file(trayClass->imagePath.c_str())	;
		}

	trayClass->setTrayAtoms();

	XSetSelectionOwner(apc->display,trayClass->_NET_SYSTEM_TRAY_SCREEN,transwc->window,CurrentTime);
	if(XGetSelectionOwner(apc->display,trayClass->_NET_SYSTEM_TRAY_SCREEN) != transwc->window)
		{
			fprintf(stderr,"lfstray: cannot find systray manager"  "\n");
			return(1);
		}

	XClientMessageEvent ev;
	ev.type=ClientMessage;
	ev.window=apc->rootWindow;
	ev.message_type=trayClass->MANAGER;
	ev.format=32;
	ev.data.l[0]=CurrentTime;
	ev.data.l[1]=trayClass->_NET_SYSTEM_TRAY_SCREEN;
	ev.data.l[2]=transwc->window;
	ev.data.l[3]=0;
	ev.data.l[4]=0;
	XSendEvent(apc->display,apc->rootWindow,False,StructureNotifyMask, (XEvent *)&ev);

	transwc->LFSTK_setSticky(true);
	trayClass->resetWindow();

	apc->LFSTK_setEventCallBack(eventCB,(void*)0xdeadbeef);
	XErrorHandler old=XSetErrorHandler(windowErrorHandler);

	int retval=apc->LFSTK_runApp();

	delete trayClass;
	delete apc;
	cairo_debug_reset_static_data();
	return retval;
}