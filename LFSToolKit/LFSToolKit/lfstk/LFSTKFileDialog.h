/*
 *
 * ©K. D. Hedger. Tue 13 Sep 20:00:53 BST 2016 keithdhedger@gmail.com

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

#define FGAP 4
#define FNAVBUTTONWID 24
#define FNAVGADGETHITE 24
#define FICONSIZE 22
#define FFILEHITE 10
#define FDIRHITE 10
#define IMAGESIZE 64

#define FILEDIALOG false
#define FOLDERDIALOG true

#define PREVIEWWIDTH 192

class LFSTK_fileDialogClass
{
	public:
		~LFSTK_fileDialogClass();
		LFSTK_fileDialogClass(LFSTK_windowClass* parentwc,const char *label,const char *startdir,bool type,const char *recentname=NULL);

		void					LFSTK_showFileDialog(void);
		void					LFSTK_showFileDialog(const char *dir,const char *title);
		bool					LFSTK_isValid(void);
		void					LFSTK_getLastFolder(void);
		const char				*LFSTK_getCurrentDir(void);
		const char				*LFSTK_getCurrentFile(void);
		const char				*LFSTK_getCurrentPath(void);
		const char				*LFSTK_getCurrentFileSelection(void);
		void					LFSTK_setWorkingDir(const char *dir);
		void					LFSTK_setRequestType(bool type);
		bool					LFSTK_getRequestType(void);
//TODO
		void					LFSTK_setShowPreview(bool);
		void					LFSTK_setNameFilter(const char *filt);
	private:
		char					*findThemedIconFromMime(const char *mimetype);
		void					setPreviewData(void);
//TODO
		void					resizeWindow(int w,int h);

		int						dirlisthite=GADGETHITE*FDIRHITE;
		const char				*recentsName="universal";
		char					**dirList=NULL;
		char					**dirImageList=NULL;
		char					**fileList=NULL;
		char					**fileImageList=NULL;
		unsigned				dirListCnt=0;
		unsigned				fileListCnt=0;
		char					*currentDir=NULL;
		char					*currentFile=NULL;
		char					*currentPath=NULL;
//TODO
		char					*filter=NULL;

		bool					mainLoop;
		bool					apply;
		bool					dialogType;
		const char				*fileImage;
		const char				*folderImage;
		bool					showPreview=false;
		LFSTK_imageClass		*tux=NULL;
		LFSTK_labelClass		*previewMimeType;
		LFSTK_labelClass		*previewSize;
		LFSTK_labelClass		*previewFileName;
		LFSTK_labelClass		*previewMode;

		LFSTK_windowClass		*wc;
		LFSTK_windowClass		*dialog;
		LFSTK_listGadgetClass	*dirListGadget;
		LFSTK_listGadgetClass	*fileListGadget;
		LFSTK_buttonClass		*buttonApply;
		LFSTK_buttonClass		*buttonCancel;
		LFSTK_lineEditClass		*dirEdit;
		void					getDirList(void);
		void					getFileList(void);
		void					freeDirList();
		void					freeFileList();
		void					cleanDirPath(void);
		void					openDir(void);
		void					doOpenDir(void);
		void					doOpenDir(const char *dir);
};

#endif
