/*
 *
 * ©K. D. Hedger. Fri 28 Aug 19:16:54 BST 2015 keithdhedger@gmail.com

 * This file (LFSTKLabel.h) is part of LFSToolKit.

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


#ifndef _LFSTKLABEL_
#define _LFSTKLABEL_

/**
 *
 * \brief Label class for LFSToolKit gadgets.
 *
 */
class LFSTK_labelClass : public LFSTK_gadgetClass
{
	public:
		LFSTK_labelClass();
		~LFSTK_labelClass();

		LFSTK_labelClass(LFSTK_windowClass* parentwc,const char* label,int x,int y,unsigned w,unsigned h,int gravity=CENTRE,int bgrav=BUTTONGRAV);
		bool			mouseUp(XButtonEvent *e) {return(true);};
		bool			mouseDown(XButtonEvent *e) {return(true);};
		bool			mouseExit(XButtonEvent *e) {return(true);};
		bool			mouseEnter(XButtonEvent *e) {return(true);};
};

#endif
