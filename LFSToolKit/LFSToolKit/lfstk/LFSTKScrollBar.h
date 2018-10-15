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


#ifndef _LFSTKSCROLLBAR_
#define _LFSTKSCROLLBAR_

/**
 *
 * \brief Button class for LFSToolKit.
 * Basic button class for LFSToolKit.
 *
 */
class LFSTK_scrollBarClass : public LFSTK_gadgetClass
{
	public:
		LFSTK_scrollBarClass();
		~LFSTK_scrollBarClass();
		LFSTK_scrollBarClass(LFSTK_windowClass* parentwc,const char* label,int x,int y,unsigned w,unsigned h,int gravity);

		bool				mouseUp(XButtonEvent *e);
		bool				mouseEnter(XButtonEvent *e);
		bool				mouseExit(XButtonEvent *e);

		void				LFSTK_clearWindow(void);
		void				LFSTK_setStyle(bevelType s);
//set indexes
		void				LFSTK_setScale(double min,double max);
		void				LFSTK_setPageUp(int page);
		void				LFSTK_setPageDown(int page);
		void				LFSTK_setScrollLine(int line);
		
		int					value=0;
	private:
		LFSTK_buttonClass	*thumb=NULL;
		LFSTK_buttonClass	*upLeft=NULL;
		LFSTK_buttonClass	*downRight=NULL;

//calbacks
		static bool			lineUpDown(void *object,void* userdata);
		static bool			thumbClicked(void *object,void* userdata);

		int					troughScroll=10;//for now set at 10%
		double				maxValue=100.0;
		double				minScale=1.0;
		double				maxScale=100.0;
		double				scale=10.0;
		int					lineScroll=10;
		bool				verticalBar=true;
		bool				noCallback=false;
};

#endif
