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

struct windowData
{
	LFSTK_windowClass	*window=NULL;
	bool				loopFlag=false;
	bool				showing=false;
};

class LFSTK_applicationClass
{
	public:
		LFSTK_applicationClass();
		~LFSTK_applicationClass();

		void						LFSTK_addWindow(windowInitStruct *wi,const char *name);
		void						LFSTK_addToolWindow(windowInitStruct *wi);
		int							LFSTK_runApp(void);
		int							LFSTK_runWindowLoop(int window);
		int							LFSTK_runWindowLoop(LFSTK_windowClass *win);
		int							LFSTK_findWindow(LFSTK_windowClass *win);

//timers
		void						LFSTK_setTimer(int seconds);
		void						LFSTK_setTimerCallBack(bool (*timer)(LFSTK_applicationClass*,void*),void* ud);

//gui
		bool						mainLoop=false;
		int							exitValue=0;
		Display						*display;
		int							screen;
		Visual						*visual;
		Window						rootWindow;		
		Colormap					cm;
		int							depth;
		int							blackColour;
		int							whiteColour;

		bool						gotARGB=false;
		int							displayWidth=0;
		int							displayHeight=0;

//globals
		bool						isDragging=false;
		LFSTK_lib					*globalLib=NULL;
		std::vector<windowData>		*windows;
		LFSTK_windowClass			*mainWindow=NULL;
		char						*configDir=NULL;
		char						*userHome=NULL;
		char						*iconThemeName=NULL;

	private:
		int							displayNum;
		bool						useTimer=false;
		int							timer=-1;
		callbackStruct				callBacks;
};

#endif
