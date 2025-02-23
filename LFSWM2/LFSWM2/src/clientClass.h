/*
 *
 * ©K. D. Hedger. Tue  9 Aug 20:42:29 BST 2022 keithdhedger@gmail.com

 * This file (clientClass.h) is part of LFSWM2.

 * LFSWM2 is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.

 * LFSWM2 is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

 * You should have received a copy of the GNU General Public License
 * along with LFSWM2.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef _CLIENTCLASS_
#define _CLIENTCLASS_
#include <stdio.h>
#include <stdlib.h>
#include <cstdlib>
#include <X11/extensions/Xrender.h>
#include <X11/extensions/shape.h>

#include "lfswm2Class.h"

#define SKIPTIME 10
#define MAXCONFIGCNT 8

static const char	*menuItemlabels[]={"Maximize","Minimize","Shade","Fullscreen","--","On Top","On Bottom","On All Desks","--","Fast Resize","Live Resize","--","Close","--","About",NULL};
static const char	*altMenuitemlabels[]={"Un-Maximize","Un-Minimize","Un-Shade","Un-Fullscreen","--","Not On Top","Not On Bottom","Only This Desk","--","Fast Resize","Live Resize","--","Close","--","About",NULL};

enum {MAXLABEL=0,MINLABEL,SHADELABEL,FSLABEL,PASS1,ONTOPLABEL,ONBOTTOMLABEL,ONALLDESKSLABEL,PASS2,FASTSIZELABEL,LIVESIZELABEL,PASS3,CLOSELABEL,PASS4,ABOUTLABEL,NUMOFMENUS};

class LFSWM2_clientClass
{
	public:
		LFSWM2_clientClass(LFSWM2_Class *mainclass,Window id);
		~LFSWM2_clientClass(void);

		void				LFSWM2_setWindowName(void);
		bool				LFSWM2_handleControls(XEvent *e);
		void				renderFrame(bool isfirst,int x=0,int y=0);
//vars
		bool				rendered=false;
		LFSWM2_Class		*mainClass=NULL;

		Window			contentWindow=None;
		Window			frameWindow=None;
		Window			transientFor=None;
		GC				frameGC=None;

		//bool				useAlternate=false;
		std::vector<std::string>	menuNames;

		std::string		name="";
		int				windowType=-1;
		bool				visible=true;
		unsigned long	onDesk=0;
		bool				visibleOnAllDesks=false;
		bool				onTop=false;
		bool				onBottom=false;
		bool				isMaximized=false;
		bool				isFullscreen=false;
		bool				isShaded=false;
		bool				canMaximize=false;
		bool				canMinimize=false;
		bool				canShade=false;
		bool				canResize=false;
		bool				canClose=false;
		bool				isActive=false;
		bool				isWithdrawn=false;
		bool				isBorderless=false;
		bool				isHidden=false;
		bool				isMinimized=false;

		int				resizeMode=0;
		int				configCnt=0;

		rectStruct		contentWindowRect;
		rectStruct		frameWindowRect;
		rectStruct		clientPreMaxRect;
		rectStruct		framePreMaxRect;
		rectStruct		clientPreFSRect;
		rectStruct		framePreFSRect;
		int				clientPreShade;

		bool				nameIsUTF=false;
		bool				moveToTop=false;

		hintsDataStruct	windowHints;

		Window			closeButton=None;
		Window			maximizeButton=None;
		Window			minimizeButton=None;
		Window			shadeButton=None;
		Window			menuButton=None;

		controlData		closeControlStruct;
		controlData		maximizeControlStruct;
		controlData		minimizeControlStruct;
		controlData		shadeControlStruct;
		controlData		menuControlStruct;

		Window			topLeftDragger=None;
		Window			topRightDragger=None;
		Window			bottomLeftDragger=None;
		Window			bottomRightDragger=None;
		Window			leftSideDragger=None;
		Window			rightSideDragger=None;
		Window			bottomDragger=None;

		void				LFSWM2_showWindow(bool checkstate=false);
		void				LFSWM2_hideWindow(bool withdraw=false);

		void				LFSWM2_fullscreenWindow(bool isfull,bool force=false);

		void				LFSWM2_maxWindow(bool ismaxed,bool force=false);
		void				LFSWM2_maxWindow(void);
		void				LFSWM2_minWindow(void);
		void				LFSWM2_shadeWindow(void);
		void				LFSWM2_sendCloseWindow(void);

		void				LFSWM2_resizeControls(void);

		void				LFSWM2_setNetWMState(XEvent *e);
		void				LFSWM2_setWMState(void);

		void				LFSWM2_drawMouseLeave(Window id,Pixmap pm,struct controlData data);

		bool				LFSWM2_doFrameMoveEvents(XEvent *e);
		bool				LFSWM2_handleEvents(XEvent *e);
		void				LFSWM2_setFrameExtents(void);

		void				LFSWM2_unSpecial(void);

		int				dragSpacing=16;
		int				leftSideDragWidth=1;
		int				riteSideDragWidth=1;
		int				smoothness=4;//TODO//
		int				controlCnt=0;
		int				minWidth;
		int				minHeight;
		Window			resizeWindow=None;
		bool				finishedResize=true;
		bool				buttonDown=false;
		int				riteButtonsWidth=0;

//bool nodecs=false;
		bool				doneRestack=false;
		void				adjustContentWindow(void);
		void				resizeContentWindow(rectStruct r,bool moveorigin=true);
		void				setWindowRects(bool resize=true);
		rectStruct		setTitlePosition(void);
		void				resetContentWindow(void);//TODO//all the routines need redoing

		long				LFSWM2_getWindowState(void);

	private:
		int				sx=0;
		int				rsx=0;
		int				rsy=0;
		int				sy=0;
		int				steps;
		bool				inControl=false;

		rectStruct		dragRect;
		rectStruct		holdFrameRect={-1,-1,-1,-1};

		Pixmap			currentRootPixmap=None;
		Picture			windowPicture=None;
		Picture			primaryPicture=None;
		XRenderPictureAttributes	pa;
		XRenderPictFormat		*picFormat;
		int				ow;
		int				oh;
		XImage			*resizeImage=NULL;

		bool				first=true;
		int				firstx=-10000;
		int				firsty=-10000;

		void				drawMouseEnter(Window id,Pixmap pm,controlData data);
		void				drawMousePressed(Window id,Pixmap pm,controlData data);
		bool				doResizeDraggers(XEvent *e);
		void				showWMMenu(int x,int y);
		static bool		wmCB(void *p,void* ud);

		void				resizeContentWindow(int w,int h,bool useframerect=false);
		void				resizeFrameWindow(void);
};

#endif
