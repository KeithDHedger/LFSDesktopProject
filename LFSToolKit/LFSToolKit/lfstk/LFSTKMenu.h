/*
 *
 * ©K. D. Hedger. Sun  4 Nov 19:37:03 GMT 2018 keithdhedger@gmail.com

 * This file (LFSTKMenuItem.h) is part of LFSToolKit.

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

#ifndef _LFSTKMENU_
#define _LFSTKMENU_

/**
 *
 * \brief Label class for LFSToolKit gadgets.
 *
 */
class LFSTK_menuClass : public LFSTK_gadgetClass
{
	public:
		LFSTK_menuClass();
		~LFSTK_menuClass();

		LFSTK_menuClass(LFSTK_windowClass *parentwc,const char *label,int x,int y,unsigned w,unsigned h,int gravity=CENTRE,int bgrav=BUTTONGRAV);
		bool			mouseExit(XButtonEvent *e) {return(true);};
		bool			mouseEnter(XButtonEvent *e) {return(true);};
		void			LFSTK_addMainMenus(menuStruct **menus,int menucnt);

	private:		
		menuStruct		**mainMenu=NULL;
		int				mainMenuCnt=0;
};


#endif
