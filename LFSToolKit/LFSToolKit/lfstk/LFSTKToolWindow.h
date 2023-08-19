/*
 *
 * ©K. D. Hedger. Fri  9 Nov 10:58:13 GMT 2018 keithdhedger@gmail.com

 * This file (LFSTKToolWindow.h) is part of LFSToolKit.

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


#ifndef _LFSTKTOOLWINDOW_
#define _LFSTKTOOLWINDOW_

/**
 * \brief Tool window class for LFSToolKit.
*/
class LFSTK_toolWindowClass : public LFSTK_windowClass
{
	public:

		~LFSTK_toolWindowClass();
		LFSTK_toolWindowClass();
		LFSTK_toolWindowClass(Display *disp,LFSTK_windowClass *wc,const char *windowtype,int x,int y,int w,int h,const char* name,LFSTK_applicationClass *app);
		LFSTK_toolWindowClass(windowInitStruct *wi,LFSTK_applicationClass *app);

//int globaly=0;

	protected:
	private:
		void	windowClassInitCommon(windowInitStruct *wi);
};

#endif
