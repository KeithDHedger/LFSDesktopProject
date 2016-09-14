/*
 *
 * ©K. D. Hedger. Tue 13 Sep 20:00:53 BST 2016 kdhedger68713@gmail.com

 * This file (LFSTKFileDialog.h) is part of LFSToolKit.

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


#ifndef _LFSTKFILEDIALOG_
#define _LFSTKFILEDIALOG_

#define FWIDTH 600
#define FHITE 400
#define FBORDER 8
#define FGAP 4
#define FNAVBUTTONWID 24
#define FNAVBUTTONHITE 24

class LFSTK_fileDialogClass  : public  LFSTK_gadgetClass
{
	public:
							~LFSTK_fileDialogClass();
							LFSTK_fileDialogClass(Window parent,const char *label,const char *startdir);
		void				LFSTK_showDialog(void);
		bool				LFSTK_isValid(void);

		LFSTK_listGadgetClass	*list;
	private:
		
		char				**dirList;
		char				**dirImageList;
		unsigned			dirListCnt;
		const char			*currentDir;
		bool				mainLoop;
		bool				apply;

		LFSTK_windowClass	*dialog;
		void				getDirList(void);
		LFSTK_buttonClass	*buttonApply;
		LFSTK_buttonClass	*buttonCancel;
};

#endif
