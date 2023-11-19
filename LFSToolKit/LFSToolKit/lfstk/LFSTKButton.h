/*
 *
 * ©K. D. Hedger. Fri 24 Jul 10:54:59 BST 2015 keithdhedger@gmail.com

 * This file (LFSTKButton.h) is part of LFSToolKit.

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


#ifndef _LFSTKBUTTON_
#define _LFSTKBUTTON_

/**
 * \brief Basic button class for LFSToolKit.
*/
class LFSTK_buttonClass : public LFSTK_gadgetClass
{
	public:
		LFSTK_buttonClass();
		~LFSTK_buttonClass();
		LFSTK_buttonClass(LFSTK_windowClass* parentwc,const char* label,int x,int y,unsigned w,unsigned h,int gravity=NorthWestGravity);

		bool	keyRelease(XKeyEvent *e);
		bool	keyPress(XKeyEvent *e);
};

#endif
