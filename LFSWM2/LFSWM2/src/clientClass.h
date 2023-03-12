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

class LFSWM2_Class;
#include "lfswm2Class.h"
class LFSWM2_clientClass
{
	public:
		LFSWM2_clientClass(LFSWM2_Class *mainclass,Window id);
		~LFSWM2_clientClass(void);

		void				LFSWM2_setWindowName(void);

		bool				LFSWM2_handleControls(XEvent *e);

//vars
		LFSWM2_Class		*mainClass;

		Window			contentWindow=0;
		Window			frameWindow=0;
		Window			transientFor=0;

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
		bool				canResize=false;

		rectStruct		contentWindowRect;
		rectStruct		frameWindowRect;
		rectStruct		clientPreMaxRect;
		rectStruct		framePreMaxRect;
		rectStruct		clientPreFSRect;
		rectStruct		framePreFSRect;
		int				clientPreShade;

		bool				nameIsUTF=false;
		bool				moveToTop=false;

		motifHints		mwmHints;

		Window			closeButton;
		Window			maximizeButton;
		Window			minimizeButton;
		Window			shadeButton;

		Window			topLeftDragger;
		Window			topRightDragger;;
		Window			bottomLeftDragger;
		Window			bottomRightDragger;
		Window			leftSideDragger;
		Window			rightSideDragger;
		Window			bottomDragger;

		void				LFSWM2_showWindow(void);
		void				LFSWM2_hideWindow(void);
		void				LFSWM2_maxWindow(bool ismaxed,bool force=false);
		void				LFSWM2_fullscreenWindow(bool isfull,bool force=false);

		void				LFSWM2_sendCloseWindow(void);
		void				LFSWM2_resizeControls(void);

		void				LFSWM2_setWMState(XEvent *e);

		void				LFSWM2_drawMouseLeave(Window id,Pixmap pm,struct controlData data);

		bool				LFSWM2_doFrameMoveEvents(XEvent *e);
		void				LFSWM2_refreshFrame(XExposeEvent *e=NULL);

		bool				LFSWM2_handleEvents(XEvent *e);

		int				dragsize=16;
		int				smoothness=5;

bool nodecs=false;
	private:
		bool				buttonDown=false;
		int				sx=0;
		int				sy=0;
		int				steps;

		void				drawMouseEnter(Window id,Pixmap pm,controlData data);
		bool				doResizeDraggers(XEvent *e);
		void				adjustContentWindow(void);
		void				resizeContentWindow(rectStruct r,bool moveorigin=true);
		void				setWindowRects(bool resize=true);
		rectStruct		setTitlePosition(void);
};

#endif
