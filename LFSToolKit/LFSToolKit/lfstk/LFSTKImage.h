/*
 *
 * ©K. D. Hedger. Thu  5 Nov 16:40:40 GMT 2015 keithdhedger@gmail.com

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
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <stdio.h>
#include <X11/extensions/shape.h>

/**
 * \brief Image class for LFSToolKit.
*/
class LFSTK_imageClass  : public  LFSTK_gadgetClass
{
	public:
		LFSTK_imageClass();
		~LFSTK_imageClass();
		LFSTK_imageClass(LFSTK_windowClass* parentwc,const char* imagepath,int x,int y,unsigned w,unsigned h,int gravity,bool scale);

		bool			mouseUp(XButtonEvent *e);
		bool			mouseEnter(XButtonEvent *e);
		bool			mouseExit(XButtonEvent *e);
		void			LFSTK_clearWindow(void);

		cairo_surface_t	*shapesfc=NULL;
		cairo_t			*shapecr=NULL;
		Pixmap			shape;

	protected:
};

#endif
