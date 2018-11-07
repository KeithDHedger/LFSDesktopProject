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

#ifndef _LFSTKMENUITEM_
#define _LFSTKMENUITEM_

/**
 *
 * \brief Label class for LFSToolKit gadgets.
 *
 */
class LFSTK_menuItemClass : public LFSTK_buttonClass
{
	public:
		LFSTK_menuItemClass();
		~LFSTK_menuItemClass();

		LFSTK_menuItemClass(LFSTK_windowClass* parentwc,const char* label,int x,int y,unsigned w,unsigned h,menuStruct *menu,int gravity=CENTRE,int bgrav=BUTTONGRAV);
		//bool			mouseUp(XButtonEvent *e) {return(true);};
		//bool			mouseDown(XButtonEvent *e) {return(true);};
		bool			mouseExit(XButtonEvent *e);// {return(true);};
		bool			mouseEnter(XButtonEvent *e);// {return(true);};
		menuStruct		*menuData=NULL;
		LFSTK_windowClass	*subwc=NULL;
		bool				mainLoop=false;

};


#endif
