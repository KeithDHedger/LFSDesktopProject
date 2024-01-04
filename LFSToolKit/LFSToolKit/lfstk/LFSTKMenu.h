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
 * \brief Menu class for LFSToolKit.
*/
class LFSTK_menuClass
{
	public:
		~LFSTK_menuClass();
		LFSTK_menuClass(LFSTK_windowClass *wc,int x,int y,unsigned w,unsigned h);

		void									LFSTK_addMainMenus(infoDataStruct **menus,int menucnt);
		void									LFSTK_showMenu(void);

		int									x;
		int									y;
		unsigned								w;
		unsigned								h;
		LFSTK_windowClass					*parentwc;
		int									maxMenusDisplayed=0;
		int									menuHeight=0;
		bool									isScrollable=false;
		LFSTK_buttonClass					*upButton=NULL;
		LFSTK_buttonClass					*downButton=NULL;
		int									topMenu=0;


		void									LFSTK_setMouseCallBack(bool (*downcb)(void*,void*),bool (*releasecb)(void*,void*),void* ud);
		void									LFSTK_freeMenus(infoDataStruct **menus,int menucnt);
		//int									LFSTK_getTextWidthForFont(const char *text);
		int									LFSTK_getTextWidthForFont(std::string text);

		bool									mainLoop=false;
		LFSTK_toolWindowClass				*mainMenuWindow=NULL;
		std::vector<LFSTK_toolWindowClass*>	*subwindows;

		static bool							menuScroll(void *object,void* userdata);
		int									mainMenuCnt=0;
		infoDataStruct							**mainMenu=NULL;
	private:		
		void 								resizeMenu(void);

		callbackStruct						callBacks;

		cairo_font_weight_t					weight=CAIRO_FONT_WEIGHT_NORMAL;
		cairo_font_slant_t					slant=CAIRO_FONT_SLANT_NORMAL;
		int									fontSize=10;
		std::string							fontName;
		std::string							fontDesc;
};

#endif