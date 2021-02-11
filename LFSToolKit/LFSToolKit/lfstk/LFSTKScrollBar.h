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
 * \brief Scrollbar class for LFSToolKit.
*/
class LFSTK_scrollBarClass : public LFSTK_gadgetClass
{
	public:
		LFSTK_scrollBarClass();
		~LFSTK_scrollBarClass();
		LFSTK_scrollBarClass(LFSTK_windowClass* parentwc,bool vertical,int x,int y,unsigned w,unsigned h,int gravity=NorthWestGravity);

		bool				mouseUp(XButtonEvent *e);
		bool				mouseDown(XButtonEvent *e);
		bool				mouseEnter(XButtonEvent *e);
		bool				mouseExit(XButtonEvent *e);
		bool				keyRelease(XKeyEvent *e);

		void				LFSTK_clearWindow(void);
		void				LFSTK_clearWindow(bool ignorecb);
		void				LFSTK_setStyle(bevelType s);
		void				LFSTK_resetHeight(int newheight);

//set indexes
		void				LFSTK_setScale(double min,double max);
		void				LFSTK_setPageScroll(int page);
		void				LFSTK_setLineScroll(int line);
		void				LFSTK_setValue(int val,bool ignore=false);
		int					LFSTK_getValue(void);
		void				LFSTK_setAllowKBControl(bool allow);
		bool				LFSTK_getAllowKBControl(void);
		void				LFSTK_scrollByLine(bool up);
		void				LFSTK_scrollByPage(bool up);

		double				scale=1.0;
		bool				reverse=false;
		double				maxValue=100.0;
		bool				lockToWindow=false;
		Window				lockToWindowRight=-1;

	private:
		bool				allowKBControl=true;
		int					value=0;
		LFSTK_buttonClass	*thumb=NULL;
		LFSTK_buttonClass	*upLeft=NULL;
		LFSTK_buttonClass	*downRight=NULL;
		bool				startDrag=false;
//calbacks
		static bool			lineUpDown(void *object,void* userdata);
		static bool			pageUpDown(void *object,void* userdata);
		static bool			thumbClicked(void *object,void* userdata);
		static bool			startThumbDrag(void *object,void* userdata);
//private routines
		void				setState(bool byvalue,bool ignorecb=false);

		int					pageScroll=5;
		double				minScale=1.0;
		double				maxScale=100.0;
		int					lineScroll=1;
		bool				verticalBar=false;
};

#endif
