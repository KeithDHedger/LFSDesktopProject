/*
 *
 * ©K. D. Hedger. Fri  8 Jan 17:02:14 GMT 2021 keithdhedger@gmail.com

 * This file (LFSTKApplication.h) is part of LFSToolKit.

 * LFSToolKit is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * at your option) any later version.

 * LFSToolKit is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

 * You should have received a copy of the GNU General Public License
 * along with LFSToolKit.  If not, see <http://www.gnu.org/licenses/>.
 */
 
#ifndef _LFSTKAPPLICATION_
#define _LFSTKAPPLICATION_

#include <vector>
#include <signal.h>
#include <X11/extensions/Xinerama.h>

struct	monitorStruct
{
	int					x;
	int					y;
	int					w;
	int					h;
};

struct windowData
{
	LFSTK_windowClass	*window=NULL;
	bool					loopFlag=false;
	bool					showing=false;
};

class LFSTK_applicationClass
{
	public:
		LFSTK_applicationClass();
		~LFSTK_applicationClass();

		void						LFSTK_addWindow(windowInitStruct *wi,const char *name);
		void						LFSTK_addToolWindow(windowInitStruct *wi);
		int						LFSTK_runApp(void);
		int						LFSTK_runWindowLoop(int window);
		int						LFSTK_runWindowLoop(LFSTK_windowClass *win);
		int						LFSTK_findWindow(LFSTK_windowClass *win);
		windowInitStruct			*LFSTK_getDefaultWInit(void);

//monitors
		void						LFSTK_loadMonitorData(void);
		int						LFSTK_getMonitorCount(void);
		const monitorStruct		*LFSTK_getMonitorData(int monitor);
		const monitorStruct		*LFSTK_getMonitors(void);

//timers
		void						LFSTK_setTimer(int delay,bool usemicroseconds=false);
		void						LFSTK_setTimerCallBack(bool (*timer)(LFSTK_applicationClass*,void*),void* ud);

//gui
		bool						mainLoop=false;
		int						exitValue=0;
		Display					*display;
		int						screen;
		Visual					*visual;
		Window					rootWindow;		
		Colormap					cm;
		int						depth;
		int						blackColour;
		int						whiteColour;
		monitorStruct			*monitors=NULL;
		int						monitorCount;

		bool						gotARGB=false;
		int						displayWidth=0;
		int						displayHeight=0;

//globals
		bool						isDragging=false;
		LFSTK_lib				*globalLib=NULL;
		std::vector<windowData>	*windows;
		LFSTK_windowClass		*mainWindow=NULL;
		std::string				configDir="";
		std::string				userHome="";
		std::string				iconThemeName="";
		std::map<unsigned long,Atom>	appAtomsHashed;

	private:
		int						displayNum;
		bool						useTimer=false;
		bool						useMicroSeconds=false;
		int						timer=-1;
		callbackStruct			callBacks;
};

#endif
