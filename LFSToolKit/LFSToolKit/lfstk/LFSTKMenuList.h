/*
 *
 * ©K. D. Hedger. Fri 23 Sep 15:31:48 BST 2016 kdhedger68713@gmail.com

 * This file (LFSTKMenuList.h) is part of LFSToolKit.

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


#ifndef _LFSTKMENULIST_
#define _LFSTKMENULIST_

#define FWIDTH 600
#define FHITE 400
#define FBORDER 8
#define FGAP 4
#define FNAVBUTTONWID 24
#define FNAVBUTTONHITE 24
#define FICONSIZE 22
#define FFILEHITE 8
#define FDIRHITE 5

class LFSTK_menuListClass
{
	public:
		~LFSTK_menuListClass();
		LFSTK_menuListClass(LFSTK_windowClass* parentwc,const char *label,const char *startdir,bool type);

};

#endif
