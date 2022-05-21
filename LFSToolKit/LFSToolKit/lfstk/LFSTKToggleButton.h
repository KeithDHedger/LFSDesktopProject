/*
 *
 * ©K. D. Hedger. Thu 17 Sep 11:55:08 BST 2015 keithdhedger@gmail.com

 * This file (LFSTKToggleButton.h) is part of LFSToolKit.

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


#ifndef _LFSTKTOGGLEBUTTON_
#define _LFSTKTOGGLEBUTTON_

enum drawStyle {TOGGLECHECK=0,TOGGLERADIO,TOGGLENORMAL};

/**
 * \brief Toglle button class for LFSToolKit.
*/
class LFSTK_toggleButtonClass  : public  LFSTK_gadgetClass
{
	public:
		LFSTK_toggleButtonClass();
		~LFSTK_toggleButtonClass();
		LFSTK_toggleButtonClass(LFSTK_windowClass* parentwc,const char* label,int x,int y,unsigned w,unsigned h,int gravity=NorthWestGravity);

		bool mouseEnter(XButtonEvent *e);
		bool mouseExit(XButtonEvent *e);
		bool mouseDown(XButtonEvent *e);
		bool mouseUp(XButtonEvent *e);

		void LFSTK_setToggleStyle(drawStyle ds);
		void LFSTK_setValue(bool val);
		bool LFSTK_getValue(void);

		cairo_surface_t	*checkOn=NULL;
		cairo_surface_t	*checkOff=NULL;
	private:
		drawStyle		boxStyle;
		bool			toggleState;
};

#endif
