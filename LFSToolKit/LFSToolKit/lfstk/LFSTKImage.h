/*
 *
 * ©K. D. Hedger. Thu  5 Nov 16:40:40 GMT 2015 kdhedger68713@gmail.com

 * This file (LFSTKImage.h) is part of LFSToolKit.

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

#ifndef _LFSTKIMAGE_
#define _LFSTKIMAGE_

/**
 *
 * \brief Image edit class for LFSToolKit.
 *
 * Basic Image gadget.
 *
 */
class LFSTK_imageClass  : public  LFSTK_gadgetClass
{
	public:
		LFSTK_imageClass();
		~LFSTK_imageClass();
		LFSTK_imageClass(LFSTK_windowClass* parentwc,const char* imagepath,int x,int y,int w,int gravity);

		bool mouseUp(XButtonEvent *e) {return(true);};
		bool mouseDown(XButtonEvent *e);
		bool mouseExit(XButtonEvent *e) {return(true);};
		bool mouseEnter(XButtonEvent *e) {return(true);};
		bool mouseDrag(XMotionEvent *e);

		void LFSTK_clearWindow(void);

		void LFSTK_setCanDrag(bool candrag);
		bool LFSTK_getCanDrag(void);
	protected:
		bool	canDrag=false;
		int		mouseDownX=0;
		int		mouseDownY=0;
};

#endif
