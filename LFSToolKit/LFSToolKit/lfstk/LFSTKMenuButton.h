/*
 *
 * ©K. D. Hedger. Fri 24 Jul 10:54:59 BST 2015 keithdhedger@gmail.com

 * This file (LFSTKMenuButton.h) is part of LFSToolKit.

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


#ifndef _LFSTKMENUBUTTON_
#define _LFSTKMENUBUTTON_
#if 1
/**
 *
 * Menu structure for menuItemClass.
 * \note Menu data passed to gadget is NOT owned by gadgetEvent
 * \note and MUST be freed by application.
 * \note Including image and icon data.
 */
struct menuItemStruct
{
	menuItemStruct():	label(NULL),userData(NULL),bc(NULL),subMenus(NULL),subMenuCnt(0),imagePath(NULL) {}
	char				*label;
	void				*userData;
	LFSTK_buttonClass	*bc;
	menuItemStruct		*subMenus;
	int					subMenuCnt;
	char				*imagePath;
};

/**
 *
 * \brief Menu button class for LFSToolKit.
 *
 * Basic menu class for LFSToolKit.
 * \note Menu data passed to gadget is NOT owned by gadgetEvent
 * \note and MUST be freed by application.
 *
 */
class LFSTK_menuButtonClass : public LFSTK_buttonClass
{
	public:
		LFSTK_menuButtonClass();
		~LFSTK_menuButtonClass();
		LFSTK_menuButtonClass(LFSTK_windowClass* parentwc,const char* label,int x,int y,unsigned w,unsigned h,int gravity);

		bool mouseDown(XButtonEvent *e);
		void LFSTK_addMenus(menuItemStruct* menus,int cnt);
		void LFSTK_updateMenus(menuItemStruct* menus,int cnt);

	private:
		void initMenuButton(void);
		menuItemStruct		*menus;
		int					menuCount;
		LFSTK_windowClass	*subwc;
		bool				builtMenu;
		const char			*menuItemFont=NULL;
};

#endif
#endif